add_library(Jaded-Core STATIC
        fs/fs.cpp
        fs/fs_pak.cpp
        fs/fs_project.cpp
)

target_link_libraries(Jaded-Core lz4_static)
set_target_properties(Jaded-Core PROPERTIES FOLDER "Libraries")
