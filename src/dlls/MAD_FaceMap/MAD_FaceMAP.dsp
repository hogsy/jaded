# Microsoft Developer Studio Project File - Name="MAD_FaceMAP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MAD_FaceMAP - Win32 Debug MAX3
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MAD_FaceMAP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MAD_FaceMAP.mak" CFG="MAD_FaceMAP - Win32 Debug MAX3"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MAD_FaceMAP - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MAD_FaceMAP - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "MAD_FaceMAP - Win32 Debug MAX3" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

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
# ADD CPP /nologo /G6 /MT /W3 /O2 /I "C:\Maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /machine:I386 /out:"C:\3D\Plugins\MAD_FaceMAP.dlm" /libpath:"C:\Maxsdk\lib" /release

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "C:\Maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"C:\3D\Plugins\MAD_FaceMAP.dlm" /pdbtype:sept /libpath:"C:\Maxsdk\lib"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MAD_FaceMAP___Win32_Debug_MAX3"
# PROP BASE Intermediate_Dir "MAD_FaceMAP___Win32_Debug_MAX3"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MAD_FaceMAP___Win32_Debug_MAX3"
# PROP Intermediate_Dir "MAD_FaceMAP___Win32_Debug_MAX3"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "C:\Max3sdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "C:\Max3sdk\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "MAX3" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib bmm.lib core.lib geom.lib gfx.lib mesh.lib maxutil.lib maxscrpt.lib paramblk2.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"F:\3DSMAX3\Plugins\MAD_FaceMAP.dlm" /pdbtype:sept /libpath:"C:\Max3sdk\lib"
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /base:"0x105b0000" /subsystem:windows /dll /debug /machine:I386 /out:"F:\3DSMAX3\Plugins\MAD_FaceMAP3.dlm" /pdbtype:sept /libpath:"C:\Max3sdk\lib"

!ENDIF 

# Begin Target

# Name "MAD_FaceMAP - Win32 Release"
# Name "MAD_FaceMAP - Win32 Debug"
# Name "MAD_FaceMAP - Win32 Debug MAX3"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DllEntry.cpp
# End Source File
# Begin Source File

SOURCE=.\MAD_FaceMAP.cpp
# End Source File
# Begin Source File

SOURCE=.\MAD_FaceMAP.def
# End Source File
# Begin Source File

SOURCE=.\MAD_FaceMAP.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\MAD_FaceMAP.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "MAX 2.0 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Util.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CLIENT.LIB

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mesh.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bmm.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\geom.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MAX 3.0 LIBS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Paramblk2.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\core.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\geom.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gfx.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Maxscrpt.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\maxutil.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\mesh.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\bmm.lib

!IF  "$(CFG)" == "MAD_FaceMAP - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MAD_FaceMAP - Win32 Debug MAX3"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
