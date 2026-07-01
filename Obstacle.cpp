#include "Obstacle.h"
#include "Board.h"
#include "Direction.h"
#include "Point.h"
#include "Utils.h"

void Obstacle::getDirOffsets(Direction d, int& dx, int& dy)
{
	dx = 0;
	dy = 0;
	switch (d)
	{
	case Direction::UP:
		dy = -1;
		break;
	case Direction::DOWN:
		dy = 1;
		break;
	case Direction::LEFT:
		dx = -1;
		break;
	case Direction::RIGHT:
		dx = 1;
		break;
	default:
		break;
	}
}

void Obstacle::drawObstacle(Board& board)
{
	for (auto& part : parts)
	{
		board.drawChar(part.getX(), part.getY(), '*');
	}
}

void Obstacle::printDetails()
{
	std::cout << "Obstacle parts:\n";
	for (const auto& part : parts)
	{
		std::cout << "Part at (" << part.getX() << ", " << part.getY() << ")\n";
	}

	std::cout << "Size: " << getSize() << "\n";
}

bool Obstacle::isPartOfObstacle(int x, int y) const
{
	for (const auto& part : parts)
	{
		if (part.getX() == x && part.getY() == y)
		{
			return true;
		}
	}

	return false;
}

void Obstacle::save(std::ostream& out) const
{
	GameObject::save(out);
	out << parts.size() << " ";
	for (const auto& p : parts)
	{
		out << p.getX() << " " << p.getY() << " ";
	}
}

void Obstacle::load(std::istream& in)
{
	GameObject::load(in);
	int size;

	in >> size;
	parts.clear();
	for (int i = 0; i < size; ++i)
	{
		int x, y;
		in >> x >> y;
		parts.emplace_back(x, y);
	}
}
