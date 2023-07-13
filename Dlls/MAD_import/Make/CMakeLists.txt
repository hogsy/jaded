set(PROJECT_NAME MAD_import)

################################################################################
# Source groups
################################################################################
set(LIBS
#    "../../../Output/Libs/MAD_loadsave___r.lib"
#    "../../../Output/Libs/MAD_mem___r.lib"
)
source_group("LIBS" FILES ${LIBS})

set(MAX25LIBS
#    "../../Max25sdk/LIB/ACAP.LIB"
#    "../../Max25sdk/LIB/BMM.LIB"
#    "../../Max25sdk/LIB/CLIENT.LIB"
#    "../../Max25sdk/LIB/Core.lib"
#    "../../Max25sdk/LIB/EDMODEL.LIB"
#    "../../Max25sdk/LIB/EXPR.LIB"
#    "../../Max25sdk/LIB/FLILIBD.LIB"
#    "../../Max25sdk/LIB/FLILIBH.LIB"
#    "../../Max25sdk/LIB/FLILIBR.LIB"
#    "../../Max25sdk/LIB/FLT.LIB"
#    "../../Max25sdk/LIB/GCOMM.LIB"
#    "../../Max25sdk/LIB/GEOM.LIB"
#    "../../Max25sdk/LIB/GFX.LIB"
#    "../../Max25sdk/LIB/MESH.LIB"
#    "../../Max25sdk/LIB/MNMATH.LIB"
#    "../../Max25sdk/LIB/PARTICLE.LIB"
#    "../../Max25sdk/LIB/PATCH.LIB"
#    "../../Max25sdk/LIB/UTIL.LIB"
#    "../../Max25sdk/LIB/VIEWFILE.LIB"
)
source_group("MAX25LIBS" FILES ${MAX25LIBS})

set(MAX3LIBS
#    "../../Max3sdk/lib/acap.lib"
#    "../../Max3sdk/lib/bmm.lib"
#    "../../Max3sdk/lib/client.lib"
#    "../../Max3sdk/lib/core.lib"
#    "../../Max3sdk/lib/edmodel.lib"
#    "../../Max3sdk/lib/expr.lib"
#    "../../Max3sdk/lib/FLILIBD.LIB"
#    "../../Max3sdk/lib/FLILIBH.LIB"
#    "../../Max3sdk/lib/FLILIBR.LIB"
#    "../../Max3sdk/lib/flt.lib"
#    "../../Max3sdk/lib/gcomm.lib"
#    "../../Max3sdk/lib/geom.lib"
#    "../../Max3sdk/lib/gfx.lib"
#    "../../Max3sdk/lib/gup.lib"
#    "../../Max3sdk/lib/helpsys.lib"
#    "../../Max3sdk/lib/Maxscrpt.lib"
#    "../../Max3sdk/lib/maxutil.lib"
#    "../../Max3sdk/lib/mesh.lib"
#    "../../Max3sdk/lib/MNMath.lib"
#    "../../Max3sdk/lib/Paramblk2.lib"
#    "../../Max3sdk/lib/particle.lib"
#    "../../Max3sdk/lib/tessint.lib"
#    "../../Max3sdk/lib/viewfile.lib"
#    "../../Max3sdk/lib/zlibdll.lib"
)
source_group("MAX3LIBS" FILES ${MAX3LIBS})

set(MAX5LIBS
#    "../../max5sdk/lib/acap.lib"
#    "../../max5sdk/lib/bmm.lib"
#    "../../max5sdk/lib/core.lib"
#    "../../max5sdk/lib/CustDlg.lib"
#    "../../max5sdk/lib/edmodel.lib"
#    "../../max5sdk/lib/expr.lib"
#    "../../max5sdk/lib/FLILIBD.LIB"
#    "../../max5sdk/lib/FLILIBH.LIB"
#    "../../max5sdk/lib/FLILIBR.LIB"
#    "../../max5sdk/lib/flt.lib"
#    "../../max5sdk/lib/gcomm2.lib"
#    "../../max5sdk/lib/geom.lib"
#    "../../max5sdk/lib/gfx.lib"
#    "../../max5sdk/lib/gup.lib"
#    "../../max5sdk/lib/helpsys.lib"
#    "../../max5sdk/lib/imageViewers.lib"
#    "../../max5sdk/lib/ManipSys.lib"
#    "../../max5sdk/lib/maxnet.lib"
#    "../../max5sdk/lib/Maxscrpt.lib"
#    "../../max5sdk/lib/maxutil.lib"
#    "../../max5sdk/lib/MenuMan.lib"
#    "../../max5sdk/lib/menus.lib"
#    "../../max5sdk/lib/mesh.lib"
#    "../../max5sdk/lib/MNMath.lib"
#    "../../max5sdk/lib/Paramblk2.lib"
#    "../../max5sdk/lib/particle.lib"
#    "../../max5sdk/lib/Poly.lib"
#    "../../max5sdk/lib/RenderUtil.lib"
#    "../../max5sdk/lib/SpringSys.lib"
#    "../../max5sdk/lib/tessint.lib"
#    "../../max5sdk/lib/viewfile.lib"
#    "../../max5sdk/lib/zlibdll.lib"
)
source_group("MAX5LIBS" FILES ${MAX5LIBS})

set(MAX8LIBS
#    "x:/Dlls/Max8sdk/lib/acap.lib"
#    "x:/Dlls/Max8sdk/lib/Amodeler.lib"
#    "x:/Dlls/Max8sdk/lib/biped.lib"
#    "x:/Dlls/Max8sdk/lib/bmm.lib"
#    "x:/Dlls/Max8sdk/lib/composite.lib"
#    "x:/Dlls/Max8sdk/lib/core.lib"
#    "x:/Dlls/Max8sdk/lib/crowd.lib"
#    "x:/Dlls/Max8sdk/lib/CustDlg.lib"
#    "x:/Dlls/Max8sdk/lib/edmodel.lib"
#    "x:/Dlls/Max8sdk/lib/expr.lib"
#    "x:/Dlls/Max8sdk/lib/Flilibd.lib"
#    "x:/Dlls/Max8sdk/lib/Flilibh.lib"
#    "x:/Dlls/Max8sdk/lib/Flilibr.lib"
#    "x:/Dlls/Max8sdk/lib/flt.lib"
#    "x:/Dlls/Max8sdk/lib/gcomm2.lib"
#    "x:/Dlls/Max8sdk/lib/geom.lib"
#    "x:/Dlls/Max8sdk/lib/geomimp.lib"
#    "x:/Dlls/Max8sdk/lib/gfx.lib"
#    "x:/Dlls/Max8sdk/lib/gup.lib"
#    "x:/Dlls/Max8sdk/lib/helpsys.lib"
#    "x:/Dlls/Max8sdk/lib/IGame.lib"
#    "x:/Dlls/Max8sdk/lib/imageViewers.lib"
#    "x:/Dlls/Max8sdk/lib/ManipSys.lib"
#    "x:/Dlls/Max8sdk/lib/maxnet.lib"
#    "x:/Dlls/Max8sdk/lib/Maxscrpt.lib"
#    "x:/Dlls/Max8sdk/lib/maxutil.lib"
#    "x:/Dlls/Max8sdk/lib/MenuMan.lib"
#    "x:/Dlls/Max8sdk/lib/menus.lib"
#    "x:/Dlls/Max8sdk/lib/mesh.lib"
#    "x:/Dlls/Max8sdk/lib/MNMath.lib"
#    "x:/Dlls/Max8sdk/lib/mtl.lib"
#    "x:/Dlls/Max8sdk/lib/Paramblk2.lib"
#    "x:/Dlls/Max8sdk/lib/particle.lib"
#    "x:/Dlls/Max8sdk/lib/ParticleFlow.lib"
#    "x:/Dlls/Max8sdk/lib/physique.lib"
#    "x:/Dlls/Max8sdk/lib/Poly.lib"
#    "x:/Dlls/Max8sdk/lib/RenderUtil.lib"
#    "x:/Dlls/Max8sdk/lib/rtmax.lib"
#    "x:/Dlls/Max8sdk/lib/SpringSys.lib"
#    "x:/Dlls/Max8sdk/lib/tessint.lib"
#    "x:/Dlls/Max8sdk/lib/viewfile.lib"
#    "x:/Dlls/Max8sdk/lib/zlibdll.lib"
)
source_group("MAX8LIBS" FILES ${MAX8LIBS})

set(Sources
    "X:/Dlls/MAD_import/Sources/MAD_import.cpp"
    "X:/Dlls/MAD_import/Sources/MAD_import.def"
    "X:/Dlls/MAD_import/Sources/MAD_import.h"
    "X:/Dlls/MAD_import/Sources/MADCDLG.cpp"
    "X:/Dlls/MAD_import/Sources/MADCDLG.h"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${LIBS}
    ${MAX25LIBS}
    ${MAX3LIBS}
    ${MAX5LIBS}
    ${MAX8LIBS}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tools")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_import8"
        TARGET_NAME_DEBUGEDITORS         "MAD_import8"
        TARGET_NAME_DEBUGMAX8            "MAD_import8"
        TARGET_NAME_DEBUGPC              "MAD_import8"
        TARGET_NAME_DEBUG                "MAD_import8"
        TARGET_NAME_FINAL                "MAD_import8"
        TARGET_NAME_MAX25                "MAD_import25"
        TARGET_NAME_MAX3                 "MAD_import3"
        TARGET_NAME_MAX5                 "MAD_import5"
        TARGET_NAME_MAX7                 "MAD_import7"
        TARGET_NAME_MAX8                 "MAD_import8"
        TARGET_NAME_PROFILE_FASTCAP      "MAD_import8"
        TARGET_NAME_PROFILE              "MAD_import8"
        TARGET_NAME_RELEASEPC            "MAD_import8"
        TARGET_NAME_RELEASE_LTCG         "MAD_import8"
        TARGET_NAME_RELEASE              "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE      "MAD_import8"
        TARGET_NAME_SPEEDEDITORS         "MAD_import8"
        TARGET_NAME_SPEEDEDITORSXENON    "MAD_import8"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_import8"
        TARGET_NAME_DEBUGEDITORS         "MAD_import8"
        TARGET_NAME_DEBUGMAX8            "MAD_import8"
        TARGET_NAME_DEBUGPC              "MAD_import8"
        TARGET_NAME_DEBUG                "MAD_import8"
        TARGET_NAME_FINAL                "MAD_import8"
        TARGET_NAME_MAX25                "MAD_import25"
        TARGET_NAME_MAX3                 "MAD_import3"
        TARGET_NAME_MAX5                 "MAD_import5"
        TARGET_NAME_MAX7                 "MAD_import7"
        TARGET_NAME_MAX8                 "MAD_import8"
        TARGET_NAME_PROFILE_FASTCAP      "MAD_import8"
        TARGET_NAME_PROFILE              "MAD_import8"
        TARGET_NAME_RELEASEPC            "MAD_import8"
        TARGET_NAME_RELEASE_LTCG         "MAD_import8"
        TARGET_NAME_RELEASE              "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE      "MAD_import8"
        TARGET_NAME_SPEEDEDITORS         "MAD_import8"
        TARGET_NAME_SPEEDEDITORSXENON    "MAD_import8"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_import8"
        TARGET_NAME_DEBUGEDITORS         "MAD_import8"
        TARGET_NAME_DEBUGMAX8            "MAD_import8"
        TARGET_NAME_DEBUGPC              "MAD_import8"
        TARGET_NAME_DEBUG                "MAD_import8"
        TARGET_NAME_FINAL                "MAD_import8"
        TARGET_NAME_MAX25                "MAD_import25"
        TARGET_NAME_MAX3                 "MAD_import3"
        TARGET_NAME_MAX5                 "MAD_import5"
        TARGET_NAME_MAX7                 "MAD_import7"
        TARGET_NAME_MAX8                 "MAD_import8"
        TARGET_NAME_PROFILE_FASTCAP      "MAD_import8"
        TARGET_NAME_PROFILE              "MAD_import8"
        TARGET_NAME_RELEASEPC            "MAD_import8"
        TARGET_NAME_RELEASE_LTCG         "MAD_import8"
        TARGET_NAME_RELEASE              "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE      "MAD_import8"
        TARGET_NAME_SPEEDEDITORS         "MAD_import8"
        TARGET_NAME_SPEEDEDITORSXENON    "MAD_import8"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_import8"
        TARGET_NAME_DEBUGEDITORS         "MAD_import8"
        TARGET_NAME_DEBUGMAX8            "MAD_import8"
        TARGET_NAME_DEBUGPC              "MAD_import8"
        TARGET_NAME_DEBUG                "MAD_import8"
        TARGET_NAME_FINAL                "MAD_import8"
        TARGET_NAME_MAX25                "MAD_import25"
        TARGET_NAME_MAX3                 "MAD_import3"
        TARGET_NAME_MAX5                 "MAD_import5"
        TARGET_NAME_MAX7                 "MAD_import7"
        TARGET_NAME_MAX8                 "MAD_import8"
        TARGET_NAME_PROFILE_FASTCAP      "MAD_import8"
        TARGET_NAME_PROFILE              "MAD_import8"
        TARGET_NAME_RELEASEPC            "MAD_import8"
        TARGET_NAME_RELEASE_LTCG         "MAD_import8"
        TARGET_NAME_RELEASE              "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_import8"
        TARGET_NAME_RELEASEXPROFILE      "MAD_import8"
        TARGET_NAME_SPEEDEDITORS         "MAD_import8"
        TARGET_NAME_SPEEDEDITORSXENON    "MAD_import8"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGMAX8            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGPC              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUG                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_FINAL                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX25"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX3"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX5"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max7"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max8"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_PROFILE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEPC            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "C:/Program Files/Autodesk/3dsMax8/plugins"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGMAX8            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGPC              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUG                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_FINAL                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX25"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX3"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX5"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max7"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max8"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_PROFILE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEPC            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "C:/Program Files/Autodesk/3dsMax8/plugins"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGMAX8            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGPC              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUG                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_FINAL                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX25"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX3"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX5"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max7"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max8"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_PROFILE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEPC            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "C:/Program Files/Autodesk/3dsMax8/plugins"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGMAX8            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUGPC              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_DEBUG                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_FINAL                "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX25"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX3"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_MAX5"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max7"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../plugins_max8"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_PROFILE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEPC            "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASE              "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "C:/Program Files/Autodesk/3dsMax8/plugins"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "C:/Program Files/Autodesk/3dsMax8/plugins"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./MAD_impo"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUG                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_FINAL                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX25                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX3                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX5                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX7                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX8                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "MAD_import3"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUG                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_FINAL                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX25                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX3                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX5                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX7                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX8                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "MAD_import3"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUG                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_FINAL                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX25                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX3                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX5                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX7                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX8                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "MAD_import3"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_DEBUG                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_FINAL                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX25                "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX3                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX5                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX7                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_MAX8                 "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_PROFILE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASE              "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "MAD_import3"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "MAD_import3"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "MAD_import3"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY                      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY                      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY                      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY                      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/ExeDlls"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreadedDebugDLL
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreadedDebugDLL
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreadedDebugDLL
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:BFObjectTools>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DEBUGMAX8>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:DebugPC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Final>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX25>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX3>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX5>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX7>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleasePC>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release_LTCG>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile_Fast>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:ReleaseXProfile>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditors>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreadedDebugDLL
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
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max25sdk/INCLUDE"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max3sdk/Include"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../max5sdk/include"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/include"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max25sdk/INCLUDE"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max3sdk/Include"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../max5sdk/include"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/include"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max25sdk/INCLUDE"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max3sdk/Include"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../max5sdk/include"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/include"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max25sdk/INCLUDE"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max3sdk/Include"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../max5sdk/include"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/include"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/include;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX_UTILS/include"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../.."
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "MAX25"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "MAX3"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "MAX5"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "MAX7"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "MAX25"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "MAX3"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "MAX5"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "MAX7"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "MAX25"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "MAX3"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "MAX5"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "MAX7"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "WIN32;"
        "_WINDOWS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "MAX25"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "MAX3"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "MAX5"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "MAX7"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:Release>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "MAX8"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "MAX8"
        ">"
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
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Final>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX8>:
                /O2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /EHa
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Profile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleasePC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release_LTCG>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /Zi;
                /EHa
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Final>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX8>:
                /O2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /EHa
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Profile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleasePC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release_LTCG>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /Zi;
                /EHa
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Final>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX8>:
                /O2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /EHa
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Profile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleasePC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release_LTCG>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /Zi;
                /EHa
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Final>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /Zi
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:MAX8>:
                /O2;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /EHa
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Profile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleasePC>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release_LTCG>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:Release>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /Zi;
                /EHa
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /Zi;
                /EHa
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG
            >
            $<$<CONFIG:Debug>:
                /DEBUG
            >
            $<$<CONFIG:Final>:
                /DEBUG
            >
            $<$<CONFIG:MAX25>:
                /DEBUG
            >
            $<$<CONFIG:MAX3>:
                /DEBUG
            >
            $<$<CONFIG:MAX5>:
                /DEBUG
            >
            $<$<CONFIG:MAX7>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NODEFAULTLIB:LIBCMTD.lib;
            /NOLOGO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG
            >
            $<$<CONFIG:Debug>:
                /DEBUG
            >
            $<$<CONFIG:Final>:
                /DEBUG
            >
            $<$<CONFIG:MAX25>:
                /DEBUG
            >
            $<$<CONFIG:MAX3>:
                /DEBUG
            >
            $<$<CONFIG:MAX5>:
                /DEBUG
            >
            $<$<CONFIG:MAX7>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NODEFAULTLIB:LIBCMTD.lib;
            /NOLOGO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG
            >
            $<$<CONFIG:Debug>:
                /DEBUG
            >
            $<$<CONFIG:Final>:
                /DEBUG
            >
            $<$<CONFIG:MAX25>:
                /DEBUG
            >
            $<$<CONFIG:MAX3>:
                /DEBUG
            >
            $<$<CONFIG:MAX5>:
                /DEBUG
            >
            $<$<CONFIG:MAX7>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NODEFAULTLIB:LIBCMTD.lib;
            /NOLOGO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO;
            /MACHINE:I386
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /DEBUG
            >
            $<$<CONFIG:DebugEditors>:
                /DEBUG
            >
            $<$<CONFIG:DEBUGMAX8>:
                /DEBUG
            >
            $<$<CONFIG:DebugPC>:
                /DEBUG
            >
            $<$<CONFIG:Debug>:
                /DEBUG
            >
            $<$<CONFIG:Final>:
                /DEBUG
            >
            $<$<CONFIG:MAX25>:
                /DEBUG
            >
            $<$<CONFIG:MAX3>:
                /DEBUG
            >
            $<$<CONFIG:MAX5>:
                /DEBUG
            >
            $<$<CONFIG:MAX7>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
            >
            $<$<CONFIG:ReleasePC>:
                /DEBUG
            >
            $<$<CONFIG:Release_LTCG>:
                /DEBUG
            >
            $<$<CONFIG:Release>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /DEBUG
            >
            $<$<CONFIG:ReleaseXProfile>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditors>:
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NODEFAULTLIB:LIBCMTD.lib;
            /NOLOGO;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO;
            /MACHINE:I386
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Debug>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Final>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:MAX8>:"
            "maxutil;"
            "SkinUtils"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Profile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "odbc32;"
        "odbccp32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Debug>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Final>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:MAX8>:"
            "maxutil;"
            "SkinUtils"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Profile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "odbc32;"
        "odbccp32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Debug>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Final>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:MAX8>:"
            "maxutil;"
            "SkinUtils"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Profile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "odbc32;"
        "odbccp32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:BFObjectTools>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Debug>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Final>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:MAX8>:"
            "maxutil;"
            "SkinUtils"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Profile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:Release>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "maxutil;"
            "SkinUtils;"
            "mtl"
        ">"
        "odbc32;"
        "odbccp32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/lib"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/lib"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/lib"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX7SDK/lib"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../Max_Utils/lib;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../MAX8SDK/lib"
        ">"
    )
endif()

