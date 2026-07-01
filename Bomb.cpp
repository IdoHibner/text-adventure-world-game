#include "Bomb.h"

void Bomb::resetBomb()
{
	collected = false;
	droped_to_expload = false;
	timer = 10;
}

void Bomb::drop(int x, int y)
{
	setPosition(x, y);
	collected = false;
	droped_to_expload = true;
}

void Bomb::save(std::ostream& out) const
{
	GameObject::save(out);
	out << droped_to_expload << " " << collected << " " << timer << " ";
}

void Bomb::load(std::istream& in)
{
	GameObject::load(in);
	in >> droped_to_expload >> collected >> timer;
}
