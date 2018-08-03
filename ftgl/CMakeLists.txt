target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/font-manager.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/freetype-gl.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/markup.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/text-buffer.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture-atlas.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture-font.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/utf8-utils.hpp")
    
target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/font-manager.cpp"
#        "${CMAKE_CURRENT_LIST_DIR}/makefont.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/text-buffer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture-atlas.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/texture-font.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/utf8-utils.cpp")