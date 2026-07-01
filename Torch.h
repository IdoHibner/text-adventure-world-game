#pragma once
#include "GameObject.h"

class Torch : public GameObject
{
	int radius;
	bool is_collected;

  public:
	Torch(int x, int y, char c = '!', int radius = 3) : GameObject(x, y, c), radius(radius), is_collected(false)
	{
	}

	Torch() : GameObject(0, 0, '!'), radius(3), is_collected(false)
	{
	}

	int getRadius() const
	{
		return radius;
	}

	void resetTorch()
	{
		is_collected = false;
	}

	bool getIsCollected() const
	{
		return is_collected;
	}

	void collect()
	{
		is_collected = true;
		setDisplayChar(' ');
	}

	int getX() const
	{
		return getPosition().getX();
	}

	int getY() const
	{
		return getPosition().getY();
	}

	void drop(int x, int y);
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
