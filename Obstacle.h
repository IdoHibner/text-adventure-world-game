#pragma once
#include "Board.h"
#include "GameObject.h"
#include <vector>

class Obstacle : public GameObject
{
	std::vector<Point> parts;

  public:
	Obstacle()
	{
	}

	void addPart(int x, int y)
	{
		parts.push_back(Point(x, y));
	}

	int getSize() const
	{
		return static_cast<int>(parts.size());
	}

	std::vector<Point>& getParts()
	{
		return parts;
	}

	const std::vector<Point>& getParts() const
	{
		return parts;
	}

	void getDirOffsets(Direction d, int& dx, int& dy);
	void drawObstacle(Board& board);
	void printDetails();
	bool isPartOfObstacle(int x, int y) const;
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
