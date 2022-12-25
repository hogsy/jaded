# Microsoft Developer Studio Project File - Name="NETwork" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NETwork - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NETwork.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NETwork.mak" CFG="NETwork - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NETwork - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "NETwork - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "NETwork - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "NETwork - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NETwork - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/NET___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\NET___r.lib"

!ELSEIF  "$(CFG)" == "NETwork - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/NET___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\NET___d.lib"

!ELSEIF  "$(CFG)" == "NETwork - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SND__"
# PROP BASE Intermediate_Dir "SND__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/NET___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\SND___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\NET___ed.lib"

!ELSEIF  "$(CFG)" == "NETwork - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "NETwork___Win32_Final"
# PROP BASE Intermediate_Dir "NETwork___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/NET___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\NET___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\NET___f.lib"

!ENDIF 

# Begin Target

# Name "NETwork - Win32 Release"
# Name "NETwork - Win32 Debug"
# Name "NETwork - Win32 Debug Editors"
# Name "NETwork - Win32 Final"
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\NET.c
# End Source File
# Begin Source File

SOURCE=..\Sources\NET.h
# End Source File
# Begin Source File

SOURCE=..\Sources\NET_io.c
# End Source File
# Begin Source File

SOURCE=..\Sources\NET_struct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.c
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Group
# End Target
# End Project
