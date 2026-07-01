#pragma once
#include "Game.h"
#include <string>
#include <vector>

enum class ActionType
{
	KEY,
	INPUT
};

struct RecordedAction
{
	int cycle = 0;
	ActionType type = ActionType::KEY;
	char key = '\0';
	std::string input;
};

class GameByFile : public Game
{
	std::vector<RecordedAction> actions;
	size_t current_action_index = 0;

	struct ExpectedResult
	{
		int cycle = 0;
		std::string description;
	};

	std::vector<ExpectedResult> expected_results;
	size_t current_result_index = 0;
	bool results_consistent = true;
	std::string failure_reason;

	void checkResult(const std::string& observed_description);

  public:
	GameByFile(const std::string& filename = "recorded_game.txt", bool silent = false);
	void run() override;
	bool processInput(bool& restart_level) override;
	std::string getRiddleInput() override;
	void waitForLevelTransitionInput() override;
	void logHealthLoss(int player_id) override;
	void logRiddle(const std::string& question, const std::string& answer, bool correct) override;
	void logGameEnd(int score) override;
	void logLevelTransition(int next_level_number) override;
};
