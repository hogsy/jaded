# Microsoft Developer Studio Project File - Name="MAD_Rasterize" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MAD_Rasterize - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAD_Rasterize.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAD_Rasterize.mak" CFG="MAD_Rasterize - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAD_Rasterize - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MAD_Rasterize - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAD_Rasterize - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_rasterize___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /I "../../../Dlls" /I "../../../Libraries/SDK/Sources" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\MAD_rasterize___r.lib"

!ELSEIF  "$(CFG)" == "MAD_Rasterize - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_rasterize___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /ZI /Od /I "../../../Libraries/SDK/Sources" /I "../../../Dlls" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\MAD_rasterize___d.lib"

!ENDIF 

# Begin Target

# Name "MAD_Rasterize - Win32 Release"
# Name "MAD_Rasterize - Win32 Debug"
# Begin Group "sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MAD_MTH.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_Rasterize.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_Rasterize.h
# End Source File
# End Group
# End Target
# End Project
