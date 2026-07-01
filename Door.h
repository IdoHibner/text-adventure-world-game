#pragma once
#include "GameObject.h"

class Door : public GameObject
{
	bool is_bombed = false;
	bool is_open;
	int keys_used = 0;

  public:
	Door(int x, int y, char c = '9') : GameObject(x, y, c), is_open(false)
	{
	}

	Door() : GameObject(0, 0, '9'), is_open(false)
	{
	}

	bool getIsOpen() const
	{
		return is_open;
	}

	bool is_door_bombed() const
	{
		return is_bombed;
	}

	void useKey();
	void resetDoor();
	void bomb_door();
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
