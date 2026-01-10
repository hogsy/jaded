@echo off
set datevar=%DATE:~6,4%%DATE:~3,2%%DATE:~0,2%
del hogsy_jade-patch_%datevar%.zip
zip -9 hogsy_jade-patch_%datevar%.zip runtime/mappings/* runtime/shaders/glsl/* changes.txt runtime/Jaded.exe runtime/Jaded.pdb runtime/run_window.bat runtime/run_console.bat runtime/run_editor.bat runtime/SDL3.dll runtime/legal.txt
echo %errorlevel%
