@echo off
setlocal EnableDelayedExpansion

echo Building Maze Runner...

set "CXX=g++"
set "EXTRA_INCLUDES="
set "EXTRA_LIBS="

if exist "C:\msys64\ucrt64\bin\g++.exe" (
    set "CXX=C:\msys64\ucrt64\bin\g++.exe"
    set "EXTRA_INCLUDES=-IC:\msys64\ucrt64\include"
    set "EXTRA_LIBS=-LC:\msys64\ucrt64\lib"
    echo Using MSYS2 UCRT toolchain: !CXX!
)

"%CXX%" -o maze_runner.exe ^
    main.cpp ^
    core/game.cpp ^
    core/gamestate.cpp ^
    gameplay/player.cpp ^
    gameplay/maze.cpp ^
    gameplay/collision.cpp ^
    gameplay/level.cpp ^
    gameplay/generator.cpp ^
    render/renderer.cpp ^
    render/text.cpp ^
    ui/hud.cpp ^
    ui/menu.cpp ^
    ui/overlay.cpp ^
    data/highscore.cpp ^
    data/leveldata.cpp ^
    utils/coords.cpp ^
    utils/timer.cpp ^
    %EXTRA_INCLUDES% ^
    %EXTRA_LIBS% ^
    -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32 ^
    -std=c++11 -O2

if %errorlevel% equ 0 (
    echo Build successful!
    echo Run with: maze_runner.exe
) else (
    echo Build failed!
)
