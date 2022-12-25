# Microsoft Developer Studio Project File - Name="MAD_import" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MAD_import - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAD_import.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAD_import.mak" CFG="MAD_import - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAD_import - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MAD_import - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MAD_import - Win32 max3 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_import___r"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x2d290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_import.pdb" /machine:I386 /out:"C:\3d\PLUGINS\MAD_import.dli"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_import___d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x2d290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_import.pdb" /debug /machine:I386 /out:"C:\3d\PLUGINS\MAD_import.dli" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MAD_impo"
# PROP BASE Intermediate_Dir "MAD_impo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MAD_impo"
# PROP Intermediate_Dir "MAD_impo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /GX /O2 /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "G:\MAX3\Maxsdk\Include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "MAX3" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x2d290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_import.pdb" /machine:I386 /out:"C:\3d\PLUGINS\MAD_import.dli"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x2d290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_import.pdb" /debug /machine:I386 /nodefaultlib:"LIBCMTD.lib" /out:"F:\3DSMAX3\PLUGINS\MAD_import3.dli"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "MAD_import - Win32 Release"
# Name "MAD_import - Win32 Debug"
# Name "MAD_import - Win32 max3 Release"
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MAD_import.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_import.def
# End Source File
# Begin Source File

SOURCE=..\Sources\MAD_import.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MADCDLG.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MADCDLG.h
# End Source File
# End Group
# Begin Group "MAX LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\MAD_FaceMap\core.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UTIL.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MESH.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GEOM.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___d.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___r.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___d.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___r.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\MAD_export\Sources\RES.rc
# End Source File
# End Group
# Begin Group "MAX3 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\zlibdll.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\bmm.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\client.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\core.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\edmodel.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\expr.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBD.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBH.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBR.LIB

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\flt.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gcomm.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\geom.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gfx.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gup.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\helpsys.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Maxscrpt.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\maxutil.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\mesh.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\MNMath.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Paramblk2.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\particle.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\tessint.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\viewfile.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\acap.lib

!IF  "$(CFG)" == "MAD_import - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_import - Win32 max3 Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\ttr.txt
# End Source File
# End Target
# End Project
