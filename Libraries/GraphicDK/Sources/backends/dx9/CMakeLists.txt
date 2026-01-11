set(PROJECT_NAME Dx9)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
#    "Dx9bench.h"
    "DX9blending.c"
    "DX9blending.h"
    "DX9buffer.c"
    "DX9buffer.h"
    "DX9console.c"
    "DX9console.h"
    "DX9draw.c"
    "DX9draw.h"
#    "Dx9Error.h"
#    "Dx9flare.c"
#    "Dx9flare.h"
    "DX9gauges.c"
    "DX9gauges.h"
    "DX9init.c"
    "DX9init.h"
    "DX9matrix.c"
    "DX9matrix.h"
    "DX9renderstate.c"
    "DX9renderstate.h"
    "DX9request.c"
    "DX9request.h"
    "DX9samplerstate.c"
    "DX9samplerstate.h"
    "DX9struct.h"
    "DX9tex.c"
    "DX9tex.h"
#    "DX9text.c"
#    "DX9text.h"
    "DX9texturestagestate.c"
    "DX9texturestagestate.h"
    "DX9uv.h"
#    "Dx9water.c"
#    "Dx9water.h"
)
source_group("" FILES ${no_group_source_files})

set(AfterFX
#    "AfterFX/Dx9AfterFX.c"
#    "AfterFX/Dx9AfterFX_Def.h"
)
source_group("AfterFX" FILES ${AfterFX})

set(ALL_FILES
    ${no_group_source_files}
    ${AfterFX}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries/Graphics Backends")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Dx9_pc_r"
        TARGET_NAME_DEBUGEDITORS         "Dx9_pc_d"
        TARGET_NAME_DEBUGMAX8            "Dx9_pc_d"
        TARGET_NAME_DEBUGPC              "Dx9_pc_d"
        TARGET_NAME_DEBUG                "Dx9_pc_d"
        TARGET_NAME_FINAL                "Dx9_pc_r"
        TARGET_NAME_MAX25                "Dx9_pc_r"
        TARGET_NAME_MAX3                 "Dx9_pc_r"
        TARGET_NAME_MAX5                 "Dx9_pc_r"
        TARGET_NAME_MAX7                 "Dx9_pc_r"
        TARGET_NAME_MAX8                 "Dx9_pc_r"
        TARGET_NAME_PROFILE_FASTCAP      "Dx9_pc_r"
        TARGET_NAME_PROFILE              "Dx9_pc_r"
        TARGET_NAME_RELEASEPC            "Dx9_pc_r"
        TARGET_NAME_RELEASE_LTCG         "Dx9_pc_r"
        TARGET_NAME_RELEASE              "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE_FAST "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE      "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORS         "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORSXENON    "Dx9_pc_r"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Dx9_pc_r"
        TARGET_NAME_DEBUGEDITORS         "Dx9_pc_d"
        TARGET_NAME_DEBUGMAX8            "Dx9_pc_d"
        TARGET_NAME_DEBUGPC              "Dx9_pc_d"
        TARGET_NAME_DEBUG                "Dx9_pc_d"
        TARGET_NAME_FINAL                "Dx9_pc_r"
        TARGET_NAME_MAX25                "Dx9_pc_r"
        TARGET_NAME_MAX3                 "Dx9_pc_r"
        TARGET_NAME_MAX5                 "Dx9_pc_r"
        TARGET_NAME_MAX7                 "Dx9_pc_r"
        TARGET_NAME_MAX8                 "Dx9_pc_r"
        TARGET_NAME_PROFILE_FASTCAP      "Dx9_pc_r"
        TARGET_NAME_PROFILE              "Dx9_pc_r"
        TARGET_NAME_RELEASEPC            "Dx9_pc_r"
        TARGET_NAME_RELEASE_LTCG         "Dx9_pc_r"
        TARGET_NAME_RELEASE              "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE_FAST "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE      "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORS         "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORSXENON    "Dx9_pc_r"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Dx9_pc_r"
        TARGET_NAME_DEBUGEDITORS         "Dx9_pc_d"
        TARGET_NAME_DEBUGMAX8            "Dx9_pc_d"
        TARGET_NAME_DEBUGPC              "Dx9_pc_d"
        TARGET_NAME_DEBUG                "Dx9_pc_d"
        TARGET_NAME_FINAL                "Dx9_pc_r"
        TARGET_NAME_MAX25                "Dx9_pc_r"
        TARGET_NAME_MAX3                 "Dx9_pc_r"
        TARGET_NAME_MAX5                 "Dx9_pc_r"
        TARGET_NAME_MAX7                 "Dx9_pc_r"
        TARGET_NAME_MAX8                 "Dx9_pc_r"
        TARGET_NAME_PROFILE_FASTCAP      "Dx9_pc_r"
        TARGET_NAME_PROFILE              "Dx9_pc_r"
        TARGET_NAME_RELEASEPC            "Dx9_pc_r"
        TARGET_NAME_RELEASE_LTCG         "Dx9_pc_r"
        TARGET_NAME_RELEASE              "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE_FAST "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE      "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORS         "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORSXENON    "Dx9_pc_r"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_BFOBJECTTOOLS        "Dx9_pc_r"
        TARGET_NAME_DEBUGEDITORS         "Dx9_pc_d"
        TARGET_NAME_DEBUGMAX8            "Dx9_pc_d"
        TARGET_NAME_DEBUGPC              "Dx9_pc_d"
        TARGET_NAME_DEBUG                "Dx9_pc_d"
        TARGET_NAME_FINAL                "Dx9_pc_r"
        TARGET_NAME_MAX25                "Dx9_pc_r"
        TARGET_NAME_MAX3                 "Dx9_pc_r"
        TARGET_NAME_MAX5                 "Dx9_pc_r"
        TARGET_NAME_MAX7                 "Dx9_pc_r"
        TARGET_NAME_MAX8                 "Dx9_pc_r"
        TARGET_NAME_PROFILE_FASTCAP      "Dx9_pc_r"
        TARGET_NAME_PROFILE              "Dx9_pc_r"
        TARGET_NAME_RELEASEPC            "Dx9_pc_r"
        TARGET_NAME_RELEASE_LTCG         "Dx9_pc_r"
        TARGET_NAME_RELEASE              "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE_FAST "Dx9_pc_r"
        TARGET_NAME_RELEASEXPROFILE      "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORS         "Dx9_pc_r"
        TARGET_NAME_SPEEDEDITORSXENON    "Dx9_pc_r"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Fastcap XBox"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Fastcap XBox"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Fastcap XBox"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_BFOBJECTTOOLS        "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_DEBUGEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGMAX8            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUGPC              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_DEBUG                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNET/Libs"
        OUTPUT_DIRECTORY_FINAL                "${CMAKE_CURRENT_SOURCE_DIR}/Fastcap XBox"
        OUTPUT_DIRECTORY_MAX25                "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX3                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX5                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX7                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_MAX8                 "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE_FASTCAP      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_PROFILE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEPC            "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE_LTCG         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASE              "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE_FAST "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_RELEASEXPROFILE      "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORS         "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
        OUTPUT_DIRECTORY_SPEEDEDITORSXENON    "${CMAKE_CURRENT_SOURCE_DIR}/./../../OutputNet/Libs"
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/MainPC/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX9/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/MainPC/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX9/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/MainPC/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX9/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Main/MainPC/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../SDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/..;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../GraphicDK/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../ENGine/Sources;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/DX9/Include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Extern/ode/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "WIN32;"
        "_PC_RETAIL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "WIN32;"
        "_PC_RETAIL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "WIN32;"
        "_PC_RETAIL"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:BFObjectTools>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:DebugEditors>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DEBUGMAX8>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:DebugPC>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Debug>:"
            "_DEBUG;"
            "USE_D3D9"
        ">"
        "$<$<CONFIG:Final>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX25>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX3>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX5>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX7>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:MAX8>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile_FastCap>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Profile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleasePC>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release_LTCG>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile_Fast>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:ReleaseXProfile>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditors>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "$<$<CONFIG:SpeedEditorsXenon>:"
            "NDEBUG;"
            "_LIB;"
            "_MBCS"
        ">"
        "WIN32;"
        "_PC_RETAIL"
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
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:DebugEditors>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DebugPC>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Debug>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Final>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            /W4;
            /Zi;
            /Wall;
            /wd4710;
            /wd4820;
            /wd4668;
            /wd4217;
            /wd4214;
            /wd4201;
            /wd4115;
            /wd4204;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:DebugEditors>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DebugPC>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Debug>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Final>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            /W4;
            /Zi;
            /Wall;
            /wd4710;
            /wd4820;
            /wd4668;
            /wd4217;
            /wd4214;
            /wd4201;
            /wd4115;
            /wd4204;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xbox")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:DebugEditors>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DebugPC>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Debug>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Final>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            /W4;
            /Zi;
            /Wall;
            /wd4710;
            /wd4820;
            /wd4668;
            /wd4217;
            /wd4214;
            /wd4201;
            /wd4115;
            /wd4204;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Xenon")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:BFObjectTools>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:DebugEditors>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DEBUGMAX8>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:DebugPC>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Debug>:
                /Gm-;
                /Od;
                /RTC1;
                /TC
            >
            $<$<CONFIG:Final>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy
            >
            $<$<CONFIG:MAX25>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX3>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX5>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX7>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:MAX8>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile_FastCap>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Profile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleasePC>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release_LTCG>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile_Fast>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:ReleaseXProfile>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditors>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            $<$<CONFIG:SpeedEditorsXenon>:
                /O2;
                /Ob1;
                /GF;
                /Oy;
                /Gy;
                /WX
            >
            /W4;
            /Zi;
            /Wall;
            /wd4710;
            /wd4820;
            /wd4668;
            /wd4217;
            /wd4214;
            /wd4201;
            /wd4115;
            /wd4204;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
endif()

