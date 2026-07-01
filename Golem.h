#pragma once
#include "Board.h"
#include "GameObject.h"
class Player;
class Game;

class Golem : public GameObject
{
  public:
	Golem(int x, int y, char c = 'X') : GameObject(x, y, c)
	{
	}

	Golem() : GameObject(0, 0, 'X')
	{
	}

	void moveGolem(Board& board, Player* players, Game* game);
	void save(std::ostream& out) const override;
	void load(std::istream& in) override;
};
