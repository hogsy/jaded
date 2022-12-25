# Microsoft Developer Studio Project File - Name="MPEGLIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=MPEGLIB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MPEGLIB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MPEGLIB.mak" CFG="MPEGLIB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MPEGLIB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MPEGLIB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MPEGLIB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/MPEG___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "_MBCS" /D "_LIB" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output/Libs\MPEG___r.lib"

!ELSEIF  "$(CFG)" == "MPEGLIB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/MPEG___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_MBCS" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output/Libs\MPEG___d.lib"

!ENDIF 

# Begin Target

# Name "MPEGLIB - Win32 Release"
# Name "MPEGLIB - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\display.c
# End Source File
# Begin Source File

SOURCE=.\getbits.c
# End Source File
# Begin Source File

SOURCE=.\getblk.c
# End Source File
# Begin Source File

SOURCE=.\gethdr.c
# End Source File
# Begin Source File

SOURCE=.\getpic.c
# End Source File
# Begin Source File

SOURCE=.\getvlc.c
# End Source File
# Begin Source File

SOURCE=.\getvlc.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\idct.c
# End Source File
# Begin Source File

SOURCE=.\motion.c
# End Source File
# Begin Source File

SOURCE=.\mpeg2dec.c
# End Source File
# Begin Source File

SOURCE=.\mpeg2dec.h
# End Source File
# Begin Source File

SOURCE=.\MPG_EXPORT.H
# End Source File
# Begin Source File

SOURCE=.\recon.c
# End Source File
# Begin Source File

SOURCE=.\store.c
# End Source File
# Begin Source File

SOURCE=.\systems.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
