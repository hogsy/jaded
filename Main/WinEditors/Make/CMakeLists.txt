set(PROJECT_NAME JadeShell)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../../Extern/Greta/regexpr2.cpp"
    "../../../Libraries/EDItors/Sources/Precomp.cpp"
    "../../../Libraries/EDItors/Sources/Precomp.h"
    "cpp.hint"
)
source_group("" FILES ${no_group_source_files})

set(DATaControl
    "../Sources/DATaControl/DATCCompression.h"
    "../Sources/DATaControl/DATCP4BFObject.cpp"
    "../Sources/DATaControl/DATCP4BFObject.h"
#    "../Sources/DATaControl/DATCP4ClientApi.cpp"
    "../Sources/DATaControl/DATCP4ClientApi.h"
#    "../Sources/DATaControl/DATCP4Message.cpp"
    "../Sources/DATaControl/DATCP4Message.h"
#    "../Sources/DATaControl/DATCPerforce.cpp"
#    "../Sources/DATaControl/DATCPerforce.h"
#    "../Sources/DATaControl/DATCPerforceIntegrate.cpp"
)
source_group("DATaControl" FILES ${DATaControl})

set(DATaControl__P4ClientUsers
    "../Sources/DATaControl/DATCP4ClientInfo.h"
    "../Sources/DATaControl/DATCP4ClientUserAdd.h"
    "../Sources/DATaControl/DATCP4ClientUserAttribute.h"
    "../Sources/DATaControl/DATCP4ClientUserChange.h"
#    "../Sources/DATaControl/DATCP4ClientUserChanges.cpp"
    "../Sources/DATaControl/DATCP4ClientUserChanges.h"
    "../Sources/DATaControl/DATCP4ClientUserDelete.h"
    "../Sources/DATaControl/DATCP4ClientUserDescribe.h"
#    "../Sources/DATaControl/DATCP4ClientUserDiff.cpp"
    "../Sources/DATaControl/DATCP4ClientUserDiff.h"
    "../Sources/DATaControl/DATCP4ClientUserEdit.h"
    "../Sources/DATaControl/DATCP4ClientUserFiles.h"
#    "../Sources/DATaControl/DATCP4ClientUserFlush.cpp"
    "../Sources/DATaControl/DATCP4ClientUserFlush.h"
    "../Sources/DATaControl/DATCP4ClientUserFStatBF.h"
    "../Sources/DATaControl/DATCP4ClientUserHeader.h"
#    "../Sources/DATaControl/DATCP4ClientUserHistory.cpp"
    "../Sources/DATaControl/DATCP4ClientUserHistory.h"
    "../Sources/DATaControl/DATCP4ClientUserIntegrate.h"
    "../Sources/DATaControl/DATCP4ClientUserLogin.h"
#    "../Sources/DATaControl/DATCP4ClientUserOpened.cpp"
    "../Sources/DATaControl/DATCP4ClientUserOpened.h"
#    "../Sources/DATaControl/DATCP4ClientUserPrint.cpp"
    "../Sources/DATaControl/DATCP4ClientUserPrint.h"
    "../Sources/DATaControl/DATCP4ClientUserReopen.h"
    "../Sources/DATaControl/DATCP4ClientUserResolve.h"
    "../Sources/DATaControl/DATCP4ClientUserRevert.h"
    "../Sources/DATaControl/DATCP4ClientUserSubmit.h"
    "../Sources/DATaControl/DATCP4ClientUserSync.h"
    "../Sources/DATaControl/DATCP4ClientUserSyncPreview.h"
    "../Sources/DATaControl/perforceui.cpp"
    "../Sources/DATaControl/perforceui.h"
)
source_group("DATaControl\\P4ClientUsers" FILES ${DATaControl__P4ClientUsers})

set(DATaControl__P4Commands
#    "../Sources/DATaControl/DATCPerforceAdd.cpp"
#    "../Sources/DATaControl/DATCPerforceChange.cpp"
#    "../Sources/DATaControl/DATCPerforceClient.cpp"
#    "../Sources/DATaControl/DATCPerforceDelete.cpp"
#    "../Sources/DATaControl/DATCPerforceDepots.cpp"
#    "../Sources/DATaControl/DATCPerforceDescribe.cpp"
#    "../Sources/DATaControl/DATCPerforceDiff.cpp"
#    "../Sources/DATaControl/DATCPerforceDirs.cpp"
#    "../Sources/DATaControl/DATCPerforceEdit.cpp"
#    "../Sources/DATaControl/DATCPerforceFileInfo.cpp"
#    "../Sources/DATaControl/DATCPerforceFiles.cpp"
#    "../Sources/DATaControl/DATCPerforceFlush.cpp"
#    "../Sources/DATaControl/DATCPerforceFStat.cpp"
#    "../Sources/DATaControl/DATCPerforceHistory.cpp"
#    "../Sources/DATaControl/DATCPerforceOpened.cpp"
#    "../Sources/DATaControl/DATCPerforcePrint.cpp"
#    "../Sources/DATaControl/DATCPerforceReopen.cpp"
#    "../Sources/DATaControl/DATCPerforceResolve.cpp"
#    "../Sources/DATaControl/DATCPerforceRevert.cpp"
#    "../Sources/DATaControl/DATCPerforceSubmit.cpp"
#    "../Sources/DATaControl/DATCPerforceSync.cpp"
#    "../Sources/DATaControl/DATCPerforceUsers.cpp"
)
source_group("DATaControl\\P4Commands" FILES ${DATaControl__P4Commands})

set(DATaControl__P4FileSys
#    "../Sources/DATaControl/DATCP4FileSys.cpp"
    "../Sources/DATaControl/DATCP4FileSys.h"
    "../Sources/DATaControl/DATCP4FileSysDelete.h"
#    "../Sources/DATaControl/DATCP4FileSysDiff.cpp"
    "../Sources/DATaControl/DATCP4FileSysDiff.h"
    "../Sources/DATaControl/DATCP4FileSysHeader.h"
#    "../Sources/DATaControl/DATCP4FileSysSubmit.cpp"
    "../Sources/DATaControl/DATCP4FileSysSubmit.h"
#    "../Sources/DATaControl/DATCP4FileSysSync.cpp"
    "../Sources/DATaControl/DATCP4FileSysSync.h"
)
source_group("DATaControl\\P4FileSys" FILES ${DATaControl__P4FileSys})

set(DATaControl__P4Utils
#    "../Sources/DATaControl/DATCPerforceVerifyLinks.cpp"
    "../Sources/DATaControl/DATCUserBreak.h"
    "../Sources/DATaControl/DATCUtils.cpp"
    "../Sources/DATaControl/DATCUtils.h"
#    "../Sources/DATaControl/DATCUtilsDiff.cpp"
)
source_group("DATaControl\\P4Utils" FILES ${DATaControl__P4Utils})

set(DATaControl__Zlib
    "../../../Extern/zlib/adler32.c"
    "../../../Extern/zlib/compress.c"
    "../../../Extern/zlib/crc32.c"
    "../../../Extern/zlib/crc32.h"
    "../../../Extern/zlib/deflate.c"
    "../../../Extern/zlib/deflate.h"
    "../../../Extern/zlib/example.c"
    "../../../Extern/zlib/gzio.c"
    "../../../Extern/zlib/infback.c"
    "../../../Extern/zlib/inffast.c"
    "../../../Extern/zlib/inffast.h"
    "../../../Extern/zlib/inffixed.h"
    "../../../Extern/zlib/inflate.c"
    "../../../Extern/zlib/inflate.h"
    "../../../Extern/zlib/inftrees.c"
    "../../../Extern/zlib/inftrees.h"
    "../../../Extern/zlib/trees.c"
    "../../../Extern/zlib/trees.h"
    "../../../Extern/zlib/uncompr.c"
    "../../../Extern/zlib/zconf.h"
    "../../../Extern/zlib/zconf.in.h"
    "../../../Extern/zlib/zlib.h"
    "../../../Extern/zlib/zutil.c"
    "../../../Extern/zlib/zutil.h"
)
source_group("DATaControl\\Zlib" FILES ${DATaControl__Zlib})

set(DIAlogs
    "../Sources/DIAlogs/BaseTabCtrl.cpp"
    "../Sources/DIAlogs/BaseTabCtrl.h"
    "../Sources/DIAlogs/BtnST.cpp"
    "../Sources/DIAlogs/BtnST.h"
    "../Sources/DIAlogs/CCheckList.cpp"
    "../Sources/DIAlogs/CCheckList.h"
#    "../Sources/DIAlogs/DATCP4FileFilter.cpp"
    "../Sources/DIAlogs/DATCP4FileFilter.h"
    "../Sources/DIAlogs/DIA_RADIOSTY2.cpp"
    "../Sources/DIAlogs/DIA_RADIOSTY2.h"
    "../Sources/DIAlogs/DIA_SKN_dlg.cpp"
    "../Sources/DIAlogs/DIA_SKN_dlg.h"
    "../Sources/DIAlogs/DIA_SmoothSel_dlg.cpp"
    "../Sources/DIAlogs/DIA_SmoothSel_dlg.h"
    "../Sources/DIAlogs/DIA_UPDATE_dlg.cpp"
    "../Sources/DIAlogs/DIA_UPDATE_dlg.h"
    "../Sources/DIAlogs/DIAanim_dlg.cpp"
    "../Sources/DIAlogs/DIAanim_dlg.h"
    "../Sources/DIAlogs/DIAbackgroundimage_dlg.cpp"
    "../Sources/DIAlogs/DIAbackgroundimage_dlg.h"
    "../Sources/DIAlogs/DIAbase.cpp"
    "../Sources/DIAlogs/DIAbase.h"
    "../Sources/DIAlogs/DIAbrickmapper_dlg.cpp"
    "../Sources/DIAlogs/DIAbrickmapper_dlg.h"
    "../Sources/DIAlogs/DIAbrofilter_dlg.cpp"
    "../Sources/DIAlogs/DIAbrofilter_dlg.h"
    "../Sources/DIAlogs/DIAchecklist_dlg.cpp"
    "../Sources/DIAlogs/DIAchecklist_dlg.h"
    "../Sources/DIAlogs/DIAcheckworld_dlg.cpp"
    "../Sources/DIAlogs/DIAcheckworld_dlg.h"
    "../Sources/DIAlogs/DIACOLOR_dlg.cpp"
    "../Sources/DIAlogs/DIACOLOR_dlg.h"
    "../Sources/DIAlogs/DIAComboBoxCompletion.cpp"
    "../Sources/DIAlogs/DIAComboBoxCompletion.h"
    "../Sources/DIAlogs/DIAcompletion_dlg.cpp"
    "../Sources/DIAlogs/DIAcompletion_dlg.h"
    "../Sources/DIAlogs/DIAcopymatrixfrom_dlg.cpp"
    "../Sources/DIAlogs/DIAcopymatrixfrom_dlg.h"
    "../Sources/DIAlogs/DIAcreategeometry_dlg.cpp"
    "../Sources/DIAlogs/DIAcreategeometry_dlg.h"
    "../Sources/DIAlogs/DIAcreategeometry_inside.cpp"
    "../Sources/DIAlogs/DIAcreategeometry_inside.h"
#    "../Sources/DIAlogs/DIAcubemapgen_dlg.cpp"
    "../Sources/DIAlogs/DIAcubemapgen_dlg.h"
    "../Sources/DIAlogs/DIAdropgro_dlg.cpp"
    "../Sources/DIAlogs/DIAdropgro_dlg.h"
    "../Sources/DIAlogs/DIAfavorites_dlg.cpp"
    "../Sources/DIAlogs/DIAfavorites_dlg.h"
    "../Sources/DIAlogs/DIAfile_dlg.cpp"
    "../Sources/DIAlogs/DIAfile_dlg.h"
    "../Sources/DIAlogs/DIAfindai_dlg.cpp"
    "../Sources/DIAlogs/DIAfindai_dlg.h"
    "../Sources/DIAlogs/DIAfindfile_dlg.cpp"
    "../Sources/DIAlogs/DIAfindfile_dlg.h"
    "../Sources/DIAlogs/DIAfontdescriptor_dlg.cpp"
    "../Sources/DIAlogs/DIAfontdescriptor_dlg.h"
    "../Sources/DIAlogs/DIAgamematerial_dlg.cpp"
    "../Sources/DIAlogs/DIAgamematerial_dlg.h"
    "../Sources/DIAlogs/DIAgamematerial_inside.cpp"
    "../Sources/DIAlogs/DIAgamematerial_inside.h"
    "../Sources/DIAlogs/DIAgaoinfo_dlg.cpp"
    "../Sources/DIAlogs/DIAgaoinfo_dlg.h"
    "../Sources/DIAlogs/DIAgraphchoose_dlg.cpp"
    "../Sources/DIAlogs/DIAgraphchoose_dlg.h"
    "../Sources/DIAlogs/DIAgrid_dlg.cpp"
    "../Sources/DIAlogs/DIAgrid_dlg.h"
    "../Sources/DIAlogs/DIAgromodifier_dlg.cpp"
    "../Sources/DIAlogs/DIAgromodifier_dlg.h"
    "../Sources/DIAlogs/DIAgromodifiersnap_dlg.cpp"
    "../Sources/DIAlogs/DIAgromodifiersnap_dlg.h"
    "../Sources/DIAlogs/DIAgrovertexpos_dlg.cpp"
    "../Sources/DIAlogs/DIAgrovertexpos_dlg.h"
    "../Sources/DIAlogs/DIAjoystick_dlg.cpp"
    "../Sources/DIAlogs/DIAjoystick_dlg.h"
    "../Sources/DIAlogs/DIAkeyboard_dlg.cpp"
    "../Sources/DIAlogs/DIAkeyboard_dlg.h"
    "../Sources/DIAlogs/DIAlang_dlg.cpp"
    "../Sources/DIAlogs/DIAlang_dlg.h"
#    "../Sources/DIAlogs/DIALightmaps.cpp"
    "../Sources/DIAlogs/DIALightmaps.h"
#    "../Sources/DIAlogs/DIAlightrej_dlg.cpp"
    "../Sources/DIAlogs/DIAlightrej_dlg.h"
    "../Sources/DIAlogs/DIAlink_dlg.cpp"
    "../Sources/DIAlogs/DIAlink_dlg.h"
    "../Sources/DIAlogs/DIAlist2_dlg.cpp"
    "../Sources/DIAlogs/DIAlist2_dlg.h"
    "../Sources/DIAlogs/DIAlist_dlg.cpp"
    "../Sources/DIAlogs/DIAlist_dlg.h"
    "../Sources/DIAlogs/DIAmatrix_dlg.cpp"
    "../Sources/DIAlogs/DIAmatrix_dlg.h"
    "../Sources/DIAlogs/DIAmessage_dlg.cpp"
    "../Sources/DIAlogs/DIAmessage_dlg.h"
    "../Sources/DIAlogs/DIAmixer_dlg.cpp"
    "../Sources/DIAlogs/DIAmixer_dlg.h"
    "../Sources/DIAlogs/DIAmorphedit_dlg.cpp"
    "../Sources/DIAlogs/DIAmorphedit_dlg.h"
    "../Sources/DIAlogs/DIAmorphing_dlg.cpp"
    "../Sources/DIAlogs/DIAmorphing_dlg.h"
    "../Sources/DIAlogs/DIAmorphingsliders_dlg.cpp"
    "../Sources/DIAlogs/DIAmorphingsliders_dlg.h"
    "../Sources/DIAlogs/DIAmsglink_dlg.cpp"
    "../Sources/DIAlogs/DIAmsglink_dlg.h"
    "../Sources/DIAlogs/DIAmtx_dlg.cpp"
    "../Sources/DIAlogs/DIAmtx_dlg.h"
    "../Sources/DIAlogs/DIAname_dlg.cpp"
    "../Sources/DIAlogs/DIAname_dlg.h"
    "../Sources/DIAlogs/DIAorder_dlg.cpp"
    "../Sources/DIAlogs/DIAorder_dlg.h"
    "../Sources/DIAlogs/DIApalette_dlg.cpp"
    "../Sources/DIAlogs/DIApalette_dlg.h"
    "../Sources/DIAlogs/DIApostit_dlg.cpp"
    "../Sources/DIAlogs/DIApostit_dlg.h"
    "../Sources/DIAlogs/DIAradiosity_dlg.cpp"
    "../Sources/DIAlogs/DIAradiosity_dlg.h"
#    "../Sources/DIAlogs/DIAreplace_dlg.cpp"
    "../Sources/DIAlogs/DIAreplace_dlg.h"
    "../Sources/DIAlogs/DIArli_dlg.cpp"
    "../Sources/DIAlogs/DIArli_dlg.h"
    "../Sources/DIAlogs/DIArliadjust_dlg.cpp"
    "../Sources/DIAlogs/DIArliadjust_dlg.h"
    "../Sources/DIAlogs/DIArlicarte_dlg.cpp"
    "../Sources/DIAlogs/DIArlicarte_dlg.h"
    "../Sources/DIAlogs/DIAsaveaddmatrix_dlg.cpp"
    "../Sources/DIAlogs/DIAsaveaddmatrix_dlg.h"
#    "../Sources/DIAlogs/DIAScreenshotSeqBuilder_dlg.cpp"
    "../Sources/DIAlogs/DIAScreenshotSeqBuilder_dlg.h"
    "../Sources/DIAlogs/DIAsecto_dlg.cpp"
    "../Sources/DIAlogs/DIAsecto_dlg.h"
    "../Sources/DIAlogs/DIAselection_dlg.cpp"
    "../Sources/DIAlogs/DIAselection_dlg.h"
    "../Sources/DIAlogs/DIAsetTextureFilter_dlg.cpp"
    "../Sources/DIAlogs/DIAsetTextureFilter_dlg.h"
    "../Sources/DIAlogs/DIAshape_dlg.cpp"
    "../Sources/DIAlogs/DIAshape_dlg.h"
    "../Sources/DIAlogs/DIAsnddisp_dlg.cpp"
    "../Sources/DIAlogs/DIAsnddisp_dlg.h"
    "../Sources/DIAlogs/DIAsndgroupwatcher_dlg.cpp"
    "../Sources/DIAlogs/DIAsndgroupwatcher_dlg.h"
    "../Sources/DIAlogs/DIAsndinsert_dlg.cpp"
    "../Sources/DIAlogs/DIAsndinsert_dlg.h"
    "../Sources/DIAlogs/DIAsndinstance_dlg.cpp"
    "../Sources/DIAlogs/DIAsndinstance_dlg.h"
    "../Sources/DIAlogs/DIAsndtrans_dlg.cpp"
    "../Sources/DIAlogs/DIAsndtrans_dlg.h"
    "../Sources/DIAlogs/DIAsndvumeter.cpp"
    "../Sources/DIAlogs/DIAsndvumeter.h"
    "../Sources/DIAlogs/DIAsndvumeterset.cpp"
    "../Sources/DIAlogs/DIAsndvumeterset.h"
    "../Sources/DIAlogs/DIAspeedact_dlg.cpp"
    "../Sources/DIAlogs/DIAspeedact_dlg.h"
    "../Sources/DIAlogs/DIAstockofcolor_dlg.cpp"
    "../Sources/DIAlogs/DIAstockofcolor_dlg.h"
    "../Sources/DIAlogs/DIAstrings.h"
    "../Sources/DIAlogs/DIAtextfile_dlg.cpp"
    "../Sources/DIAlogs/DIAtextfile_dlg.h"
    "../Sources/DIAlogs/DIAtexture_dlg.cpp"
    "../Sources/DIAlogs/DIAtexture_dlg.h"
    "../Sources/DIAlogs/DIAtimeinterpolation_dlg.cpp"
    "../Sources/DIAlogs/DIAtimeinterpolation_dlg.h"
    "../Sources/DIAlogs/DIAtoolbox_curveview.cpp"
    "../Sources/DIAlogs/DIAtoolbox_curveview.h"
    "../Sources/DIAlogs/DIAtoolbox_dlg.cpp"
    "../Sources/DIAlogs/DIAtoolbox_dlg.h"
    "../Sources/DIAlogs/DIAtoolbox_groview.cpp"
    "../Sources/DIAlogs/DIAtoolbox_groview.h"
    "../Sources/DIAlogs/DIAundo_dlg.cpp"
    "../Sources/DIAlogs/DIAundo_dlg.h"
    "../Sources/DIAlogs/DIAuvmapper_dlg.cpp"
    "../Sources/DIAlogs/DIAuvmapper_dlg.h"
    "../Sources/DIAlogs/DIAvector_dlg.cpp"
    "../Sources/DIAlogs/DIAvector_dlg.h"
#    "../Sources/DIAlogs/diawindcurve_dlg.cpp"
    "../Sources/DIAlogs/diawindcurve_dlg.h"
    "../Sources/DIAlogs/DIAwpshowstatus_dlg.cpp"
    "../Sources/DIAlogs/DIAwpshowstatus_dlg.h"
#    "../Sources/DIAlogs/DIAXeMergeFlags.cpp"
    "../Sources/DIAlogs/DIAXeMergeFlags.h"
    "../Sources/DIAlogs/EnTabCtrl.cpp"
    "../Sources/DIAlogs/EnTabCtrl.h"
    "../Sources/DIAlogs/FlatHeaderCtrl.cpp"
    "../Sources/DIAlogs/FlatHeaderCtrl.h"
    "../Sources/DIAlogs/HScrollListBox.cpp"
    "../Sources/DIAlogs/HScrollListBox.h"
    "../Sources/DIAlogs/ListCtrlStyled.cpp"
    "../Sources/DIAlogs/ListCtrlStyled.h"
    "../Sources/DIAlogs/MemDC.h"
#    "../Sources/DIAlogs/NumEdit.cpp"
    "../Sources/DIAlogs/NumEdit.h"
    "../Sources/DIAlogs/ProSliderCtrl.cpp"
    "../Sources/DIAlogs/ProSliderCtrl.h"
)
source_group("DIAlogs" FILES ${DIAlogs})

set(DIAlogs__Perforce
#    "../Sources/DIAlogs/DATCP4ChangelistComment.cpp"
    "../Sources/DIAlogs/DATCP4ChangelistComment.h"
#    "../Sources/DIAlogs/DATCP4SettingsDlg.cpp"
    "../Sources/DIAlogs/DATCP4SettingsDlg.h"
#    "../Sources/DIAlogs/DATCP4View.cpp"
    "../Sources/DIAlogs/DATCP4View.h"
    "../Sources/DIAlogs/DIAhistory_dlg.cpp"
    "../Sources/DIAlogs/DIAhistory_dlg.h"
)
source_group("DIAlogs\\Perforce" FILES ${DIAlogs__Perforce})

set(EDI
    "../Sources/EDIaction.cpp"
    "../Sources/EDIaction.h"
    "../Sources/EDIapp.cpp"
    "../Sources/EDIapp.h"
    "../Sources/EDIapp_help.cpp"
    "../Sources/EDIapp_options.h"
    "../Sources/EDIbaseframe.cpp"
    "../Sources/EDIbaseframe.h"
    "../Sources/EDIbaseframe_ini.cpp"
    "../Sources/EDIbaseview.cpp"
    "../Sources/EDIbaseview.h"
    "../Sources/EDIbaseview_game.cpp"
    "../Sources/EDIbaseview_ini.cpp"
    "../Sources/EDIbaseview_msg.cpp"
    "../Sources/EDIeditors_infos.h"
    "../Sources/EDIeditors_reg.cpp"
    "../Sources/EDIerrid.h"
#    "../Sources/EDIFileFilters.cpp"
    "../Sources/EDIFileFilters.h"
    "../Sources/EDIicons.h"
    "../Sources/EDImainframe.cpp"
    "../Sources/EDImainframe.h"
    "../Sources/EDImainframe_act.cpp"
    "../Sources/EDImainframe_act.h"
    "../Sources/EDImainframe_dd.cpp"
    "../Sources/EDImainframe_desk.cpp"
    "../Sources/EDImainframe_ini.cpp"
    "../Sources/EDImainframe_msg.cpp"
    "../Sources/EDImainframe_universe.cpp"
    "../Sources/EDImainframe_vss.cpp"
    "../Sources/EDImsg.h"
    "../Sources/EDIpaths.h"
    "../Sources/EDIsplitter.cpp"
    "../Sources/EDIsplitter.h"
    "../Sources/EDIstrings.h"
    "../Sources/EDIundo.cpp"
    "../Sources/EDIundo.h"
)
source_group("EDI" FILES ${EDI})

set(F3Dframe
    "../Sources/F3Dframe/F3Dframe.cpp"
    "../Sources/F3Dframe/F3Dframe.h"
    "../Sources/F3Dframe/F3Dstrings.h"
    "../Sources/F3Dframe/F3Dview.cpp"
    "../Sources/F3Dframe/F3Dview.h"
    "../Sources/F3Dframe/F3Dview_action.cpp"
    "../Sources/F3Dframe/F3Dview_ai.cpp"
    "../Sources/F3Dframe/F3Dview_anim.cpp"
    "../Sources/F3Dframe/F3Dview_brickmapper.cpp"
    "../Sources/F3Dframe/F3Dview_cam.cpp"
    "../Sources/F3Dframe/F3Dview_col.cpp"
    "../Sources/F3Dframe/F3Dview_curve.cpp"
    "../Sources/F3Dframe/F3DView_grid.cpp"
    "../Sources/F3Dframe/F3Dview_gro.cpp"
    "../Sources/F3Dframe/F3Dview_grp.cpp"
    "../Sources/F3Dframe/F3Dview_helpers.cpp"
    "../Sources/F3Dframe/F3Dview_mouse.cpp"
    "../Sources/F3Dframe/F3Dview_obj.cpp"
    "../Sources/F3Dframe/F3Dview_ode.cpp"
    "../Sources/F3Dframe/F3Dview_op.cpp"
    "../Sources/F3Dframe/F3DView_pick.cpp"
    "../Sources/F3Dframe/F3Dview_portal.cpp"
    "../Sources/F3Dframe/F3Dview_postit.cpp"
    "../Sources/F3Dframe/F3Dview_postit.h"
    "../Sources/F3Dframe/F3Dview_rli.cpp"
    "../Sources/F3Dframe/F3DView_Selection.cpp"
    "../Sources/F3Dframe/F3DView_subobject.cpp"
    "../Sources/F3Dframe/F3Dview_undo.cpp"
    "../Sources/F3Dframe/F3Dview_undo.h"
    "../Sources/F3Dframe/F3Dview_way.cpp"
    "../Sources/F3Dframe/F3Dview_wor.cpp"
    "../Sources/F3Dframe/F3Dview_wow.cpp"
    "../Sources/F3Dframe/F3Dview_zonebv.cpp"
)
source_group("F3Dframe" FILES ${F3Dframe})

set(Libraries__DX8
#    "../../../Extern/DX8/lib/dinput8.lib"
#    "../../../Extern/DX8/lib/dxguid.lib"
)
source_group("Libraries\\DX8" FILES ${Libraries__DX8})

set(Res
    "../Sources/Res/cam.cur"
    "../Sources/Res/campane.cur"
    "../Sources/Res/camtgt.cur"
    "../Sources/Res/camzoom.cur"
    "../Sources/Res/camzoom1.cur"
    "../Sources/Res/cur00001.cur"
    "../Sources/Res/cur00002.cur"
    "../Sources/Res/cur00003.cur"
    "../Sources/Res/cur00004.cur"
    "../Sources/Res/cursor2.cur"
    "../Sources/Res/cursor3.cur"
    "../Sources/Res/cursor4.cur"
    "../Sources/Res/cursor5.cur"
    "../Sources/Res/cursor6.cur"
    "../Sources/Res/cursor7.cur"
    "../Sources/Res/dragcopy.cur"
    "../Sources/Res/dragdel.cur"
    "../Sources/Res/dragmove.cur"
    "../Sources/Res/dragnone.cur"
    "../Sources/Res/dragquest.cur"
    "../Sources/Res/hand.cur"
    "../Sources/Res/main_idc.cur"
    "../Sources/Res/pipe.cur"
    "../Sources/Res/rarrow.cur"
    "../Sources/Res/Res.h"
)
source_group("Res" FILES ${Res})

set(VAVview
    "../Sources/VAVview/VAVlist.cpp"
    "../Sources/VAVview/VAVlist.h"
    "../Sources/VAVview/VAVlist_action.cpp"
    "../Sources/VAVview/VAVlist_ai.cpp"
    "../Sources/VAVview/VAVlist_anim.cpp"
    "../Sources/VAVview/VAVlist_col.cpp"
    "../Sources/VAVview/VAVlist_msg.cpp"
    "../Sources/VAVview/VAVlist_obj.cpp"
    "../Sources/VAVview/VAVlist_snd.cpp"
    "../Sources/VAVview/VAVtypes.h"
    "../Sources/VAVview/VAVview.cpp"
    "../Sources/VAVview/VAVview.h"
    "../Sources/VAVview/VAVview_aifunc.cpp"
    "../Sources/VAVview/VAVview_anim.cpp"
    "../Sources/VAVview/VAVview_bool.cpp"
    "../Sources/VAVview/VAVview_bounding.cpp"
    "../Sources/VAVview/VAVview_color.cpp"
    "../Sources/VAVview/VAVview_constfloat.cpp"
    "../Sources/VAVview/VAVview_constint.cpp"
    "../Sources/VAVview/VAVview_custsep.cpp"
    "../Sources/VAVview/VAVview_file.cpp"
    "../Sources/VAVview/VAVview_flags.cpp"
    "../Sources/VAVview/VAVview_float.cpp"
    "../Sources/VAVview/VAVview_go.cpp"
    "../Sources/VAVview/VAVview_grotype.cpp"
    "../Sources/VAVview/VAVview_grp.cpp"
    "../Sources/VAVview/VAVview_hexa.cpp"
    "../Sources/VAVview/VAVview_int.cpp"
    "../Sources/VAVview/VAVview_key.cpp"
    "../Sources/VAVview/VAVview_matrix.cpp"
    "../Sources/VAVview/VAVview_modifier.cpp"
    "../Sources/VAVview/VAVview_network.cpp"
    "../Sources/VAVview/VAVview_smodifier.cpp"
    "../Sources/VAVview/VAVview_sndkey.cpp"
    "../Sources/VAVview/VAVview_string.cpp"
    "../Sources/VAVview/VAVview_text.cpp"
    "../Sources/VAVview/VAVview_vector.cpp"
    "../Sources/VAVview/VAVview_zone.cpp"
)
source_group("VAVview" FILES ${VAVview})

set(ALL_FILES
    ${no_group_source_files}
    ${DATaControl}
    ${DATaControl__P4ClientUsers}
    ${DATaControl__P4Commands}
    ${DATaControl__P4FileSys}
    ${DATaControl__P4Utils}
    ${DATaControl__Zlib}
    ${DIAlogs}
    ${DIAlogs__Perforce}
    ${EDI}
    ${F3Dframe}
    ${Libraries__DX8}
    ${Res}
    ${VAVview}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/EDItors/Sources/Precomp.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE MainEdi)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "MFCProj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Jade_edr"
        TARGET_NAME_DEBUGEDITORS         "Jade_edd_xe"
        TARGET_NAME_DEBUGMAX8            "Jade_edd_xe"
        TARGET_NAME_DEBUGPC              "Jade_edd_xe"
        TARGET_NAME_DEBUG                "Jade_edd_xe"
        TARGET_NAME_FINAL                "Jade_edr"
        TARGET_NAME_MAX25                "Jade_edr"
        TARGET_NAME_MAX3                 "Jade_edr"
        TARGET_NAME_MAX5                 "Jade_edr"
        TARGET_NAME_MAX7                 "Jade_edr"
        TARGET_NAME_MAX8                 "Jade_edr"
        TARGET_NAME_PROFILE_FASTCAP      "Jade_edr"
        TARGET_NAME_PROFILE              "Jade_edr"
        TARGET_NAME_RELEASEPC            "Jade_edr"
        TARGET_NAME_RELEASE_LTCG         "Jade_edr"
        TARGET_NAME_RELEASE              "Jade_edr"
        TARGET_NAME_RELEASEXPROFILE_FAST "Jade_edr"
        TARGET_NAME_RELEASEXPROFILE      "Jade_edr"
        TARGET_NAME_SPEEDEDITORS         "Jade_edr"
        TARGET_NAME_SPEEDEDITORSXENON    "Jade_edr_xe"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Jade_edr"
        TARGET_NAME_DEBUGEDITORS         "Jaded"
        TARGET_NAME_DEBUGMAX8            "Jade_edd_xe"
        TARGET_NAME_DEBUGPC              "Jaded"
        TARGET_NAME_DEBUG                "Jaded"
        TARGET_NAME_FINAL                "Jade_edr"
        TARGET_NAME_MAX25                "Jade_edr"
        TARGET_NAME_MAX3                 "Jade_edr"
        TARGET_NAME_MAX5                 "Jade_edr"
        TARGET_NAME_MAX7                 "Jade_edr"
        TARGET_NAME_MAX8                 "Jade_edr"
        TARGET_NAME_PROFILE_FASTCAP      "Jade_edr_xe"
        TARGET_NAME_PROFILE              "Jade_edr_xe"
        TARGET_NAME_RELEASEPC            "Jade_edr_xe"
        TARGET_NAME_RELEASE_LTCG         "Jade_edr_xe"
        TARGET_NAME_RELEASE              "Jade_edr"
        TARGET_NAME_RELEASEXPROFILE_FAST "Jade_edr"
        TARGET_NAME_RELEASEXPROFILE      "Jade_edr"
        TARGET_NAME_SPEEDEDITORS         "Jade_edr"
        TARGET_NAME_SPEEDEDITORSXENON    "Jade_edr_xe"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Jade_edr_xe"
        TARGET_NAME_DEBUGEDITORS         "Jade_edd_xe"
        TARGET_NAME_DEBUGMAX8            "Jade_edd_xe"
        TARGET_NAME_DEBUGPC              "Jade_edr"
        TARGET_NAME_DEBUG                "Jade_edr"
        TARGET_NAME_FINAL                "Jade_edr"
        TARGET_NAME_MAX25                "Jade_edr"
        TARGET_NAME_MAX3                 "Jade_edr"
        TARGET_NAME_MAX5                 "Jade_edr"
        TARGET_NAME_MAX7                 "Jade_edr"
        TARGET_NAME_MAX8                 "Jade_edr"
        TARGET_NAME_PROFILE_FASTCAP      "Jade_edr_xe"
        TARGET_NAME_PROFILE              "Jade_edr_xe"
        TARGET_NAME_RELEASEPC            "Jade_edr_xe"
        TARGET_NAME_RELEASE_LTCG         "Jade_edr_xe"
        TARGET_NAME_RELEASE              "Jade_edr_xe"
        TARGET_NAME_RELEASEXPROFILE_FAST "Jade_edr_xe"
        TARGET_NAME_RELEASEXPROFILE      "Jade_edr_xe"
        TARGET_NAME_SPEEDEDITORS         "Jade_edr_xe"
        TARGET_NAME_SPEEDEDITORSXENON    "Jade_edr_xe"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Jade_edr_xe"
        TARGET_NAME_DEBUGEDITORS         "Jade_edd_xe"
        TARGET_NAME_DEBUGMAX8            "Jade_edd_xe"
        TARGET_NAME_DEBUGPC              "Jade_edr"
        TARGET_NAME_DEBUG                "Jade_edr"
        TARGET_NAME_FINAL                "Jade_edr"
        TARGET_NAME_MAX25                "Jade_edr"
        TARGET_NAME_MAX3                 "Jade_edr"
        TARGET_NAME_MAX5                 "Jade_edr"
        TARGET_NAME_MAX7                 "Jade_edr"
        TARGET_NAME_MAX8                 "Jade_edr"
        TARGET_NAME_PROFILE_FASTCAP      "Jade_edr_xe"
        TARGET_NAME_PROFILE              "Jade_edr_xe"
        TARGET_NAME_RELEASEPC            "Jade_edr_xe"
        TARGET_NAME_RELEASE_LTCG         "Jade_edr_xe"
        TARGET_NAME_RELEASE              "Jade_edr_xe"
        TARGET_NAME_RELEASEXPROFILE_FAST "Jade_edr_xe"
        TARGET_NAME_RELEASEXPROFILE      "Jade_edr_xe"
        TARGET_NAME_SPEEDEDITORS         "Jade_edr_xe"
        TARGET_NAME_SPEEDEDITORSXENON    "Jade_edr_xe"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/.."
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebug
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreaded
        >
        $<$<CONFIG:Profile>:
            MultiThreaded
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreaded
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebug
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebug
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreaded
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreaded
        >
        $<$<CONFIG:Profile>:
            MultiThreaded
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreaded
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreaded
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreaded
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreaded
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebug
        >
        $<$<CONFIG:MAX8>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreaded
        >
        $<$<CONFIG:Profile>:
            MultiThreaded
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreaded
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreaded
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreaded
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreaded
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreaded
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Debug>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Final>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Release>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_ALLOW_RTCc_IN_STL;"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_ALLOW_RTCc_IN_STL;"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_ALLOW_RTCc_IN_STL;"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Debug>:"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_ALLOW_RTCc_IN_STL;"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "ACTIVE_EDITORS;"
        "_DEBUG;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Debug>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Final>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_ALLOW_RTCc_IN_STL;"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
            "OS_NT;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Debug>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Final>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "OS_NT;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_ALLOW_RTCc_IN_STL;"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob2
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:Debug>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:Final>:
                /Ob2
            >
            $<$<CONFIG:MAX25>:
                /Ob2
            >
            $<$<CONFIG:MAX3>:
                /Ob2
            >
            $<$<CONFIG:MAX5>:
                /Ob2
            >
            $<$<CONFIG:MAX7>:
                /Ob2
            >
            $<$<CONFIG:MAX8>:
                /Ob2
            >
            $<$<CONFIG:Profile_FastCap>:
                /Ob2
            >
            $<$<CONFIG:Profile>:
                /Ob2
            >
            $<$<CONFIG:ReleasePC>:
                /Ob2
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob2
            >
            $<$<CONFIG:Release>:
                /Ob2
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob2
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob2
            >
            $<$<CONFIG:SpeedEditors>:
                /Ob2
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            /arch:SSE;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Ob2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /WX-;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /arch:SSE;
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Ob2
            >
            $<$<CONFIG:Debug>:
                /Ob2
            >
            $<$<CONFIG:Final>:
                /Ob2
            >
            $<$<CONFIG:MAX25>:
                /Ob2
            >
            $<$<CONFIG:MAX3>:
                /Ob2
            >
            $<$<CONFIG:MAX5>:
                /Ob2
            >
            $<$<CONFIG:MAX7>:
                /Ob2
            >
            $<$<CONFIG:MAX8>:
                /Ob2
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            /arch:SSE;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /WX-;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Ob2
            >
            $<$<CONFIG:Debug>:
                /Ob2
            >
            $<$<CONFIG:Final>:
                /Ob2
            >
            $<$<CONFIG:MAX25>:
                /Ob2
            >
            $<$<CONFIG:MAX3>:
                /Ob2
            >
            $<$<CONFIG:MAX5>:
                /Ob2
            >
            $<$<CONFIG:MAX7>:
                /Ob2
            >
            $<$<CONFIG:MAX8>:
                /Ob2
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /WX-;
                /Ot;
                /TP
            >
            /arch:SSE;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/adler32.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/compress.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/crc32.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/deflate.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/example.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/gzio.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/infback.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/inffast.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/inflate.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/inftrees.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/trees.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/uncompr.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Debug>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/TC"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/TC"
            ">"
            "$<$<CONFIG:Profile>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/TC"
            ">"
            "$<$<CONFIG:Release>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/TC"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/TC"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/TC"
            ">"
            "/Y-"
        )
    endif()
    source_file_compile_options(../../../Extern/zlib/zutil.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG:FULL
            >
            $<$<CONFIG:DebugEditors>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:Debug>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:Final>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX25>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX3>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX5>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX7>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX8>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Profile>:
                /DEBUG:FULL
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Release>:
                /DEBUG:FULL
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG:FULL
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG:FULL
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG:FULL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            /NODEFAULTLIB:libcd.lib;
            /NODEFAULTLIB:libc.lib;
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:DebugEditors>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:DEBUGMAX8>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG;
                /INCREMENTAL
            >
            $<$<CONFIG:DebugPC>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Debug>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Final>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:MAX25>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:MAX3>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:MAX5>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:MAX7>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:MAX8>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG;
                /INCREMENTAL;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG;
                /INCREMENTAL;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG;
                /INCREMENTAL;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG;
                /INCREMENTAL;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG:FULL;
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG;
                /INCREMENTAL;
                /ASSEMBLYDEBUG
            >
            /NODEFAULTLIB:libcd.lib;
            /NODEFAULTLIB:libc.lib;
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Debug>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Final>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX25>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX3>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX5>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX7>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX8>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            /NODEFAULTLIB:libcd.lib;
            /NODEFAULTLIB:libc.lib;
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /NODEFAULTLIB:libcmt.lib;
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Debug>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Final>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX25>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX3>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX5>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX7>:
                /DEBUG:FULL
            >
            $<$<CONFIG:MAX8>:
                /DEBUG:FULL
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG;
                /ASSEMBLYDEBUG
            >
            /NODEFAULTLIB:libcd.lib;
            /NODEFAULTLIB:libc.lib;
            /NOLOGO;
            /SAFESEH:NO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL;
            /MACHINE:I386
        )
    endif()
endif()

################################################################################
# Pre link events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMANDS
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:DebugPC> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:DebugPC> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND                $<CONFIG:Debug> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND                $<CONFIG:Debug> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMANDS
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMANDS
        COMMAND        $<CONFIG:BFObjectTools> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND        $<CONFIG:BFObjectTools> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:Release> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:Release> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND $<CONFIG:ReleaseXProfile_Fast> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND $<CONFIG:ReleaseXProfile_Fast> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND      $<CONFIG:ReleaseXProfile> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND      $<CONFIG:ReleaseXProfile> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:SpeedEditors> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:SpeedEditors> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        PRE_LINK
        COMMANDS
        COMMAND        $<CONFIG:BFObjectTools> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND        $<CONFIG:BFObjectTools> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:DebugEditors> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:DEBUGMAX8> devenv /build DebugEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND      $<CONFIG:Profile_FastCap> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:Profile> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND            $<CONFIG:ReleasePC> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:Release_LTCG> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND              $<CONFIG:Release> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND              $<CONFIG:Release> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND $<CONFIG:ReleaseXProfile_Fast> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND $<CONFIG:ReleaseXProfile_Fast> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND      $<CONFIG:ReleaseXProfile> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND      $<CONFIG:ReleaseXProfile> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND         $<CONFIG:SpeedEditors> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND         $<CONFIG:SpeedEditors> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonGraphics $ENV{SolutionPath}
        COMMAND    $<CONFIG:SpeedEditorsXenon> devenv /build SpeedEditorsXenon /project XenonMeshProcessing $ENV{SolutionPath}
    )
endif()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
    JPEGLIB
    MAD_LoadSave
    MAD_Rasterize
    MPEGLIB
    ode
    AIinterp
    DX8
    EDItors
    ENGine
    GraphicDK
    NETwork
    OpenGL
    SDK
    SouND
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:DebugEditors>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Debug>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "oldnames;"
        "advapi32;"
        "shell32;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32;"
        "winmm;"
        "msacm32;"
        "comctl32;"
        "ws2_32;"
        "NAFXCWD;"
        "libcmtd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:DebugEditors>:"
            "VFW32;"
            "strmiids"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "VFW32;"
            "strmiids"
        ">"
        "$<$<CONFIG:Debug>:"
            "VFW32;"
            "strmiids"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Profile>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "oldnames;"
        "advapi32;"
        "shell32;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32;"
        "winmm;"
        "msacm32;"
        "comctl32;"
        "ws2_32;"
        "NAFXCWD;"
        "libcmtd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Profile>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Release>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "oldnames;"
        "advapi32;"
        "shell32;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32;"
        "winmm;"
        "msacm32;"
        "comctl32;"
        "ws2_32;"
        "NAFXCWD;"
        "libcmtd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\DebugEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Profile>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:Release>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "dinput8;"
            "dxguid;"
            "d3d9;"
            "d3dx9;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonGraphics;"
            "..\..\..\Output\SpeedEditorsXenon\libs\XenonMeshProcessing"
        ">"
        "oldnames;"
        "advapi32;"
        "shell32;"
        "kernel32;"
        "user32;"
        "gdi32;"
        "winspool;"
        "comdlg32;"
        "ole32;"
        "oleaut32;"
        "uuid;"
        "odbc32;"
        "odbccp32;"
        "winmm;"
        "msacm32;"
        "comctl32;"
        "ws2_32;"
        "NAFXCWD;"
        "libcmtd"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../Output/Libs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/sdl2/lib/x86;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/Greta;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/lib"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/Greta;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/lib"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/Greta;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/lib"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Output/Libs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/sdl2/lib/x86;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../Output/Libs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/sdl2/lib/x86;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../Output/Libs;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/sdl2/lib/x86;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/glew/lib/release/win32"
    )
endif()

