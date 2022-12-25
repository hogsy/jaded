# Microsoft Developer Studio Project File - Name="SouND" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=SouND - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SouND.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SouND.mak" CFG="SouND - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SouND - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "SouND - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "SouND - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "SouND - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE "SouND - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern\DX8\include" /D "NDEBUG" /D "RELEASE" /D "PCWIN_TOOL" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND___r.lib"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "PCWIN_TOOL" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND___d.lib"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SND__"
# PROP BASE Intermediate_Dir "SND__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\SND___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND___ed.lib"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "SouND___Win32_Final"
# PROP BASE Intermediate_Dir "SouND___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SND___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MT /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern\DX8\include" /D "NDEBUG" /D "RELEASE" /D "_FINAL_" /D "WIN32" /D "_WINDOWS" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\SND___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND___f.lib"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP BASE Use_MFC 1
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "SouND___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "SouND___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/SouND___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\SND___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\SND___er.lib"

!ENDIF 

# Begin Target

# Name "SouND - Win32 Release"
# Name "SouND - Win32 Debug"
# Name "SouND - Win32 Debug Editors"
# Name "SouND - Win32 Final"
# Name "SouND - Win32 Speed Editors"
# Begin Group "Sources"

# PROP Default_Filter ""
# Begin Group "win32SND"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\win32\win32SND.c

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SND.h

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SND_Stream.c

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SND_Stream.h

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SNDdebug.c

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SNDdebug.h

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\win32\win32SNDfx.c

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "MTX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MTX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MTX.h
# End Source File
# End Group
# Begin Group "global"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SND.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Sources\SND.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDbank.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDbank.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconst.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmacros.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDspecific.h

!IF  "$(CFG)" == "SouND - Win32 Release"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDvolume.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDvolume.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDwave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDwave.h
# End Source File
# End Group
# Begin Group "stream"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDambience.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDambience.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdialog.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdialog.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmusic.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmusic.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstream.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDstream.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDtrack.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDtrack.h
# End Source File
# End Group
# Begin Group "smodifier"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDinsert.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDinsert.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmodifier.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmodifier.h
# End Source File
# End Group
# Begin Group "interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDinterface.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDinterface.h
# End Source File
# End Group
# Begin Group "convert"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDconv.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv_pcretailadpcm.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv_pcretailadpcm.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv_xboxadpcm.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDconv_xboxadpcm.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmsadpcm.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDmsadpcm.h
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDdebug.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDdebug.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDrasters.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDrasters.h
# End Source File
# End Group
# Begin Group "fx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDfx.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDfx.h
# End Source File
# End Group
# Begin Group "wac"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SNDloadingsound.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SNDloadingsound.h
# End Source File
# End Group
# Begin Group "ediSND"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\edi\ediSND.c

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSND.h

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSND_Stream.c

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSND_Stream.h

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSNDdebug.c

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSNDdebug.h

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\edi\ediSNDfx.c

!IF  "$(CFG)" == "SouND - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "SouND - Win32 Final"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "SouND - Win32 Speed Editors"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DX8\lib\dsound.lib
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.c
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
