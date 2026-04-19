# Maze Runner - Refactored Architecture

A 2D tile-based maze game built with C++ and OpenGL/FreeGLUT.

## Project Structure

```
maze-runner/
├── main.cpp                    # Entry point, GLUT setup
├── core/                       # Core game systems
│   ├── config.h               # All game constants
│   ├── types.h                # Enums and basic structs
│   ├── gamestate.h/cpp        # Game state container
│   ├── game.h/cpp             # Main game controller
├── gameplay/                   # Gameplay mechanics
│   ├── player.h/cpp           # Player entity
│   ├── maze.h/cpp             # Maze grid management
│   ├── collision.h/cpp        # Collision detection
│   ├── level.h/cpp            # Level loading
├── render/                     # Rendering systems
│   ├── colors.h               # Color constants
│   ├── renderer.h/cpp         # OpenGL drawing primitives
│   ├── text.h/cpp             # Text rendering
├── ui/                         # User interface
│   ├── hud.h/cpp              # In-game HUD
│   ├── menu.h/cpp             # Title screen
│   ├── overlay.h/cpp          # Level clear/game over screens
├── data/                       # Data management
│   ├── highscore.h/cpp        # High score persistence
│   ├── leveldata.h/cpp        # Maze definitions
└── utils/                      # Utilities
    ├── coords.h/cpp           # Coordinate conversions
    └── timer.h/cpp            # Time tracking

```

## Module Descriptions

### Core
- **config.h**: Centralized constants (grid size, player size, game settings)
- **types.h**: Enums (GameState, TileType) and basic structs (GridPos, ScreenPos)
- **gamestate.h/cpp**: Container for all mutable game state
- **game.h/cpp**: Main game controller, orchestrates all systems

### Gameplay
- **player.h/cpp**: Player position, movement flags, grid/screen conversion
- **maze.h/cpp**: Grid storage, tile queries, protected path marking
- **collision.h/cpp**: Movement validation, trap/exit detection
- **level.h/cpp**: Level loading, random tile placement

### Render
- **colors.h**: Named color constants for consistent theming
- **renderer.h/cpp**: OpenGL primitives (rects, outlines) and game element rendering
- **text.h/cpp**: GLUT bitmap text with shadow effects

### UI
- **hud.h/cpp**: In-game sidebar with stats and controls
- **menu.h/cpp**: Three-step title screen (welcome, name, difficulty)
- **overlay.h/cpp**: Level clear and game over overlays

### Data
- **highscore.h/cpp**: File-based high score persistence
- **leveldata.h/cpp**: Static maze definitions and winning paths

### Utils
- **coords.h/cpp**: Grid ↔ screen coordinate conversions
- **timer.h/cpp**: Game timer and animation time tracking

## Build Instructions

### Windows (MinGW)
```bash
build.bat
```

### Manual Build
```bash
g++ -o maze_runner.exe main.cpp core/*.cpp gameplay/*.cpp render/*.cpp ui/*.cpp data/*.cpp utils/*.cpp -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -std=c++11 -O2
```

## Design Principles

1. **Separation of Concerns**: Rendering, logic, input, and data are isolated
2. **No Deep Inheritance**: Simple structs and classes
3. **Value Types**: GridPos, ScreenPos are lightweight value types
4. **Explicit Grid Logic**: Coordinate conversions are centralized
5. **Minimal Globals**: Only the game instance in main.cpp
6. **Beginner-Readable**: Clear naming, no template metaprogramming

## Performance Characteristics

- **Grid Lookup**: O(1) array access
- **Collision**: O(1) tile check
- **Rendering**: O(tiles) = O(192) per frame
- **Movement**: O(1) tile-based, no physics
- **Memory**: ~50KB static data, minimal heap usage

## Gameplay Features

- Three difficulty levels (Easy, Medium, Hard)
- Tile-based movement (WASD or arrow keys)
- Protected winning path (obstacles never block solution)
- Trap system (costs score and lives)
- Timer-based scoring with bonuses
- High score persistence with player names
- Three-step title screen flow

## Controls

- **WASD / Arrow Keys**: Move one tile
- **R**: Restart current run
- **T**: Return to title screen
- **N / Enter**: Next level (after clearing)
- **ESC**: Quit

## Future Extension Points

- Add new tile types in `types.h` (TileType enum)
- Add new levels in `data/leveldata.cpp`
- Implement smooth movement in `player.cpp`
- Add sound effects in `game.cpp` event handlers
- Replace immediate mode OpenGL in `renderer.cpp` with VBOs
