# Microsoft Developer Studio Project File - Name="GraphicDK_xb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Xbox Static Library" 0x0b04

CFG=GraphicDK_xb - Xbox Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GraphicDK_xb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GraphicDK_xb.mak" CFG="GraphicDK_xb - Xbox Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GraphicDK_xb - Xbox Release" (based on "Xbox Static Library")
!MESSAGE "GraphicDK_xb - Xbox Debug" (based on "Xbox Static Library")
!MESSAGE "GraphicDK_xb - Xbox Tuning" (based on "Xbox Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe

!IF  "$(CFG)" == "GraphicDK_xb - Xbox Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK_xb_r"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "_XBOX" /D "NDEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK_xb_r.lib"

!ELSEIF  "$(CFG)" == "GraphicDK_xb - Xbox Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK_xb_d"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK_xb_d.lib"

!ELSEIF  "$(CFG)" == "GraphicDK_xb - Xbox Tuning"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GraphicDK_xb___Xbox_Tuning"
# PROP BASE Intermediate_Dir "GraphicDK_xb___Xbox_Tuning"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/GDK_xb_t"
# PROP Target_Dir ""
RSC=rc.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /YX /FD /G6 /Ztmp /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /O2 /I "..\..\..\Libraries\SDK" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Dlls" /D "WIN32" /D "_XBOX" /D "_DEBUG" /D "XBOX_TUNING" /YX /FD /G6 /Ztmp /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\GDK_xb_d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\GDK_xb_t.lib"

!ENDIF 

# Begin Target

# Name "GraphicDK_xb - Xbox Release"
# Name "GraphicDK_xb - Xbox Debug"
# Name "GraphicDK_xb - Xbox Tuning"
# Begin Group "GFX"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Gfx\GFX.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFX.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXcarte.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXcarte.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXdesintegrate.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXdesintegrate.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXExplode.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXExplode.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXfade.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXfade.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXflare.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXflare.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXlightning.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXlightning.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXline.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXline.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXProfiler.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXProfiler.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXripple.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXripple.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXsmoke.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXsmoke.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXspark.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspark.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXspeed.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXspeed.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXString.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXstring.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXtable.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXtable.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Gfx\GFXwater.c
# End Source File
# Begin Source File

SOURCE=..\Sources\GFX\GFXwater.h
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
# Begin Group "LIGHT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Light\LIGHTcolor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Light\LIGHTcompute.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Light\LIGHTstruct.c
# End Source File
# Begin Source File

SOURCE=..\Sources\LIGHT\LIGHTstruct.h
# End Source File
# End Group
# Begin Group "LIBraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DDRAW.LIB
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\..\Extern\dxguid.lib
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
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
# Begin Group "SELection"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SELection\SELection.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SELection\SELection.h
# End Source File
# End Group
# Begin Group "SOFT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Soft\SOFTbackgroundimage.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTbackgroundimage.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTcolor.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTcolor.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTHelper.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTHelpers.c
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTlinear.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTlinear.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTMatrixStack.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTMatrixStack.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTPickingBuffer.c
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

SOURCE=..\Sources\Soft\SOFTuvgizmo.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTUVGizmo.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Soft\SOFTzlist.c
# End Source File
# Begin Source File

SOURCE=..\Sources\SOFT\SOFTzlist.h
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

!IF  "$(CFG)" == "GraphicDK_xb - Xbox Release"

!ELSEIF  "$(CFG)" == "GraphicDK_xb - Xbox Debug"

# ADD BASE CPP /Ze /Gi- /vd1 /GR-
# SUBTRACT BASE CPP /Gf /Gy /X
# ADD CPP /Ze /Gi- /vd1 /GR-
# SUBTRACT CPP /Gf /Gy /X

!ELSEIF  "$(CFG)" == "GraphicDK_xb - Xbox Tuning"

# ADD BASE CPP /Ze /Gi- /vd1 /GR-
# SUBTRACT BASE CPP /Gf /Gy /X
# ADD CPP /Ze /Gi- /vd1 /GR-
# SUBTRACT CPP /X

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
# Begin Group "MATerial"

# PROP Default_Filter ""
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
# Begin Group "TEXture"

# PROP Default_Filter ""
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

SOURCE=..\Sources\TEXture\TEXlist.c
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
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.c
# ADD BASE CPP /Yc"Precomp.h"
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\Sources\Precomp.h
# End Source File
# End Target
# End Project
