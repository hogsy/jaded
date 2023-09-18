cmake_minimum_required(VERSION 3.23)
project(Jaded LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CMake/DetermineTargetArchitecture.cmake)
determine_target_architecture(JADE_SYSTEM_ARCH)

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
message("GIT HASH: ${GIT_COMMIT_HASH}")
add_definitions(-DGIT_COMMIT_COUNT="${GIT_COMMIT_COUNT}")
message("GIT COMMIT COUNT: ${GIT_COMMIT_COUNT}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
message("GIT BRANCH: ${GIT_BRANCH}")

set(JADE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}}/deploy/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${JADE_OUTPUT_DIR}")
foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${JADE_OUTPUT_DIR}")
endforeach ()

# Core Jade Libraries
add_subdirectory(Libraries/SDK)
