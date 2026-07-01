#include "Torch.h"

void Torch::drop(int x, int y)
{
	setPosition(x, y);
	is_collected = false;
}

void Torch::save(std::ostream& out) const
{
	GameObject::save(out);
	out << radius << " " << is_collected << " ";
}

void Torch::load(std::istream& in)
{
	GameObject::load(in);
	in >> radius >> is_collected;
}
