#!/bin/bash
set -e

BUILD_TYPE="${1:-release}"
CXX="C:/msys64/ucrt64/bin/g++.exe"
INCLUDES="-IC:/msys64/ucrt64/include"
LIBS="-LC:/msys64/ucrt64/lib"

if [[ "$BUILD_TYPE" == "debug" ]]; then
    OPT_FLAGS="-g -O0"
else
    OPT_FLAGS="-O2"
fi

echo "Building Maze Runner [$BUILD_TYPE]..."

SOURCES="main.cpp core/game.cpp core/gamestate.cpp core/input.cpp core/settings.cpp \
gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp \
gameplay/generator.cpp gameplay/generator_analysis.cpp \
render/renderer.cpp render/text.cpp \
ui/hud.cpp ui/menu.cpp ui/overlay.cpp \
data/highscore.cpp data/leveldata.cpp \
utils/coords.cpp utils/timer.cpp"

"$CXX" -o maze_runner.exe \
    $SOURCES \
    $INCLUDES $LIBS \
    -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 \
    -std=c++11 $OPT_FLAGS \
    -Wall -Wextra -Wno-unused-parameter

echo "Build successful! [$BUILD_TYPE]"
echo "Run with: ./maze_runner.exe"
