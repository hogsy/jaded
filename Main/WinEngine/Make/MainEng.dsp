# Microsoft Developer Studio Project File - Name="MainEng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MainEng - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MainEng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MainEng.mak" CFG="MainEng - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MainEng - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MainEng - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "MainEng - Win32 Final" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MainEng___r"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "RELEASE" /D "_WINDOWS" /D "PCWIN_TOOL" /D "WIN32" /D "NDEBUG" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /i "Res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 msvcrt.lib libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../../Output/ExeDlls/Jade_enr.exe" /libpath:"../Output/Libs"
# SUBTRACT LINK32 /profile /debug /force

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MainEng___d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Dlls" /D "_WINDOWS" /D "PCWIN_TOOL" /D "WIN32" /D "_DEBUG" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /i "Res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libcd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib ws2_32.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib /out:"../../../Output/ExeDlls/Jade_end.exe" /libpath:"../Output/Libs"
# SUBTRACT LINK32 /profile /force

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MainEng___Win32_Final"
# PROP BASE Intermediate_Dir "MainEng___Win32_Final"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MainEng___f"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "NDEBUG" /D "RELEASE" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Dlls" /D "PCWIN_TOOL" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /YX /FD /c
# ADD BASE RSC /l 0x40c /i "Res" /d "NDEBUG"
# ADD RSC /l 0x40c /i "Res" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib msvcrt.lib libc.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../../Output/ExeDlls/Jade_enr.exe" /libpath:"../Output/Libs"
# SUBTRACT BASE LINK32 /profile /debug /force
# ADD LINK32 msvcrt.lib libc.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib ws2_32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib /out:"../../../Output/ExeDlls/Jade.exe" /libpath:"../Output/Libs"
# SUBTRACT LINK32 /profile /debug /force

!ENDIF 

# Begin Target

# Name "MainEng - Win32 Release"
# Name "MainEng - Win32 Debug"
# Name "MainEng - Win32 Final"
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Group "DX8"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DX8\lib\d3d8.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\Output\Libs\AI___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\AI___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\AI___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\ENG___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\ENG___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\ENG___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\GDK___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\GDK___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\GDK___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\OGL___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SDK___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SDK___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SDK___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SND___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SND___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SND___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\DX8___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\DX8___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\OGL___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MPEG___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\JPEG___r.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MPEG___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\JPEG___d.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\OGL___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\DX8___f.lib

!IF  "$(CFG)" == "MainEng - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEng - Win32 Final"

!ENDIF 

# End Source File
# End Group
# Begin Group "MAI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MAIwind.c
# End Source File
# End Group
# End Target
# End Project
