# Microsoft Developer Studio Project File - Name="AIinterp_xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=AIinterp_xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AIinterp_xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AIinterp_xb.mak" CFG="AIinterp_xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AIinterp_xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "AIinterp_xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "AIinterp_xb - Xbox Tuning" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "AIinterp_xb - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI_xb_r"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Dlls" /D "_DX8" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\AI_xb_r.lib"

!ELSEIF  "$(CFG)" == "AIinterp_xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI_xb_d"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Dlls" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output/Libs\AI_xb_d.lib"

!ELSEIF  "$(CFG)" == "AIinterp_xb - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AIinterp_xb___Xbox_Tuning"
# PROP BASE Intermediate_Dir "AIinterp_xb___Xbox_Tuning"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/AI_xb_t"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Main\WinAIinterp\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Dlls" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output/Libs\AI_xb_d.lib"
# ADD LIB32 /nologo /out:"../../../Output/Libs\AI_xb_t.lib"

!ENDIF 

# Begin Target

# Name "AIinterp_xb - Xbox Release"
# Name "AIinterp_xb - Xbox Debug"
# Name "AIinterp_xb - Xbox Tuning"
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
# Begin Group "Fields"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Fields\AIdeffields.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Fields\AIfields.c
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
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Types\AIdeftyp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Types\AItypes.c
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
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\AI.c
# End Source File
# Begin Source File

SOURCE=..\Sources\AI.h
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
# Begin Source File

SOURCE=..\Sources\Precomp.c
# ADD BASE CPP /Yc"Precomp.h"
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
