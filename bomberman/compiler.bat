@echo off
cd /d "%~dp0"
gcc -Wall -Wextra -std=c11 -Iinclude -o bomberman.exe src\main.c src\game.c src\arena.c src\entities.c src\input.c src\score.c
if errorlevel 1 (
  echo Compilation echouee.
  pause
  exit /b 1
)
echo Compilation OK : bomberman.exe
echo.
echo Lancement...
bomberman.exe
