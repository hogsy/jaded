# Microsoft Developer Studio Project File - Name="MAD_RAD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MAD_RAD - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mad_rad.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mad_rad.mak" CFG="MAD_RAD - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAD_RAD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MAD_RAD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_rad___r"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /Zd /O2 /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fr /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0xf4240 /subsystem:windows /profile /map:"C:\MAD_RAD\Mad_rad.map" /debug /machine:I386 /force /out:"C:\MAD_RAD\Mad_rad.exe"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_rad___d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "../../../Dlls" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /profile /map:"C:\MAD_RAD\Mad_rad.map" /debug /debugtype:both /machine:I386 /force /out:"C:\MAD_RAD\Mad_rad.exe"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "MAD_RAD - Win32 Release"
# Name "MAD_RAD - Win32 Debug"
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MAD_LOD.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_RAD.CPP

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\Viewer.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\Viewer.h
# End Source File
# End Group
# Begin Group "Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\ddraw.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Extern\dxguid.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___d.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___r.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___d.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___r.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_rasterize___d.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_rasterize___r.lib

!IF  "$(CFG)" == "MAD_RAD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RAD - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Resources\ICON1.ICO
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_RAD.RC
# End Source File
# Begin Source File

SOURCE=..\Sources\resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Arrow.BMP
# End Source File
# Begin Source File

SOURCE=..\Sources\CUR00001.CUR
# End Source File
# Begin Source File

SOURCE=..\Sources\hand.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\HAND1.CUR
# End Source File
# Begin Source File

SOURCE=..\Sources\Icon1.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_LOD.H
# End Source File
# Begin Source File

SOURCE=..\Sources\Mapbias.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\normal.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\rotation.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\tmt_zoom.cur
# End Source File
# End Target
# End Project
