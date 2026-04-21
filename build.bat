@echo off
setlocal EnableDelayedExpansion

set "BUILD_TYPE=release"
if /i "%1"=="debug" set "BUILD_TYPE=debug"

echo Building Maze Runner [%BUILD_TYPE%]...

set "CXX=g++"
set "EXTRA_INCLUDES="
set "EXTRA_LIBS="

if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set "CXX=C:\msys64\ucrt64\bin\g++.exe"
    set "EXTRA_INCLUDES=-IC:\msys64\ucrt64\include"
    set "EXTRA_LIBS=-LC:\msys64\ucrt64\lib"
    echo Using MSYS2 UCRT toolchain: !CXX!
)

set "OPT_FLAGS=-O2"
if /i "%BUILD_TYPE%"=="debug" set "OPT_FLAGS=-g -O0"

set "SOURCES=main.cpp core/game.cpp core/gamestate.cpp core/input.cpp core/settings.cpp gameplay/player.cpp gameplay/maze.cpp gameplay/collision.cpp gameplay/level.cpp gameplay/generator.cpp gameplay/generator_analysis.cpp render/renderer.cpp render/text.cpp ui/hud.cpp ui/menu.cpp ui/overlay.cpp data/highscore.cpp data/leveldata.cpp utils/coords.cpp utils/timer.cpp"

"%CXX%" -o maze_runner.exe ^
    %SOURCES% ^
    %EXTRA_INCLUDES% ^
    %EXTRA_LIBS% ^
    -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
    -std=c++11 %OPT_FLAGS% ^
    -Wall -Wextra -Wno-unused-parameter

if %errorlevel% equ 0 (
    echo Build successful! [%BUILD_TYPE%]
    echo Run with: maze_runner.exe
) else (
    echo Build failed!
)
