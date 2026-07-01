#pragma once
#include "Point.h"
#include <iostream>

class Board;

class GameObject
{
	Point position;
	char displayChar;

  public:
	GameObject(int x = 0, int y = 0, char c = ' ') : position(x, y), displayChar(c)
	{
	}

	void setPosition(int x, int y)
	{
		position.setX(x);
		position.setY(y);
	}

	Point& getPosition()
	{
		return position;
	}

	const Point& getPosition() const
	{
		return position;
	}

	void setDisplayChar(char c)
	{
		displayChar = c;
	}

	char getDisplayChar() const
	{
		return displayChar;
	}

	void draw(Board& board) const;
	virtual void save(std::ostream& out) const;
	virtual void load(std::istream& in);
};
