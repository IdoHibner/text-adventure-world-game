#pragma once
#include "GameObject.h"
#include "Utils.h"
#include <vector>

class Board;

class Spring
{
	std::vector<Point> parts;
	Direction push_direction;
	int active_length;

  public:
	Spring() : push_direction(Direction::STAY), active_length(0)
	{
	}

	void resetSpring()
	{
		active_length = static_cast<int>(parts.size());
	}

	void addPart(int x, int y)
	{
		parts.push_back(Point(x, y));
		active_length++;
	}

	void setDirection(Direction d)
	{
		push_direction = d;
	}

	Direction getDirection() const
	{
		return push_direction;
	}

	int getFullLength() const
	{
		return static_cast<int>(parts.size());
	}

	bool isPartOfSpring(int x, int y) const;
	void drawSpring(Board& board);
	void save(std::ostream& out) const;
	void load(std::istream& in);
};
