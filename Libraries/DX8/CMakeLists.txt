set(PROJECT_NAME DX8)

################################################################################
# Source groups
################################################################################
set(Headers
    "DX8debugfct.h"
    "DX8init.h"
    "DX8renderstate.h"
    "DX8request.h"
    "DX8tex.h"
    "Precomp.h"
)
source_group("Headers" FILES ${Headers})

set(Sources
    "DX8debugfct.c"
    "DX8init.c"
    "DX8renderstate.c"
    "DX8request.c"
    "DX8tex.c"
    "Precomp.c"
)
source_group("Sources" FILES ${Sources})

set(ALL_FILES
    ${Headers}
    ${Sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries/Graphics Backends")

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Precomp.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "${PROJECT_NAME}"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "${PROJECT_NAME}"
        TARGET_NAME_DEBUG                "DX8___d"
        TARGET_NAME_FINAL                "DX8___f"
        TARGET_NAME_MAX25                "${PROJECT_NAME}"
        TARGET_NAME_MAX3                 "${PROJECT_NAME}"
        TARGET_NAME_MAX5                 "${PROJECT_NAME}"
        TARGET_NAME_MAX7                 "${PROJECT_NAME}"
        TARGET_NAME_MAX8                 "${PROJECT_NAME}"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "DX8___r"
        TARGET_NAME_RELEASE_LTCG         "DX8___r"
        TARGET_NAME_RELEASE              "DX8___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "DX8___r"
        TARGET_NAME_RELEASEXPROFILE      "DX8___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "DX8___edr"
        TARGET_NAME_DEBUGEDITORS         "DX8___edd"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "DX8___edd"
        TARGET_NAME_DEBUG                "DX8___d"
        TARGET_NAME_FINAL                "DX8___f"
        TARGET_NAME_MAX25                "${PROJECT_NAME}"
        TARGET_NAME_MAX3                 "${PROJECT_NAME}"
        TARGET_NAME_MAX5                 "${PROJECT_NAME}"
        TARGET_NAME_MAX7                 "${PROJECT_NAME}"
        TARGET_NAME_MAX8                 "${PROJECT_NAME}"
        TARGET_NAME_PROFILE_FASTCAP      "${PROJECT_NAME}"
        TARGET_NAME_PROFILE              "${PROJECT_NAME}"
        TARGET_NAME_RELEASEPC            "DX8___r"
        TARGET_NAME_RELEASE_LTCG         "DX8___r"
        TARGET_NAME_RELEASE              "DX8___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "DX8___r"
        TARGET_NAME_RELEASEXPROFILE      "DX8___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "DX8___r"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "DX8___edr"
        TARGET_NAME_DEBUG                "DX8___d"
        TARGET_NAME_FINAL                "DX8___f"
        TARGET_NAME_MAX25                "${PROJECT_NAME}"
        TARGET_NAME_MAX3                 "${PROJECT_NAME}"
        TARGET_NAME_MAX5                 "${PROJECT_NAME}"
        TARGET_NAME_MAX7                 "${PROJECT_NAME}"
        TARGET_NAME_MAX8                 "${PROJECT_NAME}"
        TARGET_NAME_PROFILE_FASTCAP      "DX8___r"
        TARGET_NAME_PROFILE              "DX8___r"
        TARGET_NAME_RELEASEPC            "DX8___r"
        TARGET_NAME_RELEASE_LTCG         "DX8___r"
        TARGET_NAME_RELEASE              "DX8___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "DX8___r"
        TARGET_NAME_RELEASEXPROFILE      "DX8___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "DX8___r"
        TARGET_NAME_DEBUGEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_DEBUGMAX8            "${PROJECT_NAME}"
        TARGET_NAME_DEBUGPC              "DX8___edr"
        TARGET_NAME_DEBUG                "DX8___d"
        TARGET_NAME_FINAL                "DX8___f"
        TARGET_NAME_MAX25                "${PROJECT_NAME}"
        TARGET_NAME_MAX3                 "${PROJECT_NAME}"
        TARGET_NAME_MAX5                 "${PROJECT_NAME}"
        TARGET_NAME_MAX7                 "${PROJECT_NAME}"
        TARGET_NAME_MAX8                 "${PROJECT_NAME}"
        TARGET_NAME_PROFILE_FASTCAP      "DX8___r"
        TARGET_NAME_PROFILE              "DX8___r"
        TARGET_NAME_RELEASEPC            "DX8___r"
        TARGET_NAME_RELEASE_LTCG         "DX8___r"
        TARGET_NAME_RELEASE              "DX8___r"
        TARGET_NAME_RELEASEXPROFILE_FAST "DX8___r"
        TARGET_NAME_RELEASEXPROFILE      "DX8___r"
        TARGET_NAME_SPEEDEDITORS         "${PROJECT_NAME}"
        TARGET_NAME_SPEEDEDITORSXENON    "${PROJECT_NAME}"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/../../Output/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../Output/$<CONFIG>/${PROJECT_NAME}/../Libs"
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
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX8/include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/WinEditors/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Dlls;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:Profile>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "_WINDOWS;"
        "WIN32"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "RELEASE;"
            "NDEBUG;"
            "PCWIN_TOOL"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX3>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX5>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX7>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
        ">"
        "$<$<CONFIG:MAX8>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
            "JADEFUSION"
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
            "JADEFUSION"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "_DEBUG;"
            "ACTIVE_EDITORS;"
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
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
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
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
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
                /RTC1;
                /Zi
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:DebugPC>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
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
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
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
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
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
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:DebugEditors>:
                /Od;
                /RTC1;
                /Zi;
                /TP
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Od;
                /RTC1;
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
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Final>:
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
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
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
                /TP
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
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

