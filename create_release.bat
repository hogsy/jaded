@echo off
set datevar=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
del web/releases/hogsy_jade-patch_%datevar%.zip
zip -9 web/releases/hogsy_jade-patch_%datevar%.zip mappings/* changes.txt Jaded.exe Jaded.pdb run_window.bat run_console.bat run_editor.bat SDL3.dll legal.txt
copy changes.txt web/releases/changes.txt /Y
echo %errorlevel%
