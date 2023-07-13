set(PROJECT_NAME OpenGL)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../Sources/Precomp.c"
    "../Sources/Precomp.h"
)
source_group("" FILES ${no_group_source_files})

set(LIBraries
#    "../../../Extern/ddraw.lib"
)
source_group("LIBraries" FILES ${LIBraries})

set(Sources
    "../Sources/OGLAfterEffects.c"
    "../Sources/OGLAfterEffects.h"
    "../Sources/OGLCloneDraw.c"
    "../Sources/OGLdebugfct.c"
    "../Sources/OGLdebugfct.h"
    "../Sources/OGLinit.c"
    "../Sources/OGLinit.h"
    "../Sources/OGLrenderstate.c"
    "../Sources/OGLrenderstate.h"
    "../Sources/OGLrequest.c"
    "../Sources/OGLrequest.h"
    "../Sources/OGLShiftDraw.c"
    "../Sources/OGLStringDraw.c"
    "../Sources/OGLtex.c"
    "../Sources/OGLtex.h"
    "../Sources/OGLWater.cpp"
    "../Sources/OGLWater.h"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${no_group_source_files}
    ${LIBraries}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries/Graphics Backends")

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
        TARGET_NAME_BFOBJECTTOOLS        "OGL___r"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "${PROJECT_NAME}"
        TARGET_NAME_DEBUG                "OGL___d"
        TARGET_NAME_FINAL                "OGL___f"
        TARGET_NAME_MAX25                "OGL___r"
        TARGET_NAME_MAX3                 "OGL___r"
        TARGET_NAME_MAX5                 "OGL___r"
        TARGET_NAME_MAX7                 "OGL___r"
        TARGET_NAME_MAX8                 "OGL___r"
        TARGET_NAME_PROFILE_FASTCAP      "OGL___r"
        TARGET_NAME_PROFILE              "OGL___r"
        TARGET_NAME_RELEASEPC            "OGL___r"
        TARGET_NAME_RELEASE_LTCG         "OGL___r"
        TARGET_NAME_RELEASE              "OGL___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "OGL___r"
        TARGET_NAME_RELEASEXPROFILE      "OGL___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "OGL___er"
        TARGET_NAME_DEBUGEDITORS         "OGL___ed"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "OGL___ed"
        TARGET_NAME_DEBUG                "OGL___d"
        TARGET_NAME_FINAL                "OGL___f"
        TARGET_NAME_MAX25                "OGL___r"
        TARGET_NAME_MAX3                 "OGL___r"
        TARGET_NAME_MAX5                 "OGL___r"
        TARGET_NAME_MAX7                 "OGL___r"
        TARGET_NAME_MAX8                 "OGL___r"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "OGL___r"
        TARGET_NAME_RELEASE_LTCG         "OGL___r"
        TARGET_NAME_RELEASE              "OGL___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "OGL___r"
        TARGET_NAME_RELEASEXPROFILE      "OGL___r"
        TARGET_NAME_SPEEDEDITORS         "OGL___er"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "OGL___er"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "OGL___er"
        TARGET_NAME_DEBUG                "OGL___d"
        TARGET_NAME_FINAL                "OGL___f"
        TARGET_NAME_MAX25                "OGL___r"
        TARGET_NAME_MAX3                 "OGL___r"
        TARGET_NAME_MAX5                 "OGL___r"
        TARGET_NAME_MAX7                 "OGL___r"
        TARGET_NAME_MAX8                 "OGL___r"
        TARGET_NAME_PROFILE_FASTCAP      "OGL___er"
        TARGET_NAME_PROFILE              "OGL___er"
        TARGET_NAME_RELEASEPC            "OGL___r"
        TARGET_NAME_RELEASE_LTCG         "OGL___r"
        TARGET_NAME_RELEASE              "OGL___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "OGL___r"
        TARGET_NAME_RELEASEXPROFILE      "OGL___r"
        TARGET_NAME_SPEEDEDITORS         "OGL___er"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "OGL___er"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "OGL___er"
        TARGET_NAME_DEBUG                "OGL___d"
        TARGET_NAME_FINAL                "OGL___f"
        TARGET_NAME_MAX25                "OGL___r"
        TARGET_NAME_MAX3                 "OGL___r"
        TARGET_NAME_MAX5                 "OGL___r"
        TARGET_NAME_MAX7                 "OGL___r"
        TARGET_NAME_MAX8                 "OGL___r"
        TARGET_NAME_PROFILE_FASTCAP      "OGL___er"
        TARGET_NAME_PROFILE              "OGL___er"
        TARGET_NAME_RELEASEPC            "OGL___r"
        TARGET_NAME_RELEASE_LTCG         "OGL___r"
        TARGET_NAME_RELEASE              "OGL___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "OGL___r"
        TARGET_NAME_RELEASEXPROFILE      "OGL___r"
        TARGET_NAME_SPEEDEDITORS         "OGL___er"
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
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
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
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
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
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
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
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
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
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
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
            MultiThreaded
        >
        $<$<CONFIG:MAX25>:
            MultiThreaded
        >
        $<$<CONFIG:MAX3>:
            MultiThreaded
        >
        $<$<CONFIG:MAX5>:
            MultiThreaded
        >
        $<$<CONFIG:MAX7>:
            MultiThreaded
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
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
            MultiThreaded
        >
        $<$<CONFIG:MAX25>:
            MultiThreaded
        >
        $<$<CONFIG:MAX3>:
            MultiThreaded
        >
        $<$<CONFIG:MAX5>:
            MultiThreaded
        >
        $<$<CONFIG:MAX7>:
            MultiThreaded
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
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
            MultiThreaded
        >
        $<$<CONFIG:MAX25>:
            MultiThreaded
        >
        $<$<CONFIG:MAX3>:
            MultiThreaded
        >
        $<$<CONFIG:MAX5>:
            MultiThreaded
        >
        $<$<CONFIG:MAX7>:
            MultiThreaded
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebug
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
            MultiThreadedDebug
        >
        $<$<CONFIG:SpeedEditorsXenon>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
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
            MultiThreaded
        >
        $<$<CONFIG:MAX25>:
            MultiThreaded
        >
        $<$<CONFIG:MAX3>:
            MultiThreaded
        >
        $<$<CONFIG:MAX5>:
            MultiThreaded
        >
        $<$<CONFIG:MAX7>:
            MultiThreaded
        >
        $<$<CONFIG:MAX8>:
            MultiThreaded
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebug
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
            MultiThreadedDebug
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
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/glew/include"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/glew/include"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources;"
            "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:BFObjectTools>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:Debug>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Final>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX25>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX3>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX5>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX7>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:MAX8>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:Profile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:Release>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEngine/Sources"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Main/WinEditors/Sources"
        ">"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Extern/ode/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "PCWIN_TOOL;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "RELEASE;"
            "_FINAL_;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX25>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX3>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX5>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX7>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX8>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Profile>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "PCWIN_TOOL;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "RELEASE;"
            "_FINAL_;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX25>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX3>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX5>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX7>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX8>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "PCWIN_TOOL;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "RELEASE;"
            "_FINAL_;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX25>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX3>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX5>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX7>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX8>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "PCWIN_TOOL;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "RELEASE;"
            "_FINAL_;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX25>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX3>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX5>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX7>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:MAX8>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:Release>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob1;
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
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleasePC>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob2;
                /GF;
                /Gy;
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
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            $<$<CONFIG:ReleasePC>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob1;
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
                /Gy;
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
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob1;
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
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob1;
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
                /Gy;
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
                /Ob2;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Debug>:
                /Od;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX3>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX5>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX7>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX8>:
                /Ob1;
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
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Ob1;
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
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /Ot;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/O2"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/O2"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/O2"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/O2"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/O2"
            ">"
            "$<$<CONFIG:Profile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/O2"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:DebugEditors>:"
                "/O2"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../Sources/OGLinit.c ${FILE_CL_OPTIONS})
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

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/lib/release/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../extern/glew/lib/release/win32"
    )
endif()

