#pragma once
#include "Direction.h"

class Point
{
	int x, y;
	int diff_x = 0, diff_y = 0;

  public:
	Point(int _x = 0, int _y = 0) : x(_x), y(_y)
	{
	}

	int getX() const
	{
		return x;
	}

	int getY() const
	{
		return y;
	}

	void setX(int _x)
	{
		x = _x;
	}

	void setY(int _y)
	{
		y = _y;
	}

	int getDiffX() const
	{
		return diff_x;
	}

	void setDiffX(int val)
	{
		diff_x = val;
	}

	int getDiffY() const
	{
		return diff_y;
	}

	void setDiffY(int val)
	{
		diff_y = val;
	}

	void move();
	void setDirection(Direction dir);
	Direction getDirection() const;
};
