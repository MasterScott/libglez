target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/draw.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/glez.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Font.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader.cpp")

add_subdirectory(detail)