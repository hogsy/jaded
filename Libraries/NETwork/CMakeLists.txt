add_library(Jaded-Net OBJECT
    Sources/NET.c
    Sources/NET.h
    Sources/NET_defs.c
    Sources/NET_defs.h
    Sources/NET_io.c
    Sources/NET_io.h
    Sources/NET_struct.h
    Sources/Precomp.c
    Sources/Precomp.h
)

target_include_directories(Jaded-Net
        PUBLIC
        Sources

        PRIVATE
        #TODO: clean this up
        ../../Extern/ode/include
        #../../Extern/sdl3/include
        ../../Main/WinEditors/Sources
        ../../Dlls
)

target_link_libraries(Jaded-Net Jaded-SDK Jaded-Engine)
set_target_properties(Jaded-Net PROPERTIES FOLDER "Libraries")
