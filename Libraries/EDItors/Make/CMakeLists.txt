set(PROJECT_NAME EDItors)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../Sources/Precomp.cpp"
    "../Sources/Precomp.h"
)
source_group("" FILES ${no_group_source_files})

set(ACTions
    "../Sources/ACTions/EACTframe.cpp"
    "../Sources/ACTions/EACTframe.h"
    "../Sources/ACTions/EACTframe_act.cpp"
    "../Sources/ACTions/EACTframe_act.h"
    "../Sources/ACTions/EACTframe_ini.cpp"
    "../Sources/ACTions/EACTframe_msg.cpp"
    "../Sources/ACTions/EACTview.cpp"
    "../Sources/ACTions/EACTview.h"
)
source_group("ACTions" FILES ${ACTions})

set(AIscript
    "../Sources/AIscript/AIerrid.h"
    "../Sources/AIscript/AIframe.cpp"
    "../Sources/AIscript/AIframe.h"
    "../Sources/AIscript/AIframe_act.cpp"
    "../Sources/AIscript/AIframe_act.h"
    "../Sources/AIscript/AIframe_comp.cpp"
    "../Sources/AIscript/AIframe_debug.cpp"
    "../Sources/AIscript/AIframe_ini.cpp"
    "../Sources/AIscript/AIframe_msg.cpp"
    "../Sources/AIscript/AIframe_perforce.cpp"
    "../Sources/AIscript/AIleftframe.cpp"
    "../Sources/AIscript/AIleftframe.h"
    "../Sources/AIscript/AIstrings.h"
)
source_group("AIscript" FILES ${AIscript})

set(AIscript__Compiler
    "../Sources/AIscript/Compiler/AIcompile.cpp"
    "../Sources/AIscript/Compiler/AIcompile.h"
    "../Sources/AIscript/Compiler/AIcompile_2C.cpp"
    "../Sources/AIscript/Compiler/AIcompile_optim.cpp"
    "../Sources/AIscript/Compiler/AIcompile_out.cpp"
    "../Sources/AIscript/Compiler/AIcompile_pp.cpp"
    "../Sources/AIscript/Compiler/AIcompile_ref.cpp"
    "../Sources/AIscript/Compiler/AIcompile_scan.cpp"
    "../Sources/AIscript/Compiler/AIcompile_vars.cpp"
)
source_group("AIscript\\Compiler" FILES ${AIscript__Compiler})

set(AIscript__EditView
    "../Sources/AIscript/EditView/AIleftview.cpp"
    "../Sources/AIscript/EditView/AIleftview.h"
    "../Sources/AIscript/EditView/AIpane.cpp"
    "../Sources/AIscript/EditView/AIpane.h"
    "../Sources/AIscript/EditView/AIundo.cpp"
    "../Sources/AIscript/EditView/AIundo.h"
    "../Sources/AIscript/EditView/AIview.cpp"
    "../Sources/AIscript/EditView/AIview.h"
    "../Sources/AIscript/EditView/AIview_colors.cpp"
    "../Sources/AIscript/EditView/AIview_km.cpp"
    "../Sources/AIscript/EditView/AIview_mdfy.cpp"
    "../Sources/AIscript/EditView/AIview_scan.cpp"
)
source_group("AIscript\\EditView" FILES ${AIscript__EditView})

set(BROwser
    "../Sources/BROwser/BROerrid.h"
    "../Sources/BROwser/BROframe.cpp"
    "../Sources/BROwser/BROframe.h"
    "../Sources/BROwser/BROframe_act.cpp"
    "../Sources/BROwser/BROframe_act.h"
    "../Sources/BROwser/BROframe_extras.cpp"
    "../Sources/BROwser/BROframe_fav.cpp"
    "../Sources/BROwser/BROframe_impex.cpp"
    "../Sources/BROwser/BROframe_ini.cpp"
    "../Sources/BROwser/BROframe_mdfy.cpp"
    "../Sources/BROwser/BROframe_msg.cpp"
    "../Sources/BROwser/BROframe_perforce.cpp"
    "../Sources/BROwser/BROframe_vss.cpp"
    "../Sources/BROwser/BROgrpctrl.cpp"
    "../Sources/BROwser/BROgrpctrl.h"
    "../Sources/BROwser/BROlistctrl.cpp"
    "../Sources/BROwser/BROlistctrl.h"
    "../Sources/BROwser/BROmsg.h"
    "../Sources/BROwser/BROstrings.h"
    "../Sources/BROwser/BROtreectrl.cpp"
    "../Sources/BROwser/BROtreectrl.h"
)
source_group("BROwser" FILES ${BROwser})

set(EVEnts
    "../Sources/EVEnts/EVEevent.cpp"
    "../Sources/EVEnts/EVEevent.h"
    "../Sources/EVEnts/EVEevent_aif.cpp"
    "../Sources/EVEnts/EVEevent_interpolationkey.cpp"
    "../Sources/EVEnts/EVEevent_morph.cpp"
    "../Sources/EVEnts/EVEevent_timekey.cpp"
    "../Sources/EVEnts/EVEframe.cpp"
    "../Sources/EVEnts/EVEframe.h"
    "../Sources/EVEnts/EVEframe_act.cpp"
    "../Sources/EVEnts/EVEframe_act.h"
    "../Sources/EVEnts/EVEframe_anim.cpp"
    "../Sources/EVEnts/EVEframe_ini.cpp"
    "../Sources/EVEnts/EVEframe_msg.cpp"
    "../Sources/EVEnts/EVEinside.cpp"
    "../Sources/EVEnts/EVEinside.h"
    "../Sources/EVEnts/EVEmsg.h"
    "../Sources/EVEnts/EVEscroll.cpp"
    "../Sources/EVEnts/EVEscroll.h"
    "../Sources/EVEnts/EVEtrack.cpp"
    "../Sources/EVEnts/EVEtrack.h"
)
source_group("EVEnts" FILES ${EVEnts})

set(LOGfile
    "../Sources/LOGfile/LOGframe.cpp"
    "../Sources/LOGfile/LOGframe.h"
    "../Sources/LOGfile/LOGframe_act.cpp"
    "../Sources/LOGfile/LOGframe_act.h"
    "../Sources/LOGfile/LOGframe_ini.cpp"
    "../Sources/LOGfile/LOGframe_msg.cpp"
    "../Sources/LOGfile/LOGmsg.h"
)
source_group("LOGfile" FILES ${LOGfile})

set(MATerial
    "../Sources/MATerial/MATframe.cpp"
    "../Sources/MATerial/MATframe.h"
    "../Sources/MATerial/MATframe_act.cpp"
    "../Sources/MATerial/MATframe_act.h"
    "../Sources/MATerial/MATframe_Convert.cpp"
    "../Sources/MATerial/MATframe_ini.cpp"
    "../Sources/MATerial/MATframe_msg.cpp"
    "../Sources/MATerial/MuTex.cpp"
    "../Sources/MATerial/MuTex.h"
)
source_group("MATerial" FILES ${MATerial})

set(MENu
    "../Sources/MENu/MENframe.cpp"
    "../Sources/MENu/MENframe.h"
    "../Sources/MENu/MENframe_msg.cpp"
    "../Sources/MENu/MENin.cpp"
    "../Sources/MENu/MENin.h"
    "../Sources/MENu/MENinmenu.cpp"
    "../Sources/MENu/MENinmenu.h"
    "../Sources/MENu/MENlist.cpp"
    "../Sources/MENu/MENlist.h"
    "../Sources/MENu/MENmenu.cpp"
    "../Sources/MENu/MENmenu.h"
    "../Sources/MENu/MENstrings.h"
    "../Sources/MENu/MENsubmenu.cpp"
    "../Sources/MENu/MENsubmenu.h"
)
source_group("MENu" FILES ${MENu})

set(OUTput
    "../Sources/OUTput/OUTframe.cpp"
    "../Sources/OUTput/OUTframe.h"
    "../Sources/OUTput/OUTframe_act.cpp"
    "../Sources/OUTput/OUTframe_act.h"
    "../Sources/OUTput/OUTframe_ini.cpp"
    "../Sources/OUTput/OUTframe_msg.cpp"
    "../Sources/OUTput/OUTmsg.h"
    "../Sources/OUTput/OUTstrings.h"
)
source_group("OUTput" FILES ${OUTput})

set(PENdingCheckins
    "../Sources/PENdingCheckins/PENCframe.cpp"
    "../Sources/PENdingCheckins/PENCframe.h"
    "../Sources/PENdingCheckins/PENCframe_act.cpp"
    "../Sources/PENdingCheckins/PENCframe_act.h"
    "../Sources/PENdingCheckins/PENCframe_ini.cpp"
    "../Sources/PENdingCheckins/PENCframe_msg.cpp"
)
source_group("PENdingCheckins" FILES ${PENdingCheckins})

set(PERforce
    "../Sources/PERForce/EditLog.cpp"
    "../Sources/PERForce/EditLog.h"
    "../Sources/PERForce/PERCDataCtrl.h"
    "../Sources/PERForce/PERCDataTreeCtrl.h"
#    "../Sources/PERForce/PERCframe.cpp"
    "../Sources/PERForce/PERCframe.h"
#    "../Sources/PERForce/PERCframe_act.cpp"
    "../Sources/PERForce/PERCframe_act.h"
#    "../Sources/PERForce/PERCframe_ini.cpp"
#    "../Sources/PERForce/PERCframe_msg.cpp"
    "../Sources/PERForce/PERmsg.h"
    "../Sources/PERForce/PERtreectrl.cpp"
    "../Sources/PERForce/PERtreectrl.h"
    "../Sources/PERForce/SubclassWnd.cpp"
    "../Sources/PERForce/SubclassWnd.h"
)
source_group("PERforce" FILES ${PERforce})

set(PERforceCheck
#    "../Sources/PERForceCheck/PERCheck.cpp"
    "../Sources/PERForceCheck/PERCheck.h"
#    "../Sources/PERForceCheck/PERCheckBottomTreeListCtrl.cpp"
    "../Sources/PERForceCheck/PERCheckBottomTreeListCtrl.h"
#    "../Sources/PERForceCheck/PERCheckBottomView.cpp"
    "../Sources/PERForceCheck/PERCheckBottomView.h"
#    "../Sources/PERForceCheck/PERCheckframe.cpp"
    "../Sources/PERForceCheck/PERCheckframe.h"
#    "../Sources/PERForceCheck/PERCheckframe_act.cpp"
    "../Sources/PERForceCheck/PERCheckframe_act.h"
#    "../Sources/PERForceCheck/PERCheckframe_ini.cpp"
#    "../Sources/PERForceCheck/PERCheckframe_msg.cpp"
#    "../Sources/PERForceCheck/PERCheckTopTreeListCtrl.cpp"
    "../Sources/PERForceCheck/PERCheckTopTreeListCtrl.h"
#    "../Sources/PERForceCheck/PERCheckTopView.cpp"
    "../Sources/PERForceCheck/PERCheckTopView.h"
#    "../Sources/PERForceCheck/PERCheckTreeListHeaderCtrl.cpp"
    "../Sources/PERForceCheck/PERCheckTreeListHeaderCtrl.h"
)
source_group("PERforceCheck" FILES ${PERforceCheck})

set(PROperties
    "../Sources/PROperties/PROframe.cpp"
    "../Sources/PROperties/PROframe.h"
    "../Sources/PROperties/PROframe_act.cpp"
    "../Sources/PROperties/PROframe_act.h"
    "../Sources/PROperties/PROframe_ini.cpp"
    "../Sources/PROperties/PROframe_msg.cpp"
    "../Sources/PROperties/PROstrings.h"
)
source_group("PROperties" FILES ${PROperties})

set(PreFaB
    "../Sources/PreFaB/PFBdata.cpp"
    "../Sources/PreFaB/PFBdata.h"
    "../Sources/PreFaB/PFBframe.cpp"
    "../Sources/PreFaB/PFBframe.h"
    "../Sources/PreFaB/PFBframe_act.cpp"
    "../Sources/PreFaB/PFBframe_act.h"
    "../Sources/PreFaB/PFBframe_ini.cpp"
    "../Sources/PreFaB/PFBframe_msg.cpp"
    "../Sources/PreFaB/PFBtreeview.cpp"
    "../Sources/PreFaB/PFBtreeview.h"
    "../Sources/PreFaB/PFBview.cpp"
    "../Sources/PreFaB/PFBview.h"
)
source_group("PreFaB" FILES ${PreFaB})

set(RASters
    "../Sources/RASters/RASframe.cpp"
    "../Sources/RASters/RASframe.h"
    "../Sources/RASters/RASframe_act.h"
    "../Sources/RASters/RASframe_msg.cpp"
)
source_group("RASters" FILES ${RASters})

set(SELection
#    "../Sources/SELection/SELectionframe.cpp"
    "../Sources/SELection/SELectionframe.h"
#    "../Sources/SELection/SELectionframe_act.cpp"
    "../Sources/SELection/SELectionframe_act.h"
#    "../Sources/SELection/SELectionframe_ini.cpp"
#    "../Sources/SELection/SELectionframe_msg.cpp"
#    "../Sources/SELection/SELectionGridCtrl.cpp"
    "../Sources/SELection/SELectionGridCtrl.h"
#    "../Sources/SELection/SELectionGridCtrlCell.cpp"
    "../Sources/SELection/SELectionGridCtrlCell.h"
    "../Sources/SELection/SELectionGridData.h"
#    "../Sources/SELection/SELectionHelper.cpp"
    "../Sources/SELection/SELectionHelper.h"
#    "../Sources/SELection/SELectionHelper_CellToData.cpp"
#    "../Sources/SELection/SELectionHelper_DataToCell.cpp"
#    "../Sources/SELection/SELectionHelper_ValidateData.cpp"
)
source_group("SELection" FILES ${SELection})

set(SELection__GridCtrl
#    "../Sources/SELection/GridCtrl/BtnDataBase.cpp"
    "../Sources/SELection/GridCtrl/BtnDataBase.h"
    "../Sources/SELection/GridCtrl/CellRange.h"
#    "../Sources/SELection/GridCtrl/GridBtnCell.cpp"
    "../Sources/SELection/GridCtrl/GridBtnCell.h"
#    "../Sources/SELection/GridCtrl/GridBtnCellBase.cpp"
    "../Sources/SELection/GridCtrl/GridBtnCellBase.h"
#    "../Sources/SELection/GridCtrl/GridCell.cpp"
    "../Sources/SELection/GridCtrl/GridCell.h"
#    "../Sources/SELection/GridCtrl/GridCellBase.cpp"
    "../Sources/SELection/GridCtrl/GridCellBase.h"
#    "../Sources/SELection/GridCtrl/GridCellCheck.cpp"
    "../Sources/SELection/GridCtrl/GridCellCheck.h"
#    "../Sources/SELection/GridCtrl/GridCellCombo.cpp"
    "../Sources/SELection/GridCtrl/GridCellCombo.h"
#    "../Sources/SELection/GridCtrl/GridCellNumeric.cpp"
    "../Sources/SELection/GridCtrl/GridCellNumeric.h"
#    "../Sources/SELection/GridCtrl/GridCtrl.cpp"
    "../Sources/SELection/GridCtrl/GridCtrl.h"
#    "../Sources/SELection/GridCtrl/GridDropTarget.cpp"
    "../Sources/SELection/GridCtrl/GridDropTarget.h"
#    "../Sources/SELection/GridCtrl/InPlaceEdit.cpp"
    "../Sources/SELection/GridCtrl/InPlaceEdit.h"
#    "../Sources/SELection/GridCtrl/InPlaceEditNumeric.cpp"
    "../Sources/SELection/GridCtrl/InPlaceEditNumeric.h"
    "../Sources/SELection/GridCtrl/MemDC.h"
#    "../Sources/SELection/GridCtrl/TitleTip.cpp"
    "../Sources/SELection/GridCtrl/TitleTip.h"
)
source_group("SELection\\GridCtrl" FILES ${SELection__GridCtrl})

set(SOuNd
    "../Sources/SOuNd/SONerrid.h"
    "../Sources/SOuNd/SONframe.cpp"
    "../Sources/SOuNd/SONframe.h"
    "../Sources/SOuNd/SONframe_act.cpp"
    "../Sources/SOuNd/SONframe_act.h"
    "../Sources/SOuNd/SONframe_bank.cpp"
    "../Sources/SOuNd/SONframe_ini.cpp"
    "../Sources/SOuNd/SONframe_insert.cpp"
    "../Sources/SOuNd/SONframe_Instance.cpp"
    "../Sources/SOuNd/SONframe_mdf.cpp"
    "../Sources/SOuNd/SONframe_msg.cpp"
    "../Sources/SOuNd/SONframe_mtx.cpp"
    "../Sources/SOuNd/SONframe_smodifier.cpp"
    "../Sources/SOuNd/SONframe_sound.cpp"
    "../Sources/SOuNd/SONmixer.cpp"
    "../Sources/SOuNd/SONmixer.h"
    "../Sources/SOuNd/SONmsg.h"
    "../Sources/SOuNd/SONpane.cpp"
    "../Sources/SOuNd/SONpane.h"
    "../Sources/SOuNd/SONstrings.h"
    "../Sources/SOuNd/SONutil.cpp"
    "../Sources/SOuNd/SONutil.h"
    "../Sources/SOuNd/SONview.cpp"
    "../Sources/SOuNd/SONview.h"
    "../Sources/SOuNd/SONview_group.cpp"
    "../Sources/SOuNd/SONview_group.h"
    "../Sources/SOuNd/SONview_smd.cpp"
    "../Sources/SOuNd/SONview_smd.h"
    "../Sources/SOuNd/SONvumeter.cpp"
    "../Sources/SOuNd/SONvumeter.h"
)
source_group("SOuNd" FILES ${SOuNd})

set(TEXTe
    "../Sources/TEXTe/TEXTframe.cpp"
    "../Sources/TEXTe/TEXTframe.h"
    "../Sources/TEXTe/TEXTframe_act.cpp"
    "../Sources/TEXTe/TEXTframe_act.h"
    "../Sources/TEXTe/TEXTframe_ini.cpp"
    "../Sources/TEXTe/TEXTframe_msg.cpp"
    "../Sources/TEXTe/TEXTimportexport.cpp"
    "../Sources/TEXTe/TEXTscroll.cpp"
    "../Sources/TEXTe/TEXTscroll.h"
)
source_group("TEXTe" FILES ${TEXTe})

set(TEXtures
    "../Sources/TEXtures/TEXframe.cpp"
    "../Sources/TEXtures/TEXframe.h"
    "../Sources/TEXtures/TEXframe_act.cpp"
    "../Sources/TEXtures/TEXframe_act.h"
    "../Sources/TEXtures/TEXframe_ini.cpp"
    "../Sources/TEXtures/TEXframe_msg.cpp"
    "../Sources/TEXtures/TEXscroll.cpp"
    "../Sources/TEXtures/TEXscroll.h"
    "../Sources/TEXtures/TEXutil.cpp"
    "../Sources/TEXtures/TEXutil.h"
)
source_group("TEXtures" FILES ${TEXtures})

set(ALL_FILES
    ${no_group_source_files}
    ${ACTions}
    ${AIscript}
    ${AIscript__Compiler}
    ${AIscript__EditView}
    ${BROwser}
    ${EVEnts}
    ${LOGfile}
    ${MATerial}
    ${MENu}
    ${OUTput}
    ${PENdingCheckins}
    ${PERforce}
    ${PERforceCheck}
    ${PROperties}
    ${PreFaB}
    ${RASters}
    ${SELection}
    ${SELection__GridCtrl}
    ${SOuNd}
    ${TEXTe}
    ${TEXtures}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/../Sources/Precomp.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "MFCProj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "EDI___er"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "${PROJECT_NAME}"
        TARGET_NAME_DEBUG                "${PROJECT_NAME}"
        TARGET_NAME_FINAL                "EDI___er"
        TARGET_NAME_MAX25                "EDI___er"
        TARGET_NAME_MAX3                 "EDI___er"
        TARGET_NAME_MAX5                 "EDI___er"
        TARGET_NAME_MAX7                 "EDI___er"
        TARGET_NAME_MAX8                 "EDI___er"
        TARGET_NAME_PROFILE_FASTCAP      "EDI___er"
        TARGET_NAME_PROFILE              "EDI___er"
        TARGET_NAME_RELEASEPC            "EDI___er"
        TARGET_NAME_RELEASE_LTCG         "EDI___er"
        TARGET_NAME_RELEASE              "EDI___er"
        TARGET_NAME_RELEASEXPROFILE_FAST "EDI___er"
        TARGET_NAME_RELEASEXPROFILE      "EDI___er"
        TARGET_NAME_SPEEDEDITORS         "EDI___er"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "EDI___er"
        TARGET_NAME_DEBUGEDITORS         "EDI___ed"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "EDI___ed"
        TARGET_NAME_DEBUG                "EDI___ed"
        TARGET_NAME_FINAL                "EDI___er"
        TARGET_NAME_MAX25                "EDI___er"
        TARGET_NAME_MAX3                 "EDI___er"
        TARGET_NAME_MAX5                 "EDI___er"
        TARGET_NAME_MAX7                 "EDI___er"
        TARGET_NAME_MAX8                 "EDI___er"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "${PROJECT_NAME}"
        TARGET_NAME_RELEASE_LTCG         "${PROJECT_NAME}"
        TARGET_NAME_RELEASE              "EDI___er"
        TARGET_NAME_RELEASEXPROFILE_FAST "EDI___er"
        TARGET_NAME_RELEASEXPROFILE      "EDI___er"
        TARGET_NAME_SPEEDEDITORS         "EDI___er"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "${PROJECT_NAME}"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "EDI___er"
        TARGET_NAME_DEBUG                "EDI___er"
        TARGET_NAME_FINAL                "EDI___er"
        TARGET_NAME_MAX25                "EDI___er"
        TARGET_NAME_MAX3                 "EDI___er"
        TARGET_NAME_MAX5                 "EDI___er"
        TARGET_NAME_MAX7                 "EDI___er"
        TARGET_NAME_MAX8                 "EDI___er"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "${PROJECT_NAME}"
        TARGET_NAME_RELEASE_LTCG         "${PROJECT_NAME}"
        TARGET_NAME_RELEASE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEXPROFILE_FAST "${PROJECT_NAME}"
        TARGET_NAME_RELEASEXPROFILE      "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "${PROJECT_NAME}"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "EDI___er"
        TARGET_NAME_DEBUG                "EDI___er"
        TARGET_NAME_FINAL                "EDI___er"
        TARGET_NAME_MAX25                "EDI___er"
        TARGET_NAME_MAX3                 "EDI___er"
        TARGET_NAME_MAX5                 "EDI___er"
        TARGET_NAME_MAX7                 "EDI___er"
        TARGET_NAME_MAX8                 "EDI___er"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "${PROJECT_NAME}"
        TARGET_NAME_RELEASE_LTCG         "${PROJECT_NAME}"
        TARGET_NAME_RELEASE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEXPROFILE_FAST "${PROJECT_NAME}"
        TARGET_NAME_RELEASEXPROFILE      "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Debug>:"
            "_XENON_RENDER;"
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
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Debug>:"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T;"
            "_USE_32BIT_TIME_T"
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
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
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
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
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
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_DEBUG;"
        "ACTIVE_EDITORS;"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_XENON_RENDER;"
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
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Release>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "SPEED_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
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
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:Final>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleasePC>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
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
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Debug>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Final>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Debug>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Final>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob2;
                /GF;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    endif()
endif()

