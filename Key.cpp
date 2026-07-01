#include "Key.h"

void Key::drop(int x, int y)
{
	setPosition(x, y);
	collected = false;
}

void Key::save(std::ostream& out) const
{
	GameObject::save(out);
	out << collected << " ";
}

void Key::load(std::istream& in)
{
	GameObject::load(in);
	in >> collected;
}
