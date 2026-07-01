#include "Game.h"
#include "Board.h"
#include <algorithm>
#include <io.h>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

bool Game::resetGameSession()
{
	current_level_index = 0;
	current_riddle_index = 0;
	players[0].resetHealth();
	players[1].resetHealth();
	players[0].setTeamate(&players[1]);
	players[1].setTeamate(&players[0]);
	resetGameScore();

	return loadRiddlesFromFile("riddles.txt");
}

void Game::movePlayer(Player& player)
{
	if (player.finished_level || player.health == 0)
	{
		return;
	}

	int new_x = player.getPosition().getX() + player.getPosition().getDiffX();
	int new_y = player.getPosition().getY() + player.getPosition().getDiffY();

	if (player.is_launching && game_board.isPlayer(new_x, new_y))
	{
		if (player.teamate != nullptr)
		{
			player.teamate->receiveSpringPush(player.active_spring, player.current_speed,
			                                  player.spring_force_moves_left);

			return;
		}
	}

	if ((!game_board.isWall(new_x, new_y) && !game_board.isPlayer(new_x, new_y)) || player.is_compressing)
	{
		if (handlePlayerInteraction(player, new_x, new_y))
		{
			return;
		}

		int curr_x = player.getPosition().getX();
		int curr_y = player.getPosition().getY();
		char previous_char = ' ';

		for (const auto& bomb : bombs)
		{
			if (bomb.isActive() && bomb.getX() == curr_x && bomb.getY() == curr_y)
			{
				previous_char = '@';
				break;
			}
		}

		if (previous_char == ' ')
		{
			for (const auto& golem : golems)
			{
				if (golem.getPosition().getX() == curr_x && golem.getPosition().getY() == curr_y)
				{
					previous_char = golem.getDisplayChar();
					break;
				}
			}
		}

		game_board.setCharAt(curr_x, curr_y, previous_char);
		game_board.drawChar(curr_x, curr_y, previous_char);
		player.getPosition().move();
		game_board.setCharAt(new_x, new_y, player.getDisplayChar());
	}

	game_board.drawChar(player.getPosition().getX(), player.getPosition().getY(), player.getDisplayChar());
}

bool Game::handlePlayerInteraction(Player& player, int& new_x, int& new_y)
{
	if (game_board.isKey(new_x, new_y))
	{
		player.pickUpKey(new_x, new_y);
	}

	if (game_board.isTorch(new_x, new_y))
	{
		player.pickUpTorch(new_x, new_y);
	}

	if (game_board.isPortal(new_x, new_y))
	{
		handlePortal(player, new_x, new_y);

		return true;
	}

	if (game_board.isCookie(new_x, new_y))
	{
		game_board.setCharAt(new_x, new_y, ' ');
		addLevelPoints(POINTS_COOKIE);
	}

	if (game_board.isBomb(new_x, new_y))
	{
		player.pickUpBomb(new_x, new_y);
	}

	if (game_board.isRiddle(new_x, new_y))
	{
		handleRiddle(player, new_x, new_y);

		return true;
	}

	if (game_board.isSwitch(new_x, new_y))
	{
		handleSwitch(player, new_x, new_y);

		return true;
	}

	if (game_board.isSpring(new_x, new_y) || player.is_launching || player.is_compressing)
	{
		player.handleSpring(new_x, new_y);

		return true;
	}

	if (game_board.isObstacle(new_x, new_y))
	{
		player.moveObstacle(new_x, new_y);
	}

	if (game_board.isGolem(new_x, new_y))
	{
		damagePlayer(player, "a Golem");
		if (player.getIfDead())
		{
			return true;
		}
	}

	if (game_board.isDoor(new_x, new_y))
	{
		handleDoor(player, new_x, new_y);

		return true;
	}

	return false;
}

void Game::handlePortal(Player& player, int& new_x, int& new_y)
{
	if (portals.size() < 2)
	{
		return;
	}

	int target_x = -1;
	int target_y = -1;

	for (size_t i = 0; i < portals.size(); ++i)
	{
		if (new_x == portals[i].getX() && new_y == portals[i].getY())
		{
			size_t target_index = (i == 0) ? 1 : 0;

			target_x = portals[target_index].getX();
			target_y = portals[target_index].getY();
			break;
		}
	}

	if (target_x == -1)
	{
		return;
	}

	int set_x = target_x + player.getPosition().getDiffX();
	int set_y = target_y + player.getPosition().getDiffY();
	bool destination_found = game_board.isInsideBounds(set_x, set_y) && game_board.getCharAt(set_x, set_y) == ' ';

	if (!destination_found)
	{
		for (int dy = -1; dy <= 1 && !destination_found; ++dy)
		{
			for (int dx = -1; dx <= 1; ++dx)
			{
				if (dx == 0 && dy == 0)
				{
					continue;
				}

				int check_x = target_x + dx;
				int check_y = target_y + dy;

				if (game_board.isInsideBounds(check_x, check_y) && game_board.getCharAt(check_x, check_y) == ' ')
				{
					set_x = check_x;
					set_y = check_y;
					destination_found = true;
					break;
				}
			}
		}
	}

	if (!destination_found)
	{
		player.getPosition().setDirection(Direction::STAY);

		return;
	}

	int old_x = player.getPosition().getX();
	int old_y = player.getPosition().getY();

	game_board.setCharAt(old_x, old_y, ' ');
	game_board.drawChar(old_x, old_y, ' ');
	player.setPosition(set_x, set_y);
	game_board.setCharAt(set_x, set_y, player.getDisplayChar());
	game_board.drawChar(set_x, set_y, player.getDisplayChar());
}

void Game::handleRiddle(Player& player, int& new_x, int& new_y)
{
	for (auto& riddle : riddles)
	{
		if (new_x != riddle.getPosition().getX() || new_y != riddle.getPosition().getY())
		{
			continue;
		}

		if (!riddle.isSolved())
		{
			game_board.showRiddlePopup(riddle.getQuestion());
			std::string player_answer = getRiddleInput();
			bool correct = riddle.checkAnswer(player_answer);

			logRiddle(riddle.getQuestion(), player_answer, correct);

			if (!game_board.areSilent())
			{
				gotoxy(6, 18);
				std::cout << (correct ? "*** CORRECT! The path is open ***" : "*** WRONG! Try again later ***");
			}

			if (correct)
			{
				game_board.setCharAt(new_x, new_y, ' ');
				addLevelPoints(POINTS_RIDDLE_CORRECT);
			}
			else
			{
				player.getPosition().setDirection(Direction::STAY);
				new_x = player.getPosition().getX();
				new_y = player.getPosition().getY();
				addLevelPoints(-POINTS_RIDDLE_WRONG);
			}

			if (!game_board.areSilent())
			{
				if (!correct)
				{
					sleep2000();
				}

				cls();
				game_board.printBoard();
			}
		}

		break;
	}
}

void Game::handleSwitch(Player& player, int& new_x, int& new_y)
{
	for (size_t i = 0; i < switches.size(); ++i)
	{
		if (new_x != switches[i].getPosition().getX() || new_y != switches[i].getPosition().getY())
		{
			continue;
		}

		game_board.setCharAt(new_x, new_y, switches[i].isPressed() ? '\\' : '/');
		player.getPosition().setDirection(Direction::STAY);
		new_x = player.getPosition().getX();
		new_y = player.getPosition().getY();
		switches[i].press();

		if (i == 0)
		{
			game_board.displaySwitch1Status(switches[i]);
		}
		else
		{
			game_board.displaySwitch2Status(switches[i]);
		}

		break;
	}
}

void Game::handleDoor(Player& player, int& new_x, int& new_y)
{
	bool all_switches_on = true;

	for (const auto& game_switch : switches)
	{
		if (!game_switch.isPressed())
		{
			all_switches_on = false;
			break;
		}
	}

	if (!all_switches_on)
	{
		if (player.holding_item == Inventory::KEY)
		{
			game_board.displayGeneralMessage("Switches inactive! Door locked.");
		}

		player.getPosition().setDirection(Direction::STAY);
		new_x = player.getPosition().getX();
		new_y = player.getPosition().getY();

		return;
	}

	bool used_key_this_cycle = false;

	if (player.holding_item == Inventory::KEY)
	{
		door.useKey();
		player.holding_item = Inventory::NONE;
		player.held_key_index = -1;
		game_board.updateHeldItemAndRedraw(player.player_id, "None");
		game_board.displayUsedKeyMessage(player.player_id);
		used_key_this_cycle = true;
	}

	if (door.getIsOpen())
	{
		game_board.displayDoorOpenedMessage(player.player_id);
	}

	if (door.getIsOpen() && !used_key_this_cycle)
	{
		player.finished_level = true;
		int old_x = player.getPosition().getX();
		int old_y = player.getPosition().getY();

		game_board.setCharAt(old_x, old_y, ' ');
		game_board.drawChar(old_x, old_y, ' ');

		return;
	}

	player.getPosition().setDirection(Direction::STAY);
	new_x = player.getPosition().getX();
	new_y = player.getPosition().getY();
}

bool Game::isPathClear(int start_x, int start_y, int target_x, int target_y)
{
	int dx = abs(target_x - start_x);
	int dy = abs(target_y - start_y);
	int sx = (start_x < target_x) ? 1 : -1;
	int sy = (start_y < target_y) ? 1 : -1;
	int err = dx - dy;

	int curr_x = start_x;
	int curr_y = start_y;

	while (true)
	{
		if (curr_x == target_x && curr_y == target_y)
		{
			return true;
		}

		if (curr_x != start_x || curr_y != start_y)
		{
			char c = game_board.getCharAt(curr_x, curr_y);

			if (game_board.isWall(curr_x, curr_y) || c == '9' || c == '#')
			{
				return false;
			}
		}

		int e2 = 2 * err;
		bool step_x = (e2 > -dy);
		bool step_y = (e2 < dx);

		if (step_x && step_y)
		{
			int next_x = curr_x + sx;
			int next_y = curr_y + sy;

			char c1 = game_board.getCharAt(next_x, curr_y);
			char c2 = game_board.getCharAt(curr_x, next_y);

			bool b1 = (game_board.isWall(next_x, curr_y));
			bool b2 = (game_board.isWall(curr_x, next_y));

			if (b1 && b2)
			{
				return false;
			}
		}

		if (step_x)
		{
			err -= dy;
			curr_x += sx;
		}

		if (step_y)
		{
			err += dx;
			curr_y += sy;
		}
	}
}

void Game::updateBombs()
{
	for (size_t i = 0; i < bombs.size(); ++i)
	{
		if (bombs[i].isActive())
		{
			game_board.displayBombDroppedMessage(bombs[i].getTimer());
			bombs[i].tick();
			if (!is_silent)
			{
				sleep100();
			}

			if (bombs[i].getTimer() == 0)
			{
				game_board.displayGeneralMessage("Bomb exploded!");

				int bx = bombs[i].getX();
				int by = bombs[i].getY();

				if (!is_silent)
				{
					gotoxy(bx, by);
				}

				if (colors_enabled)
				{
					setTextColor(Color::RED);
				}

				if (!is_silent)
				{
					std::cout << '*';
				}

				if (colors_enabled)
				{
					setTextColor(Color::WHITE);
				}

				std::vector<Point> targets;

				for (int dy = -BOMB_EXPLOSION_RADIUS; dy <= BOMB_EXPLOSION_RADIUS; ++dy)
				{
					for (int dx = -BOMB_EXPLOSION_RADIUS; dx <= BOMB_EXPLOSION_RADIUS; ++dx)
					{
						int tx = bx + dx;
						int ty = by + dy;

						if (tx >= 0 && tx < BOARD_WIDTH && ty >= 0 && ty < BOARD_HEIGHT)
						{
							if (!isPathClear(bx, by, tx, ty))
							{
								continue;
							}

							targets.emplace_back(tx, ty);
						}
					}
				}

				for (const auto& target : targets)
				{
					int tx = target.getX();
					int ty = target.getY();

					char c = game_board.getCharAt(tx, ty);

					if ((c == 'W' || c == '?' || c == 'K' || c == '9' || c == '/' || c == '\\' || c == '@' ||
					     c == '*' || c == '+' || c == '>' || c == '<' || c == '!' || c == 'X') &&
					    !game_board.isInLegendArea(tx, ty))
					{
						game_board.setCharAt(tx, ty, ' ');
						if (!is_silent)
						{
							gotoxy(tx, ty);
							std::cout << ' ';
						}

						if (c == '9')
						{
							door.bomb_door();
							game_board.displayGeneralMessage("The door was destroyed");
						}

						if (c == 'K')
						{
							game_board.displayGeneralMessage("You destroyed a key");
						}

						if (c == '/' || c == '\\')
						{
							game_board.displayGeneralMessage("You destroyed a switch");
							switches.erase(std::remove_if(switches.begin(), switches.end(),
							                              [tx, ty](const Switch& game_switch)
							                              {
								                              return game_switch.getPosition().getX() == tx &&
								                                     game_switch.getPosition().getY() == ty;
							                              }),
							               switches.end());
						}

						if (c == '?')
						{
							game_board.displayGeneralMessage("You destroyed a riddle");
						}

						if (c == '>' || c == '<')
						{
							game_board.displayGeneralMessage("You destroyed a portal");
							portals.erase(std::remove_if(portals.begin(), portals.end(), [tx, ty](const Point& portal)
							                             { return portal.getX() == tx && portal.getY() == ty; }),
							              portals.end());
						}

						if (c == '!')
						{
							game_board.displayGeneralMessage("You destroyed a torch");
						}

						if (c == 'X')
						{
							game_board.displayGeneralMessage("You destroyed a golem! +" +
							                                 std::to_string(Game::POINTS_GOLEM_KILL) + " points!");
							addLevelPoints(Game::POINTS_GOLEM_KILL);

							for (size_t golem_index = 0; golem_index < golems.size(); ++golem_index)
							{
								if (golems[golem_index].getPosition().getX() == tx &&
								    golems[golem_index].getPosition().getY() == ty)
								{
									golems.erase(golems.begin() + golem_index);
									break;
								}
							}
						}
					}
					else if (c == '$' || c == '&')
					{
						if (tx == players[0].getPosition().getX() && ty == players[0].getPosition().getY())
						{
							damagePlayer(players[0], "a bomb");
							if (players[0].getIfDead())
							{
								game_board.printBoard();
							}
						}
						else if (tx == players[1].getPosition().getX() && ty == players[1].getPosition().getY())
						{
							damagePlayer(players[1], "a bomb");
							if (players[1].getIfDead())
							{
								game_board.printBoard();
							}
						}
					}
				}

				bombs[i].resetBomb();

				if (game_board.getCharAt(bx, by) != '$' && game_board.getCharAt(bx, by) != '&')
				{
					game_board.setCharAt(bx, by, ' ');
					if (!is_silent)
					{
						gotoxy(bx, by);
						std::cout << ' ';
					}
				}
			}
		}
	}
}

bool Game::checkPlayersStatus()
{
	if (players[0].getIfDead() && players[1].getIfDead())
	{
		game_board.displayGeneralMessage("Both players have died!");
		if (!is_silent)
		{
			sleep2000();
		}

		return false;
	}

	return true;
}

void Game::handleLevelCompletion(int& bonus_points)
{
	bool is_final_level = (current_level_index == levels.size() - 1);

	bonus_points = MAX_TIME_BONUS - (game_cycle_counter * TIME_PENALTY);
	if (bonus_points < 0)
	{
		bonus_points = 0;
	}

	game_points += bonus_points + level_points;

	if (!is_final_level)
	{
		logLevelTransition(current_level_index + 2);
		game_board.showLevelCompleteMessage();
		game_board.displayTimeBonus(bonus_points);
		game_board.displayLevelScore(level_points + bonus_points);
		game_board.displayCurrentGameScore(game_points);
		game_board.displayCurrentGameScore(game_points);

		if (!is_silent)
		{
			sleep2000();
		}

		waitForLevelTransitionInput();
	}
	else
	{
		logGameEnd(game_points);
	}

	current_level_index++;
}

Game::Game()
{
	struct _finddata_t screen_file;
	intptr_t search_handle = _findfirst("adv-world_*.screen", &screen_file);

	if (search_handle != -1L)
	{
		do
		{
			levels.push_back(screen_file.name);
		} while (_findnext(search_handle, &screen_file) == 0);

		_findclose(search_handle);
	}

	std::sort(levels.begin(), levels.end());

	std::string final_level = "finallevel.txt";

	std::ifstream f_final(final_level);
	if (f_final.is_open())
	{
		levels.push_back(final_level);
		f_final.close();
	}
}

char Game::showMenu()
{
	cls();

	std::ifstream file("menu.txt");

	std::vector<std::string> lines;
	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			if (line.find("[ C ]") != std::string::npos && colors_enabled)
			{
				size_t pos = line.find("O F F");

				if (pos != std::string::npos)
				{
					line.replace(pos, 5, "O N  ");
				}
			}

			lines.push_back(line);
		}

		file.close();

		for (size_t i = 0; i < lines.size(); ++i)
		{
			std::cout << lines[i];
			if (i < lines.size() - 1)
			{
				std::cout << std::endl;
			}
		}
	}
	else
	{
		std::cout << "1. Start Game" << std::endl;
		std::cout << "L. Load Game" << std::endl;
		std::cout << "8. Instructions" << std::endl;
		std::cout << "9. Exit" << std::endl;
	}

	hideCursor();
	char player_choice = _getch();

	return player_choice;
}

void Game::addLevelPoints(int points)
{
	level_points += points;
	game_board.displayPointsMessage(std::to_string(level_points));
}

void Game::damagePlayer(Player& player, const std::string& source)
{
	bool died = player.takeDamage();
	int player_id = player.getPlayerId();

	logHealthLoss(player_id);
	if (player_id == 1)
	{
		game_board.updatePlayer1HealthDisplay(player.getPlayerHealth());
	}
	else
	{
		game_board.updatePlayer2HealthDisplay(player.getPlayerHealth());
	}

	game_board.displayGeneralMessage("Player " + std::to_string(player_id) + " was hit by " + source + "!");

	if (died)
	{
		game_board.setCharAt(player.getPosition().getX(), player.getPosition().getY(), ' ');
		player.setPosition(-1, -1);
		game_board.displayGeneralMessage("Player " + std::to_string(player_id) + " has died!");
	}
}

void Game::startNewGame(bool resume)
{
	if (loaded_game)
	{
		resume = true;
		loaded_game = false;
	}

	int bonus_points = 0;

	if (!resume)
	{
		if (!resetGameSession())
		{
			game_board.displayGeneralMessage("Error loading game resources! Press any key...");
			_getch();

			return;
		}
	}

	while (current_level_index < levels.size())
	{
		int riddles_index_start = current_riddle_index;
		bool restart_level = false;

		if (!resume)
		{
			resetLevelScoring();
			if (!initLevel())
			{
				game_board.displayGeneralMessage("Error loading level! Press any key...");
				_getch();
				break;
			}
		}
		else
		{
			resume = false;
		}

		if (!is_silent)
		{
			cls();
		}

		game_board.printBoard();
		hideCursor();

		bool is_final_level = (current_level_index == levels.size() - 1);

		if (is_final_level)
		{
			game_board.displayEndScore(game_points);
		}

		bool level_complete = false;

		while (!level_complete)
		{
			if (!is_final_level)
			{
				bool all_players_finished = true;

				for (auto& p : players)
				{
					movePlayer(p);

					game_board.clearLightSources();
					for (const auto& player : players)
					{
						if (player.isHoldingTorch())
						{
							game_board.addLightSource(player.getPosition().getX(), player.getPosition().getY());
						}
					}

					p.updateVision();

					if (!p.isFinishedLevel())
					{
						all_players_finished = false;
					}
				}

				if (game_cycle_counter % 2 == 0)
				{
					for (auto& g : golems)
					{
						g.moveGolem(game_board, players, this);
					}
				}

				updateBombs();

				if (!checkPlayersStatus())
				{
					return;
				}

				if (all_players_finished)
				{
					level_complete = true;
					break;
				}
			}

			if (!processInput(restart_level))
			{
				if (restart_level)
				{
					break;
				}
				else
				{
					logGameEnd(game_points + level_points);

					return;
				}
			}

			if (!is_silent)
			{
				sleep();
			}

			game_cycle_counter++;
		}

		if (restart_level)
		{
			std::random_device rd;

			std::mt19937 g(rd());
			if (riddles_index_start < riddles.size())
			{
				std::shuffle(riddles.begin() + riddles_index_start, riddles.end(), g);
			}

			current_riddle_index = riddles_index_start;
			resetLevelScoring();
			continue;
		}
		else
		{
			handleLevelCompletion(bonus_points);
		}
	}
}

void Game::initSprings()
{
	springs.clear();

	bool processed[BOARD_HEIGHT][BOARD_WIDTH] = {false};

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			char c = game_board.getCharAt(x, y);

			if (c == '#' && !processed[y][x])
			{
				Spring new_spring;

				bool is_horizontal = (x + 1 < BOARD_WIDTH && game_board.getCharAt(x + 1, y) == '#');
				bool is_vertical = (y + 1 < BOARD_HEIGHT && game_board.getCharAt(x, y + 1) == '#');

				if (is_horizontal)
				{
					int temp_x = x;

					while (temp_x < BOARD_WIDTH && game_board.getCharAt(temp_x, y) == '#')
					{
						new_spring.addPart(temp_x, y);
						processed[y][temp_x] = true;
						temp_x++;
					}

					int last_x = temp_x - 1;

					if (x > 0 && game_board.isWall(x - 1, y))
					{
						new_spring.setDirection(Direction::RIGHT);
					}
					else if (last_x < BOARD_WIDTH - 1 && game_board.isWall(last_x + 1, y))
					{
						new_spring.setDirection(Direction::LEFT);
					}
				}
				else
				{
					int temp_y = y;

					while (temp_y < BOARD_HEIGHT && game_board.getCharAt(x, temp_y) == '#')
					{
						new_spring.addPart(x, temp_y);
						processed[temp_y][x] = true;
						temp_y++;
					}

					int last_y = temp_y - 1;

					if (y > 0 && game_board.isWall(x, y - 1))
					{
						new_spring.setDirection(Direction::DOWN);
					}
					else if (last_y < BOARD_HEIGHT - 1 && game_board.isWall(x, last_y + 1))
					{
						new_spring.setDirection(Direction::UP);
					}

					if (new_spring.getDirection() == Direction::STAY && !is_vertical)
					{
						if (x > 0 && game_board.isWall(x - 1, y))
						{
							new_spring.setDirection(Direction::RIGHT);
						}
						else if (x < BOARD_WIDTH - 1 && game_board.isWall(x + 1, y))
						{
							new_spring.setDirection(Direction::LEFT);
						}
					}
				}

				if (new_spring.getDirection() != Direction::STAY)
				{
					springs.push_back(new_spring);
				}
			}
		}
	}
}

void Game::initObstacles()
{
	obstacles.clear();

	bool processed[BOARD_HEIGHT][BOARD_WIDTH] = {false};

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			char c = game_board.getCharAt(x, y);

			if (c == '*' && !processed[y][x])
			{
				Obstacle new_obstacle;
				bool is_horizontal = (x + 1 < BOARD_WIDTH && game_board.getCharAt(x + 1, y) == '*');

				if (is_horizontal)
				{
					int temp_x = x;

					while (temp_x < BOARD_WIDTH && game_board.getCharAt(temp_x, y) == '*')
					{
						new_obstacle.addPart(temp_x, y);
						processed[y][temp_x] = true;
						temp_x++;
					}
				}
				else
				{
					int temp_y = y;

					while (temp_y < BOARD_HEIGHT && game_board.getCharAt(x, temp_y) == '*')
					{
						new_obstacle.addPart(x, temp_y);
						processed[temp_y][x] = true;
						temp_y++;
					}
				}

				obstacles.push_back(new_obstacle);
			}
		}
	}
}

bool Game::initLevel()
{
	if (!game_board.loadBoard(levels[current_level_index]))
	{
		std::cerr << "Error loading level: " << levels[current_level_index] << std::endl;

		return false;
	}

	for (size_t i = 0; i < riddles.size(); ++i)
	{
		riddles[i].setPosition(-1, -1);
	}

	keys.clear();
	switches.clear();
	bombs.clear();
	torches.clear();
	portals.clear();
	golems.clear();

	door.resetDoor();
	initSprings();
	initObstacles();

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			char c = game_board.getCharAt(x, y);

			if (c == '$')
			{
				if (players[0].getIfDead())
				{
					game_board.setCharAt(x, y, ' ');
				}
				else
				{
					players[0].setPosition(x, y);
					players[0].resetPlayer();
				}
			}
			else if (c == '&')
			{
				if (players[1].getIfDead())
				{
					game_board.setCharAt(x, y, ' ');
				}
				else
				{
					players[1].setPosition(x, y);
					players[1].resetPlayer();
				}
			}
			else if (c == 'K')
			{
				keys.emplace_back(x, y);
			}
			else if (c == '?')
			{
				if (current_riddle_index < riddles.size())
				{
					riddles[current_riddle_index].setPosition(x, y);
					riddles[current_riddle_index].setSolved(false);
					current_riddle_index++;
				}
			}
			else if (c == '\\')
			{
				switches.emplace_back(x, y);
			}
			else if (c == '@')
			{
				bombs.emplace_back(x, y);
			}
			else if (c == '!')
			{
				torches.emplace_back(x, y);
			}
			else if (c == 'X')
			{
				golems.emplace_back(x, y);
			}
			else if (c == '9')
			{
				door.setPosition(x, y);
			}
			else if (c == '>' || c == '<')
			{
				portals.emplace_back(x, y);
			}
		}
	}

	if (current_level_index != levels.size() - 1)
	{
		game_board.updatePlayer1HealthDisplay(players[0].getPlayerHealth());
		game_board.updatePlayer2HealthDisplay(players[1].getPlayerHealth());
	}

	return true;
}

void Game::showInstructions()
{
	std::ifstream file("instructions.txt");

	if (!file.is_open())
	{
		std::cerr << "Error: Could not open instructions.txt" << std::endl;
		std::cout << "Press any key to return to menu..." << std::endl;
		_getch();

		return;
	}

	std::vector<std::string> allLines;
	std::string line;

	while (std::getline(file, line))
	{
		allLines.push_back(line);
	}

	file.close();

	int currentPage = 1;
	const int linesPerPage = 20;
	int totalPages = static_cast<int>((allLines.size() + linesPerPage - 1) / linesPerPage);

	bool viewingInstructions = true;

	while (viewingInstructions)
	{
		cls();

		int startLine = (currentPage - 1) * linesPerPage;
		int endLine = startLine + linesPerPage;

		if (endLine > static_cast<int>(allLines.size()))
		{
			endLine = static_cast<int>(allLines.size());
		}

		for (int i = startLine; i < endLine; ++i)
		{
			std::cout << allLines[i] << std::endl;
		}

		std::cout << "\n========================================================" << std::endl;
		std::cout << " Page " << currentPage << "/" << totalPages << " | ";

		if (currentPage < totalPages)
		{
			std::cout << "[N] Next Page  ";
		}

		if (currentPage > 1)
		{
			std::cout << "[P] Previous Page  ";
		}

		std::cout << "[M] Main Menu" << std::endl;
		std::cout << "========================================================" << std::endl;

		char key = _getch();

		key = tolower(key);

		if (key == 'n' && currentPage < totalPages)
		{
			currentPage++;
		}
		else if (key == 'p' && currentPage > 1)
		{
			currentPage--;
		}
		else if (key == 'm')
		{
			viewingInstructions = false;
		}
	}

	cls();
}

bool Game::loadRiddlesFromFile(const std::string& filename)
{
	std::ifstream file(filename);
	riddles.clear();
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open riddles file: " << filename << std::endl;

		return false;
	}

	std::string lineQuestion;
	std::string lineAnswer;
	int index = 0;

	while (std::getline(file, lineQuestion) && std::getline(file, lineAnswer))
	{
		if (!lineQuestion.empty() && lineQuestion.back() == '\r')
		{
			lineQuestion.pop_back();
		}

		if (!lineAnswer.empty() && lineAnswer.back() == '\r')
		{
			lineAnswer.pop_back();
		}

		riddles.emplace_back();
		riddles.back().setQuestion(lineQuestion);
		riddles.back().setAnswer(lineAnswer);
		riddles.back().setSolved(false);
	}

	file.close();

	if (riddle_seed == 0)
	{
		std::random_device rd;

		riddle_seed = rd();
	}

	std::mt19937 g(riddle_seed);
	std::shuffle(riddles.begin(), riddles.end(), g);

	return true;
}

void Game::displayPauseMenu()
{
	cls();
	std::ifstream file("pause_menu.txt");

	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			std::cout << line << std::endl;
		}

		file.close();
	}
	else
	{
		std::cout << "GAME PAUSED\n\n[ESC] Resume\n[S] Save Game\n[R] Restart Level\n[H] Main Menu" << std::endl;
	}
}

void Game::saveGame(const std::string& filename)
{
	std::ofstream out(filename);
	if (!out.is_open())
	{
		return;
	}

	out << "SAVEv2" << " ";
	out << current_level_index << " ";
	out << game_points << " " << level_points << " " << game_cycle_counter << " ";
	out << riddle_seed << " ";
	out << current_riddle_index << " ";

	game_board.save(out);

	players[0].save(out);
	players[1].save(out);

	out << keys.size() << " ";
	for (const auto& k : keys)
	{
		k.save(out);
	}

	out << switches.size() << " ";
	for (const auto& s : switches)
	{
		s.save(out);
	}

	out << bombs.size() << " ";
	for (const auto& b : bombs)
	{
		b.save(out);
	}

	out << springs.size() << " ";
	for (const auto& s : springs)
	{
		s.save(out);
	}

	out << obstacles.size() << " ";
	for (const auto& o : obstacles)
	{
		o.save(out);
	}

	out << torches.size() << " ";
	for (const auto& t : torches)
	{
		t.save(out);
	}

	out << portals.size() << " ";
	for (const auto& p : portals)
	{
		out << p.getX() << " " << p.getY() << " ";
	}

	out << golems.size() << " ";
	for (const auto& g : golems)
	{
		g.save(out);
	}

	out << riddles.size() << " ";
	for (const auto& r : riddles)
	{
		r.save(out);
	}

	door.save(out);

	out.close();
	if (!is_silent)
	{
		cls();
		std::cout << "Game Saved Successfully!";
		sleep2000();
	}
}

bool Game::loadGame(const std::string& filename)
{
	std::ifstream in(filename);
	if (!in.is_open())
	{
		return false;
	}

	std::string magic;

	in >> magic;
	bool has_saved_portals = (magic == "SAVEv2");

	if (magic != "SAVEv1" && !has_saved_portals)
	{
		return false;
	}

	in >> current_level_index;
	in >> game_points >> level_points >> game_cycle_counter;
	in >> riddle_seed;
	in >> current_riddle_index;

	game_board.load(in);
	game_board.setColorsEnabled(colors_enabled);

	players[0].load(in);
	players[0].getPosition().setDirection(Direction::STAY);
	players[1].load(in);
	players[1].getPosition().setDirection(Direction::STAY);

	size_t size;

	in >> size;
	keys.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Key k;
		k.load(in);
		keys.push_back(k);
	}

	in >> size;
	switches.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Switch s;
		s.load(in);
		switches.push_back(s);
	}

	in >> size;
	bombs.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Bomb b;
		b.load(in);
		bombs.push_back(b);
	}

	in >> size;
	springs.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Spring s;
		s.load(in);
		springs.push_back(s);
	}

	in >> size;
	obstacles.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Obstacle o;
		o.load(in);
		obstacles.push_back(o);
	}

	in >> size;
	torches.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Torch t;
		t.load(in);
		torches.push_back(t);
	}

	portals.clear();
	if (has_saved_portals)
	{
		in >> size;
		for (size_t i = 0; i < size; ++i)
		{
			int x, y;
			in >> x >> y;
			portals.emplace_back(x, y);
		}
	}
	else
	{
		for (int y = 0; y < BOARD_HEIGHT; ++y)
		{
			for (int x = 0; x < BOARD_WIDTH; ++x)
			{
				if (game_board.isPortal(x, y))
				{
					portals.emplace_back(x, y);
				}
			}
		}
	}

	in >> size;
	golems.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Golem g;
		g.load(in);
		golems.push_back(g);
	}

	in >> size;
	riddles.clear();
	for (size_t i = 0; i < size; ++i)
	{
		Riddle r;
		r.load(in);
		riddles.push_back(r);
	}

	door.load(in);

	players[0].restoreObjectReferences();
	players[1].restoreObjectReferences();

	in.close();

	players[0].setTeamate(&players[1]);
	players[1].setTeamate(&players[0]);
	if (players[0].isHoldingTorch())
	{
		game_board.addLightSource(players[0].getPosition().getX(), players[0].getPosition().getY());
	}

	if (players[1].isHoldingTorch())
	{
		game_board.addLightSource(players[1].getPosition().getX(), players[1].getPosition().getY());
	}

	loaded_game = true;

	return true;
}

std::vector<std::string> Game::getSaveFileList()
{
	std::vector<std::string> files;
	struct _finddata_t c_file;
	intptr_t hFile;

	if ((hFile = _findfirst("*.save", &c_file)) != -1L)
	{
		do
		{
			std::string name = c_file.name;
			size_t lastindex = name.find_last_of(".");

			if (lastindex != std::string::npos)
			{
				name = name.substr(0, lastindex);
			}

			files.push_back(name);
		} while (_findnext(hFile, &c_file) == 0);
		_findclose(hFile);
	}

	return files;
}

bool Game::showLoadGameMenu()
{
	std::vector<std::string> saves = getSaveFileList();
	if (saves.empty())
	{
		cls();
		std::cout << "No saved games found! Press any key to return.";
		_getch();

		return false;
	}

	int choice = 0;

	while (true)
	{
		cls();
		std::cout << "Select a game to load:\n\n";

		for (size_t i = 0; i < saves.size(); ++i)
		{
			if (i == choice)
			{
				std::cout << " > ";
			}
			else
			{
				std::cout << "   ";
			}

			std::cout << saves[i] << "\n";
		}

		std::cout << "\n[Enter] Load  [Esc] Back";

		char c = _getch();

		if (c == 27)
		{
			return false;
		}

		if (c == 13)
		{
			if (loadGame(saves[choice] + ".save"))
			{
				return true;
			}
			else
			{
				cls();
				std::cout << "Failed to load game!";
				_getch();
			}
		}

		if (c == 72)
		{
			if (choice > 0)
			{
				choice--;
			}
		}

		if (c == 80)
		{
			if (choice < saves.size() - 1)
			{
				choice++;
			}
		}
	}
}
