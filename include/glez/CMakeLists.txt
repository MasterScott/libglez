target_sources(glez PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/Texture.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Sprite.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/Font.hpp"

        "${CMAKE_CURRENT_LIST_DIR}/color.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/glez.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/shader.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/draw.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/record.hpp"
        )

add_subdirectory(detail)