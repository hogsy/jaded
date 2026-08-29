
if (MSVC)
    add_compile_options(
            /wd4201 # nameless struct/union
            /wd4100 # unreferenced formal parameter
            /wd4115 # named type definition in parentheses
    )
endif ()

# sigh...
include_directories(.)
include_directories(dlls)

###################################################################################
# External Libraries

add_subdirectory(dlls/JPEGLIB)
add_subdirectory(dlls/MPEGLIB)

add_subdirectory(extern/ode)
add_subdirectory(extern/lz4/build/cmake)
add_subdirectory(extern/zlib)

###################################################################################
# Jaded

add_subdirectory(jaded/core)

###################################################################################
# Tools

add_subdirectory(tools/unpacker)

###################################################################################
# Libraries

add_subdirectory(dlls/MAD_loadSave)
add_subdirectory(dlls/MAD_mem)
add_subdirectory(dlls/MAD_rasterize)

add_subdirectory(libraries)

###################################################################################
# Apps

add_subdirectory(main)          # Main Program
