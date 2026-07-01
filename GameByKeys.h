#pragma once
#include "Game.h"
#include <fstream>

class GameByKeys : public Game
{
	bool is_recording;
	std::ofstream record_file;
	unsigned int seed = 0;
	std::ofstream result_file;

  public:
	GameByKeys(bool save_mode = false);
	void run() override;
	bool processInput(bool& restart_level) override;
	std::string getRiddleInput() override;
	void waitForLevelTransitionInput() override;
	void logHealthLoss(int player_id) override;
	void logRiddle(const std::string& question, const std::string& answer, bool correct) override;
	void logGameEnd(int score) override;
	void logLevelTransition(int next_level_number) override;
};
