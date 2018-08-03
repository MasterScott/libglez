target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/mat4.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/opengl.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/vec234.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertex-attribute.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertex-buffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/vertex-buffer.impl.hpp")

target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/vertex-attribute.cpp")

target_include_directories(glez PRIVATE "${CMAKE_CURRENT_LIST_DIR}")