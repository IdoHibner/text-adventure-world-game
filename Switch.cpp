#include "Switch.h"

void Switch::save(std::ostream& out) const
{
	GameObject::save(out);
	out << is_pressed << " ";
}

void Switch::load(std::istream& in)
{
	GameObject::load(in);
	in >> is_pressed;
}
