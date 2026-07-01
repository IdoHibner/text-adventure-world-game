#include "Utils.h"
#include <cstdlib>
#include <iostream>
#include <windows.h>

using namespace std;

void gotoxy(int x, int y)
{
	HANDLE hConsoleOutput;
	COORD dwCursorPosition;

	cout.flush();
	dwCursorPosition.X = x;
	dwCursorPosition.Y = y;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
}

void hideCursor()
{
	HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CURSOR;

	CURSOR.dwSize = 1;
	CURSOR.bVisible = FALSE;
	SetConsoleCursorInfo(myconsole, &CURSOR);
}

void cls()
{
	system("cls");
}

void sleep()
{
	::Sleep(50);
}

void sleep100()
{
	::Sleep(100);
}

void sleep2000()
{
	::Sleep(2000);
}

void sleep500()
{
	::Sleep(500);
}

void setTextColor(Color color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, (WORD)color);
}

Color getColorForChar(char c)
{
	switch (c)
	{
	case '$':
		return Color::LIGHTGREEN;
	case '&':
		return Color::LIGHTBLUE;
	case '#':
		return Color::DARKGREY;
	case 'K':
		return Color::BLUE;
	case '*':
		return Color::GREEN;
	case '@':
		return Color::RED;
	case '?':
		return Color::MAGENTA;
	case '!':
		return Color::YELLOW;
	case '9':
		return Color::BROWN;
	case '/':
	case '\\':
		return Color::LIGHTCYAN;
	case '>':
		return Color::LIGHTMAGENTA;
	case '+':
		return Color::CYAN;
	case 'X':
		return Color::LIGHTRED;
	case ' ':
		return Color::BLACK;
	default:
		return Color::WHITE;
	}
}

void setColorsEnabled(bool enabled)
{
	g_colors_enabled = enabled;
}

bool areColorsEnabled()
{
	return g_colors_enabled;
}
