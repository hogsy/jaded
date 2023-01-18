@echo off
del hogsy_jade-patch.zip
zip hogsy_jade-patch.zip mappings/* changes.txt Jade_edd.exe SDL2.dll
echo %errorlevel%