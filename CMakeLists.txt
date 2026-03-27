cmake_minimum_required(VERSION 3.23)
project(Jaded
        DESCRIPTION "Community patched version of Ubisoft's Jade engine."
        HOMEPAGE_URL "https://hogsy.me/jaded.htm"
        LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)

include(DetermineTargetArchitecture.cmake)
determine_target_architecture(JADE_SYSTEM_ARCH)

###################################################################################

# Get the current working branch
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the latest abbreviated commit hash of the working branch
execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the number of commits on the working branch
execute_process(
        COMMAND git rev-list HEAD --count
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
add_definitions(-DGIT_COMMIT_COUNT="${GIT_COMMIT_COUNT}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")

###################################################################################

set(JADE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/runtime/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${JADE_OUTPUT_DIR}")
foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${JADE_OUTPUT_DIR}")
endforeach ()

add_compile_options(/MP /W4)

add_definitions(
        -DACTIVE_EDITORS
        -D_USE_32BIT_TIME_T
        -D_CRT_SECURE_NO_WARNINGS
        -D_CRT_NONSTDC_NO_WARNINGS
        -D_AFXDLL
)

# To be clear, I'm aware CMake supports $<$<NOT:$<CONFIG:Blah>>:Blah for compilation options, but the syntax is gross
# and not fantastically clear from a glance in my opinion - hence why we've got the blocks below for the different
# targets w/ messages for clarity during generation.

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Configuring for debug compilation")

    # good fucking grief, so /MT flag seems to exclude _DEBUG
    # despite us compiling for a debug target? but it looks like
    # for /MTd it gets added automatically (by CMake?)
    # so we've got to define it here I guess...
    add_definitions(-D_DEBUG)

    # ideally the solution here is to use the correct /MTd
    # or whatever, but that's been giving me issues due to
    # some other crap. we'll sort this out later.

    add_compile_options(/RTC1)
elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message("Configuring for release w/ debug compilation")

    add_compile_options(-Od /Zi /RTC1)
    add_link_options(/DEBUG /OPT:REF /OPT:ICF)
else ()
    message("Configuring for release compilation")

    add_compile_options(/O2)
endif ()

add_subdirectory(src)
