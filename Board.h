#pragma once
#include "Point.h"
#include "Switch.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

static const int BOARD_HEIGHT = 25;
static const int BOARD_WIDTH = 80;
static const int LEGEND_HEIGHT = 5;
static const int LEGEND_WIDTH = 80;

class Board
{
	char board[BOARD_HEIGHT][BOARD_WIDTH] = {};

	bool is_dark_map[BOARD_HEIGHT][BOARD_WIDTH] = {};

	std::vector<Point> light_sources;
	int light_radius = 3;

	bool colors_enabled = false;

	int legend_x = -1;
	int legend_y = -1;

	void initLegend();
	void fixMissingPlayer(char playerChar);
	bool isInsideBoard(int x, int y) const;
	void setCharIfInside(int x, int y, char c);

  public:
	Board();

	bool isInsideBounds(int x, int y) const
	{
		return x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT;
	}

	void setColorsEnabled(bool enabled)
	{
		colors_enabled = enabled;
		::setColorsEnabled(enabled);
	}

	bool areColorsEnabled() const
	{
		return colors_enabled;
	}

	bool is_silent = false;

	void setSilent(bool silent)
	{
		is_silent = silent;
	}

	bool areSilent() const
	{
		return is_silent;
	}

	char getCharAt(int x, int y) const
	{
		return isInsideBounds(x, y) ? board[y][x] : 'W';
	}

	bool isWall(int x, int y) const
	{
		char c = getCharAt(x, y);

		return c == 'W' || c == '-' || c == '|';
	}

	bool isPlayer(int x, int y) const
	{
		char c = getCharAt(x, y);

		return c == '$' || c == '&';
	}

	bool isKey(int x, int y) const
	{
		return getCharAt(x, y) == 'K';
	}

	bool isDoor(int x, int y) const
	{
		return getCharAt(x, y) == '9';
	}

	bool isRiddle(int x, int y) const
	{
		return getCharAt(x, y) == '?';
	}

	bool isSwitch(int x, int y) const
	{
		char c = getCharAt(x, y);

		return c == '\\' || c == '/';
	}

	bool isBomb(int x, int y) const
	{
		return getCharAt(x, y) == '@';
	}

	bool isSpring(int x, int y) const
	{
		return getCharAt(x, y) == '#';
	}

	bool isObstacle(int x, int y) const
	{
		return getCharAt(x, y) == '*';
	}

	bool isTorch(int x, int y) const
	{
		return getCharAt(x, y) == '!';
	}

	bool isCookie(int x, int y) const
	{
		return getCharAt(x, y) == '+';
	}

	bool isPortal(int x, int y) const
	{
		char c = getCharAt(x, y);

		return c == '>' || c == '<';
	}

	bool isGolem(int x, int y) const
	{
		return getCharAt(x, y) == 'X';
	}

	void setCharAt(int x, int y, char c)
	{
		if (isInsideBounds(x, y))
		{
			board[y][x] = c;
		}
	}

	bool loadBoard(const std::string& filename);
	void printBoard();
	bool isInLegendArea(int x, int y) const;
	void updateHeldItemAndRedraw(int player_id, const std::string& text);
	void displayUsedKeyMessage(int player_id);
	void displayBombDroppedMessage(int timer);
	void displayGeneralMessage(const std::string& message);
	void displayPointsMessage(const std::string& message);
	void displayEndScore(int score);
	void displayTimeBonus(int bonus);
	void displayLevelScore(int score);
	void displayCurrentGameScore(int score);
	void displaySwitch1Status(Switch& s);
	void clearSwitch1Status();
	void clearSwitch2Status();
	void displaySwitch2Status(Switch& s);
	void displayDoorOpenedMessage(int player_id);
	void updatePlayer1HealthDisplay(int health);
	void updatePlayer2HealthDisplay(int health);
	void clearMessages();
	void clearPointsMessages();
	void showRiddlePopup(const std::string& question);
	void showLevelCompleteMessage();
	void drawChar(int x, int y, char c);
	void writeTextToBoard(int default_row, int default_col, const std::string& text);
	void addLightSource(int x, int y);
	void clearLightSources();
	bool isCellVisible(int col, int row) const;
	void save(std::ostream& out) const;
	void load(std::istream& in);
};
