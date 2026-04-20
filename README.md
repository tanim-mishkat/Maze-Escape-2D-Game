# Maze Runner

Maze Runner is a 2D maze escape game built with C++, OpenGL, and FreeGLUT. The project uses procedurally generated tile mazes, a sidebar HUD, keyboard and mouse driven menus, and plain-text persistence for settings and high scores.

## Project Overview

The game runs as a five-stage campaign with per-level maze sizes ranging from `15x21` to `29x41`. Each stage generates a dense, deterministic maze with one guaranteed route from the start to the exit plus many misleading branches, dead ends, rooms, limited late-stage loops, and several deliberate 3-way or 4-way decision junctions. Score is derived from completion time, so faster clears earn higher totals.

## Gameplay Summary

- Start from the stage entrance and move tile by tile through a procedurally generated maze.
- Reach the exit as quickly as possible to maximize your score.
- Explore carefully: side paths can lead to dead ends, rooms, loops, and misleading junctions.
- Finish all five stages as quickly as possible.

## Features

- Procedural maze generation with deterministic per-level specifications
- One guaranteed start-to-exit solution path per stage
- Dense branch layouts with dead ends, rooms, and limited late-stage loops
- Multiple 3-way and 4-way intersections per stage to create route uncertainty
- Score, timer, and high-score systems
- Hold-to-move keyboard input for both WASD and arrow keys
- Mouse-clickable menus, overlays, and settings UI
- Editable player name saved between sessions
- Animated exit flag and styled HUD/overlay panels

## Tech Stack

- C++11
- OpenGL
- FreeGLUT

## Campaign Stages

| Stage | Name | Grid | Decision Junctions | Par Time |
|-------|------|------|--------------------|----------|
| 1 | Orientation Grid | 15x21 | 4 | 35s |
| 2 | Archive Drift | 17x25 | 4 | 42s |
| 3 | Foundry Knots | 19x29 | 4 | 50s |
| 4 | Blackout Loop | 25x35 | 5 | 68s |
| 5 | Final Nexus | 29x41 | 6 | 82s |

## Project Structure

```text
.
|-- core/          # game controller, state, config, shared types
|-- gameplay/      # player, maze, collision, level loading, maze generator
|-- render/        # OpenGL drawing, colors, bitmap text
|-- ui/            # HUD, menus, overlays
|-- data/          # level specifications and high-score persistence
|-- utils/         # coordinate conversion and timer utilities
|-- main.cpp       # GLUT setup and callback registration
|-- build.bat      # Windows build script
|-- libfreeglut.dll
|-- maze_highscores.txt
`-- maze_settings.txt
```

## Dependencies

- A C++11-compatible compiler
- FreeGLUT headers and library
- OpenGL and GLU system libraries
- On Windows, the current build links against `winmm` and `gdi32`

The repository also includes `libfreeglut.dll` in the root for runtime use on Windows.

## Build and Run

The repository includes a Windows build script:

```bat
build.bat
maze_runner.exe
```

Debug build:

```bat
build.bat debug
maze_runner.exe
```

`build.bat` first looks for `C:\msys64\ucrt64\bin\g++.exe` and falls back to `g++` from `PATH`.

Manual compile command used by the script:

```bat
g++ -o maze_runner.exe main.cpp core/game.cpp core/gamestate.cpp gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp gameplay/generator.cpp render/renderer.cpp render/text.cpp ui/hud.cpp ui/menu.cpp ui/overlay.cpp data/highscore.cpp data/leveldata.cpp utils/coords.cpp utils/timer.cpp -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -std=c++11 -O2 -Wall -Wextra -Wno-unused-parameter
```

## Controls

### Gameplay

- `W`, `A`, `S`, `D` or arrow keys: move tile by tile, including hold-to-move
- `P`: pause
- `Esc`: pause during gameplay
- `M`: return to the main menu
- `R`: restart the run during active play

### Menus and Overlays

- Main menu: up/down arrows, `1`-`4`, `Enter`, or mouse
- Pause menu: `P` or `Esc` resume, `R` restart current level, `M` main menu, `Q` quit
- Level clear: `N` or `Enter` continue, or click the continue button
- End screen: `R` replay, `M` menu, `Q` quit, or click a button

### Settings

- `E` or `Enter`: begin editing the player name
- Text input: type the new name
- `Backspace`: delete while editing
- `Enter`: save while editing
- `Esc`: cancel editing, or go back when not editing
- Mouse: click `Edit`, `Save`, `Cancel`, `Back`, or the name field

## Game Objective and Rules

- The player starts at the stage-specific entrance defined in `data/leveldata.cpp`.
- Reaching the exit wins the stage immediately.
- Walls block movement.
- The generator deliberately introduces several 3-way and 4-way intersections on every stage.
- Stage score is based on completion time relative to that stage's par time.
- Clearing stage 5 wins the campaign.

## Configuration Notes

- Core game constants live in `core/config.h`.
- Stage tuning data lives in `data/leveldata.cpp`.
- Player name is stored in `maze_settings.txt` as `NAME=<value>`.
- High scores are stored in `maze_highscores.txt` as `name|score`.
- Changing compile-time constants or level definitions requires rebuilding the executable.

## Screenshots and Assets

This repository does not include a dedicated screenshot, texture, or audio asset folder. The maze, player, exit flag, HUD, menus, and overlays are drawn procedurally in the `render/` and `ui/` modules.

## Known Limitations

- The included build automation is Windows-only (`build.bat`).
- Rendering uses OpenGL immediate mode and GLUT bitmap fonts.
- Campaign length is a compile-time constant (`5` stages), while individual stage grid sizes come from `data/leveldata.cpp`.
- Runtime data is written to text files in the repository root.
- `R` is state-dependent: in the pause menu it restarts the current level, while during active play and on end screens it restarts the run.
- No audio system is implemented in the current codebase.

## Contribution Notes

There is no `CONTRIBUTING.md` in the repository. If you extend the project, keep the gameplay rules, controls, and configuration described here aligned with `core/game.cpp`, `core/config.h`, `data/leveldata.cpp`, and the UI text in `ui/`.

## License

No license file is present in this repository.
