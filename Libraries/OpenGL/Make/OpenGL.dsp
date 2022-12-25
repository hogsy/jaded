# Microsoft Developer Studio Project File - Name="OpenGL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OpenGL - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenGL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenGL.mak" CFG="OpenGL - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenGL - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenGL - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenGL - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenGL - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenGL - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenGL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/OGL___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "RELEASE" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\OGL___r.lib"

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/OGL___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\OGL___d.lib"

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OGL__"
# PROP BASE Intermediate_Dir "OGL__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/OGL___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\OGL___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\OGL___ed.lib"

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OpenGL___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "OpenGL___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/OGL___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\OGL___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\OGL___er.lib"

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OpenGL___Win32_Final"
# PROP BASE Intermediate_Dir "OpenGL___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/OGL___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\OGL___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\OGL___f.lib"

!ENDIF 

# Begin Target

# Name "OpenGL - Win32 Release"
# Name "OpenGL - Win32 Debug"
# Name "OpenGL - Win32 Debug Editors"
# Name "OpenGL - Win32 Speed Editors"
# Name "OpenGL - Win32 Final"
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\OGLdebugfct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLdebugfct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLinit.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLinit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLrenderstate.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLrenderstate.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLrequest.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLrequest.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLtex.c
# End Source File
# Begin Source File

SOURCE=..\Sources\OGLtex.h
# End Source File
# End Group
# Begin Group "LIBraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DDRAW.LIB

!IF  "$(CFG)" == "OpenGL - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Speed Editors"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Extern\OPENGL32.LIB
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
