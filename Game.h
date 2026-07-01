#pragma once
#include "Board.h"
#include "Bomb.h"
#include "Door.h"
#include "GameObject.h"
#include "Golem.h"
#include "Key.h"
#include "Obstacle.h"
#include "Player.h"
#include "Riddle.h"
#include "Spring.h"
#include "Switch.h"
#include "Torch.h"
#include "Utils.h"
#include <conio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Game
{
	friend class Player;

  protected:
	Board game_board;

	int current_level_index = 0;
	int current_riddle_index = 0;
	int current_spring_index = 0;
	int current_obstacle_index = 0;

	Door door;

	std::vector<std::string> levels;
	std::vector<Riddle> riddles;
	std::vector<Key> keys;
	std::vector<Switch> switches;
	std::vector<Bomb> bombs;
	std::vector<Spring> springs;
	std::vector<Obstacle> obstacles;
	std::vector<Torch> torches;
	std::vector<Point> portals;
	std::vector<Golem> golems;

	int game_points = 0, level_points = 0;

	Player players[2] = {Player(0, 0, '$', 1, "wxadse", game_board, keys, door, bombs, springs, obstacles, torches),
	                     Player(0, 0, '&', 2, "imjlko", game_board, keys, door, bombs, springs, obstacles, torches)};

	int game_cycle_counter = 0;
	const int MAX_TIME_BONUS = 4000;
	const int TIME_PENALTY = 2;

	const int BOMB_EXPLOSION_RADIUS = 3;

	unsigned int riddle_seed = 0;
	bool loaded_game = false;

	void resetGameScore()
	{
		game_points = 0;
	}

	void resetLevelScoring()
	{
		level_points = 0, game_cycle_counter = 0;
	}

	bool colors_enabled = false;

	void toggleColors()
	{
		colors_enabled = !colors_enabled;
		game_board.setColorsEnabled(colors_enabled);
	}

	bool resetGameSession();
	void movePlayer(Player& player);
	bool handlePlayerInteraction(Player& player, int& new_x, int& new_y);
	void handlePortal(Player& player, int& new_x, int& new_y);
	void handleRiddle(Player& player, int& new_x, int& new_y);
	void handleSwitch(Player& player, int& new_x, int& new_y);
	void handleDoor(Player& player, int& new_x, int& new_y);
	bool isPathClear(int start_x, int start_y, int target_x, int target_y);
	void updateBombs();
	virtual bool processInput(bool& restart_level) = 0;
	bool checkPlayersStatus();
	void handleLevelCompletion(int& bonus_points);

  public:
	static const int POINTS_COOKIE = 100;
	static const int POINTS_RIDDLE_CORRECT = 500;
	static const int POINTS_RIDDLE_WRONG = 200;
	static const int POINTS_GOLEM_KILL = 1000;

	bool is_silent = false;

	Game();

	void setSilent(bool silent)
	{
		game_board.setSilent(silent);
		is_silent = silent;
	}

	virtual ~Game()
	{
	}

	virtual void logHealthLoss(int player_id)
	{
	}

	virtual void logRiddle(const std::string& question, const std::string& answer, bool correct)
	{
	}

	virtual void logGameEnd(int score)
	{
	}

	virtual void logLevelTransition(int next_level_number)
	{
	}

	char showMenu();
	virtual void run() = 0;
	virtual std::string getRiddleInput() = 0;
	virtual void waitForLevelTransitionInput() = 0;
	void addLevelPoints(int points);
	void damagePlayer(Player& player, const std::string& source);
	void startNewGame(bool resume = false);
	void initSprings();
	void initObstacles();
	bool initLevel();
	void showInstructions();
	bool loadRiddlesFromFile(const std::string& filename);
	void displayPauseMenu();
	void saveGame(const std::string& filename);
	bool loadGame(const std::string& filename);
	static std::vector<std::string> getSaveFileList();
	bool showLoadGameMenu();
};
