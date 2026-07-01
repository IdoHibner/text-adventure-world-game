#include "GameByFile.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <windows.h>

void GameByFile::checkResult(const std::string& observed_description)
{
	if (current_result_index < expected_results.size())
	{
		if (results_consistent && expected_results[current_result_index].description != observed_description)
		{
			results_consistent = false;
			failure_reason = "Mismatch at result index " + std::to_string(current_result_index) + ": Expected '" +
			                 expected_results[current_result_index].description + "', Observed '" +
			                 observed_description + "'";
		}

		current_result_index++;
	}
	else
	{
		if (results_consistent)
		{
			results_consistent = false;
			failure_reason = "Unexpected result observed: " + observed_description;
		}
	}
}

GameByFile::GameByFile(const std::string& filename, bool silent) : current_action_index(0)
{
	if (silent)
	{
		setSilent(true);
	}

	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open recording file: " << filename << std::endl;

		return;
	}

	std::string temp;
	unsigned int seed = 0;

	if (file >> temp && temp == "Seed")
	{
		file >> seed;
		srand(seed);
	}

	if (file >> temp && temp == "RiddleSeed")
	{
		file >> riddle_seed;
	}

	if (file >> temp && temp == "Colors")
	{
		int c = 0;

		file >> c;
		colors_enabled = (c == 1);
		game_board.setColorsEnabled(colors_enabled);
	}

	RecordedAction action;

	while (file >> temp)
	{
		if (temp == "Cycle")
		{
			file >> action.cycle;
			file >> temp;
			if (temp == "Key")
			{
				int key_int;

				file >> key_int;
				action.key = (char)key_int;
				action.type = ActionType::KEY;
				actions.push_back(action);
			}

			else if (temp == "Input")
			{
				std::getline(file, action.input);
				if (!action.input.empty() && action.input[0] == ' ')
				{
					action.input.erase(0, 1);
				}

				if (!action.input.empty() && action.input.back() == '\r')
				{
					action.input.pop_back();
				}

				action.type = ActionType::INPUT;
				actions.push_back(action);
			}
		}
	}

	file.close();

	std::ifstream res_file("adv-world.result");
	if (res_file.is_open())
	{
		std::string line;

		while (std::getline(res_file, line))
		{
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			if (line.empty())
			{
				continue;
			}

			ExpectedResult res;

			res.description = line;

			std::stringstream ss(line);
			ss >> res.cycle;

			expected_results.push_back(res);
		}

		res_file.close();
	}
}

void GameByFile::run()
{
	startNewGame();

	system("cls");
	if (results_consistent && current_result_index < expected_results.size())
	{
		results_consistent = false;
		failure_reason = "Missing expected result: " + expected_results[current_result_index].description;
	}

	if (results_consistent)
	{
		std::cout << "Test passed: Results match adv-world.result" << std::endl;
	}
	else
	{
		std::cout << "Test failed: Results do NOT match adv-world.result" << std::endl;
		std::cout << "Reason: " << failure_reason << std::endl;
	}

	if (!is_silent)
	{
		Sleep(3000);
	}
}

bool GameByFile::processInput(bool& restart_level)
{
	while (current_action_index < actions.size() && actions[current_action_index].cycle <= game_cycle_counter)
	{
		if (actions[current_action_index].cycle == game_cycle_counter &&
		    actions[current_action_index].type == ActionType::KEY)
		{
			char key = std::tolower(actions[current_action_index].key);

			if (key == 27 || key == 'h' || key == 'r')
			{
				if (key == 'r')
				{
					restart_level = true;
					current_action_index++;

					return false;
				}

				else if (key == 'h')
				{
					current_action_index++;
					if (!is_silent)
					{
						Sleep(2000);
						system("cls");
						std::cout << "Replay Finished." << std::endl;
					}
					else
					{
						system("cls");
					}

					if (!is_silent)
					{
						Sleep(3000);
					}

					return false;
				}
			}
			else
			{
				for (auto& p : players)
				{
					p.handleButtonPress(key);
				}
			}
		}

		current_action_index++;
	}

	if (results_consistent && current_result_index < expected_results.size())
	{
		if (game_cycle_counter > expected_results[current_result_index].cycle)
		{
			results_consistent = false;
			failure_reason =
			    "Missed expected result event (cycle passed): " + expected_results[current_result_index].description;
		}
	}

	if (current_action_index >= actions.size())
	{
		return false;
	}

	return true;
}

std::string GameByFile::getRiddleInput()
{
	for (size_t i = current_action_index; i < actions.size(); ++i)
	{
		if (actions[i].type == ActionType::INPUT)
		{
			return actions[i].input;
		}
	}

	return "";
}

void GameByFile::waitForLevelTransitionInput()
{
	for (size_t i = current_action_index; i < actions.size(); ++i)
	{
		if (actions[i].type == ActionType::KEY && actions[i].key == ' ')
		{
			current_action_index = i + 1;

			return;
		}
	}

	if (!is_silent)
	{
		Sleep(2000);
	}
}

void GameByFile::logHealthLoss(int player_id)
{
	std::stringstream ss;

	ss << game_cycle_counter << " Player " << player_id << " lost a life";
	checkResult(ss.str());
}

void GameByFile::logRiddle(const std::string& question, const std::string& answer, bool correct)
{
	std::stringstream ss;

	ss << game_cycle_counter << " Riddle: " << question << " Answer: " << answer
	   << " Result: " << (correct ? "Correct" : "Wrong");
	checkResult(ss.str());
}

void GameByFile::logGameEnd(int score)
{
	std::stringstream ss;

	ss << game_cycle_counter << " Game finished. Score: " << score;
	checkResult(ss.str());
}

void GameByFile::logLevelTransition(int next_level_number)
{
	std::stringstream ss;

	ss << game_cycle_counter << " Moved to next room. Level: " << next_level_number;
	checkResult(ss.str());
}
