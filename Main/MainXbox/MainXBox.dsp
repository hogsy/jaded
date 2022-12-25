# Microsoft Developer Studio Project File - Name="MainXBox" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Application" 0x0b01

CFG=MainXBox - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MainXBox.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MainXBox.mak" CFG="MainXBox - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MainXBox - Xbox Release" (based on "Xbox Application")
!MESSAGE "MainXBox - Xbox Debug" (based on "Xbox Application")
!MESSAGE "MainXBox - Xbox Tuning" (based on "Xbox Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Output/ExeDlls"
# PROP Intermediate_Dir "../../Output/Tmp/MainXBox_r"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
RSC=rc.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\..\..\Main\MainXBox\Sources" /I "..\..\..\Libraries\Dx8DK\Sources" /I "..\..\..\Dlls" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\Sources" /I "..\..\Main\MainXBox\Sources" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /D "_DX8" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /subsystem:xbox /fixed:no /tmp /OPT:REF
# ADD LINK32 xapilib.lib d3d8.lib d3dx8.lib xgraphics.lib dsound.lib dmusic.lib xnet.lib xboxkrnl.lib /nologo /machine:I386 /out:"../../Output/ExeDlls/MainXBoxR.exe" /subsystem:xbox /fixed:no /tmp /OPT:REF
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000
# ADD XBE /nologo /stack:0x10000
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/ExeDlls"
# PROP Intermediate_Dir "../../Output/Tmp/MainXBox_d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
RSC=rc.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Main\MainXBox\Sources" /I "..\..\Libraries\Dx8DK\Sources" /I "..\..\Dlls" /I "..\..\Libraries\SDK\Sources" /I "..\Sources" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX"Precomp.h" /FD /G6 /Ztmp /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /tmp
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /out:"../../Output/ExeDlls/MainXBoxD.exe" /subsystem:xbox /fixed:no /tmp
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x10000 /debug /out:"../../Output/ExeDlls/MainXBoxD.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MainXBox___Xbox_Tuning"
# PROP BASE Intermediate_Dir "MainXBox___Xbox_Tuning"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/ExeDlls"
# PROP Intermediate_Dir "../../Output/Tmp/MainXBox_t"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
RSC=rc.exe
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Main\MainXBox\Sources" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /I "..\..\Libraries\SDK\Sources" /I "..\Sources" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX"Precomp.h" /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Main\MainXBox\Sources" /I "..\..\Libraries\Dx8DK\Sources" /I "..\..\Dlls" /I "..\..\Libraries\SDK\Sources" /I "..\Sources" /D "_DX8" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /YX"Precomp.h" /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /subsystem:xbox /fixed:no /tmp
# ADD LINK32 xapilibd.lib d3d8d.lib d3dx8d.lib xgraphicsd.lib dsoundd.lib dmusicd.lib xnetd.lib xboxkrnl.lib /nologo /incremental:no /debug /machine:I386 /out:"../../Output/ExeDlls/MainXBoxT.exe" /subsystem:xbox /fixed:no /tmp
XBE=imagebld.exe
# ADD BASE XBE /nologo /stack:0x10000 /debug
# ADD XBE /nologo /stack:0x10000 /debug /out:"../../Output/ExeDlls/MainXBoxT.xbe"
XBCP=xbecopy.exe
# ADD BASE XBCP /NOLOGO
# ADD XBCP /NOLOGO

!ENDIF 

# Begin Target

# Name "MainXBox - Xbox Release"
# Name "MainXBox - Xbox Debug"
# Name "MainXBox - Xbox Tuning"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Sources\WinMain.c
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\..\Output\Libs\AI_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\AI_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\AI_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\DX8_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\DX8_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\DX8_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\ENG_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\ENG_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\ENG_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\GDK_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\GDK_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\GDK_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SDK_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SDK_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SDK_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SND_xb_d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SND_xb_r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\SND_xb_t.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\MPEG_xb__r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\JPEG_xb__r.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\MPEG_xb__d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Output\Libs\JPEG_xb__d.lib

!IF  "$(CFG)" == "MainXBox - Xbox Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Debug"

!ELSEIF  "$(CFG)" == "MainXBox - Xbox Tuning"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Sources\Precomp.c
# End Source File
# Begin Source File

SOURCE=.\Sources\Precomp.h
# End Source File
# End Target
# End Project
