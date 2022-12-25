# Microsoft Developer Studio Project File - Name="SouND_xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=SouND_xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SouND_xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SouND_xb.mak" CFG="SouND_xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SouND_xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "SouND_xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "SouND_xb - Xbox Tuning" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "SouND_xb - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND_xb_r"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND_xb_r.lib"

!ELSEIF  "$(CFG)" == "SouND_xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SouND_xb___Xbox_Debug"
# PROP BASE Intermediate_Dir "SouND_xb___Xbox_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND_xb_d"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_DEBUG1" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND_xb_d.lib"

!ELSEIF  "$(CFG)" == "SouND_xb - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SouND_xb___Xbox_Tuning"
# PROP BASE Intermediate_Dir "SouND_xb___Xbox_Tuning"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND_xb_t"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\SND_xb_d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND_xb_t.lib"

!ENDIF 

# Begin Target

# Name "SouND_xb - Xbox Release"
# Name "SouND_xb - Xbox Debug"
# Name "SouND_xb - Xbox Tuning"
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE="$(XDK)\xbox\lib\dsound.lib"
# End Source File
# End Group
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Group "Xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\xbox\xbSND.c
# End Source File
# Begin Source File

SOURCE=..\Sources\xbox\xbSND_Stream.c
# End Source File
# Begin Source File

SOURCE=..\Sources\xbox\xbSND_Stream.h
# End Source File
# Begin Source File

SOURCE=..\Sources\xbox\xbSNDfx.c
# End Source File
# End Group
# Begin Group "MTX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MTX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MTX.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\SND.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SND.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDambience.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDambience.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdebug.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdebug.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdialog.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdialog.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdirect.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdirect.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDfamilly.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDfamilly.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDfx.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDfx.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDinterface.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDinterface.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDloadingsound.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDloadingsound.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmacros.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmodifier.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmodifier.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmusic.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmusic.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDrasters.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDrasters.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstream.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstream.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDtrack.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDtrack.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDvolume.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDvolume.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDwave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDwave.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
