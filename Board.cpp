#include "Board.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void Board::initLegend()
{
	int width = 79;
	int col1_width = 20;
	int col2_width = 20;

	for (int i = 0; i < width; ++i)
	{
		setCharIfInside(legend_x + i, legend_y, '-');
		setCharIfInside(legend_x + i, legend_y + 4, '-');
	}

	for (int r = 1; r <= 3; ++r)
	{
		int currentRow = legend_y + r;

		if (currentRow >= BOARD_HEIGHT)
		{
			break;
		}

		setCharIfInside(legend_x, currentRow, '|');
		setCharIfInside(legend_x + col1_width, currentRow, '|');
		setCharIfInside(legend_x + col1_width + col2_width, currentRow, '|');
		setCharIfInside(legend_x + width - 1, currentRow, '|');
	}

	writeTextToBoard(1, 2, "Player 1:");
	writeTextToBoard(1, 2 + col1_width, "Player 2:");

	writeTextToBoard(2, 2, "Health:");
	writeTextToBoard(2, 2 + col1_width, "Health:");
	writeTextToBoard(2, 2 + col1_width + col2_width, "Score: 0");

	writeTextToBoard(3, 2, "Held item: None");
	writeTextToBoard(3, 2 + col1_width, "Held item: None");
	writeTextToBoard(3, 2 + col1_width + col2_width, "Switch 1: OFF    Switch 2: OFF");
}

void Board::fixMissingPlayer(char playerChar)
{
	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		if (legend_y != -1 && i >= legend_y && i < legend_y + 5)
		{
			continue;
		}

		for (int j = 0; j < BOARD_WIDTH; ++j)
		{
			if (board[i][j] == ' ')
			{
				board[i][j] = playerChar;

				return;
			}
		}
	}
}

bool Board::isInsideBoard(int x, int y) const
{
	return isInsideBounds(x, y);
}

void Board::setCharIfInside(int x, int y, char c)
{
	if (isInsideBoard(x, y))
	{
		board[y][x] = c;
	}
}

Board::Board()
{
}

bool Board::loadBoard(const std::string& filename)
{
	light_sources.clear();

	legend_x = -1;
	legend_y = -1;
	bool player1_not_in_legend = false;
	bool player2_not_in_legend = false;

	ifstream file(filename);
	if (!file.is_open())
	{
		return false;
	}

	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			board[i][j] = ' ';
			is_dark_map[i][j] = false;
		}
	}

	string line;

	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		if (getline(file, line))
		{
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			for (int j = 0; j < BOARD_WIDTH && j < line.size(); ++j)
			{
				char c = line[j];

				if (c == 'L')
				{
					legend_x = 0;
					legend_y = i;
					c = ' ';
				}

				if (isInLegendArea(j, i))
				{
					board[i][j] = ' ';
				}
				else
				{
					if (c == '^')
					{
						board[i][j] = ' ';
						is_dark_map[i][j] = true;
					}
					else if (c == 'w')
					{
						board[i][j] = 'W';
						is_dark_map[i][j] = true;
					}
					else
					{
						board[i][j] = c;

						if (c == '$')
						{
							player1_not_in_legend = true;
						}

						if (c == '&')
						{
							player2_not_in_legend = true;
						}
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	file.close();

	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			if (!is_dark_map[i][j] && board[i][j] != 'W' && board[i][j] != '-' && board[i][j] != '|')
			{
				bool near_object = false;
				int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
				int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};

				for (int k = 0; k < 8; k++)
				{
					int ni = i + dy[k];
					int nj = j + dx[k];

					if (ni >= 0 && ni < BOARD_HEIGHT && nj >= 0 && nj < BOARD_WIDTH)
					{
						if (is_dark_map[ni][nj])
						{
							near_object = true;
							break;
						}
					}
				}

				if (near_object)
				{
					is_dark_map[i][j] = true;
				}
			}
		}
	}

	if (!player1_not_in_legend && legend_y != -1)
	{
		fixMissingPlayer('$');
	}

	if (!player2_not_in_legend && legend_y != -1)
	{
		fixMissingPlayer('&');
	}

	if (legend_y != -1)
	{
		initLegend();
	}

	return true;
}

void Board::printBoard()
{
	if (is_silent)
	{
		return;
	}

	for (int i = 0; i < BOARD_HEIGHT; i++)
	{
		gotoxy(0, i);
		for (int j = 0; j < BOARD_WIDTH; j++)
		{
			char c = isCellVisible(j, i) ? board[i][j] : ' ';

			if (colors_enabled && !isInLegendArea(j, i))
			{
				setTextColor(getColorForChar(c));
			}

			cout << c;

			if (colors_enabled)
			{
				setTextColor(Color::WHITE);
			}
		}

		if (i < BOARD_HEIGHT - 1)
		{
			cout << endl;
		}
	}

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}
}

bool Board::isInLegendArea(int x, int y) const
{
	if (legend_x == -1 || legend_y == -1)
	{
		return false;
	}

	return (x >= legend_x && x < legend_x + LEGEND_WIDTH && y >= legend_y && y < legend_y + LEGEND_HEIGHT);
}

void Board::updateHeldItemAndRedraw(int player_id, const std::string& text)
{
	int row = legend_y + 3;

	int col;

	if (player_id == 1)
	{
		col = legend_x + 13;
	}
	else
	{
		col = legend_x + 33;
	}

	const int MAX_LEN = 6;
	std::string item_name = text;

	if (item_name.size() < MAX_LEN)
	{
		item_name.resize(MAX_LEN, ' ');
	}

	for (int i = 0; i < MAX_LEN; ++i)
	{
		if (row < BOARD_HEIGHT && (col + i) < BOARD_WIDTH)
		{
			board[row][col + i] = item_name[i];
		}
	}

	printBoard();
}

void Board::displayUsedKeyMessage(int player_id)
{
	clearMessages();

	int row = legend_y + 1;
	int col = legend_x + 42;

	std::string message = "P" + std::to_string(player_id) + " used a key!";

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::displayBombDroppedMessage(int timer)
{
	int row = legend_y + 1;
	int col = legend_x + 42;
	int len = legend_y + 36;

	for (int i = 0; i < 35; ++i)
	{
		if (col + i < BOARD_WIDTH)
		{
			board[row][col + i] = ' ';
			if (!is_silent)
			{
				gotoxy(col + i, row);
				std::cout << ' ';
			}
		}
	}

	std::string message = "The bomb will explode in " + std::to_string(timer);

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}

	for (int i = 0; i < (int)message.size(); ++i)
	{
		if (col + i < BOARD_WIDTH)
		{
			board[row][col + i] = message[i];
			if (!is_silent)
			{
				gotoxy(col + i, row);
				std::cout << message[i];
			}
		}
	}
}

void Board::displayGeneralMessage(const std::string& message)
{
	clearMessages();
	int row = legend_y + 1;
	int col = legend_x + 42;

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::displayPointsMessage(const std::string& message)
{
	clearPointsMessages();
	int row = legend_y + 2;
	int col = legend_x + 49;

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::displayEndScore(int score)
{
	if (is_silent)
	{
		return;
	}

	const int ROW = 18;
	const int COL = 33;
	std::string message = std::to_string(score);

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}

	for (int i = 0; i < (int)message.size(); ++i)
	{
		gotoxy(COL + i, ROW);
		cout << message[i];
	}
}

void Board::displayTimeBonus(int bonus)
{
	if (is_silent)
	{
		return;
	}

	const int ROW = 12;
	const int COL = 31;
	std::string message = std::to_string(bonus);

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}

	for (int i = 0; i < (int)message.size(); ++i)
	{
		gotoxy(COL + i, ROW);
		cout << message[i];
	}
}

void Board::displayLevelScore(int score)
{
	if (is_silent)
	{
		return;
	}

	const int ROW = 14;
	const int COL = 38;
	std::string message = std::to_string(score);

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}

	for (int i = 0; i < (int)message.size(); ++i)
	{
		gotoxy(COL + i, ROW);
		cout << message[i];
	}
}

void Board::displayCurrentGameScore(int score)
{
	if (is_silent)
	{
		return;
	}

	const int ROW = 16;
	const int COL = 37;
	std::string message = std::to_string(score);

	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}

	for (int i = 0; i < (int)message.size(); ++i)
	{
		gotoxy(COL + i, ROW);
		cout << message[i];
	}
}

void Board::displaySwitch1Status(Switch& s)
{
	clearSwitch1Status();

	int row = legend_y + 3;
	int col = legend_x + 51;
	std::string message = s.isPressed() ? " ON" : " OFF";

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::clearSwitch1Status()
{
	int row = legend_y + 3;
	int col = legend_x + 51;
	int len = legend_y + 4;

	for (int i = 0; i < len; ++i)
	{
		board[row][col + i] = ' ';
	}

	printBoard();
}

void Board::clearSwitch2Status()
{
	int row = legend_y + 3;
	int col = legend_x + 68;
	int len = legend_y + 4;

	for (int i = 0; i < len; ++i)
	{
		board[row][col + i] = ' ';
	}

	printBoard();
}

void Board::displaySwitch2Status(Switch& s)
{
	clearSwitch2Status();

	int row = legend_y + 3;
	int col = legend_x + 68;
	std::string message = s.isPressed() ? " ON" : " OFF";

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::displayDoorOpenedMessage(int player_id)
{
	int row = legend_y + 1;
	int col = legend_x + 56;

	std::string message = " The door is open!";

	for (int i = 0; i < (int)message.size(); ++i)
	{
		board[row][col + i] = message[i];
	}

	printBoard();
}

void Board::updatePlayer1HealthDisplay(int health)
{
	int row = legend_y + 2;
	int col = legend_x + 10;
	std::string health_str = std::to_string(health);

	for (int i = 0; i < (int)health_str.size(); ++i)
	{
		board[row][col + i] = health_str[i];
	}

	printBoard();
}

void Board::updatePlayer2HealthDisplay(int health)
{
	int row = legend_y + 2;
	int col = legend_x + 30;
	std::string health_str = std::to_string(health);

	for (int i = 0; i < (int)health_str.size(); ++i)
	{
		board[row][col + i] = health_str[i];
	}

	printBoard();
}

void Board::clearMessages()
{
	int row = legend_y + 1;
	int col = legend_x + 42;
	int len = legend_y + 36;

	for (int i = 0; i < len; ++i)
	{
		board[row][col + i] = ' ';
	}

	printBoard();
}

void Board::clearPointsMessages()
{
	int row = legend_y + 2;
	int col = legend_x + 49;
	int len = legend_y + 26;

	for (int i = 0; i < len; ++i)
	{
		board[row][col + i] = ' ';
	}

	printBoard();
}

void Board::showRiddlePopup(const std::string& question)
{
	if (is_silent)
	{
		return;
	}

	cls();

	std::ifstream file("riddle_menu.txt");
	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			if (colors_enabled)
			{
				setTextColor(Color::WHITE);
			}

			std::cout << line << std::endl;
		}

		file.close();
	}
	else
	{
		if (colors_enabled)
		{
			setTextColor(Color::WHITE);
		}

		std::cout << "RIDDLE TIME!\n\n" << question << "\n\nAnswer: ";

		return;
	}

	int x = 6;
	int y = 11;
	int max_line_len = 65;

	std::string curr_line = "";
	std::string word = "";

	for (size_t i = 0; i < question.length(); i++)
	{
		if (question[i] != ' ')
		{
			word += question[i];
		}

		if (question[i] == ' ' || i == question.length() - 1)
		{
			if (curr_line.length() + word.length() + 1 > max_line_len)
			{
				gotoxy(x, y);
				std::cout << curr_line;

				y++;
				curr_line = word + " ";
			}
			else
			{
				curr_line += word + " ";
			}

			word = "";
		}
	}

	gotoxy(x, y);
	std::cout << curr_line;

	gotoxy(19, 16);
}

void Board::showLevelCompleteMessage()
{
	if (is_silent)
	{
		return;
	}

	cls();

	std::ifstream file("level_complete.txt");

	if (file.is_open())
	{
		std::string line;

		while (std::getline(file, line))
		{
			if (colors_enabled)
			{
				setTextColor(Color::WHITE);
			}

			std::cout << line << std::endl;
		}

		file.close();
	}
	else
	{
		if (colors_enabled)
		{
			setTextColor(Color::WHITE);
		}

		std::cout << "\n\n\t*** LEVEL COMPLETE! ***\n\n\tMoving to next level..." << std::endl;
	}
}

void Board::drawChar(int x, int y, char c)
{
	if (is_silent || !isInsideBounds(x, y))
	{
		return;
	}

	gotoxy(x, y);
	if (colors_enabled && !isInLegendArea(x, y))
	{
		setTextColor(getColorForChar(c));
	}

	cout << c;
	if (colors_enabled)
	{
		setTextColor(Color::WHITE);
	}
}

void Board::writeTextToBoard(int default_row, int default_col, const std::string& text)
{
	int row = legend_y + default_row;
	int col = legend_x + default_col;

	for (size_t i = 0; i < text.length(); ++i)
	{
		if (row >= 0 && row < BOARD_HEIGHT && (col + i) >= 0 && (col + i) < BOARD_WIDTH)
		{
			board[row][col + i] = text[i];
		}
	}
}

void Board::addLightSource(int x, int y)
{
	light_sources.push_back(Point(x, y));
}

void Board::clearLightSources()
{
	light_sources.clear();
}

bool Board::isCellVisible(int col, int row) const
{
	bool is_dark = false;

	if (col >= 0 && col < BOARD_WIDTH && row >= 0 && row < BOARD_HEIGHT)
	{
		is_dark = is_dark_map[row][col];
	}

	if (!is_dark)
	{
		return true;
	}

	if (light_sources.empty())
	{
		return false;
	}

	for (const auto& source : light_sources)
	{
		int dx = col - source.getX();
		int dy = row - source.getY();

		if ((dx * dx + dy * dy) <= (light_radius * light_radius))
		{
			return true;
		}
	}

	return false;
}

void Board::save(std::ostream& out) const
{
	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		for (int j = 0; j < BOARD_WIDTH; ++j)
		{
			out << (int)board[i][j] << " ";
		}
	}

	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		for (int j = 0; j < BOARD_WIDTH; ++j)
		{
			out << is_dark_map[i][j] << " ";
		}
	}

	out << colors_enabled << " " << legend_x << " " << legend_y << " ";

	out << light_sources.size() << " ";
	for (const auto& p : light_sources)
	{
		out << p.getX() << " " << p.getY() << " ";
	}
}

void Board::load(std::istream& in)
{
	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		for (int j = 0; j < BOARD_WIDTH; ++j)
		{
			int c;

			in >> c;
			board[i][j] = (char)c;
		}
	}

	for (int i = 0; i < BOARD_HEIGHT; ++i)
	{
		for (int j = 0; j < BOARD_WIDTH; ++j)
		{
			in >> is_dark_map[i][j];
		}
	}

	in >> colors_enabled >> legend_x >> legend_y;
	int size;

	in >> size;
	light_sources.clear();
	for (int i = 0; i < size; ++i)
	{
		int x, y;
		in >> x >> y;
		light_sources.emplace_back(x, y);
	}
}
