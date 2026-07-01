#include "Player.h"
#include "Board.h"
#include "Door.h"
#include "Game.h"
#include "Point.h"
#include "Utils.h"
#include <cctype>
#include <conio.h>

using namespace std;

void Player::drawAtCurrentPosition(char c)
{
	board.drawChar(getPosition().getX(), getPosition().getY(), c);
}

void Player::receiveSpringPush(Spring* spring, int speed, int force_left)
{
	this->active_spring = spring;
	this->current_speed = speed;
	this->spring_force_moves_left = force_left;
	this->is_launching = true;
	this->is_spring_releasing = false;
	this->spring_first_move = false;

	if (active_spring)
	{
		getPosition().setDirection(active_spring->getDirection());
	}
}

void Player::stopSpring()
{
	is_launching = false;
	spring_force_moves_left = 0;
	current_speed = 1;
	active_spring = nullptr;
	spring_first_move = true;
	is_compressing = false;
	is_spring_releasing = false;
	chars_compressed = 0;
	is_opposite_direction = false;
	is_waiting_on_spring = false;
}

void Player::handleButtonPress(char button_pressed)
{
	if (health == 0)
	{
		return;
	}

	Direction spring_dir = Direction::STAY;

	if (is_launching && active_spring)
	{
		spring_dir = active_spring->getDirection();
	}

	size_t i;

	for (i = 0; i < NUM_BUTTONS - 1; i++)
	{
		if (tolower(buttons[i]) == tolower(button_pressed))
		{
			if (is_spring_releasing)
			{
				return;
			}

			if (is_compressing)
			{
				if (is_waiting_on_spring && i <= KEY_STAY)
				{
					is_waiting_on_spring = false;
				}

				return;
			}

			if (is_launching)
			{
				if (i == KEY_STAY)
				{
					return;
				}

				if ((spring_dir == Direction::UP && i == KEY_DOWN) || (spring_dir == Direction::DOWN && i == KEY_UP) ||
				    (spring_dir == Direction::RIGHT && i == KEY_LEFT) ||
				    (spring_dir == Direction::LEFT && i == KEY_RIGHT))
				{
					return;
				}
			}

			switch (i)
			{
			case KEY_UP:
				getPosition().setDirection(Direction::UP);
				break;
			case KEY_DOWN:
				getPosition().setDirection(Direction::DOWN);
				break;
			case KEY_LEFT:
				getPosition().setDirection(Direction::LEFT);
				break;
			case KEY_RIGHT:
				getPosition().setDirection(Direction::RIGHT);
				break;
			case KEY_STAY:
				getPosition().setDirection(Direction::STAY);
				break;
			case KEY_DROP:
				if (holding_item != Inventory::NONE)
				{
					if (holding_item == Inventory::KEY || holding_item == Inventory::TORCH)
					{
						dropKeyOrTorch();
					}
					else if (holding_item == Inventory::BOMB)
					{
						dropBomb();
					}
				}

				break;
			}
		}
	}
}

void Player::pickUpKey(int& new_x, int& new_y)
{
	if (holding_item == Inventory::NONE)
	{
		board.setCharAt(new_x, new_y, ' ');
		for (size_t i = 0; i < keys.size(); ++i)
		{
			if (new_x == keys[i].getX() && new_y == keys[i].getY())
			{
				keys[i].collect();
				held_key_index = static_cast<int>(i);
				break;
			}
		}

		holding_item = Inventory::KEY;
		board.updateHeldItemAndRedraw(player_id, "Key");
	}
	else
	{
		getPosition().setDirection(Direction::STAY);
		new_x = getPosition().getX();
		new_y = getPosition().getY();
		board.displayGeneralMessage("P" + std::to_string(player_id) + ": already holding an item");
	}
}

void Player::pickUpTorch(int& new_x, int& new_y)
{
	if (holding_item == Inventory::NONE)
	{
		board.setCharAt(new_x, new_y, ' ');
		for (size_t i = 0; i < torches.size(); ++i)
		{
			if (new_x == torches[i].getX() && new_y == torches[i].getY())
			{
				torches[i].collect();
				held_torch_index = static_cast<int>(i);
				break;
			}
		}

		holding_item = Inventory::TORCH;
		board.updateHeldItemAndRedraw(player_id, "Torch");
	}
	else
	{
		getPosition().setDirection(Direction::STAY);
		new_x = getPosition().getX();
		new_y = getPosition().getY();
		board.displayGeneralMessage("P" + std::to_string(player_id) + ": already holding an item");
	}
}

void Player::pickUpBomb(int& new_x, int& new_y)
{
	if (holding_item == Inventory::NONE)
	{
		board.setCharAt(new_x, new_y, ' ');

		for (size_t i = 0; i < bombs.size(); ++i)
		{
			if (new_x == bombs[i].getX() && new_y == bombs[i].getY())
			{
				bombs[i].collect();
				held_bomb_index = static_cast<int>(i);
				if (bombs[i].getTimer() < 5)
				{
					board.displayGeneralMessage("You picked up a bomb with timer: " +
					                            std::to_string(bombs[i].getTimer()));
				}
			}
		}

		holding_item = Inventory::BOMB;
		board.updateHeldItemAndRedraw(player_id, "Bomb");
	}
	else
	{
		getPosition().setDirection(Direction::STAY);
		new_x = getPosition().getX();
		new_y = getPosition().getY();
		board.displayGeneralMessage("P" + std::to_string(player_id) + ": already holding an item");
	}
}

void Player::moveObstacle(int& new_x, int& new_y)
{
	for (size_t i = 0; i < obstacles.size(); ++i)
	{
		if (obstacles[i].isPartOfObstacle(new_x, new_y))
		{
			active_obstacle = &obstacles[i];
			break;
		}
	}

	int obstacle_size = active_obstacle->getSize();
	bool is_allowed_to_move = true;

	int effective_force = current_speed;

	if (obstacle_size == 2 && effective_force == 1 && teamate != nullptr)
	{
		if (teamate->getPosition().getDirection() == getPosition().getDirection())
		{
			int dx = getPosition().getDiffX();
			int dy = getPosition().getDiffY();

			int behind_x = getPosition().getX() - dx;
			int behind_y = getPosition().getY() - dy;

			if (teamate->getPosition().getX() == behind_x && teamate->getPosition().getY() == behind_y)
			{
				effective_force = 2;
			}
		}
	}

	if (effective_force >= obstacle_size)
	{
		int dx = getPosition().getDiffX();
		int dy = getPosition().getDiffY();

		for (auto& part : active_obstacle->getParts())
		{
			int target_x = part.getX() + dx;
			int target_y = part.getY() + dy;

			char char_at_target = board.getCharAt(target_x, target_y);

			if (char_at_target != ' ')
			{
				if (!active_obstacle->isPartOfObstacle(target_x, target_y))
				{
					is_allowed_to_move = false;
					break;
				}
			}
		}
	}
	else
	{
		is_allowed_to_move = false;
	}

	if (is_allowed_to_move)
	{
		for (auto& part : active_obstacle->getParts())
		{
			int old_x = part.getX();
			int old_y = part.getY();

			board.setCharAt(old_x, old_y, ' ');
			gotoxy(old_x, old_y);
			cout << ' ';
		}

		int dx = getPosition().getDiffX();
		int dy = getPosition().getDiffY();

		for (auto& part : active_obstacle->getParts())
		{
			part.setX(part.getX() + dx);
			part.setY(part.getY() + dy);
		}

		for (auto& part : active_obstacle->getParts())
		{
			int new_x = part.getX();
			int new_y = part.getY();

			board.setCharAt(new_x, new_y, '*');
			gotoxy(new_x, new_y);
			cout << '*';
		}
	}
	else
	{
		getPosition().setDirection(Direction::STAY);
		new_x = getPosition().getX();
		new_y = getPosition().getY();
		is_launching = false;
		active_spring = nullptr;
		current_speed = 1;
		spring_force_moves_left = 0;
	}
}

void Player::handleSpring(int& new_x, int& new_y)
{
	if (!is_launching && !is_compressing)
	{
		for (size_t i = 0; i < springs.size(); ++i)
		{
			if (springs[i].isPartOfSpring(new_x, new_y))
			{
				active_spring = &springs[i];
				break;
			}
		}

		Direction spring_dir = active_spring->getDirection();

		if (getPosition().getDirection() == Direction::UP && spring_dir == Direction::DOWN)
		{
			is_opposite_direction = true;
		}
		else if (getPosition().getDirection() == Direction::DOWN && spring_dir == Direction::UP)
		{
			is_opposite_direction = true;
		}
		else if (getPosition().getDirection() == Direction::LEFT && spring_dir == Direction::RIGHT)
		{
			is_opposite_direction = true;
		}
		else if (getPosition().getDirection() == Direction::RIGHT && spring_dir == Direction::LEFT)
		{
			is_opposite_direction = true;
		}
	}

	if (is_opposite_direction)
	{
		is_compressing = true;
		int length = active_spring->getFullLength();
		int distance = length * length;

		if (chars_compressed < length)
		{
			if (chars_compressed == 0)
			{
				is_waiting_on_spring = true;
			}

			chars_compressed++;

			if (spring_first_move)
			{
				board.setCharAt(getPosition().getX(), getPosition().getY(), ' ');
				spring_first_move = false;
			}
			else
			{
				board.setCharAt(getPosition().getX(), getPosition().getY(), ' ');
			}

			drawAtCurrentPosition(' ');
			getPosition().setX(new_x);
			getPosition().setY(new_y);
			drawAtCurrentPosition(getDisplayChar());
		}
		else
		{
			if (is_waiting_on_spring)
			{
				return;
			}

			is_compressing = false;
			is_launching = true;
			is_spring_releasing = true;
			is_opposite_direction = false;
			spring_force_moves_left = distance;
			current_speed = length;
			getPosition().setDirection(active_spring->getDirection());
		}
	}

	if (is_launching)
	{
		if (is_spring_releasing)
		{
			if (chars_compressed == 0)
			{
				is_spring_releasing = false;
				board.setCharAt(getPosition().getX(), getPosition().getY(), getDisplayChar());
				drawAtCurrentPosition(getDisplayChar());
			}
			else
			{
				board.setCharAt(getPosition().getX(), getPosition().getY(), '#');
				drawAtCurrentPosition('#');
				getPosition().setX(getPosition().getX() + getPosition().getDiffX());
				getPosition().setY(getPosition().getY() + getPosition().getDiffY());
				drawAtCurrentPosition(getDisplayChar());
				chars_compressed--;
			}
		}

		else
		{
			if (active_spring)
			{
				active_spring->resetSpring();
			}

			if (spring_force_moves_left > 0)
			{
				spring_force_moves_left--;
			}

			for (int i = 0; i < current_speed; ++i)
			{
				board.setCharAt(getPosition().getX(), getPosition().getY(), ' ');
				drawAtCurrentPosition(' ');

				int dx = 0;
				int dy = 0;

				Direction dir = getPosition().getDirection();

				if (active_spring != nullptr)
				{
					dir = active_spring->getDirection();
				}

				switch (dir)
				{
				case Direction::UP:
					dy = -1;
					break;
				case Direction::DOWN:
					dy = 1;
					break;
				case Direction::LEFT:
					dx = -1;
					break;
				case Direction::RIGHT:
					dx = 1;
					break;
				default:
					break;
				}

				int next_x = getPosition().getX() + dx;
				int next_y = getPosition().getY() + dy;

				if (holding_item == Inventory::NONE)
				{
					if (board.isKey(next_x, next_y))
					{
						pickUpKey(next_x, next_y);
					}
					else if (board.isTorch(next_x, next_y))
					{
						pickUpTorch(next_x, next_y);
					}
					else if (board.isBomb(next_x, next_y))
					{
						pickUpBomb(next_x, next_y);
					}
				}

				if (board.isPlayer(next_x, next_y))
				{
					int beyond_x = next_x + dx;
					int beyond_y = next_y + dy;
					bool blocked = false;

					if (board.isWall(beyond_x, beyond_y) || (board.isDoor(beyond_x, beyond_y) && !door.getIsOpen()) ||
					    board.isSpring(beyond_x, beyond_y))
					{
						blocked = true;
					}
					else if (board.isObstacle(beyond_x, beyond_y))
					{
						Obstacle* obs_ptr = nullptr;
						for (auto& obs : obstacles)
						{
							if (obs.isPartOfObstacle(beyond_x, beyond_y))
							{
								obs_ptr = &obs;
								break;
							}
						}

						if (obs_ptr)
						{
							for (const auto& part : obs_ptr->getParts())
							{
								int p_next_x = part.getX() + dx;
								int p_next_y = part.getY() + dy;

								if (board.getCharAt(p_next_x, p_next_y) != ' ' &&
								    !obs_ptr->isPartOfObstacle(p_next_x, p_next_y))
								{
									blocked = true;
									break;
								}
							}
						}
					}

					if (blocked)
					{
						this->stopSpring();

						board.setCharAt(getPosition().getX(), getPosition().getY(), getDisplayChar());
						drawAtCurrentPosition(getDisplayChar());

						if (teamate != nullptr)
						{
							teamate->stopSpring();
						}
					}
					else if (teamate != nullptr)
					{
						teamate->receiveSpringPush(active_spring, current_speed, spring_force_moves_left);
					}

					break;
				}

				if (holding_item != Inventory::NONE &&
				    (board.isKey(next_x, next_y) || board.isTorch(next_x, next_y) || board.isBomb(next_x, next_y)))
				{
					board.displayGeneralMessage("P" + std::to_string(player_id) + ": already holding an item");
					this->stopSpring();

					board.setCharAt(getPosition().getX(), getPosition().getY(), getDisplayChar());
					drawAtCurrentPosition(getDisplayChar());

					if (teamate != nullptr)
					{
						teamate->stopSpring();
					}

					break;
				}

				if (board.isWall(next_x, next_y) || (board.isDoor(next_x, next_y) && !door.getIsOpen()) ||
				    board.isSpring(next_x, next_y))
				{
					is_launching = false;
					spring_force_moves_left = 0;
					current_speed = 1;
					active_spring = nullptr;
					spring_first_move = true;

					board.setCharAt(getPosition().getX(), getPosition().getY(), getDisplayChar());
					drawAtCurrentPosition(getDisplayChar());
					break;
				}

				if (board.isObstacle(next_x, next_y))
				{
					Direction original_dir = getPosition().getDirection();

					if (active_spring)
					{
						getPosition().setDirection(active_spring->getDirection());
					}

					moveObstacle(next_x, next_y);
					getPosition().setDirection(original_dir);
				}

				board.setCharAt(getPosition().getX(), getPosition().getY(), ' ');
				drawAtCurrentPosition(' ');

				getPosition().setX(next_x);
				getPosition().setY(next_y);

				drawAtCurrentPosition(getDisplayChar());
				board.setCharAt(next_x, next_y, getDisplayChar());
			}
		}

		Direction user_dir = getPosition().getDirection();
		Direction spring_dir = active_spring ? active_spring->getDirection() : Direction::STAY;

		bool is_lateral = false;

		if ((spring_dir == Direction::UP || spring_dir == Direction::DOWN) &&
		    (user_dir == Direction::LEFT || user_dir == Direction::RIGHT))
		{
			is_lateral = true;
		}

		if ((spring_dir == Direction::LEFT || spring_dir == Direction::RIGHT) &&
		    (user_dir == Direction::UP || user_dir == Direction::DOWN))
		{
			is_lateral = true;
		}

		if (is_launching && is_lateral)
		{
			int lat_dx = 0, lat_dy = 0;

			switch (user_dir)
			{
			case Direction::UP:
				lat_dy = -1;
				break;
			case Direction::DOWN:
				lat_dy = 1;
				break;
			case Direction::LEFT:
				lat_dx = -1;
				break;
			case Direction::RIGHT:
				lat_dx = 1;
				break;
			default:
				break;
			}

			int lat_x = getPosition().getX() + lat_dx;
			int lat_y = getPosition().getY() + lat_dy;

			if (holding_item == Inventory::NONE)
			{
				if (board.isKey(lat_x, lat_y))
				{
					pickUpKey(lat_x, lat_y);
				}
				else if (board.isTorch(lat_x, lat_y))
				{
					pickUpTorch(lat_x, lat_y);
				}
				else if (board.isBomb(lat_x, lat_y))
				{
					pickUpBomb(lat_x, lat_y);
				}
			}

			bool lateral_blocked_by_item = false;

			if (holding_item != Inventory::NONE &&
			    (board.isKey(lat_x, lat_y) || board.isTorch(lat_x, lat_y) || board.isBomb(lat_x, lat_y)))
			{
				board.displayGeneralMessage("P" + std::to_string(player_id) + ": already holding an item");
				lateral_blocked_by_item = true;
			}

			if (!lateral_blocked_by_item && !board.isWall(lat_x, lat_y) && !board.isObstacle(lat_x, lat_y) &&
			    (!board.isDoor(lat_x, lat_y) || door.getIsOpen()) && !board.isSpring(lat_x, lat_y))
			{
				board.setCharAt(getPosition().getX(), getPosition().getY(), ' ');
				drawAtCurrentPosition(' ');

				getPosition().setX(lat_x);
				getPosition().setY(lat_y);

				board.setCharAt(lat_x, lat_y, getDisplayChar());
				drawAtCurrentPosition(getDisplayChar());
			}
			else
			{
				stopSpring();
			}
		}

		if (spring_force_moves_left == 0)
		{
			is_launching = false;
			current_speed = 1;
			active_spring = nullptr;
			spring_first_move = true;
		}
	}
}

void Player::dropKeyOrTorch()
{
	if (holding_item != Inventory::NONE)
	{
		Point drop_point = getDropPointForItem();
		int drop_x = drop_point.getX();
		int drop_y = drop_point.getY();

		if (board.getCharAt(drop_x, drop_y) != ' ')
		{
			return;
		}

		if (holding_item == Inventory::KEY)
		{
			keys[held_key_index].drop(drop_x, drop_y);
			board.setCharAt(drop_x, drop_y, 'K');
			keys[held_key_index].draw(board);
			held_key_index = -1;
		}

		if (holding_item == Inventory::TORCH)
		{
			torches[held_torch_index].drop(drop_x, drop_y);
			board.setCharAt(drop_x, drop_y, '!');
			torches[held_torch_index].draw(board);
			held_torch_index = -1;
		}

		holding_item = Inventory::NONE;
		board.updateHeldItemAndRedraw(player_id, "None");
	}
}

void Player::dropBomb()
{
	if (holding_item == Inventory::BOMB)
	{
		bombs[held_bomb_index].drop(getPosition().getX(), getPosition().getY());
		holding_item = Inventory::NONE;
		board.updateHeldItemAndRedraw(player_id, "None");
		held_bomb_index = -1;
	}
}

Point Player::getDropPointForItem() const
{
	int px = getPosition().getX();
	int py = getPosition().getY();

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			int drop_x = px + dx;
			int drop_y = py + dy;

			if (board.getCharAt(drop_x, drop_y) == ' ')
			{
				return Point(drop_x, drop_y);
			}
		}
	}

	return Point(px, py);
}

void Player::resetPlayer()
{
	finished_level = false;
	getPosition().setDirection(Direction::STAY);
	holding_item = Inventory::NONE;

	is_compressing = false;
	is_launching = false;
	is_spring_releasing = false;
	chars_compressed = 0;
	active_spring = nullptr;
	current_speed = 1;
	spring_force_moves_left = 0;
	spring_first_move = true;
	is_opposite_direction = false;
	is_waiting_on_spring = false;

	active_obstacle = nullptr;
}

bool Player::takeDamage()
{
	if (health > 0)
	{
		health--;
	}

	if (health == 0)
	{
		is_dead = true;
	}

	return is_dead;
}

void Player::updateVision()
{
	int current_radius = isHoldingTorch() ? 4 : 0;

	int refresh_range = current_radius + 3;

	int px = getPosition().getX();
	int py = getPosition().getY();

	for (int i = py - refresh_range; i <= py + refresh_range; ++i)
	{
		for (int j = px - refresh_range; j <= px + refresh_range; ++j)
		{
			if (i >= 0 && i < BOARD_HEIGHT && j >= 0 && j < BOARD_WIDTH)
			{
				if (i == BOARD_HEIGHT - 1 && j == BOARD_WIDTH - 1)
				{
					continue;
				}

				gotoxy(j, i);

				if (board.isCellVisible(j, i))
				{
					if (!is_compressing && !is_spring_releasing)
					{
						board.drawChar(j, i, board.getCharAt(j, i));
					}
				}
				else
				{
					board.drawChar(j, i, ' ');
				}
			}
		}
	}

	if (!finished_level && !is_dead)
	{
		board.drawChar(getPosition().getX(), getPosition().getY(), getDisplayChar());
	}
}

void Player::save(std::ostream& out) const
{
	GameObject::save(out);
	out << player_id << " " << (int)holding_item << " " << health << " " << is_dead << " ";
	out << held_key_index << " " << held_bomb_index << " " << held_torch_index << " ";
	out << finished_level << " " << isHoldingTorch() << " ";

	out << is_compressing << " " << is_launching << " " << is_spring_releasing << " ";
	out << chars_compressed << " " << current_speed << " " << spring_force_moves_left << " ";
	out << spring_first_move << " " << is_opposite_direction << " ";

	int spring_index = -1;

	if (active_spring)
	{
		for (size_t i = 0; i < springs.size(); ++i)
		{
			if (&springs[i] == active_spring)
			{
				spring_index = (int)i;
				break;
			}
		}
	}

	out << spring_index << " ";

	int obstacle_index = -1;

	if (active_obstacle)
	{
		for (size_t i = 0; i < obstacles.size(); ++i)
		{
			if (&obstacles[i] == active_obstacle)
			{
				obstacle_index = (int)i;
				break;
			}
		}
	}

	out << obstacle_index << " ";
}

void Player::load(std::istream& in)
{
	GameObject::load(in);
	int val;

	in >> player_id >> val >> health >> is_dead;
	holding_item = (Inventory)val;
	in >> held_key_index >> held_bomb_index >> held_torch_index;
	bool saved_torch_state;

	in >> finished_level >> saved_torch_state;

	in >> is_compressing >> is_launching >> is_spring_releasing;
	in >> chars_compressed >> current_speed >> spring_force_moves_left;
	in >> spring_first_move >> is_opposite_direction;

	int spring_index;

	in >> spring_index;
	loaded_spring_index = spring_index;
	active_spring = nullptr;

	int obstacle_index;

	in >> obstacle_index;
	loaded_obstacle_index = obstacle_index;
	active_obstacle = nullptr;
}

void Player::restoreObjectReferences()
{
	if (loaded_spring_index >= 0 && loaded_spring_index < (int)springs.size())
	{
		active_spring = &springs[loaded_spring_index];
	}

	if (loaded_obstacle_index >= 0 && loaded_obstacle_index < (int)obstacles.size())
	{
		active_obstacle = &obstacles[loaded_obstacle_index];
	}

	loaded_spring_index = -1;
	loaded_obstacle_index = -1;
}
