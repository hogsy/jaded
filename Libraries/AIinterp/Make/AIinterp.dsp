# Microsoft Developer Studio Project File - Name="AIinterp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AIinterp - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AIinterp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AIinterp.mak" CFG="AIinterp - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AIinterp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AIinterp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "AIinterp - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "AIinterp - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "AIinterp - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AIinterp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O1 /Ob2 /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "RELEASE" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI___r.lib"

!ELSEIF  "$(CFG)" == "AIinterp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI___d.lib"

!ELSEIF  "$(CFG)" == "AIinterp - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AIinterp__"
# PROP BASE Intermediate_Dir "AIinterp__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Extern\DX8\include" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\AI___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI___ed.lib"

!ELSEIF  "$(CFG)" == "AIinterp - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AIinterp___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "AIinterp___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\AI___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI___er.lib"

!ELSEIF  "$(CFG)" == "AIinterp - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "AIinterp___Win32_Final"
# PROP BASE Intermediate_Dir "AIinterp___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\AI___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI___f.lib"

!ENDIF 

# Begin Target

# Name "AIinterp - Win32 Release"
# Name "AIinterp - Win32 Debug"
# Name "AIinterp - Win32 Debug Editors"
# Name "AIinterp - Win32 Speed Editors"
# Name "AIinterp - Win32 Final"
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\AI.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_dll\AI2C_dll.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctdefs.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctheader.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist0.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist1.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist2.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist3.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist4.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist5.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist6.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist7.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist8.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI2C_fctlist9.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIBench.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIBench.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIdebug.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIdebug.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIengine.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIengine.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AImsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AIsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIstack.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AItools.h
# End Source File
# End Group
# Begin Group "Keywords"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Keywords\AIdefkey.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Keywords\AIkeywords.c
# End Source File
# End Group
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Types\AIdeftyp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Types\AItypes.c
# End Source File
# End Group
# Begin Group "Functions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Functions\AIdeffct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_action.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_ai.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_ani.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_arr.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_cam.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_cast.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_col.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_COLOR.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_dbg.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_dyn.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_GFX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_grid.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_io.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_light.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_MAT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_msg.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_mth.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_net.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_obj.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_objtext.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_save.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_snd.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_text.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_time.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_tra.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_view.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_way.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_wor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Functions\AIfunctions_XMEN.c
# End Source File
# End Group
# Begin Group "Categs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Categs\AIcateg.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Categs\AIdefcateg.h
# End Source File
# End Group
# Begin Group "Fields"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Fields\AIdeffields.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Fields\AIfields.c
# End Source File
# End Group
# Begin Group "Events"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Events\EVEconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEnt_aifunc.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEnt_aifunc.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEnt_interpolationkey.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEnt_interpolationkey.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEplay.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEplay.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Events\EVEstruct.h
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
