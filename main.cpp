#include "Game.h"
#include "GameByFile.h"
#include "GameByKeys.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	Game* game = nullptr;
	bool is_load_mode = false;
	bool is_save_mode = false;

	bool is_silent_mode = false;

	if (argc > 1)
	{
		std::string arg1 = argv[1];

		if (arg1 == "-load")
		{
			is_load_mode = true;
		}
		else if (arg1 == "-save")
		{
			is_save_mode = true;
		}

		if (argc > 2)
		{
			std::string arg2 = argv[2];

			if (arg2 == "-silent")
			{
				if (is_load_mode)
				{
					is_silent_mode = true;
				}
			}
		}
	}

	if (is_load_mode)
	{
		game = new GameByFile("adv-world.steps", is_silent_mode);
	}
	else
	{
		game = new GameByKeys(is_save_mode);
	}

	if (game)
	{
		game->run();
		delete game;
	}
	else
	{
		std::cerr << "Failed to initialize game." << std::endl;

		return 1;
	}

	return 0;
}
