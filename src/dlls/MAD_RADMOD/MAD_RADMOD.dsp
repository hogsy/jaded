# Microsoft Developer Studio Project File - Name="MAD_RADMOD" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MAD_RADMOD - Win32 Hybrid
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAD_RADMOD.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAD_RADMOD.mak" CFG="MAD_RADMOD - Win32 Hybrid"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAD_RADMOD - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MAD_RADMOD - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MAD_RADMOD - Win32 Hybrid" (based on "Win32 (x86) Application")
!MESSAGE "MAD_RADMOD - Win32 Max3 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /O2 /I "C:\Maxsdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /out:"C:\3D\Plugins\MAD_RADMOD.dlm" /libpath:"C:\Maxsdk\lib" /release

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "C:\Maxsdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib util.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"e:\3D\Plugins\MAD_RADMOD.dlm" /pdbtype:sept /libpath:"C:\Maxsdk\lib"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MAD_RADMOD___Win32_Hybrid"
# PROP BASE Intermediate_Dir "MAD_RADMOD___Win32_Hybrid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MAD_RADMOD___Win32_Hybrid"
# PROP Intermediate_Dir "MAD_RADMOD___Win32_Hybrid"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "C:\Max3sdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MAX3" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"F:\3DSMAX3\Plugins\MAD_RADMOD.dlm" /pdbtype:sept /libpath:"C:\Max3sdk\lib"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MAD_RADMOD___Win32_Max3_Release"
# PROP BASE Intermediate_Dir "MAD_RADMOD___Win32_Max3_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RADMOD3"
# PROP Intermediate_Dir "RADMOD3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /O2 /I "C:\Max3sdk\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "MAX3" /YX /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "G:\MAX3\Maxsdk\Include" /I "../../../Dlls" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "MAX3" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /out:"C:\3D\Plugins\MAD_RADMOD.dlm" /libpath:"C:\Max3sdk\lib" /release
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x2d290000" /subsystem:windows /dll /debug /machine:I386 /out:"F:\3DSMAX3\PLUGINS\MAD_RADMOD3.dlm" /release

!ENDIF 

# Begin Target

# Name "MAD_RADMOD - Win32 Release"
# Name "MAD_RADMOD - Win32 Debug"
# Name "MAD_RADMOD - Win32 Hybrid"
# Name "MAD_RADMOD - Win32 Max3 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\MAD_RADMOD.cpp
# End Source File
# Begin Source File

SOURCE=.\MAD_RADMOD.def
# End Source File
# Begin Source File

SOURCE=.\MAD_RADMOD.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MAD_RADMOD.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "MAX3 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\zlibdll.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\bmm.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\client.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\core.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\edmodel.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\expr.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBD.LIB
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBH.LIB
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\FLILIBR.LIB
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\flt.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gcomm.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\geom.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\gup.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\helpsys.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Maxscrpt.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\MNMath.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\particle.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\tessint.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\viewfile.lib
# End Source File
# Begin Source File

SOURCE=G:\MAX3\Maxsdk\lib\acap.lib
# End Source File
# End Group
# Begin Group "MAX25 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=C:\maxsdk\LIB\TMT_DLL.ilk

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\VIEWFILE.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\BMM.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\CLIENT.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\Core.lib

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\EDMODEL.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\EXPR.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\FLILIBD.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\FLILIBH.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\FLILIBR.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\FLT.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\GCOMM.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\GEOM.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\GFX.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\MESH.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\MNMATH.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\PARTICLE.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\PATCH.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\UTIL.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=C:\maxsdk\LIB\ACAP.LIB

!IF  "$(CFG)" == "MAD_RADMOD - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Hybrid"

!ELSEIF  "$(CFG)" == "MAD_RADMOD - Win32 Max3 Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
