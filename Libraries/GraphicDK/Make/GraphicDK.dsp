# Microsoft Developer Studio Project File - Name="GraphicDK" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GraphicDK - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GraphicDK.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GraphicDK.mak" CFG="GraphicDK - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GraphicDK - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GraphicDK - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "GraphicDK - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "GraphicDK - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "GraphicDK - Win32 Final" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cwcl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK___r"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "RELEASE" /D "_WINDOWS" /D "WIN32" /D "NDEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK___r.lib"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK___d"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /W3 /GX /Zi /Od /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "_WINDOWS" /D "WIN32" /D "_DEBUG" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK___d.lib"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GDK__"
# PROP BASE Intermediate_Dir "GDK__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\GDK___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK___ed.lib"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GraphicDK___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "GraphicDK___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\GDK___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK___er.lib"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "GraphicDK___Win32_Final"
# PROP BASE Intermediate_Dir "GraphicDK___Win32_Final"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK___f"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /W3 /GX /O2 /Ob2 /I "..\..\..\Main\WinEngine\Sources" /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /I "..\..\..\extern\dx8\Include" /D "WIN32" /D "NDEBUG" /D "RELEASE" /D "_WINDOWS" /D "_FINAL_" /D "PCWIN_TOOL" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\GDK___r.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK___f.lib"

!ENDIF 

# Begin Target

# Name "GraphicDK - Win32 Release"
# Name "GraphicDK - Win32 Debug"
# Name "GraphicDK - Win32 Debug Editors"
# Name "GraphicDK - Win32 Speed Editors"
# Name "GraphicDK - Win32 Final"
# Begin Group "TEXture"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TEXture\TEX_Bink.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEX_Bink.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEX_mesh.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEX_mesh.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEX_mesh_internal.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXanimated.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXanimated.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXconvert.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXconvert.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXeditorfct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXeditorfct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXfile.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXfile.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXhardwareload.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXhardwareload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXList.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXmanager.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXmanager.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXmemory.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXmemory.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXprocedural.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXprocedural.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\TEXstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\Water_FFT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXture\Water_FFT.h
# End Source File
# End Group
# Begin Group "MATerial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MATerial\MATCompute.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATCompute.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATDraw.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATmulti.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATmulti.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATmultitexture.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATmultitexture.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATShadow.c

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# ADD CPP /Zi /Od /Yu

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATShadow.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATSingle.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATSingle.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATSprite.c

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# ADD CPP /Zi /Od /Ob1
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

# ADD BASE CPP /O2 /Ob1
# SUBTRACT BASE CPP /Z<none> /YX /Yc /Yu
# ADD CPP /O2 /Ob1
# SUBTRACT CPP /Z<none> /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATSprite.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATSprite_Load.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATstruct.h
# End Source File
# End Group
# Begin Group "GEOmetric"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_BRONX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_DEF.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_DRAW.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_FE.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_GEO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_OK3.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_OPT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_RADIO.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_RT.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LIGHTCUT_TOOLS.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LODCmpt.c

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_LODCmpt.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_MRM.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_MRM.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_SKIN.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_SKIN.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_STRIP.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEO_STRIP.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOboundingvolume.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOboundingvolume.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOcreateobject.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOcreateobject.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEODebugObject.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEODebugObject.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOdebugobjectdef.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOload.c

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

# ADD CPP /Ze /Gi- /vd1 /GR-
# SUBTRACT CPP /Gf /Gy /X

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOload.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobject.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobject.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobjectaccess.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobjectaccess.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobjectcomputing.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOobjectcomputing.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOstaticLOD.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOstaticLOD.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOsubobject.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOsubobject.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOzone.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GEOmetric\GEOzone.h
# End Source File
# End Group
# Begin Group "CAMera"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\CAMera\CAMera.c
# End Source File
# Begin Source File

SOURCE=..\Sources\CAMera\CAMera.h
# End Source File
# Begin Source File

SOURCE=..\Sources\CAMera\CAMstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\CAMera\CAMstruct.h
# End Source File
# End Group
# Begin Group "SOFT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTbackgroundimage.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTbackgroundimage.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTcolor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTcolor.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTHelper.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTHelpers.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTlinear.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTlinear.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTMatrixStack.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTMatrixStack.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTPickingBuffer.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTPickingBuffer.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTstruct.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTuvgizmo.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTUVGizmo.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTzlist.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTzlist.h
# End Source File
# End Group
# Begin Group "SELection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SELection\SELection.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SELection\SELection.h
# End Source File
# End Group
# Begin Group "GDInterface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GDInterface\GDInterface.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GDInterface\GDInterface.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GDInterface\GDIrasters.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GDInterface\GDIrasters.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GDInterface\GDIresolution.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GDInterface\GDIresolution.h
# End Source File
# End Group
# Begin Group "LIBraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DDRAW.LIB

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Extern\dxguid.lib

!IF  "$(CFG)" == "GraphicDK - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Speed Editors"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GraphicDK - Win32 Final"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "LIGHT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\LIGHT\LIGHTcolor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\LIGHT\LIGHTcompute.c
# End Source File
# Begin Source File

SOURCE=..\Sources\LIGHT\LIGHTstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\LIGHT\LIGHTstruct.h
# End Source File
# End Group
# Begin Group "GRObject"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GRObject\GROedit.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROimportfrommad.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROimportfrommad.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROmemstats.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROmemstats.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROrender.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROrender.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROsave.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROsave.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GRObject\GROstruct.h
# End Source File
# End Group
# Begin Group "WAYpoint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\WAYpoint\WAYlink.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYpoint\WAYlink.h
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYpoint\WAYpoint.c
# End Source File
# Begin Source File

SOURCE=..\Sources\WAYpoint\WAYpoint.h
# End Source File
# End Group
# Begin Group "STRing"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\STRing\STRaccess.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STReffects.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STRfont.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STRparse.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STRrender.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STRstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\STRing\STRstruct.h
# End Source File
# End Group
# Begin Group "PArticleGenerator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\PArticleGenerator\PAGstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\PArticleGenerator\PAGstruct.h
# End Source File
# End Group
# Begin Group "GFX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\GFX\GFX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFX.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXcarte.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXcarte.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXdesintegrate.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXdesintegrate.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXExplode.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXExplode.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXfade.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXfade.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXflare.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXflare.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXlightning.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXlightning.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXline.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXline.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXProfiler.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXProfiler.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXripple.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXripple.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXsmoke.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXsmoke.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspark.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspark.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspeed.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspeed.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXString.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXstring.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXtable.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXtable.h
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXwater.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXwater.h
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
