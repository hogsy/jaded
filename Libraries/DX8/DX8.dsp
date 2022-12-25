# Microsoft Developer Studio Project File - Name="DX8" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=DX8 - Win32 Debug Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DX8.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DX8.mak" CFG="DX8 - Win32 Debug Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DX8 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DX8 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "DX8 - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "DX8 - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "DX8 - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DX8 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/DX8___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "RELEASE" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8___r.lib"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/DX8___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8___d.lib"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DX8___Win32_Debug_Editors"
# PROP BASE Intermediate_Dir "DX8___Win32_Debug_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DX8___Win32_Debug_Editors"
# PROP Intermediate_Dir "DX8___Win32_Debug_Editors"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /GZ /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../Output\Libs\DX8___d.lib"
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8___edd.lib"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Speed Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DX8___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "DX8___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "DX8___Win32_Speed_Editors"
# PROP Intermediate_Dir "DX8___Win32_Speed_Editors"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../Output\Libs\DX8___r.lib"
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8___edr.lib"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "DX8___Win32_Final"
# PROP BASE Intermediate_Dir "DX8___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/DX8___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\extern\dx8\include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../Output\Libs\DX8___r.lib"
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8___f.lib"

!ENDIF 

# Begin Target

# Name "DX8 - Win32 Release"
# Name "DX8 - Win32 Debug"
# Name "DX8 - Win32 Debug Editors"
# Name "DX8 - Win32 Speed Editors"
# Name "DX8 - Win32 Final"
# Begin Source File

SOURCE=.\DX8debugfct.c
# End Source File
# Begin Source File

SOURCE=.\DX8debugfct.h
# End Source File
# Begin Source File

SOURCE=.\DX8init.c
# End Source File
# Begin Source File

SOURCE=.\DX8init.h
# End Source File
# Begin Source File

SOURCE=.\DX8renderstate.c
# End Source File
# Begin Source File

SOURCE=.\DX8renderstate.h
# End Source File
# Begin Source File

SOURCE=.\DX8request.c
# End Source File
# Begin Source File

SOURCE=.\DX8request.h
# End Source File
# Begin Source File

SOURCE=.\DX8tex.c
# End Source File
# Begin Source File

SOURCE=.\DX8tex.h
# End Source File
# Begin Source File

SOURCE=.\Precomp.c

!IF  "$(CFG)" == "DX8 - Win32 Release"

# ADD CPP /Yc"Precomp.h"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Debug"

# ADD CPP /Yc"Precomp.h"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Debug Editors"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "DX8 - Win32 Speed Editors"

# ADD CPP /Yc"Precomp.h"

!ELSEIF  "$(CFG)" == "DX8 - Win32 Final"

# ADD CPP /Yc"Precomp.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Precomp.h
# End Source File
# End Target
# End Project
