# Microsoft Developer Studio Project File - Name="ENGine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ENGine - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ENGine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ENGine.mak" CFG="ENGine - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ENGine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ENGine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "ENGine - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "ENGine - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "ENGine - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ENGine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/ENG___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /Ob2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "RELEASE" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\ENG___r.lib"

!ELSEIF  "$(CFG)" == "ENGine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/ENG___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\ENG___d.lib"

!ELSEIF  "$(CFG)" == "ENGine - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ENGine__"
# PROP BASE Intermediate_Dir "ENGine__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/ENG___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\ENG___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\ENG___ed.lib"

!ELSEIF  "$(CFG)" == "ENGine - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ENGine___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "ENGine___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/ENG___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\ENG___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\ENG___er.lib"

!ELSEIF  "$(CFG)" == "ENGine - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ENGine___Win32_Final"
# PROP BASE Intermediate_Dir "ENGine___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/ENG___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\ENG___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\ENG___f.lib"

!ENDIF 

# Begin Target

# Name "ENGine - Win32 Release"
# Name "ENGine - Win32 Debug"
# Name "ENGine - Win32 Debug Editors"
# Name "ENGine - Win32 Speed Editors"
# Name "ENGine - Win32 Final"
# Begin Group "ENG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\ENGcall.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGcall.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGloop.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGloop.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGmsg.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGstring.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGvars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ENGvars.h
# End Source File
# End Group
# Begin Group "WORld"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\WORld\WOR.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WOR.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORcheck.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORcheck.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORexporttomad.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORexporttomad.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORimportfrommad.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORimportfrommad.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORrender.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORrender.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORuniverse.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORuniverse.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORupdate.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORupdate.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORvars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WORld\WORvars.h
# End Source File
# End Group
# Begin Group "OBJects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\OBJects\OBJ.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJ.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJBoundingvolume.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJBoundingVolume.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJculling.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJculling.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJgizmo.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJgizmo.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJgrp.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJgrp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJorient.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJorient.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJslowaccess.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJslowaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OBJects\OBJstruct.h
# End Source File
# End Group
# Begin Group "DYNamics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\DYNamics\DYN.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYN.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNaccess.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNBasic.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNBasic.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNcol.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNcol.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNConstraint.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNConstraint.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNSolid.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNsolid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNvars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\DYNamics\DYNvars.h
# End Source File
# End Group
# Begin Group "EOT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\EOT\EOT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOT.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EOT\EOTstruct.h
# End Source File
# End Group
# Begin Group "ANImation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\ANImation\ANI.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANI.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANImain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANImain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIplay.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIplay.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIrender.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIrender.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\ANIstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\NIMtoTracks.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ANImation\NIMtoTracks.h
# End Source File
# End Group
# Begin Group "INTersection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\INTersection\INT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INT.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTaccess.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTSnP.c
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTSnP.h
# End Source File
# Begin Source File

SOURCE=..\Sources\INTersection\INTstruct.h
# End Source File
# End Group
# Begin Group "WAYs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\WAYs\WAY.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAY.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYs\WAYstruct.h
# End Source File
# End Group
# Begin Group "COLlision"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\COLlision\COL.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COL.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLactor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLactor.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLcob.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLcob.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLedit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLedit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLray.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLray.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLreport.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLreport.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLset.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLset.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLvars.h
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLzdx.c
# End Source File
# Begin Source File

SOURCE=..\Sources\COLlision\COLzdx.h
# End Source File
# End Group
# Begin Group "GRP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GRP\GRPload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPmain.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPmain.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPorient.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPorient.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRP\GRPsave.h
# End Source File
# End Group
# Begin Group "RECalage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\RECalage\REC.c
# End Source File
# Begin Source File

SOURCE=..\Sources\RECalage\REC.h
# End Source File
# Begin Source File

SOURCE=..\Sources\RECalage\RECconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\RECalage\RECdyn.c
# End Source File
# Begin Source File

SOURCE=..\Sources\RECalage\RECdyn.h
# End Source File
# End Group
# Begin Group "TEXT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TEXT\TEXT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXT\TEXT.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXT\TEXTload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXT\TEXTload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXT\TEXTstruct.h
# End Source File
# End Group
# Begin Group "ACTions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\ACTions\ACTcompute.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTcompute.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\ACTstruct.h
# End Source File
# End Group
# Begin Group "GRId"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GRId\GRI_compute.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_compute.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_display.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_display.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_load.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_load.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_save.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_save.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_struct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_vars.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRId\GRI_vars.h
# End Source File
# End Group
# Begin Group "MoDiFier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GAO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GAO.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GEN.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GEN.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GEO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_GEO.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_LAZY.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_LAZY.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_MPAG.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_MPAG.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_PROTEX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_PROTEX.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_ROTR.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_ROTR.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SDW.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SDW.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SNAKE.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SNAKE.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SOUNDFX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SOUNDFX.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SOUNDLOADING.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SOUNDLOADING.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SPG.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_SPG.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_XMEC.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_XMEC.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_XMEN.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFmodifier_XMEN.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MoDiFier\MDFstruct.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.c
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
