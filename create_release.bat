@echo off
del hogsy_jade-patch.zip
zip hogsy_jade-patch.zip mappings/* Jade_edd.exe SDL2.dll
echo %errorlevel%