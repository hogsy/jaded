# Microsoft Developer Studio Project File - Name="GS_PS2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GS_PS2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GS_PS2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GS_PS2.mak" CFG="GS_PS2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GS_PS2 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GS_PS2 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GSP___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEngine\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\Sources" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GSP___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEngine\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\Sources" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "GS_PS2 - Win32 Release"
# Name "GS_PS2 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GSPdebugfct.c

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPdebugfct.h

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPinit.c

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPinit.h

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPrequest.c

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPrequest.h

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPtex.c

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GSPtex.h

!IF  "$(CFG)" == "GS_PS2 - Win32 Release"

!ELSEIF  "$(CFG)" == "GS_PS2 - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
