@echo off
del hogsy_jade-patch.zip
zip hogsy_jade-patch.zip mappings/* changes.txt JadedEditor.exe SDL2.dll
echo %errorlevel%