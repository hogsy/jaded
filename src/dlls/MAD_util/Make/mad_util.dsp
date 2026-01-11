# Microsoft Developer Studio Project File - Name="mad_util" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mad_util - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mad_util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mad_util.mak" CFG="mad_util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mad_util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mad_util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mad_util - Win32 max3 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_util___r"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /Gm /GX /ZI /Od /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib util.lib /nologo /base:"0X2D290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_util.pdb" /machine:I386 /out:"C:\3d\PLUGINS\MAD_util.dlu" /libpath:"c:\maxsdk\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/MAD_util___d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /GX /ZI /Od /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib util.lib /nologo /base:"0X2D290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_util.pdb" /debug /machine:I386 /out:"C:\3d\PLUGINS\MAD_util.dlu" /pdbtype:sept /libpath:"c:\maxsdk\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mad_util"
# PROP BASE Intermediate_Dir "mad_util"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "mad_util"
# PROP Intermediate_Dir "mad_util"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "c:\maxsdk\include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "G:\MAX3\Maxsdk\Include" /I "../../../Dlls" /D "_WINDLL" /D "_USRDLL" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "MAX3" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib util.lib /nologo /base:"0X2D290000" /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_util.pdb" /machine:I386 /out:"C:\3d\PLUGINS\MAD_util.dlu" /libpath:"c:\maxsdk\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /subsystem:windows /dll /pdb:"../../../Output/ExeDlls/MAD_util.pdb" /debug /machine:I386 /nodefaultlib:"LIBCMT.lib" /out:"F:\3DSMAX3\PLUGINS\MAD_util3.dlu" /libpath:"c:\max3sdk\lib"
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "mad_util - Win32 Release"
# Name "mad_util - Win32 Debug"
# Name "mad_util - Win32 max3 Release"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Sources\mad_util.rc
# End Source File
# Begin Source File

SOURCE=..\Sources\resource.h
# End Source File
# End Group
# Begin Group "libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___d.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___r.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___d.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___r.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\mad_util.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\mad_util.def
# End Source File
# Begin Source File

SOURCE=..\Sources\mad_util.h
# End Source File
# End Group
# Begin Group "MAX 3 LIBs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\zlibdll.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\bmm.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\client.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\core.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\edmodel.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\expr.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBD.LIB

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBH.LIB

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBR.LIB

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\flt.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gcomm.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\geom.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gfx.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gup.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\helpsys.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Maxscrpt.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\maxutil.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\mesh.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\MNMath.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Paramblk2.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\particle.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\tessint.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\viewfile.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\acap.lib

!IF  "$(CFG)" == "mad_util - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mad_util - Win32 max3 Release"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
