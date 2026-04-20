# Maze Runner

Maze Runner is a 2D maze escape game built with C++, OpenGL, and FreeGLUT. The project uses procedurally generated tile mazes, a sidebar HUD, keyboard and mouse driven menus, and plain-text persistence for settings and high scores.

## Project Overview

The game runs as a five-stage campaign on a fixed 12x16 grid. Each stage generates a maze with a single valid route from the start to the locked exit. The player must find the key placed along that route, avoid traps and blocking obstacles on misleading branches, and finish quickly to maximize score.

## Gameplay Summary

- Start from the fixed spawn point and move one tile at a time.
- Find the key before attempting to leave through the exit.
- Explore carefully: side paths can lead to dead ends, traps, or blocked routes.
- Finish all five stages before running out of lives.

## Features

- Procedural maze generation with deterministic per-level specifications
- One guaranteed start-to-exit solution path per stage
- Dead ends, traps, and obstacles placed to increase navigation difficulty
- Key-and-locked-exit objective on every stage
- Score, timer, life, and high-score systems
- Mouse-clickable menus, overlays, and settings UI
- Editable player name saved between sessions
- Animated exit flag, traps, key, and styled HUD/overlay panels

## Tech Stack

- C++11
- OpenGL
- FreeGLUT

## Campaign Stages

| Stage | Name | Traps | Obstacles | Par Time |
|-------|------|-------|-----------|----------|
| 1 | Orientation Grid | 3 | 1 | 30s |
| 2 | Archive Drift | 4 | 2 | 28s |
| 3 | Foundry Knots | 5 | 2 | 26s |
| 4 | Blackout Loop | 6 | 3 | 24s |
| 5 | Final Nexus | 7 | 3 | 22s |

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

- `W`, `A`, `S`, `D` or arrow keys: move one tile
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

- The player starts at row `9`, column `1`.
- Every stage requires collecting the key before the locked exit can be used.
- Walls and brown obstacles block movement.
- Red traps deduct `1` life and `125` points, then respawn the player at the start.
- Picking up a key awards `200` points.
- Clearing a stage awards a level bonus based on stage data, clear time, and remaining lives.
- Losing all lives ends the run. Clearing stage 5 wins the campaign.

## Configuration Notes

- Core game constants live in `core/config.h`.
- Stage tuning data lives in `data/leveldata.cpp`.
- Player name is stored in `maze_settings.txt` as `NAME=<value>`.
- High scores are stored in `maze_highscores.txt` as `name|score`.
- Changing compile-time constants or level definitions requires rebuilding the executable.

## Screenshots and Assets

This repository does not include a dedicated screenshot, texture, or audio asset folder. The maze, player, traps, key, exit flag, HUD, menus, and overlays are drawn procedurally in the `render/` and `ui/` modules.

## Known Limitations

- The included build automation is Windows-only (`build.bat`).
- Rendering uses OpenGL immediate mode and GLUT bitmap fonts.
- Grid size and campaign length are compile-time constants (`12x16`, `5` stages).
- Runtime data is written to text files in the repository root.
- `R` is state-dependent: in the pause menu it restarts the current level, while during active play and on end screens it restarts the run.
- No audio system is implemented in the current codebase.

## Contribution Notes

There is no `CONTRIBUTING.md` in the repository. If you extend the project, keep the gameplay rules, controls, and configuration described here aligned with `core/game.cpp`, `core/config.h`, `data/leveldata.cpp`, and the UI text in `ui/`.

## License

No license file is present in this repository.
