#pragma once
#include "Board.h"
#include "GameObject.h"
#include "Key.h"
#include <vector>
#include "Bomb.h"
#include "Door.h"
#include "Inventory.h"
#include "Obstacle.h"
#include "Spring.h"
#include "Torch.h"

class Game;

static const int NUM_BUTTONS = 7;

class Player : public GameObject
{
	friend class Game;

	int player_id;
	Inventory holding_item = Inventory::NONE;
	char buttons[NUM_BUTTONS];
	int health = 5;
	bool is_dead = false;

	Player* teamate = nullptr;

	Board& board;
	Door& door;
	std::vector<Key>& keys;
	std::vector<Bomb>& bombs;
	std::vector<Spring>& springs;
	std::vector<Obstacle>& obstacles;
	std::vector<Torch>& torches;

	int held_key_index = -1;
	int held_bomb_index = -1;
	int held_torch_index = -1;

	bool finished_level = false;

	bool is_compressing = false;
	bool is_launching = false;
	bool is_spring_releasing = false;
	int chars_compressed = 0;

	Spring* active_spring = nullptr;
	int current_speed = 1;
	int spring_force_moves_left = 0;
	bool spring_first_move = true;
	bool is_opposite_direction = false;
	bool is_waiting_on_spring = false;

	Obstacle* active_obstacle = nullptr;
	int loaded_spring_index = -1;
	int loaded_obstacle_index = -1;

	void drawAtCurrentPosition(char c);

  public:
	enum KeyIndex
	{
		KEY_UP = 0,
		KEY_DOWN = 1,
		KEY_LEFT = 2,
		KEY_RIGHT = 3,
		KEY_STAY = 4,
		KEY_DROP = 5
	};

	Player(int x, int y, char c, int id, const char (&the_buttons)[NUM_BUTTONS], Board& the_board,
	       std::vector<Key>& the_keys, Door& the_door, std::vector<Bomb>& the_bombs, std::vector<Spring>& the_springs,
	       std::vector<Obstacle>& the_obstacles, std::vector<Torch>& the_torches)
	    : GameObject(x, y, c), player_id(id), board(the_board), door(the_door), keys(the_keys), bombs(the_bombs),
	      springs(the_springs), obstacles(the_obstacles), torches(the_torches)
	{
		for (int i = 0; i < NUM_BUTTONS; ++i)
		{
			buttons[i] = the_buttons[i];
		}
	}

	void setTeamate(Player* p)
	{
		teamate = p;
	}

	bool isFinishedLevel() const
	{
		return finished_level;
	}

	void resetHealth()
	{
		health = 5;
		is_dead = false;
	}

	int getPlayerHealth() const
	{
		return health;
	}

	int getPlayerId() const
	{
		return player_id;
	}

	void setDead()
	{
		is_dead = true;
	}

	bool getIfDead() const
	{
		return is_dead;
	}

	bool isHoldingTorch() const
	{
		return holding_item == Inventory::TORCH;
	}

	void pickUpKey(int& new_x, int& new_y);
	void pickUpTorch(int& new_x, int& new_y);
	void pickUpBomb(int& new_x, int& new_y);
	void moveObstacle(int& new_x, int& new_y);
	void handleSpring(int& new_x, int& new_y);
	void dropKeyOrTorch();
	void dropBomb();
	Point getDropPointForItem() const;
	void receiveSpringPush(Spring* spring, int speed, int force_left);
	void stopSpring();
	void handleButtonPress(char button);
	void resetPlayer();
	bool takeDamage();
	void updateVision();
	void save(std::ostream& out) const;
	void load(std::istream& in);
	void restoreObjectReferences();
};
