# MAZE RUNNER

MAZE RUNNER is a 2D procedural maze escape game built with C++11, OpenGL, and FreeGLUT. The project combines deterministic maze generation, tile-based movement, time-based scoring, keyboard and mouse driven menus, and plain-text persistence in a modular desktop game codebase.

> Repository status note
> This repository includes runnable Windows executables in the root directory. It also includes source code, `build.bat`, and MSYS2/VS Code build configuration. However, a syntax-only verification of the checked-in source on April 20, 2026 found compile issues in `core/game.cpp`, so the bundled executable is currently the most reliable path for evaluation.

## Table of Contents

- [Quick Start](#quick-start)
- [Introduction](#introduction)
- [Problem Statement](#problem-statement)
- [Objectives](#objectives)
- [Key Features](#key-features)
- [Demo / Screenshots](#demo--screenshots)
- [Tech Stack](#tech-stack)
- [Campaign Overview](#campaign-overview)
- [Architecture / How It Works](#architecture--how-it-works)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Setup](#setup)
- [Build Instructions](#build-instructions)
- [Run Instructions](#run-instructions)
- [Usage Guide](#usage-guide)
- [Project Walkthrough](#project-walkthrough)
- [Controls](#controls)
- [Configuration](#configuration)
- [Assets and Resources](#assets-and-resources)
- [Deployment / Distribution Notes](#deployment--distribution-notes)
- [Testing and Verification](#testing-and-verification)
- [Limitations](#limitations)
- [Future Improvements](#future-improvements)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [Contributors / Author](#contributors--author)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Quick Start

If you want to review or play the project immediately, run the bundled executable from the repository root:

```powershell
.\maze_runner.exe
```

Keep `maze_runner.exe` and `libfreeglut.dll` in the same directory. The game reads and writes `maze_settings.txt` and `maze_highscores.txt` in the working directory.

## Introduction

MAZE RUNNER is a small desktop game project centered on procedural maze generation. The player moves tile by tile through a five-stage campaign, trying to reach each exit as quickly as possible. Each stage uses a fixed design specification and seed, so the maze content is generated rather than hand-authored, but still follows a controlled difficulty curve.

From an undergraduate project perspective, the repository demonstrates several core topics in a compact codebase:

- modular C++ program structure
- event-driven desktop programming with GLUT
- procedural content generation
- path analysis and scoring logic
- basic persistence through text files
- game UI flow with menus, overlays, and HUD panels

## Problem Statement

Static mazes are easy to memorize and do not show much algorithmic depth. This project addresses that by generating challenging but fair mazes that still guarantee a valid route from the start to the exit. The goal is to create a replayable maze game while keeping the implementation understandable, modular, and suitable for coursework review.

## Objectives

- Build a complete desktop maze game with a clear gameplay loop.
- Generate mazes procedurally instead of storing fixed layouts.
- Control difficulty through measurable maze properties such as path length, dead ends, turns, and junctions.
- Provide a simple user interface for playing, pausing, renaming the player, and finishing the campaign.
- Persist player settings and high scores between sessions.
- Organize the code into focused modules that are easy to study and maintain.

## Key Features

- Five-stage campaign with increasing maze size and difficulty.
- Deterministic procedural maze generation driven by stage-specific specifications.
- Candidate-based maze selection using path, branch, turn, dead-end, and junction analysis.
- Optional layout enhancement through rooms, loops, and added decision junctions.
- Tile-based player movement with held-key repeat support for both WASD and arrow keys.
- Mouse-aware menu screens and overlays alongside keyboard navigation.
- HUD panel showing player name, stage, timer, score, grid size, par time, and best score.
- Persistent player name settings in `maze_settings.txt`.
- Persistent top-five campaign scores in `maze_highscores.txt`.
- Entire visual presentation drawn procedurally in OpenGL without a dedicated asset folder.

## Demo / Screenshots

Use the following placeholders when adding screenshots later:

| Placeholder | Intended capture |
| --- | --- |
| `{{SCREENSHOT_MAIN_MENU}}` | Main menu with the four primary options |
| `{{SCREENSHOT_HOW_TO_PLAY}}` | How to Play screen |
| `{{SCREENSHOT_SETTINGS_SCREEN}}` | Settings screen with player-name editing |
| `{{SCREENSHOT_GAMEPLAY_STAGE_1}}` | Early-stage gameplay with HUD visible |
| `{{SCREENSHOT_GAMEPLAY_STAGE_5}}` | Late-stage gameplay showing increased maze complexity |
| `{{SCREENSHOT_PAUSE_MENU}}` | Pause overlay during gameplay |
| `{{SCREENSHOT_LEVEL_CLEAR_OVERLAY}}` | Level-complete overlay after reaching an exit |
| `{{SCREENSHOT_CAMPAIGN_VICTORY}}` | Final victory screen with high-score list |

## Tech Stack

| Area | Technology |
| --- | --- |
| Programming language | C++11 |
| Rendering | OpenGL |
| Windowing and input | FreeGLUT |
| Graphics support libraries | GLU, Windows graphics/runtime libraries |
| Build system | Windows batch script (`build.bat`) |
| Toolchain configuration | MSYS2 UCRT64 + MinGW-w64, VS Code workspace settings |
| Persistence | Plain-text files |

## Campaign Overview

The campaign data comes from `data/leveldata.cpp`.

| Stage | Name | Grid | Candidate Count | Junction Target | Par Time |
| --- | --- | --- | ---: | ---: | ---: |
| 1 | Orientation Grid | 15 x 21 | 28 | 4 | 35 s |
| 2 | Archive Drift | 17 x 25 | 34 | 4 | 42 s |
| 3 | Foundry Knots | 19 x 29 | 40 | 4 | 50 s |
| 4 | Blackout Loop | 25 x 35 | 52 | 5 | 68 s |
| 5 | Final Nexus | 29 x 41 | 64 | 6 | 82 s |

## Architecture / How It Works

### Runtime flow

```text
main.cpp
  -> Game
     -> Level / Maze / Player / Timer
     -> MazeGenerator
     -> Renderer / TextRenderer
     -> HUD / Menu / Overlay
     -> HighScoreManager / SettingsManager
```

### Maze generation flow

The maze system is implemented mainly in `gameplay/generator.cpp` and `gameplay/generator_analysis.cpp`.

1. Generate multiple candidate layouts using depth-first search carving on a wall-filled grid.
2. Build a critical path from the start position to the selected exit.
3. Analyze each candidate using measurable properties such as path length, turns, dead ends, branch depth, detour potential, and decision junction count.
4. Score the candidates with weights from `core/config.h`.
5. Select the best candidate that satisfies the stage goals, or the best available candidate if none meet all thresholds.
6. Enhance the final layout with optional rooms, loops, and extra junctions while preserving playability.

### Game-state flow

The current UI flow visible in the source is:

- Main Menu
- How to Play
- Settings
- Playing
- Paused
- Level Cleared
- Campaign Won

The main menu starts a new five-stage run from stage 1. The current UI does not expose a separate stage-select screen.

## Project Structure

```text
MAZE RUNNER/
|-- core/                  # game state, config, input, settings
|-- gameplay/              # maze, player, collision, levels, generator
|-- render/                # OpenGL drawing and text rendering
|-- ui/                    # menus, HUD, overlays
|-- data/                  # level specifications and high-score storage
|-- utils/                 # coordinates and timer utilities
|-- .vscode/               # local editor and task configuration
|-- build.bat              # Windows build script
|-- main.cpp               # program entry point and GLUT callbacks
|-- maze_runner.exe        # bundled Windows executable
|-- libfreeglut.dll        # bundled runtime dependency
|-- maze_settings.txt      # saved player name
`-- maze_highscores.txt    # saved top campaign scores
```

## Prerequisites

### To run the bundled executable

- Windows environment
- `maze_runner.exe` and `libfreeglut.dll` in the same folder
- permission to create or update text files in the working directory

### To work with the source code

- MSYS2 UCRT64 or another Windows C++ toolchain with `g++`
- FreeGLUT headers and libraries
- OpenGL and GLU system libraries
- A terminal that can run `build.bat`

The repository's editor settings point to `C:\msys64\ucrt64\bin\g++.exe` and `C:\msys64\ucrt64\include`.

## Installation

1. Download or clone the repository.
2. Keep the project files together in a writable directory.
3. If you only need to evaluate the game, use the bundled executable.
4. If you want to work on the code, install the required compiler and FreeGLUT development libraries first.

## Setup

The project expects its runtime files in the repository root:

- `maze_runner.exe` for launching the game
- `libfreeglut.dll` for the Windows runtime dependency
- `maze_settings.txt` for the saved player name
- `maze_highscores.txt` for the saved high-score table

If you move the executable elsewhere, keep the runtime DLL with it and ensure the directory remains writable for settings and score persistence.

## Build Instructions

The intended Windows build entry point is:

```bat
build.bat
```

For a debug-oriented build:

```bat
build.bat debug
```

According to `build.bat`, the script first looks for:

```text
C:\msys64\ucrt64\bin\g++.exe
```

If that compiler is not found, it falls back to `g++` on `PATH`.

### Important note about the current source tree

The repository clearly includes source files, build tooling, and editor configuration for building from source. However, the checked-in source did not pass a syntax-only verification on April 20, 2026 because of issues in `core/game.cpp`. In the repository's current state, source builds should therefore be treated as an intended workflow that still needs cleanup, not as a guaranteed clean build path.

## Run Instructions

### Recommended path

Run the bundled executable from the project root:

```powershell
.\maze_runner.exe
```

### If you repair and rebuild the source

Run the generated executable from the root directory so the game can continue using the existing settings and high-score files.

## Usage Guide

1. Launch the game and choose `Start New Game` from the main menu.
2. Move through the maze using WASD or the arrow keys.
3. Reach the exit flag tile before the timer window runs out to maximize score.
4. Continue through all five stages to finish the campaign.
5. Open `Settings` from the main menu if you want to change the saved player name.
6. Complete the full campaign to record a final score in the high-score table.

## Project Walkthrough

### 1. Main Menu

`{{SCREENSHOT_MAIN_MENU}}`

The main menu provides the current entry points visible in the codebase: Start New Game, How to Play, Settings, and Quit. It supports keyboard selection, Enter confirmation, and mouse hover/click interaction.

### 2. How to Play

`{{SCREENSHOT_HOW_TO_PLAY}}`

This screen explains the objective, controls, and the maze design idea before play begins. It acts as the in-game instruction page for new users.

### 3. Settings

`{{SCREENSHOT_SETTINGS_SCREEN}}`

The settings screen allows the player name to be edited and saved. That name is reused in the HUD and in the persisted high-score list.

### 4. Gameplay

`{{SCREENSHOT_GAMEPLAY_STAGE_1}}`

During play, the left side shows the maze and the right side shows the HUD. The player moves one tile at a time, the timer runs continuously, and the stage score depends on completion speed relative to the configured par time.

`{{SCREENSHOT_GAMEPLAY_STAGE_5}}`

Later stages increase the grid size and maze complexity, introducing longer routes and more decision points.

### 5. Pause State

`{{SCREENSHOT_PAUSE_MENU}}`

The pause menu can be opened during gameplay and offers resume, restart level, return to main menu, and quit actions.

### 6. Level Completion

`{{SCREENSHOT_LEVEL_CLEAR_OVERLAY}}`

After reaching an exit, the game displays the stage-clear overlay with the recorded stage time and stage score before continuing to the next level.

### 7. Campaign Completion

`{{SCREENSHOT_CAMPAIGN_VICTORY}}`

After the fifth stage, the game shows a victory overlay and the persisted top campaign scores.

## Controls

### Gameplay controls

| Input | Action |
| --- | --- |
| `W`, `A`, `S`, `D` | Move tile by tile |
| Arrow keys | Move tile by tile |
| Hold movement key | Trigger repeated movement after a short delay |
| `P` | Pause during gameplay |
| `Esc` | Pause during gameplay |
| `M` | Return to main menu |
| `R` | Restart the current run while actively playing |
| `N` or `Enter` | Continue after clearing a stage |

### Menu and overlay controls

| Context | Input | Action |
| --- | --- | --- |
| Main Menu | Up / Down | Change selection |
| Main Menu | `1` to `4` | Jump to one of the menu options |
| Main Menu | `Enter` | Confirm selection |
| How to Play | `Enter` or `Esc` | Return to main menu |
| Settings | `E` or `Enter` | Start editing the player name |
| Settings | `Backspace` | Delete one character while editing |
| Settings | `Enter` | Save edited name |
| Settings | `Esc` | Cancel edit or go back |
| Pause Menu | `P` or `Esc` | Resume |
| Pause Menu | `R` | Restart current level |
| Pause Menu | `M` | Return to main menu |
| Pause Menu | `Q` | Quit |
| Victory Overlay | `R` | Replay the run |
| Victory Overlay | `M` | Return to menu |
| Victory Overlay | `Q` | Quit |

Mouse interaction is also implemented for the main menu, settings screen, pause menu, level-clear button, and victory overlay buttons.

## Configuration

| File | Purpose |
| --- | --- |
| [`core/config.h`](core/config.h) | Shared gameplay, timing, rendering, and scoring constants |
| [`data/leveldata.cpp`](data/leveldata.cpp) | Stage names, grid sizes, seeds, candidate counts, room budgets, loop budgets, and par times |
| [`maze_settings.txt`](maze_settings.txt) | Saved player name in `NAME=<value>` format |
| [`maze_highscores.txt`](maze_highscores.txt) | Saved top-five campaign scores in `name|score` format |

Key configuration areas already exposed by the source include:

- window size and board layout metrics
- hold-to-move delay and repeat interval
- scoring window multiplier and divisor
- maze-evaluation weights used during candidate scoring
- stage-level difficulty parameters

Changing compile-time constants requires rebuilding the executable.

## Assets and Resources

This repository does not include a dedicated texture, sprite, audio, or screenshot asset directory. The maze floor, walls, player marker, exit marker, HUD, menus, and overlays are all drawn procedurally in the rendering and UI modules.

Bundled runtime resources include:

- `maze_runner.exe`
- `libfreeglut.dll`
- sample persistence files for settings and high scores

## Deployment / Distribution Notes

This project is structured as a lightweight Windows desktop application rather than an installer-based product.

For distribution, the minimum practical set is:

- `maze_runner.exe`
- `libfreeglut.dll`

The save files can either be shipped with the project or created by the game at runtime, as long as the working directory is writable.

The repository root also contains additional Windows executables such as `main.exe` and `maze_validator.exe`, but their roles are not clearly documented by the source or build files. This README treats `maze_runner.exe` as the primary runnable artifact.

## Testing and Verification

The repository does not include an automated test suite, CI pipeline, or dedicated test directory. Based on the code and included artifacts, the project appears to rely mainly on manual verification through gameplay.

Current evidence available in the repository:

- source modules are organized across `core/`, `gameplay/`, `render/`, `ui/`, `data/`, and `utils/`
- a Windows build script and VS Code/MSYS2 configuration are present
- prebuilt Windows executables are included in the root directory
- settings and high-score persistence files are included

Additional verification performed during documentation preparation:

- a syntax-only compile check against the checked-in source did not pass because of errors in `core/game.cpp`

## Limitations

- The repository is Windows-centered in both build tooling and distributed artifacts.
- The current checked-in source has compile inconsistencies, so source builds are not presently a fully clean path.
- Rendering uses immediate-mode OpenGL and GLUT bitmap text rather than a modern rendering pipeline.
- The game stores settings and scores as plain text in the working directory.
- No separate audio system, asset pipeline, or installer is present.
- The current UI starts a new campaign from stage 1 and does not expose a dedicated stage-select screen.

## Future Improvements

- Repair the current source/build inconsistencies and revalidate `build.bat`.
- Add automated tests for maze generation, scoring, collision, and persistence.
- Add a dedicated high-score screen and broader post-run statistics.
- Introduce packaging that separates build artifacts from user save data.
- Modernize the rendering path for easier optimization and future extension.
- Add configurable difficulty or gameplay modes without modifying source files.

## Troubleshooting

| Issue | Likely cause | Suggested action |
| --- | --- | --- |
| `maze_runner.exe` does not start | Missing runtime DLL | Keep `libfreeglut.dll` beside the executable |
| `build.bat` cannot find the compiler | MSYS2 or `g++` is not installed or not on `PATH` | Install the expected toolchain or expose `g++` on `PATH` |
| Source build fails immediately | Current source tree has compile issues | Use the bundled executable for evaluation, or fix the source before rebuilding |
| Name or score changes are not saved | Working directory is not writable | Run the game from a writable directory |
| Display or rendering problems occur | Graphics/OpenGL support issue | Try a machine with working OpenGL support and updated graphics drivers |

## FAQ

### Does the game generate a new maze every run?

The mazes are procedurally generated, but the stage definitions include fixed seeds and generation rules. That means the layouts are generated algorithmically while still following a deterministic stage setup.

### When is the score saved?

The current code saves high scores when the full five-stage campaign is completed, not after each individual stage.

### Where are player names and scores stored?

The player name is stored in `maze_settings.txt`, and the top campaign scores are stored in `maze_highscores.txt`.

### Does the project depend on external art or audio assets?

No. The visuals are generated procedurally in code, and no audio system is implemented in the current repository.

## Contributors / Author

Explicit authorship information is not included in the current repository metadata. Before final academic submission, this section should be updated with:

- student name(s)
- student ID(s)
- course and section
- institution
- submission date

## License

No license file is present in this repository.

## Acknowledgements

- OpenGL for the rendering foundation
- FreeGLUT for the windowing, input, and event loop support
- MSYS2 / MinGW-w64 for the Windows-oriented development toolchain reflected in the project configuration
