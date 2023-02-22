@echo off
set datevar=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
del releases/hogsy_jade-patch_%datevar%.zip
zip releases/hogsy_jade-patch_%datevar%.zip mappings/* changes.txt Jaded.exe SDL2.dll
del releases/hogsy_jade-source_%datevar%.zip
zip releases/hogsy_jade-source_%datevar%.zip -r Main/ Dlls/ Extern/ Libraries/ Tools/ -i *.rc *.bmp *.ico *.c *.cpp *.h *.sln *.vcxproj *.vcxproj.filters
echo %errorlevel%