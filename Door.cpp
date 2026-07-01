#include "Door.h"

void Door::useKey()
{
	keys_used++;
	if (keys_used == 2)
	{
		is_open = true;
	}
}

void Door::resetDoor()
{
	is_open = false;
	keys_used = 0;
	is_bombed = false;
}

void Door::bomb_door()
{
	is_bombed = true;
}

void Door::save(std::ostream& out) const
{
	GameObject::save(out);
	out << is_bombed << " " << is_open << " " << keys_used << " ";
}

void Door::load(std::istream& in)
{
	GameObject::load(in);
	in >> is_bombed >> is_open >> keys_used;
}
