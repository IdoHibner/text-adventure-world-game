#include "Point.h"
void Point::move()
{
	x += diff_x;
	y += diff_y;
}

void Point::setDirection(Direction dir)
{
	switch (dir)
	{
	case Direction::UP:
		diff_x = 0;
		diff_y = -1;
		break;
	case Direction::DOWN:
		diff_x = 0;
		diff_y = 1;
		break;
	case Direction::LEFT:
		diff_x = -1;
		diff_y = 0;
		break;
	case Direction::RIGHT:
		diff_x = 1;
		diff_y = 0;
		break;
	case Direction::STAY:
		diff_x = 0;
		diff_y = 0;
		break;
	}
}

Direction Point::getDirection() const
{
	if (diff_x == 0 && diff_y == -1)
	{
		return Direction::UP;
	}
	else if (diff_x == 0 && diff_y == 1)
	{
		return Direction::DOWN;
	}
	else if (diff_x == -1 && diff_y == 0)
	{
		return Direction::LEFT;
	}
	else if (diff_x == 1 && diff_y == 0)
	{
		return Direction::RIGHT;
	}
	else
	{
		return Direction::STAY;
	}
}
