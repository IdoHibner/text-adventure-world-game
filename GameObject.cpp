#include "GameObject.h"
#include "Board.h"

void GameObject::draw(Board& board) const
{
	board.drawChar(position.getX(), position.getY(), displayChar);
}

void GameObject::save(std::ostream& out) const
{
	out << position.getX() << " " << position.getY() << " " << displayChar << " ";
}

void GameObject::load(std::istream& in)
{
	int x, y;
	in >> x >> y >> displayChar;
	position.setX(x);
	position.setY(y);
}
