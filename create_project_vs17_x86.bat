@echo off
mkdir solutions
cd solutions
cmake .. -G "Visual Studio 17 2022" -A Win32 -D CMAKE_BUILD_TYPE=Debug
pause