#include "Spring.h"
#include "Board.h"

bool Spring::isPartOfSpring(int x, int y) const
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

void Spring::drawSpring(Board& board)
{
	for (int i = 0; i < parts.size(); ++i)
	{
		if (i < active_length)
		{
			board.drawChar(parts[i].getX(), parts[i].getY(), '#');
		}
		else
		{
			board.drawChar(parts[i].getX(), parts[i].getY(), ' ');
		}
	}
}

void Spring::save(std::ostream& out) const
{
	out << parts.size() << " ";
	for (const auto& p : parts)
	{
		out << p.getX() << " " << p.getY() << " ";
	}

	out << (int)push_direction << " " << active_length << " ";
}

void Spring::load(std::istream& in)
{
	int size;

	in >> size;
	parts.clear();
	for (int i = 0; i < size; ++i)
	{
		int x, y;
		in >> x >> y;
		parts.emplace_back(x, y);
	}

	int dir;

	in >> dir;
	push_direction = (Direction)dir;
	in >> active_length;
}
