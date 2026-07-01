#include "Golem.h"
#include "Game.h"
#include "Player.h"
#include <cstdlib>

class Board;
class Player;

void Golem::moveGolem(Board& board, Player* players, Game* game)
{
	int direction = rand() % 4;

	switch (direction)
	{
	case 0:
		getPosition().setDirection(Direction::UP);
		break;
	case 1:
		getPosition().setDirection(Direction::DOWN);
		break;
	case 2:
		getPosition().setDirection(Direction::LEFT);
		break;
	case 3:
		getPosition().setDirection(Direction::RIGHT);
		break;
	default:
		getPosition().setDirection(Direction::STAY);
		break;
	}

	int new_x = getPosition().getX() + getPosition().getDiffX();
	int new_y = getPosition().getY() + getPosition().getDiffY();
	int old_x = getPosition().getX();
	int old_y = getPosition().getY();

	char target_char = board.getCharAt(new_x, new_y);
	bool is_allowed_to_move = false;

	if (target_char == ' ')
	{
		is_allowed_to_move = true;
	}
	else if (target_char == '$' || target_char == '&')
	{
		is_allowed_to_move = true;
		for (int i = 0; i < 2; ++i)
		{
			if (!players[i].getIfDead() && players[i].getPosition().getX() == new_x &&
			    players[i].getPosition().getY() == new_y)
			{
				if (game)
				{
					game->damagePlayer(players[i], "a Golem");
				}
			}
		}
	}
	else
	{
		getPosition().setDirection(Direction::STAY);
	}

	if (is_allowed_to_move)
	{
		char prev_char = ' ';

		for (int i = 0; i < 2; ++i)
		{
			if (!players[i].getIfDead() && players[i].getPosition().getX() == old_x &&
			    players[i].getPosition().getY() == old_y)
			{
				prev_char = players[i].getDisplayChar();
				break;
			}
		}

		board.setCharAt(old_x, old_y, prev_char);
		if (board.areColorsEnabled())
		{
			setTextColor(getColorForChar(prev_char));
		}

		board.drawChar(old_x, old_y, prev_char);
		if (board.areColorsEnabled())
		{
			setTextColor(Color::WHITE);
		}

		getPosition().move();
		board.setCharAt(new_x, new_y, getDisplayChar());
		if (board.areColorsEnabled())
		{
			setTextColor(getColorForChar(getDisplayChar()));
		}

		board.drawChar(new_x, new_y, getDisplayChar());
		if (board.areColorsEnabled())
		{
			setTextColor(Color::WHITE);
		}
	}
}

void Golem::save(std::ostream& out) const
{
	GameObject::save(out);
}

void Golem::load(std::istream& in)
{
	GameObject::load(in);
}
