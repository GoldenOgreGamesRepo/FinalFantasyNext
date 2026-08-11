@echo off
cd /d "%~dp0"

set COMPILER=toolchain\bin\g++.exe
set SRC=src
set INC=external\SFML\include
set LIB=external\SFML\lib

if not exist build mkdir build

"%COMPILER%" "%SRC%\main.cpp" ^
    -std=c++17 ^
    -I"%INC%" ^
    -L"%LIB%" ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio ^
    -o build\MyRPG.exe

copy external\SFML\bin\sfml-graphics-3.dll build\ >nul
copy external\SFML\bin\sfml-window-3.dll build\ >nul
copy external\SFML\bin\sfml-system-3.dll build\ >nul
copy external\SFML\bin\sfml-audio-3.dll build\ >nul
copy "toolchain\bin\libstdc++-6.dll" build\  >nul
copy toolchain\bin\libgcc_s_seh-1.dll build\ >nul
copy toolchain\bin\libwinpthread-1.dll build\ >nul

build\MyRPG.exe
