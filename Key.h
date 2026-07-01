#pragma once
#include "GameObject.h"

class Key : public GameObject
{
	bool collected;

  public:
	Key(int x, int y, char c = 'K') : GameObject(x, y, c), collected(false)
	{
	}

	Key() : GameObject(0, 0, 'K'), collected(false)
	{
	}

	bool isCollected() const
	{
		return collected;
	}

	void collect()
	{
		collected = true;
	}

	void setCollected(bool is_collected)
	{
		collected = is_collected;
	}

	void resetKey()
	{
		collected = false;
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
