# Microsoft Developer Studio Project File - Name="MainEdi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MainEdi - Win32 Speed Editors
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MainEdi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MainEdi.mak" CFG="MainEdi - Win32 Speed Editors"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MainEdi - Win32 Debug Editors" (based on "Win32 (x86) Console Application")
!MESSAGE "MainEdi - Win32 Speed Editors" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MainEdi___Wi"
# PROP BASE Intermediate_Dir "MainEdi___Wi"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MainEdi___ed"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W4 /Gm /GX /Zi /Od /I "x:\MainEdi" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_DEBUG_" /D "_INTEL_386_" /D "_VISUAL_" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /G6 /MTd /W3 /GX /Z7 /Od /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "ACTIVE_EDITORS" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"x:/MainEdi/Out/Main_d.exe"
# ADD LINK32 advapi32.lib shell32.lib ctl3d32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib comctl32.lib ws2_32.lib NAFXCWD.LIB libcmtd.lib /nologo /subsystem:windows /pdb:none /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib /out:"../../../Output/ExeDlls/Jade_edd.exe" /libpath:"../Output/Libs"
# SUBTRACT LINK32 /profile

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MainEdi___Win32_Speed_Editors"
# PROP BASE Intermediate_Dir "MainEdi___Win32_Speed_Editors"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../Output/ExeDlls"
# PROP Intermediate_Dir "../../../Output/Tmp/MainEdi___er"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /GX /Zi /Od /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Dlls" /D "ACTIVE_EDITORS" /D "_AFXDLL" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Fr /Yu"Precomp.h" /FD /c
# ADD CPP /nologo /G6 /MTd /W3 /GX /Ob2 /I "..\..\..\Libraries\SDK\Sources" /I "..\..\..\Libraries\GraphicDK\Sources" /I "..\..\..\Extern" /I "..\..\..\Libraries" /I "..\Sources" /I "..\..\..\Main\WinEditors\Sources" /I "..\..\..\Dlls" /I "..\..\..\Extern\DX8\include" /D "_DEBUG" /D "ACTIVE_EDITORS" /D "WIN32" /D "_WINDOWS" /D "SPEED_EDITORS" /Yu"Precomp.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib msacm32.lib ws2_32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /out:"../../../Output/ExeDlls/Jade_edd.exe" /libpath:"../Output/Libs"
# ADD LINK32 advapi32.lib shell32.lib ctl3d32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib msacm32.lib comctl32.lib ws2_32.lib NAFXCWD.LIB libcmtd.lib /nologo /subsystem:windows /pdb:none /machine:I386 /nodefaultlib:"libcd.lib" /nodefaultlib:"libc.lib" /nodefaultlib /out:"../../../Output/ExeDlls/Jade_edr.exe" /libpath:"../Output/Libs"
# SUBTRACT LINK32 /profile /debug

!ENDIF 

# Begin Target

# Name "MainEdi - Win32 Debug Editors"
# Name "MainEdi - Win32 Speed Editors"
# Begin Group "VAVview"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist.h
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_action.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_ai.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_anim.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_col.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_obj.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVlist_snd.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVtypes.h
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_aifunc.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_anim.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_bool.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_bounding.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_color.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_constfloat.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_constint.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_custsep.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_file.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_flags.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_float.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_go.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_grotype.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_grp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_hexa.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_int.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_key.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_matrix.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_modifier.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_network.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_smodifier.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_sndkey.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_string.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_text.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_vector.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\VAVview\VAVview_zone.cpp
# End Source File
# End Group
# Begin Group "F3Dframe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_action.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_ai.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_anim.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_cam.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_col.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_curve.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3DView_grid.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_gro.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_grp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_helpers.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_mouse.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_obj.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_op.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3DView_pick.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_rli.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3DView_Selection.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3DView_subobject.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_undo.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_undo.h
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_way.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_wor.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\F3Dframe\F3Dview_zonebv.cpp
# End Source File
# End Group
# Begin Group "Res"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\Res\ALPHA.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\bigfile.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\bmppropexp.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cadenasferme.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cadenasouvert.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cam.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\campane.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\camtgt.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\camzoom.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\camzoom1.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\COLOR.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cur00002.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cur00003.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor4.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor5.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor6.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cursor7.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\dragcopy.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\dragdel.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\dragmove.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\dragnone.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\dragquest.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eai_break.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eai_break1.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eai_breakstop.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eai_error.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eai_idi_.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\ebro_idr.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\ebro_tb.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_about.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_image.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_image1.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_intro.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_tb.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_bumpoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_bumpon.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_fontoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_fonton.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_interfaceoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_interfaceon.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapalphaoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapalphaon.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapcoloroff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapcoloron.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapkeepborderoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapkeepborderon.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_mipmapoff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_MipmapOn.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_updatefonton.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\EDITEX_updatepaloff.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\elog_tb.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eout_idr.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\epro_idr.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eson_toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\eve_idi_.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\goparent.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\Gro_ChangeId.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\Gro_FlipNormals.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\hand.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\icon_cur.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\iconPick.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\idimain.ico
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\lckmenu.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\LUMIN.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\main_idc.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\Morph_delete.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\morph_down.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\morph_first.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\morph_last.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\morph_up.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\pipe.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\rarrow.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\Res.h
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\Res.rc
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\selection_crossing.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\selection_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\unlckmenu.bmp
# End Source File
# End Group
# Begin Group "DIAlogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIA_SKN_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIA_SKN_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIA_UPDATE_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIA_UPDATE_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAanim_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAanim_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAbackgroundimage_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAbackgroundimage_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAbase.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAbase.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAchecklist_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAchecklist_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcheckworld_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcheckworld_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIACOLOR_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIACOLOR_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcompletion_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcompletion_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcopymatrixfrom_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcopymatrixfrom_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcreategeometry_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcreategeometry_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcreategeometry_inside.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAcreategeometry_inside.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAdropgro_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAdropgro_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfavorites_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfavorites_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfile_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfile_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfindai_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfindai_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfindfile_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfindfile_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfontdescriptor_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAfontdescriptor_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgaoinfo_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgaoinfo_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgromodifier_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgromodifier_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgromodifiersnap_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgromodifiersnap_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgrovertexpos_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAgrovertexpos_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAjoystick_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAjoystick_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAkeyboard_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAkeyboard_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlang_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlang_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlink_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlink_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlist_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAlist_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmatrix_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmatrix_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmessage_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmessage_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmixer_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmixer_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmorphing_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmorphing_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmsglink_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmsglink_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmtx_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAmtx_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAname_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAname_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAorder_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAorder_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIApalette_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIApalette_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAradiosity_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAradiosity_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIArli_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIArli_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIArlicarte_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIArlicarte_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsaveaddmatrix_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsaveaddmatrix_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAselection_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAselection_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAshape_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAshape_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsmodifier_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsmodifier_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndgenplayer_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndgenplayer_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndinsert_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndinsert_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndinstance_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndinstance_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndmute_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndmute_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndtrans_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndtrans_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndvumeter.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndvumeter.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndvumeterset.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAsndvumeterset.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAspeedact_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAspeedact_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtexture_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtexture_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtimeinterpolation_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtimeinterpolation_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_curveview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_curveview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_groview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAtoolbox_groview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAundo_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAundo_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAuvmapper_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAuvmapper_dlg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAvector_dlg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\DIAlogs\DIAvector_dlg.h
# End Source File
# End Group
# Begin Group "EDI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Sources\EDIaction.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIaction.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIapp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIapp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIapp_help.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIapp_options.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseview.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseview.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseview_game.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseview_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIbaseview_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIeditors_infos.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIeditors_reg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIerrid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIicons.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_act.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_act.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_dd.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_desk.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_ini.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_msg.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_universe.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImainframe_vss.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDImsg.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIpaths.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIsplitter.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIsplitter.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIstrings.h
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIundo.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\EDIundo.h
# End Source File
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Group "DX8"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\DX8\lib\d3d8.lib
# End Source File
# End Group
# Begin Group "OPENGL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Extern\OPENGL32.LIB
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\Output\Libs\AI___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\EDI___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\ENG___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\GDK___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SDK___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SND___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\NET___ed.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\GDK___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\AI___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\SDK___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\ENG___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\OGL___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\EDI___er.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MPEG___r.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\JPEG___r.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MPEG___d.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\JPEG___d.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_rasterize___d.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_rasterize___r.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___d.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_loadsave___r.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\OGL___ed.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___r.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\MAD_mem___d.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\DX8___edr.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Output\Libs\DX8___edd.lib

!IF  "$(CFG)" == "MainEdi - Win32 Debug Editors"

!ELSEIF  "$(CFG)" == "MainEdi - Win32 Speed Editors"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\Sources\Res\bitmap6.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\cur00004.cur
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_idr_.bmp
# End Source File
# Begin Source File

SOURCE=..\Sources\Res\edi_image2.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\Libraries\EDItors\Sources\Precomp.cpp
# ADD CPP /Yc"Precomp.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\Libraries\EDItors\Sources\Precomp.h
# End Source File
# End Target
# End Project
