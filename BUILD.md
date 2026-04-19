# Build Instructions

## Prerequisites

You need FreeGLUT and OpenGL development libraries installed.

### Windows (MinGW)

1. Install MinGW-w64
2. Install FreeGLUT:
   - Download from: https://www.transmissionzero.co.uk/software/freeglut-devel/
   - Extract to MinGW directory
   - Or use: `pacman -S mingw-w64-x86_64-freeglut` (if using MSYS2)

### Linux (Ubuntu/Debian)

```bash
sudo apt-get install freeglut3-dev
```

### macOS

```bash
brew install freeglut
```

## Build Commands

### Windows (MinGW)
```bash
g++ -o maze_runner.exe main.cpp core/game.cpp core/gamestate.cpp gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp render/renderer.cpp render/text.cpp ui/hud.cpp ui/menu.cpp ui/overlay.cpp data/highscore.cpp data/leveldata.cpp utils/coords.cpp utils/timer.cpp -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 -std=c++11 -O2
```

### Linux
```bash
g++ -o maze_runner main.cpp core/game.cpp core/gamestate.cpp gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp render/renderer.cpp render/text.cpp ui/hud.cpp ui/menu.cpp ui/overlay.cpp data/highscore.cpp data/leveldata.cpp utils/coords.cpp utils/timer.cpp -lglut -lGLU -lGL -lm -std=c++11 -O2
```

### macOS
```bash
g++ -o maze_runner main.cpp core/game.cpp core/gamestate.cpp gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp render/renderer.cpp render/text.cpp ui/hud.cpp ui/menu.cpp ui/overlay.cpp data/highscore.cpp data/leveldata.cpp utils/coords.cpp utils/timer.cpp -framework GLUT -framework OpenGL -std=c++11 -O2
```

## Using CMake (Recommended)

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.10)
project(MazeRunner)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

add_executable(maze_runner
    main.cpp
    core/game.cpp
    core/gamestate.cpp
    gameplay/player.cpp
    gameplay/maze.cpp
    gameplay/collision.cpp
    gameplay/level.cpp
    render/renderer.cpp
    render/text.cpp
    ui/hud.cpp
    ui/menu.cpp
    ui/overlay.cpp
    data/highscore.cpp
    data/leveldata.cpp
    utils/coords.cpp
    utils/timer.cpp
)

target_link_libraries(maze_runner ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
```

Then build:
```bash
mkdir build
cd build
cmake ..
make
```

## Troubleshooting

### "GL/freeglut.h: No such file or directory"
- FreeGLUT is not installed or not in include path
- Install FreeGLUT development package for your platform

### Linking errors
- Make sure OpenGL libraries are installed
- Check library names match your platform (-lglut vs -lfreeglut)

### Runtime errors
- Ensure FreeGLUT DLL is in PATH (Windows)
- Check OpenGL drivers are installed
