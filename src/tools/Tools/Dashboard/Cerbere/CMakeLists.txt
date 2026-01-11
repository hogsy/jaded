set(PROJECT_NAME Cerbere)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "Cerbere.h"
    "CerbereDlg.h"
    "Defines.h"
    "DefinesShared.h"
    "FileWatch.h"
    "Main.h"
    "PCwatch.h"
    "resource.h"
    "Splash.h"
    "stdafx.h"
    "SystemTray.h"
    "VersionReader.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Resource_Files
    "res/Cerbere.rc2"
)
source_group("Resource Files" FILES ${Resource_Files})

set(Source_Files
    "Cerbere.cpp"
    "CerbereDlg.cpp"
    "FileWatch.cpp"
    "Main.cpp"
    "PCwatch.cpp"
    "Splash.cpp"
    "stdafx.cpp"
    "SystemTray.cpp"
    "VersionReader.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Resource_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tools/Incomplete")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Cerbere)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "MFCProj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "CerbereD"
        TARGET_NAME_DEBUGEDITORS         "CerbereD"
        TARGET_NAME_DEBUGMAX8            "CerbereD"
        TARGET_NAME_DEBUGPC              "CerbereD"
        TARGET_NAME_DEBUG                "CerbereD"
        TARGET_NAME_FINAL                "CerbereD"
        TARGET_NAME_MAX25                "CerbereD"
        TARGET_NAME_MAX3                 "CerbereD"
        TARGET_NAME_MAX5                 "CerbereD"
        TARGET_NAME_MAX7                 "CerbereD"
        TARGET_NAME_MAX8                 "CerbereD"
        TARGET_NAME_PROFILE_FASTCAP      "CerbereD"
        TARGET_NAME_PROFILE              "CerbereD"
        TARGET_NAME_RELEASEPC            "Cerbere"
        TARGET_NAME_RELEASE_LTCG         "Cerbere"
        TARGET_NAME_RELEASE              "Cerbere"
        TARGET_NAME_RELEASEXPROFILE_FAST "Cerbere"
        TARGET_NAME_RELEASEXPROFILE      "Cerbere"
        TARGET_NAME_SPEEDEDITORS         "CerbereD"
        TARGET_NAME_SPEEDEDITORSXENON    "CerbereD"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "CerbereD"
        TARGET_NAME_DEBUGEDITORS         "CerbereD"
        TARGET_NAME_DEBUGMAX8            "CerbereD"
        TARGET_NAME_DEBUGPC              "CerbereD"
        TARGET_NAME_DEBUG                "CerbereD"
        TARGET_NAME_FINAL                "CerbereD"
        TARGET_NAME_MAX25                "CerbereD"
        TARGET_NAME_MAX3                 "CerbereD"
        TARGET_NAME_MAX5                 "CerbereD"
        TARGET_NAME_MAX7                 "CerbereD"
        TARGET_NAME_MAX8                 "CerbereD"
        TARGET_NAME_PROFILE_FASTCAP      "CerbereD"
        TARGET_NAME_PROFILE              "CerbereD"
        TARGET_NAME_RELEASEPC            "Cerbere"
        TARGET_NAME_RELEASE_LTCG         "Cerbere"
        TARGET_NAME_RELEASE              "Cerbere"
        TARGET_NAME_RELEASEXPROFILE_FAST "Cerbere"
        TARGET_NAME_RELEASEXPROFILE      "Cerbere"
        TARGET_NAME_SPEEDEDITORS         "CerbereD"
        TARGET_NAME_SPEEDEDITORSXENON    "CerbereD"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "CerbereD"
        TARGET_NAME_DEBUGEDITORS         "CerbereD"
        TARGET_NAME_DEBUGMAX8            "CerbereD"
        TARGET_NAME_DEBUGPC              "CerbereD"
        TARGET_NAME_DEBUG                "CerbereD"
        TARGET_NAME_FINAL                "CerbereD"
        TARGET_NAME_MAX25                "CerbereD"
        TARGET_NAME_MAX3                 "CerbereD"
        TARGET_NAME_MAX5                 "CerbereD"
        TARGET_NAME_MAX7                 "CerbereD"
        TARGET_NAME_MAX8                 "CerbereD"
        TARGET_NAME_PROFILE_FASTCAP      "CerbereD"
        TARGET_NAME_PROFILE              "CerbereD"
        TARGET_NAME_RELEASEPC            "Cerbere"
        TARGET_NAME_RELEASE_LTCG         "Cerbere"
        TARGET_NAME_RELEASE              "Cerbere"
        TARGET_NAME_RELEASEXPROFILE_FAST "Cerbere"
        TARGET_NAME_RELEASEXPROFILE      "Cerbere"
        TARGET_NAME_SPEEDEDITORS         "CerbereD"
        TARGET_NAME_SPEEDEDITORSXENON    "CerbereD"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "CerbereD"
        TARGET_NAME_DEBUGEDITORS         "CerbereD"
        TARGET_NAME_DEBUGMAX8            "CerbereD"
        TARGET_NAME_DEBUGPC              "CerbereD"
        TARGET_NAME_DEBUG                "CerbereD"
        TARGET_NAME_FINAL                "CerbereD"
        TARGET_NAME_MAX25                "CerbereD"
        TARGET_NAME_MAX3                 "CerbereD"
        TARGET_NAME_MAX5                 "CerbereD"
        TARGET_NAME_MAX7                 "CerbereD"
        TARGET_NAME_MAX8                 "CerbereD"
        TARGET_NAME_PROFILE_FASTCAP      "CerbereD"
        TARGET_NAME_PROFILE              "CerbereD"
        TARGET_NAME_RELEASEPC            "Cerbere"
        TARGET_NAME_RELEASE_LTCG         "Cerbere"
        TARGET_NAME_RELEASE              "Cerbere"
        TARGET_NAME_RELEASEXPROFILE_FAST "Cerbere"
        TARGET_NAME_RELEASEXPROFILE      "Cerbere"
        TARGET_NAME_SPEEDEDITORS         "CerbereD"
        TARGET_NAME_SPEEDEDITORSXENON    "CerbereD"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/Release"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/Debug"
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
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
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
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
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
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
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
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile_FastCap>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Profile>:
            MultiThreadedDebugDLL
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
        "_WINDOWS;"
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
        "_WINDOWS;"
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
        "_WINDOWS;"
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
        "_WINDOWS;"
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
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Gm-;
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
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Zc:wchar_t
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
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Gm-;
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
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Zc:wchar_t
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
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Gm-;
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
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Zc:wchar_t
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
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:Release>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Gm-;
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
                /Gm-;
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
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Zc:wchar_t
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
            /SUBSYSTEM:WINDOWS
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
            /SUBSYSTEM:WINDOWS
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
            /SUBSYSTEM:WINDOWS
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
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Version;"
        "Xbdm"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Version;"
        "Xbdm"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Version;"
        "Xbdm"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Version;"
        "Xbdm"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

