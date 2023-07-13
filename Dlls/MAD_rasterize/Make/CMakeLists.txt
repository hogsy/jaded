set(PROJECT_NAME MAD_Rasterize)

################################################################################
# Source groups
################################################################################
set(sources
#    "../Sources/MAD_MTH.c"
    "../Sources/MAD_Rasterize.c"
    "../Sources/MAD_Rasterize.h"
)
source_group("sources" FILES ${sources})

set(ALL_FILES
    ${sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_rasterize___r"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "MAD_rasterize___r"
        TARGET_NAME_DEBUG                "MAD_rasterize___d"
        TARGET_NAME_FINAL                "MAD_rasterize___r"
        TARGET_NAME_MAX25                "MAD_rasterize___r"
        TARGET_NAME_MAX3                 "MAD_rasterize___r"
        TARGET_NAME_MAX5                 "MAD_rasterize___r"
        TARGET_NAME_MAX7                 "MAD_rasterize___r"
        TARGET_NAME_MAX8                 "MAD_rasterize___r"
        TARGET_NAME_PROFILE_FASTCAP      "MAD_rasterize___r"
        TARGET_NAME_PROFILE              "MAD_rasterize___r"
        TARGET_NAME_RELEASEPC            "MAD_rasterize___r"
        TARGET_NAME_RELEASE_LTCG         "MAD_rasterize___r"
        TARGET_NAME_RELEASE              "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE      "MAD_rasterize___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "MAD_rasterize___r"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "${PROJECT_NAME}"
        TARGET_NAME_DEBUG                "MAD_rasterize___d"
        TARGET_NAME_FINAL                "MAD_rasterize___r"
        TARGET_NAME_MAX25                "MAD_rasterize___r"
        TARGET_NAME_MAX3                 "MAD_rasterize___r"
        TARGET_NAME_MAX5                 "MAD_rasterize___r"
        TARGET_NAME_MAX7                 "MAD_rasterize___r"
        TARGET_NAME_MAX8                 "MAD_rasterize___r"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "MAD_rasterize___r"
        TARGET_NAME_RELEASE_LTCG         "MAD_rasterize___r"
        TARGET_NAME_RELEASE              "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE      "MAD_rasterize___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "${PROJECT_NAME}"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "MAD_rasterize___r"
        TARGET_NAME_DEBUG                "MAD_rasterize___d"
        TARGET_NAME_FINAL                "MAD_rasterize___r"
        TARGET_NAME_MAX25                "MAD_rasterize___r"
        TARGET_NAME_MAX3                 "MAD_rasterize___r"
        TARGET_NAME_MAX5                 "MAD_rasterize___r"
        TARGET_NAME_MAX7                 "MAD_rasterize___r"
        TARGET_NAME_MAX8                 "MAD_rasterize___r"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "MAD_rasterize___r"
        TARGET_NAME_RELEASE_LTCG         "MAD_rasterize___r"
        TARGET_NAME_RELEASE              "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE      "MAD_rasterize___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "${PROJECT_NAME}"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "MAD_rasterize___r"
        TARGET_NAME_DEBUG                "MAD_rasterize___d"
        TARGET_NAME_FINAL                "MAD_rasterize___r"
        TARGET_NAME_MAX25                "MAD_rasterize___r"
        TARGET_NAME_MAX3                 "MAD_rasterize___r"
        TARGET_NAME_MAX5                 "MAD_rasterize___r"
        TARGET_NAME_MAX7                 "MAD_rasterize___r"
        TARGET_NAME_MAX8                 "MAD_rasterize___r"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "MAD_rasterize___r"
        TARGET_NAME_RELEASE_LTCG         "MAD_rasterize___r"
        TARGET_NAME_RELEASE              "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "MAD_rasterize___r"
        TARGET_NAME_RELEASEXPROFILE      "MAD_rasterize___r"
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
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
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
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
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
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
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
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../../Output/Libs"
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
            MultiThreaded
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreaded
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
            MultiThreaded
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
            MultiThreaded
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreaded
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
            MultiThreaded
        >
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Final>:
            MultiThreaded
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../Libraries/SDK/Sources"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG"
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
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32;"
        "PCWIN_TOOL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
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
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32;"
        "PCWIN_TOOL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
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
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32;"
        "PCWIN_TOOL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "_CRT_SECURE_NO_DEPRECATE;"
            "_USE_32BIT_TIME_T"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
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
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_XENON_RENDER;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32;"
        "PCWIN_TOOL"
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
                /Ob1;
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
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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
                /Ob1;
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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
                /Ob1;
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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
                /O2;
                /Ob1;
                /GF;
                /Zi;
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Zi;
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

