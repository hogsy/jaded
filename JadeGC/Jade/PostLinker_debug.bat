rem @echo off

set root_dir=D:\boulot\nouveaujade
set out_dir=rel-debug
set makeexe=makereld.exe

pushd %root_dir%\Output\ExeDlls\gamecubeDll
subst z: /D

if exist %out_dir% rmdir /S /Q %out_dir%
mkdir %out_dir%

subst z: %root_dir%\Output\ExeDlls\gamecubeDll


%makeexe% x:\jadegc_ia2cd.elf Z:\AIgc_dll000d.plf Z:\AIgc_dll001d.plf Z:\AIgc_dll002d.plf Z:\AIgc_dll003d.plf Z:\AIgc_dll004d.plf Z:\AIgc_dll005d.plf Z:\AIgc_dll006d.plf Z:\AIgc_dll007d.plf Z:\AIgc_dll008d.plf Z:\AIgc_dll009d.plf Z:\AIgc_dll010d.plf Z:\AIgc_dll011d.plf Z:\AIgc_dll012d.plf Z:\AIgc_dll013d.plf Z:\AIgc_dll014d.plf Z:\AIgc_dll015d.plf Z:\AIgc_dll016d.plf Z:\AIgc_dll017d.plf Z:\AIgc_dll018d.plf Z:\AIgc_dll019d.plf Z:\AIgc_dll020d.plf Z:\AIgc_dll021d.plf Z:\AIgc_dll022d.plf Z:\AIgc_dll023d.plf Z:\AIgc_dll024d.plf Z:\AIgc_dll025d.plf Z:\AIgc_dll026d.plf Z:\AIgc_dll027d.plf Z:\AIgc_dll028d.plf Z:\AIgc_dll029d.plf Z:\AIgc_dll030d.plf Z:\AIgc_dll031d.plf Z:\AIgc_dll032d.plf Z:\AIgc_dll033d.plf Z:\AIgc_dll034d.plf Z:\AIgc_dll035d.plf Z:\AIgc_dll036d.plf Z:\AIgc_dll037d.plf Z:\AIgc_dll038d.plf Z:\AIgc_dll039d.plf Z:\AIgc_dll040d.plf Z:\AIgc_dll041d.plf Z:\AIgc_dll042d.plf Z:\AIgc_dll043d.plf Z:\AIgc_dll044d.plf Z:\AIgc_dll045d.plf Z:\AIgc_dll046d.plf Z:\AIgc_dll047d.plf Z:\AIgc_dll048d.plf Z:\AIgc_dll049d.plf Z:\AIgc_dll050d.plf Z:\AIgc_dll051d.plf Z:\AIgc_dll052d.plf Z:\AIgc_dll053d.plf Z:\AIgc_dll054d.plf Z:\AIgc_dll055d.plf Z:\AIgc_dll056d.plf Z:\AIgc_dll057d.plf Z:\AIgc_dll058d.plf Z:\AIgc_dll059d.plf Z:\AIgc_dll060d.plf Z:\AIgc_dll061d.plf Z:\AIgc_dll062d.plf Z:\AIgc_dll063d.plf Z:\AIgc_dll064d.plf Z:\AIgc_dll065d.plf Z:\AIgc_dll066d.plf Z:\AIgc_dll067d.plf Z:\AIgc_dll068d.plf Z:\AIgc_dll069d.plf Z:\AIgc_dll070d.plf Z:\AIgc_dll071d.plf Z:\AIgc_dll072d.plf Z:\AIgc_dll073d.plf Z:\AIgc_dll074d.plf Z:\AIgc_dll075d.plf Z:\AIgc_dll076d.plf Z:\AIgc_dll077d.plf Z:\AIgc_dll078d.plf Z:\AIgc_dll079d.plf Z:\AIgc_dll080d.plf Z:\AIgc_dll081d.plf Z:\AIgc_dll082d.plf Z:\AIgc_dll083d.plf Z:\AIgc_dll084d.plf Z:\AIgc_dll085d.plf Z:\AIgc_dll086d.plf Z:\AIgc_dll087d.plf Z:\AIgc_dll088d.plf Z:\AIgc_dll089d.plf 

move x:\jadegc_ia2cd.str .\%out_dir%\
move %root_dir%\Output\ExeDlls\gamecubeDll\*d.rel .\%out_dir%\

pause