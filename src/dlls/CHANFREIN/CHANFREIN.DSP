# Microsoft Developer Studio Project File - Name="CHANFREIN" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CHANFREIN - Win32 max3 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CHANFREIN.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CHANFREIN.MAK" CFG="CHANFREIN - Win32 max3 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CHANFREIN - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CHANFREIN - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CHANFREIN - Win32 max3 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /Gm /GX /ZI /Od /I "c:\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X2D290000" /subsystem:windows /dll /machine:I386 /out:"c:\3d\plugins\Chamfer.dlm" /libpath:"c:\maxsdk\lib"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "c:\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X2D290000" /subsystem:windows /dll /debug /machine:I386 /out:"c:\3d\plugins\Chamfer.dlm" /pdbtype:sept /libpath:"c:\maxsdk\lib"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CHANFREIN___Win32_max3_Release"
# PROP BASE Intermediate_Dir "CHANFREIN___Win32_max3_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "CHANFREIN___Win32_max3_Release"
# PROP Intermediate_Dir "CHANFREIN___Win32_max3_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "c:\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /GX /I "c:\max3sdk\include" /I "../../../Dlls" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_USRDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB bmm.lib core.lib geom.lib gfx.lib mesh.lib patch.lib util.lib /nologo /base:"0X2D290000" /subsystem:windows /dll /debug /machine:I386 /out:"c:\3d\plugins\Chamfer.dlm" /pdbtype:sept /libpath:"c:\maxsdk\lib"
# ADD LINK32 COMCTL32.LIB KERNEL32.LIB USER32.LIB GDI32.LIB WINSPOOL.LIB COMDLG32.LIB ADVAPI32.LIB SHELL32.LIB OLE32.LIB OLEAUT32.LIB UUID.LIB /nologo /base:"0X2D290000" /subsystem:windows /dll /machine:I386 /out:"F:\3DSMAX3\PLUGINS\Chamfer3.dlm" /pdbtype:sept /libpath:"c:\maxsdk\lib"
# SUBTRACT LINK32 /debug

!ENDIF 

# Begin Target

# Name "CHANFREIN - Win32 Release"
# Name "CHANFREIN - Win32 Debug"
# Name "CHANFREIN - Win32 max3 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CHANFREIN.cpp
# End Source File
# Begin Source File

SOURCE=.\CHANFREIN.def
# End Source File
# Begin Source File

SOURCE=.\CHANFREIN.rc
# End Source File
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CHANFREIN.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "MAX LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\MAD_FaceMap\Util.lib

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAD_FaceMap\core.lib

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAD_FaceMap\geom.lib

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAD_FaceMap\mesh.lib

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAD_FaceMap\bmm.lib

!IF  "$(CFG)" == "CHANFREIN - Win32 Release"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 Debug"

!ELSEIF  "$(CFG)" == "CHANFREIN - Win32 max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MAX3 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=C:\Max3sdk\lib\zlibdll.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\bmm.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\client.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\core.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\edmodel.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\expr.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\FLILIBD.LIB
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\FLILIBH.LIB
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\FLILIBR.LIB
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\flt.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\gcomm.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\geom.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\gup.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\helpsys.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\Maxscrpt.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\MNMath.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\particle.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\tessint.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\viewfile.lib
# End Source File
# Begin Source File

SOURCE=C:\Max3sdk\lib\acap.lib
# End Source File
# End Group
# End Target
# End Project
