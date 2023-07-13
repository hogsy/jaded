set(PROJECT_NAME GenerateBatch)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "ReadMe.txt"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "stdafx.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "GenerateBatch.cpp"
    "stdafx.cpp"
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
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        PDB_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        PDB_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
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
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:SpeedEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Gm;
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
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:SpeedEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Gm;
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
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:SpeedEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Gm;
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
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:DebugPC>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Debug>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Final>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX25>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX3>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX5>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX7>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:MAX8>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile_FastCap>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:Profile>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /Zi
            >
            $<$<CONFIG:SpeedEditors>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /Gm;
                /Od;
                /RTC1;
                /ZI
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:DEBUGMAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugPC>:
                /INCREMENTAL
            >
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Final>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX25>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX3>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX5>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX7>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile_FastCap>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /INCREMENTAL
            >
            $<$<CONFIG:ReleasePC>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release_LTCG>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:SpeedEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /INCREMENTAL
            >
            /DEBUG;
            /MACHINE:X86;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:DEBUGMAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugPC>:
                /INCREMENTAL
            >
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Final>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX25>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX3>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX5>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX7>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile_FastCap>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /INCREMENTAL
            >
            $<$<CONFIG:ReleasePC>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release_LTCG>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:SpeedEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /INCREMENTAL
            >
            /DEBUG;
            /MACHINE:X86;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:DEBUGMAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugPC>:
                /INCREMENTAL
            >
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Final>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX25>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX3>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX5>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX7>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile_FastCap>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /INCREMENTAL
            >
            $<$<CONFIG:ReleasePC>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release_LTCG>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:SpeedEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /INCREMENTAL
            >
            /DEBUG;
            /MACHINE:X86;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:DEBUGMAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:DebugPC>:
                /INCREMENTAL
            >
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Final>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX25>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX3>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX5>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX7>:
                /INCREMENTAL
            >
            $<$<CONFIG:MAX8>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile_FastCap>:
                /INCREMENTAL
            >
            $<$<CONFIG:Profile>:
                /INCREMENTAL
            >
            $<$<CONFIG:ReleasePC>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release_LTCG>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:ReleaseXProfile>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            $<$<CONFIG:SpeedEditors>:
                /INCREMENTAL
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /INCREMENTAL
            >
            /DEBUG;
            /MACHINE:X86;
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

