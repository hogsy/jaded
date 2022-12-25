rem @echo off

set root_dir=D:\boulot\nouveaujade
set out_dir=rel-release
set makeexe=makerel.exe

pushd %root_dir%\Output\ExeDlls\gamecubeDll
subst z: /D

if exist %out_dir% rmdir /S /Q %out_dir%
mkdir %out_dir%

subst z: %root_dir%\Output\ExeDlls\gamecubeDll


%makeexe% x:\jadegc_ia2cr.elf Z:\AIgc_dll000r.plf Z:\AIgc_dll001r.plf Z:\AIgc_dll002r.plf Z:\AIgc_dll003r.plf Z:\AIgc_dll004r.plf Z:\AIgc_dll005r.plf Z:\AIgc_dll006r.plf Z:\AIgc_dll007r.plf Z:\AIgc_dll008r.plf Z:\AIgc_dll009r.plf Z:\AIgc_dll010r.plf Z:\AIgc_dll011r.plf Z:\AIgc_dll012r.plf Z:\AIgc_dll013r.plf Z:\AIgc_dll014r.plf Z:\AIgc_dll015r.plf Z:\AIgc_dll016r.plf Z:\AIgc_dll017r.plf Z:\AIgc_dll018r.plf Z:\AIgc_dll019r.plf Z:\AIgc_dll020r.plf Z:\AIgc_dll021r.plf Z:\AIgc_dll022r.plf Z:\AIgc_dll023r.plf Z:\AIgc_dll024r.plf Z:\AIgc_dll025r.plf Z:\AIgc_dll026r.plf Z:\AIgc_dll027r.plf Z:\AIgc_dll028r.plf Z:\AIgc_dll029r.plf Z:\AIgc_dll030r.plf Z:\AIgc_dll031r.plf Z:\AIgc_dll032r.plf Z:\AIgc_dll033r.plf Z:\AIgc_dll034r.plf Z:\AIgc_dll035r.plf Z:\AIgc_dll036r.plf Z:\AIgc_dll037r.plf Z:\AIgc_dll038r.plf Z:\AIgc_dll039r.plf Z:\AIgc_dll040r.plf Z:\AIgc_dll041r.plf Z:\AIgc_dll042r.plf Z:\AIgc_dll043r.plf Z:\AIgc_dll044r.plf Z:\AIgc_dll045r.plf Z:\AIgc_dll046r.plf Z:\AIgc_dll047r.plf Z:\AIgc_dll048r.plf Z:\AIgc_dll049r.plf Z:\AIgc_dll050r.plf Z:\AIgc_dll051r.plf Z:\AIgc_dll052r.plf Z:\AIgc_dll053r.plf Z:\AIgc_dll054r.plf Z:\AIgc_dll055r.plf Z:\AIgc_dll056r.plf Z:\AIgc_dll057r.plf Z:\AIgc_dll058r.plf Z:\AIgc_dll059r.plf Z:\AIgc_dll060r.plf Z:\AIgc_dll061r.plf Z:\AIgc_dll062r.plf Z:\AIgc_dll063r.plf Z:\AIgc_dll064r.plf Z:\AIgc_dll065r.plf Z:\AIgc_dll066r.plf Z:\AIgc_dll067r.plf Z:\AIgc_dll068r.plf Z:\AIgc_dll069r.plf Z:\AIgc_dll070r.plf Z:\AIgc_dll071r.plf Z:\AIgc_dll072r.plf Z:\AIgc_dll073r.plf Z:\AIgc_dll074r.plf Z:\AIgc_dll075r.plf Z:\AIgc_dll076r.plf Z:\AIgc_dll077r.plf Z:\AIgc_dll078r.plf Z:\AIgc_dll079r.plf Z:\AIgc_dll080r.plf Z:\AIgc_dll081r.plf Z:\AIgc_dll082r.plf Z:\AIgc_dll083r.plf Z:\AIgc_dll084r.plf Z:\AIgc_dll085r.plf Z:\AIgc_dll086r.plf Z:\AIgc_dll087r.plf Z:\AIgc_dll088r.plf Z:\AIgc_dll089r.plf 

move x:\jadegc_ia2cr.str .\%out_dir%\
move %root_dir%\Output\ExeDlls\gamecubeDll\*r.rel .\%out_dir%\

pause