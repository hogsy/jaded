set(PROJECT_NAME SelfUpdater)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "ReadMe.txt"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "Psapi.h"
    "StdAfx.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "SelfUpdater.cpp"
    "StdAfx.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tools")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE SelfUpdater)

################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../../"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalRelease"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/tmp/LocalDebug"
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
            MultiThreadedDebug
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
            MultiThreadedDebug
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
            MultiThreadedDebug
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
            MultiThreadedDebug
        >
        $<$<NOT:$<OR:$<CONFIG:BFObjectTools>,$<CONFIG:DebugEditors>,$<CONFIG:DEBUGMAX8>,$<CONFIG:DebugPC>,$<CONFIG:Debug>,$<CONFIG:Final>,$<CONFIG:MAX25>,$<CONFIG:MAX3>,$<CONFIG:MAX5>,$<CONFIG:MAX7>,$<CONFIG:MAX8>,$<CONFIG:Profile_FastCap>,$<CONFIG:Profile>,$<CONFIG:ReleasePC>,$<CONFIG:Release_LTCG>,$<CONFIG:Release>,$<CONFIG:ReleaseXProfile_Fast>,$<CONFIG:ReleaseXProfile>,$<CONFIG:SpeedEditors>,$<CONFIG:SpeedEditorsXenon>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Final>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_MBCS"
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
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /RTC1;
                /ZI
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /RTC1;
                /ZI
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /RTC1;
                /ZI
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:SpeedEditors>:
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Od;
                /RTC1;
                /ZI
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    endif()
    source_file_compile_options(SelfUpdater.cpp ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:BFObjectTools>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DEBUGMAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:DebugPC>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Final>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX25>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX3>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX5>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX7>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:MAX8>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile_FastCap>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Profile>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:ReleasePC>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release_LTCG>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile_Fast>:"
                "/O2"
            ">"
            "$<$<CONFIG:ReleaseXProfile>:"
                "/O2"
            ">"
            "$<$<CONFIG:SpeedEditors>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:SpeedEditorsXenon>:"
                "/Od;/RTC1"
            ">"
        )
    endif()
    source_file_compile_options(StdAfx.cpp ${FILE_CL_OPTIONS})
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
            $<$<CONFIG:MAX8>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
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
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NOLOGO;
            /MACHINE:X86;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
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
            $<$<CONFIG:MAX8>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
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
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NOLOGO;
            /MACHINE:X86;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
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
            $<$<CONFIG:MAX8>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
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
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NOLOGO;
            /MACHINE:X86;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
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
            $<$<CONFIG:MAX8>:
                /DEBUG
            >
            $<$<CONFIG:Profile_FastCap>:
                /DEBUG
            >
            $<$<CONFIG:Profile>:
                /DEBUG
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
                /DEBUG
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /DEBUG
            >
            /NOLOGO;
            /MACHINE:X86;
            /SUBSYSTEM:WINDOWS;
            /INCREMENTAL:NO
        )
    endif()
endif()

