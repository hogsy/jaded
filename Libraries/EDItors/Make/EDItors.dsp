# Microsoft Developer Studio Project File - Name="EDItors" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=EDItors - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "EDItors.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "EDItors.mak" CFG="EDItors - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EDItors - Win32 Debug Editors" (based on "Win32 (x86) Static Library")
!MESSAGE "EDItors - Win32 Speed Editors" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "EDItors - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "EDItors__"
# PROP BASE Intermediate_Dir "EDItors__"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/EDI___ed"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /I "x:\SDK" /I "x:\SDK\Libraries" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Zi /Od /I "..\..\..\Extern\DX8\include" /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\Engine\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"x:/SDK/Out\EDI___d.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\EDI___ed.lib"

!ELSEIF  "$(CFG)" == "EDItors - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "EDItors___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "EDItors___Win32_Speed_Editors"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/Libs"
# PROP Intermediate_Dir "../../../Output/Tmp/EDI___er"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\Engine\Sources" /I "..\..\..\Dlls" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /O2 /Ob2 /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Libraries\Engine\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "SPEED_EDITORS" /Yu"Precomp.h" /FD /c
# ADD BASE RSC /l 0x40c
# ADD RSC /l 0x40c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"../../../Output\Libs\EDI___ed.lib"
# ADD LIB32 /nologo /out:"../../../Output\Libs\EDI___er.lib"

!ENDIF 

# Begin Target

# Name "EDItors - Win32 Debug Editors"
# Name "EDItors - Win32 Speed Editors"
# Begin Group "BROwser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\BROwser\BROerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_extras.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_fav.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_impex.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_mdfy.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROframe_vss.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROgrpctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROgrpctrl.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROlistctrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROlistctrl.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROtreectrl.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\BROwser\BROtreectrl.h
# End Source File
# End Group
# Begin Group "TEXtures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXtures\TEXscroll.h
# End Source File
# End Group
# Begin Group "PROperties"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\PROperties\PROstrings.h
# End Source File
# End Group
# Begin Group "OUTput"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\OUTput\OUTstrings.h
# End Source File
# End Group
# Begin Group "LOGfile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\LOGfile\LOGmsg.h
# End Source File
# End Group
# Begin Group "AIscript"

# PROP Default_Filter ""
# Begin Group "Compiler"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_2C.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_optim.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_out.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_pp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_ref.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_scan.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\Compiler\AIcompile_vars.cpp
# End Source File
# End Group
# Begin Group "EditView"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIleftview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIleftview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIpane.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIpane.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIundo.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIundo.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview_colors.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview_km.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview_mdfy.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\EditView\AIview_scan.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\AIscript\AIerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_comp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_debug.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIleftframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIleftframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\AIscript\AIstrings.h
# End Source File
# End Group
# Begin Group "MENu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MENu\MENframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENin.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENin.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENinmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENinmenu.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENlist.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENlist.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENmenu.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENsubmenu.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MENu\MENsubmenu.h
# End Source File
# End Group
# Begin Group "RASters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\RASters\RASframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\RASters\RASframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\RASters\RASframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\RASters\RASframe_msg.cpp
# End Source File
# End Group
# Begin Group "MATerial"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe_Convert.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MATframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MuTex.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\MATerial\MuTex.h
# End Source File
# End Group
# Begin Group "TEXTe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\TEXTe\TEXTscroll.h
# End Source File
# End Group
# Begin Group "SOuNd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\SOuNd\SONerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_bank.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_insert.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_Instance.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_mdf.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_mtx.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_smodifier.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONframe_sound.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONmixer.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONmixer.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONpane.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONpane.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONutil.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONutil.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smd.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smd.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smdins.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smdins.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smdlist.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONview_smdlist.h
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONvumeter.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\SOuNd\SONvumeter.h
# End Source File
# End Group
# Begin Group "ACTions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ACTions\EACTview.h
# End Source File
# End Group
# Begin Group "EVEnts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEevent.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEevent.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEevent_aif.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEevent_interpolationkey.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEevent_timekey.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe_anim.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEinside.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEinside.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEmsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEscroll.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEscroll.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEtrack.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EVEnts\EVEtrack.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Precomp.cpp
# ADD CPP /Yc"Precomp.h"
# End Source File
# End Target
# End Project
