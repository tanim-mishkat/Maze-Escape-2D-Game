# Architecture Diagram

## System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                            │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  GLUT Callbacks → Game Instance → Render/Update     │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      core/Game                              │
│  ┌──────────────────────────────────────────────────────┐   │
│  │  • Orchestrates all systems                          │   │
│  │  • Handles input → game logic                        │   │
│  │  • Manages game state transitions                    │   │
│  │  • Owns: Player, Level, Timer, HighScores           │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
         │              │              │              │
         ▼              ▼              ▼              ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│  gameplay/   │ │   render/    │ │     ui/      │ │    data/     │
│              │ │              │ │              │ │              │
│  Player      │ │  Renderer    │ │  HUD         │ │  HighScore   │
│  Maze        │ │  Text        │ │  Menu        │ │  LevelData   │
│  Level       │ │  Colors      │ │  Overlay     │ │              │
│  Collision   │ │              │ │              │ │              │
└──────────────┘ └──────────────┘ └──────────────┘ └──────────────┘
         │                                                  │
         ▼                                                  ▼
┌──────────────────────────────────────────────────────────────┐
│                        utils/                                │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  Coords: Grid ↔ Screen conversions                    │  │
│  │  Timer: Time tracking and animation                   │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
```

## Data Flow

### Input Flow
```
User Input (Keyboard)
    │
    ▼
GLUT Callbacks (main.cpp)
    │
    ▼
Game::handleKeyDown/Up/Special
    │
    ├─► Title Screen Logic → Update GameState
    │
    └─► Gameplay Logic
            │
            ▼
        tryMoveByTile
            │
            ├─► Collision::canMoveTo (Maze)
            │
            ├─► Player::setPosition
            │
            └─► Check Trap/Exit
                    │
                    ├─► handleTrapHit → Update Lives/Score
                    │
                    └─► handleExitReached → Level Clear
```

### Render Flow
```
glutIdleFunc → update()
    │
    ▼
Game::update()
    │
    ├─► Timer::updateAnimationTime
    │
    └─► glutPostRedisplay()
            │
            ▼
        glutDisplayFunc → display()
            │
            ▼
        Game::render(windowWidth, windowHeight)
            │
            ├─► STATE_TITLE_SCREEN
            │       │
            │       └─► Menu::drawTitleScreen
            │               │
            │               ├─► Welcome Step
            │               ├─► Name Entry Step
            │               └─► Difficulty Step
            │
            └─► STATE_PLAYING / LEVEL_CLEARED / etc.
                    │
                    ├─► Renderer::drawFilledRect (background)
                    │
                    ├─► HUD::drawPanel (game canvas)
                    │
                    ├─► glPushMatrix / glTranslatef
                    │       │
                    │       ├─► Renderer::drawFloor
                    │       ├─► Renderer::drawMaze
                    │       ├─► Renderer::drawExit (animated)
                    │       ├─► Renderer::drawTraps (animated)
                    │       ├─► Renderer::drawObstacles
                    │       └─► Renderer::drawPlayer
                    │
                    ├─► HUD::drawHud (sidebar)
                    │
                    └─► Overlay::draw* (if needed)
```

## Module Dependencies

```
main.cpp
  └─► core/Game
        ├─► core/GameState
        ├─► core/Config
        ├─► core/Types
        │
        ├─► gameplay/Player
        │     └─► utils/Coords
        │
        ├─► gameplay/Level
        │     ├─► gameplay/Maze
        │     │     └─► data/LevelData
        │     └─► data/LevelData
        │
        ├─► gameplay/Collision
        │     ├─► gameplay/Maze
        │     └─► gameplay/Player
        │
        ├─► data/HighScore
        │
        ├─► utils/Timer
        │
        ├─► render/Renderer
        │     ├─► render/Colors
        │     └─► utils/Coords
        │
        ├─► render/Text
        │
        ├─► ui/HUD
        │     ├─► render/Renderer
        │     └─► render/Text
        │
        ├─► ui/Menu
        │     ├─► render/Renderer
        │     ├─► render/Text
        │     └─► ui/HUD
        │
        └─► ui/Overlay
              ├─► render/Renderer
              └─► render/Text
```

## Class Relationships

```
┌─────────────────┐
│  GameStateData  │  (struct - plain data)
│  ─────────────  │
│  + state        │
│  + lives        │
│  + score        │
│  + playerName   │
└─────────────────┘
        △
        │ owns
        │
┌─────────────────┐         ┌─────────────────┐
│      Game       │────────>│     Player      │
│  ─────────────  │  owns   │  ─────────────  │
│  + update()     │         │  + getGridPos() │
│  + render()     │         │  + setPosition()│
│  + handleInput()│         └─────────────────┘
└─────────────────┘
        │ owns
        │
        ├──────────────────>┌─────────────────┐
        │                   │      Level      │
        │                   │  ─────────────  │
        │                   │  + load()       │
        │                   │  + getMaze()    │
        │                   └─────────────────┘
        │                           │ owns
        │                           ▼
        │                   ┌─────────────────┐
        │                   │      Maze       │
        │                   │  ─────────────  │
        │                   │  + getTile()    │
        │                   │  + isBlocking() │
        │                   └─────────────────┘
        │
        ├──────────────────>┌─────────────────┐
        │                   │      Timer      │
        │                   │  ─────────────  │
        │                   │  + start()      │
        │                   │  + getElapsed() │
        │                   └─────────────────┘
        │
        └──────────────────>┌─────────────────┐
                            │ HighScoreManager│
                            │  ─────────────  │
                            │  + load()       │
                            │  + save()       │
                            │  + add()        │
                            └─────────────────┘
```

## Coordinate System

```
Screen Space (OpenGL)          Grid Space (Game Logic)
┌─────────────────► X          ┌─────────────────► col
│ (0,0)                         │ (0,0)
│                               │
│  ┌─────────────┐              │  ┌─────────────┐
│  │             │              │  │ [0][0]      │
│  │   Window    │              │  │             │
│  │             │              │  │             │
│  └─────────────┘              │  └─────────────┘
▼ Y                             ▼ row

Conversion: utils/Coords
  • gridToScreen(row, col) → (x, y)
  • screenToGrid(x, y) → (row, col)
  • playerScreenToGrid(playerX, playerY, size) → (row, col)
  • playerGridToScreen(row, col, padding) → (x, y)

Note: Y-axis is inverted between spaces
  Screen: Y increases downward
  Grid: Row increases downward, but rendered upward
```

## State Machine

```
                    ┌──────────────────┐
                    │ STATE_TITLE_     │
                    │   SCREEN         │
                    └──────────────────┘
                            │
                            │ Start Game
                            ▼
                    ┌──────────────────┐
                    │ STATE_PLAYING    │◄─────┐
                    └──────────────────┘      │
                            │                 │
                ┌───────────┼───────────┐     │
                │           │           │     │
         Trap Hit      Exit Reached  Lives=0 │
                │           │           │     │
                ▼           ▼           ▼     │
         Reset Player  ┌──────────┐  ┌──────────┐
                │      │ STATE_   │  │ STATE_   │
                └─────>│ LEVEL_   │  │ GAME_    │
                       │ CLEARED  │  │ OVER     │
                       └──────────┘  └──────────┘
                            │              │
                       Next Level      Restart
                            │              │
                            └──────────────┘
```

## File Size Distribution

```
Module          Files    Lines    %
────────────────────────────────────
core/              6      400    18%
gameplay/          8      350    16%
render/            5      300    14%
ui/                6      600    28%
data/              4      250    11%
utils/             4      150     7%
main.cpp           1      130     6%
────────────────────────────────────
Total:            34    2,180   100%
```

## Memory Layout (Approximate)

```
Stack (per frame):
  • Local variables: ~1 KB
  • Function call stack: ~4 KB

Heap (dynamic):
  • Game instance: ~100 bytes
  • Player: ~50 bytes
  • Level/Maze: ~800 bytes (12x16 grid)
  • HighScores: ~500 bytes (5 entries)
  • Total: ~1.5 KB

Static (program lifetime):
  • Level definitions: ~20 KB (3 mazes + paths)
  • Constants: ~1 KB
  • Code: ~200 KB (compiled)
  • Total: ~221 KB

Total Memory: ~227 KB (very lightweight!)
```
