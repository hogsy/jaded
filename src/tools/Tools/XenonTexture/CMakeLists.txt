set(PROJECT_NAME XenonTexture)

################################################################################
# Source groups
################################################################################
set(Source_Files
    "Sources/CD3DXBlt.cpp"
    "Sources/CD3DXBlt.h"
    "Sources/XenonTexture.cpp"
    "Sources/XenonTexture.h"
    "Sources/XeSurface.cpp"
    "Sources/XeSurface.h"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tools")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "XenonTexture"
        TARGET_NAME_DEBUGEDITORS         "XenonTexture"
        TARGET_NAME_DEBUGMAX8            "XenonTexture"
        TARGET_NAME_DEBUGPC              "XenonTexture"
        TARGET_NAME_DEBUG                "XenonTexture"
        TARGET_NAME_FINAL                "XenonTexture"
        TARGET_NAME_MAX25                "XenonTexture"
        TARGET_NAME_MAX3                 "XenonTexture"
        TARGET_NAME_MAX5                 "XenonTexture"
        TARGET_NAME_MAX7                 "XenonTexture"
        TARGET_NAME_MAX8                 "XenonTexture"
        TARGET_NAME_PROFILE_FASTCAP      "XenonTexture"
        TARGET_NAME_PROFILE              "XenonTexture"
        TARGET_NAME_RELEASEPC            "XenonTexture"
        TARGET_NAME_RELEASE_LTCG         "XenonTexture"
        TARGET_NAME_RELEASE              "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE      "XenonTexture"
        TARGET_NAME_SPEEDEDITORS         "XenonTexture"
        TARGET_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "XenonTexture"
        TARGET_NAME_DEBUGEDITORS         "XenonTexture"
        TARGET_NAME_DEBUGMAX8            "XenonTexture"
        TARGET_NAME_DEBUGPC              "XenonTexture"
        TARGET_NAME_DEBUG                "XenonTexture"
        TARGET_NAME_FINAL                "XenonTexture"
        TARGET_NAME_MAX25                "XenonTexture"
        TARGET_NAME_MAX3                 "XenonTexture"
        TARGET_NAME_MAX5                 "XenonTexture"
        TARGET_NAME_MAX7                 "XenonTexture"
        TARGET_NAME_MAX8                 "XenonTexture"
        TARGET_NAME_PROFILE_FASTCAP      "XenonTexture"
        TARGET_NAME_PROFILE              "XenonTexture"
        TARGET_NAME_RELEASEPC            "XenonTexture"
        TARGET_NAME_RELEASE_LTCG         "XenonTexture"
        TARGET_NAME_RELEASE              "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE      "XenonTexture"
        TARGET_NAME_SPEEDEDITORS         "XenonTexture"
        TARGET_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "XenonTexture"
        TARGET_NAME_DEBUGEDITORS         "XenonTexture"
        TARGET_NAME_DEBUGMAX8            "XenonTexture"
        TARGET_NAME_DEBUGPC              "XenonTexture"
        TARGET_NAME_DEBUG                "XenonTexture"
        TARGET_NAME_FINAL                "XenonTexture"
        TARGET_NAME_MAX25                "XenonTexture"
        TARGET_NAME_MAX3                 "XenonTexture"
        TARGET_NAME_MAX5                 "XenonTexture"
        TARGET_NAME_MAX7                 "XenonTexture"
        TARGET_NAME_MAX8                 "XenonTexture"
        TARGET_NAME_PROFILE_FASTCAP      "XenonTexture"
        TARGET_NAME_PROFILE              "XenonTexture"
        TARGET_NAME_RELEASEPC            "XenonTexture"
        TARGET_NAME_RELEASE_LTCG         "XenonTexture"
        TARGET_NAME_RELEASE              "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE      "XenonTexture"
        TARGET_NAME_SPEEDEDITORS         "XenonTexture"
        TARGET_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "XenonTexture"
        TARGET_NAME_DEBUGEDITORS         "XenonTexture"
        TARGET_NAME_DEBUGMAX8            "XenonTexture"
        TARGET_NAME_DEBUGPC              "XenonTexture"
        TARGET_NAME_DEBUG                "XenonTexture"
        TARGET_NAME_FINAL                "XenonTexture"
        TARGET_NAME_MAX25                "XenonTexture"
        TARGET_NAME_MAX3                 "XenonTexture"
        TARGET_NAME_MAX5                 "XenonTexture"
        TARGET_NAME_MAX7                 "XenonTexture"
        TARGET_NAME_MAX8                 "XenonTexture"
        TARGET_NAME_PROFILE_FASTCAP      "XenonTexture"
        TARGET_NAME_PROFILE              "XenonTexture"
        TARGET_NAME_RELEASEPC            "XenonTexture"
        TARGET_NAME_RELEASE_LTCG         "XenonTexture"
        TARGET_NAME_RELEASE              "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        TARGET_NAME_RELEASEXPROFILE      "XenonTexture"
        TARGET_NAME_SPEEDEDITORS         "XenonTexture"
        TARGET_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Speed Editors"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/DebugEditors"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGMAX8            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUGPC              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_FINAL                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX25                "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX3                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX5                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX7                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_MAX8                 "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_PROFILE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEPC            "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE_LTCG         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE              "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASEXPROFILE      "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORS         "${OUTPUT_DIRECTORY}"
        ARCHIVE_OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${OUTPUT_DIRECTORY}"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUG                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_FINAL                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX25                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX3                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX5                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX7                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX8                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUG                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_FINAL                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX25                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX3                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX5                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX7                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX8                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUG                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_FINAL                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX25                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX3                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX5                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX7                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX8                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "XenonTexture"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ARCHIVE_OUTPUT_NAME_BFOBJECTTOOLS        "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGMAX8            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUGPC              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_DEBUG                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_FINAL                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX25                "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX3                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX5                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX7                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_MAX8                 "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE_FASTCAP      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_PROFILE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEPC            "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE_LTCG         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASE              "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE_FAST "XenonTexture"
        ARCHIVE_OUTPUT_NAME_RELEASEXPROFILE      "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORS         "XenonTexture"
        ARCHIVE_OUTPUT_NAME_SPEEDEDITORSXENON    "XenonTexture"
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
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{XEDK}/include/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{XEDK}/include/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{XEDK}/include/win32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "$ENV{XEDK}/include/win32"
    )
endif()

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
        "_USRDLL;"
        "XENONTEXTURE_EXPORTS;"
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
        "_USRDLL;"
        "XENONTEXTURE_EXPORTS;"
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
        "_USRDLL;"
        "XENONTEXTURE_EXPORTS;"
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
        "_USRDLL;"
        "XENONTEXTURE_EXPORTS;"
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
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Zi
            >
            $<$<CONFIG:Release>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
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
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Zi
            >
            $<$<CONFIG:Release>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
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
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Zi
            >
            $<$<CONFIG:Release>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
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
                /Zi
            >
            $<$<CONFIG:Release_LTCG>:
                /Zi
            >
            $<$<CONFIG:Release>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /Zi
            >
            $<$<CONFIG:ReleaseXProfile>:
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
        "$(XEDK)/lib/win32/d3d9;"
        "$(XEDK)/lib/win32/xgraphics;"
        "$(XEDK)/lib/win32/d3dx9"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$(XEDK)/lib/win32/d3d9;"
        "$(XEDK)/lib/win32/xgraphics;"
        "$(XEDK)/lib/win32/d3dx9"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$(XEDK)/lib/win32/d3d9;"
        "$(XEDK)/lib/win32/xgraphics;"
        "$(XEDK)/lib/win32/d3dx9"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$(XEDK)/lib/win32/d3d9;"
        "$(XEDK)/lib/win32/xgraphics;"
        "$(XEDK)/lib/win32/d3dx9"
    )
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

