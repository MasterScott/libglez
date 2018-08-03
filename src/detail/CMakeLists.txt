target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/CommandBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/DrawCommand.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/VertexDoubleBuffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/program.cpp"
        )

add_subdirectory(freetype-gl)