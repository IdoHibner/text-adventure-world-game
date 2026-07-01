# Text Adventure World Game

C++ console based text adventure game where two players cooperate to complete multi level puzzle rooms. 
The game focuses on object-oriented design, file based level loading, interactive console gameplay, and support for save/load and replay modes.

## Main Features

- Two player cooperative gameplay with separate controls for each player
- Multi level board system loaded from `.screen` files
- Puzzle mechanics including keys, switches, doors, riddles, bombs, torches, springs, portals, movable obstacles, and enemies
- Shared score system with rewards for collecting cookies, solving riddles, bombing enemies, and earning bonus points for faster level completion
- Player health system and level restart flow
- Save/load support for game progress
- Recording and replay support using command line modes
- Optional colored console display

## Technologies

- C++17
- Object-Oriented Programming
- Visual Studio / MSVC
- Windows Console API

## Project Structure

```text
.
├── main.cpp
├── Game.*              # Core game flow and shared game logic
├── GameByKeys.*        # Interactive keyboard-based gameplay
├── GameByFile.*        # Replay/load mode based on recorded actions
├── Board.*             # Board loading, rendering, legend and visibility handling
├── Player.*            # Player movement, inventory, health and interactions
├── GameObject.*        # Base class for game objects
├── Key / Door / Bomb / Torch / Spring / Switch / Riddle / Obstacle / Golem
├── adv-world_*.screen  # Level files
├── *.txt               # Menus, instructions, riddles and end screens
└── Text_Adventure_World_Game.sln / .vcxproj
```

## How to Run

1. Clone or download the repository.
2. Open `Text_Adventure_World_Game.sln` in Visual Studio.
3. Build the solution, preferably in `x64` mode.
4. Run the project from Visual Studio.

The repository does not include a pre-built `.exe` file. The executable is generated only after building the project.

## Command-Line Modes

After building the project, the generated executable can also be run from the command line. Make sure the required `.txt` and `.screen` files are accessible from the running directory.

```bash
Text_Adventure_World_Game.exe
```
Runs the game normally in interactive keyboard mode.

```bash
Text_Adventure_World_Game.exe -save
```
Runs the game and records the session into `adv-world.steps` and `adv-world.result`.

```bash
Text_Adventure_World_Game.exe -load
```
Replays a recorded session from `adv-world.steps`.

```bash
Text_Adventure_World_Game.exe -load -silent
```
Runs replay mode silently and compares the replay output with `adv-world.result`.

## Controls

| Action | Player 1 | Player 2 |
|---|---:|---:|
| Move up | W | I |
| Move down | X | M |
| Move left | A | J |
| Move right | D | L |
| Stay | S | K |
| Drop item | E | O |

Additional keys:

- `ESC` - pause menu
- `C` - toggle color mode
- `S` - save game
- `L` - load saved game from the main menu

## Notes

This project uses Windows specific console functions such as `_getch`, `Sleep`, `gotoxy`, and `system("cls")`, so it is intended to be built and run on Windows with Visual Studio / MSVC.
