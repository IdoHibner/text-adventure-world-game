#pragma once
#include "GameObject.h"
class Bomb : public GameObject
{
	bool droped_to_expload;
	bool collected;
	int timer = 10;

  public:
	Bomb(int x, int y, char c = '@') : GameObject(x, y, c), droped_to_expload(false), collected(false)
	{
	}

	Bomb() : GameObject(0, 0, '@'), droped_to_expload(false), collected(false)
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

	bool isActive() const
	{
		return droped_to_expload && !collected;
	}

	int getX() const
	{
		return getPosition().getX();
	}

	int getY() const
	{
		return getPosition().getY();
	}

	int getTimer() const
	{
		return timer;
	}

	void tick()
	{
		if (timer > 0)
		{
			timer--;
		}
	}

	void resetBomb();
	void drop(int x, int y);
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
