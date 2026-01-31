add_executable(unpacker unpacker.cpp)

target_link_libraries(unpacker
        Jaded-Core
)
set_target_properties(unpacker PROPERTIES FOLDER "Tools")
