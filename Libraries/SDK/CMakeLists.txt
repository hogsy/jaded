add_library(Jaded-SDK STATIC
        Sources/BASe/ERRors/ERR.c
        Sources/BASe/ERRors/ERRdefs.cpp
        Sources/BASe/MEMory/MEMdyn.c
        Sources/BASe/MEMory/MEMdynOpt.c
        Sources/BASe/MEMory/MEMLog.c
        Sources/BASe/MEMory/MEMLogOpt.c
        Sources/BASe/MEMory/MEMpro.c
        Sources/BASe/MEMory/MEMproOpt.c
        Sources/BASe/BAS.c
        Sources/BASe/BASarray.c
        Sources/BASe/BAScrc.c
        Sources/BASe/BASlog.c)

target_include_directories(Jaded-SDK PUBLIC Sources)
