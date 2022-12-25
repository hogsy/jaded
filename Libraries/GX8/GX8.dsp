# Microsoft Developer Studio Project File - Name="GX8_xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=GX8_xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GX8_xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GX8_xb.mak" CFG="GX8_xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GX8_xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "GX8_xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "GX8_xb - Xbox Tuning" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "GX8_xb - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/GX8_xb_r"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Libraries" /I "..\..\Libraries\DX8DK\Sources" /I "..\..\Dlls" /D "WIN32" /D "_XBOX" /D "NDEBUG" /D "_PALETTE2TRUECOLOR" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8_xb_r.lib"

!ELSEIF  "$(CFG)" == "GX8_xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/GX8_xb_d"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Libraries" /I "..\..\Libraries\DX8DK\Sources" /I "..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "_PALETTE2TRUECOLOR" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8_xb_d.lib"

!ELSEIF  "$(CFG)" == "GX8_xb - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GX8_xb___Xbox_Tuning"
# PROP BASE Intermediate_Dir "GX8_xb___Xbox_Tuning"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Output/Libs"
# PROP Intermediate_Dir "../../Output/Tmp/GX8_xb_t"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Extern" /I "..\..\Libraries" /I "..\..\Libraries\GraphicDK\Sources" /I "..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\Libraries\SDK" /I "..\..\Main\WinEditors\Sources" /I "..\..\Libraries\SDK\Sources" /I "..\..\Libraries" /I "..\..\Libraries\Dx8DK\Sources" /I "..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /D "_PALETTE2TRUECOLOR" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../Output\Libs\GX8_xb_d.lib"
# ADD LIB32 /nologo /out:"../../Output\Libs\DX8_xb_t.lib"

!ENDIF 

# Begin Target

# Name "GX8_xb - Xbox Release"
# Name "GX8_xb - Xbox Debug"
# Name "GX8_xb - Xbox Tuning"
# Begin Group "Vertex Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VShaders\sprite_xbox.vsh

!IF  "$(CFG)" == "GX8_xb - Xbox Release"

# Begin Custom Build
InputDir=.\VShaders
InputPath=.\VShaders\sprite_xbox.vsh

"$(InputDir)\sprite_xbox.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xsasm /h $(InputDir)\sprite_xbox.vsh

# End Custom Build

!ELSEIF  "$(CFG)" == "GX8_xb - Xbox Debug"

# Begin Custom Build
InputDir=.\VShaders
InputPath=.\VShaders\sprite_xbox.vsh

"$(InputDir)\sprite_xbox.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xsasm /h $(InputDir)\sprite_xbox.vsh

# End Custom Build

!ELSEIF  "$(CFG)" == "GX8_xb - Xbox Tuning"

# Begin Custom Build
InputDir=.\VShaders
InputPath=.\VShaders\sprite_xbox.vsh

"$(InputDir)\sprite_xbox.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	xsasm /h $(InputDir)\sprite_xbox.vsh

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\BuildUVs.c
# End Source File
# Begin Source File

SOURCE=.\BuildUVs.h
# End Source File
# Begin Source File

SOURCE=.\DX8AddInfo.h
# End Source File
# Begin Source File

SOURCE=.\DX8debugfct.c
# End Source File
# Begin Source File

SOURCE=.\DX8debugfct.h
# End Source File
# Begin Source File

SOURCE=.\DX8init.c
# End Source File
# Begin Source File

SOURCE=.\DX8init.h
# End Source File
# Begin Source File

SOURCE=.\DX8Light.c
# End Source File
# Begin Source File

SOURCE=.\DX8Light.h
# End Source File
# Begin Source File

SOURCE=.\DX8renderstate.c
# End Source File
# Begin Source File

SOURCE=.\DX8renderstate.h
# End Source File
# Begin Source File

SOURCE=.\DX8request.c
# End Source File
# Begin Source File

SOURCE=.\DX8request.h
# End Source File
# Begin Source File

SOURCE=.\DX8tex.c
# End Source File
# Begin Source File

SOURCE=.\DX8tex.h
# End Source File
# Begin Source File

SOURCE=.\DX8VertexBuffer.c
# End Source File
# Begin Source File

SOURCE=.\DX8VertexBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Precomp.c
# End Source File
# Begin Source File

SOURCE=.\Precomp.h
# End Source File
# Begin Source File

SOURCE=.\VertexShaders.c
# End Source File
# Begin Source File

SOURCE=.\VertexShaders.h
# End Source File
# End Target
# End Project
