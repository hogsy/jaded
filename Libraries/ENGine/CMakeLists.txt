add_library(Jaded-Engine STATIC
        Sources/ENGcall.c
        Sources/ENGinit.c
        Sources/ENGloop.c
        Sources/ENGmsg.c
        Sources/ENGvars.c)

target_include_directories(Jaded-Engine PUBLIC Sources)
target_link_libraries(Jaded-Engine
        Jaded-SDK)
