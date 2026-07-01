#include "GameByKeys.h"
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <random>

GameByKeys::GameByKeys(bool save_mode) : is_recording(save_mode)
{
	if (is_recording)
	{
		record_file.open("adv-world.steps");
		if (record_file.is_open())
		{
			seed = (unsigned int)time(0);
			srand(seed);

			std::random_device rd;

			riddle_seed = rd();

			record_file << "Seed " << seed << std::endl;
			record_file << "RiddleSeed " << riddle_seed << std::endl;

			result_file.open("adv-world.result");
		}
	}
	else
	{
		srand((unsigned int)time(0));
	}
}

void GameByKeys::run()
{
	bool running = true;

	while (running)
	{
		char player_choice = showMenu();

		player_choice = tolower(player_choice);

		switch (player_choice)
		{
		case '1':
			if (is_recording)
			{
				if (record_file.is_open())
				{
					record_file.close();
				}

				record_file.open("adv-world.steps", std::ofstream::out | std::ofstream::trunc);
				if (record_file.is_open())
				{
					seed = (unsigned int)time(0);
					srand(seed);

					std::random_device rd;

					riddle_seed = rd();

					record_file << "Seed " << seed << std::endl;
					record_file << "RiddleSeed " << riddle_seed << std::endl;
					record_file << "Colors " << (colors_enabled ? "1" : "0") << std::endl;

					if (result_file.is_open())
					{
						result_file.close();
					}

					result_file.open("adv-world.result", std::ofstream::out | std::ofstream::trunc);
				}
			}

			startNewGame();
			break;
		case 'l':
			if (showLoadGameMenu())
			{
				if (is_recording)
				{
					if (record_file.is_open())
					{
						record_file.close();
					}

					if (result_file.is_open())
					{
						result_file.close();
					}
				}

				startNewGame(true);
			}

			break;
		case '8':
			showInstructions();
			break;
		case '9':
			running = false;
			break;
		case 'c':
			toggleColors();
			break;

		default:
			break;
		}
	}
}

bool GameByKeys::processInput(bool& restart_level)
{
	if (_kbhit())
	{
		char key = std::tolower(_getch());
		bool is_final_level = (current_level_index == levels.size() - 1);

		if (key == 27)
		{
			if (is_final_level)
			{
				if (is_recording && record_file.is_open())
				{
					record_file << "Cycle " << game_cycle_counter << " Key " << 27 << std::endl;
				}

				cls();

				return false;
			}

			displayPauseMenu();

			bool resume_game = false;

			if (is_recording && record_file.is_open())
			{
				record_file << "Cycle " << game_cycle_counter << " Key " << 27 << std::endl;
			}

			while (!resume_game)
			{
				if (_kbhit())
				{
					key = _getch();
					if (is_recording && record_file.is_open())
					{
						int record_key = key;

						if (key == 27)
						{
							record_key = 27;
						}

						record_file << "Cycle " << game_cycle_counter << " Key " << record_key << std::endl;
					}

					if (key == 27)
					{
						cls();
						game_board.printBoard();
						resume_game = true;
					}
					else if (key == 'h')
					{
						cls();

						return false;
					}
					else if (key == 'r')
					{
						restart_level = true;
						resume_game = true;
					}
					else if (key == 's')
					{
						cls();
						std::cout << "Enter save game name: ";
						std::string saveName;

						std::getline(std::cin, saveName);
						if (!saveName.empty())
						{
							std::string filename = saveName + ".save";
							bool save_game = true;

							std::ifstream f(filename);
							if (f.is_open())
							{
								f.close();
								std::cout << "File already exists. Overwrite? (y/n): ";
								while (true)
								{
									char confirm = std::tolower(_getch());

									if (confirm == 'y')
									{
										save_game = true;
										break;
									}
									else if (confirm == 'n')
									{
										save_game = false;
										break;
									}
								}
							}

							if (save_game)
							{
								saveGame(filename);
							}
						}

						cls();
						displayPauseMenu();
					}
				}
			}

			if (restart_level)
			{
				return false;
			}
		}
		else
		{
			if (!is_final_level)
			{
				bool moved = false;

				for (int i = 0; i < 2; ++i)
				{
					if (players[i].getIfDead())
					{
						continue;
					}

					players[i].handleButtonPress(key);
				}

				if (is_recording && record_file.is_open())
				{
					record_file << "Cycle " << game_cycle_counter << " Key " << (int)key << std::endl;
				}
			}
		}
	}

	return true;
}

std::string GameByKeys::getRiddleInput()
{
	std::string answer;

	std::getline(std::cin, answer);
	if (!answer.empty() && answer.back() == '\r')
	{
		answer.pop_back();
	}

	if (is_recording && record_file.is_open())
	{
		record_file << "Cycle " << game_cycle_counter << " Input " << answer << std::endl;
	}

	return answer;
}

void GameByKeys::waitForLevelTransitionInput()
{
	while (true)
	{
		if (_kbhit())
		{
			char key = _getch();

			if (key == ' ')
			{
				if (is_recording && record_file.is_open())
				{
					record_file << "Cycle " << game_cycle_counter << " Key " << (int)' ' << std::endl;
				}

				break;
			}
		}
	}
}

void GameByKeys::logHealthLoss(int player_id)
{
	if (result_file.is_open())
	{
		result_file << game_cycle_counter << " Player " << player_id << " lost a life" << std::endl;
	}
}

void GameByKeys::logRiddle(const std::string& question, const std::string& answer, bool correct)
{
	if (result_file.is_open())
	{
		result_file << game_cycle_counter << " Riddle: " << question << " Answer: " << answer
		            << " Result: " << (correct ? "Correct" : "Wrong") << std::endl;
	}
}

void GameByKeys::logGameEnd(int score)
{
	if (result_file.is_open())
	{
		result_file << game_cycle_counter << " Game finished. Score: " << score << std::endl;
	}
}

void GameByKeys::logLevelTransition(int next_level_number)
{
	if (result_file.is_open())
	{
		result_file << game_cycle_counter << " Moved to next room. Level: " << next_level_number << std::endl;
	}
}
