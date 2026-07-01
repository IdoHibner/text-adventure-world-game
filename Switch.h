#pragma once
#include "GameObject.h"
class Switch : public GameObject
{
	bool is_pressed;

  public:
	Switch(int x, int y, char c = '\\') : GameObject(x, y, c), is_pressed(false)
	{
	}

	Switch() : GameObject(0, 0, '\\'), is_pressed(false)
	{
	}

	void press()
	{
		is_pressed = !is_pressed;
	}

	bool isPressed() const
	{
		return is_pressed;
	}

	void resetSwitch()
	{
		is_pressed = false;
	}

	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
