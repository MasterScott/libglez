/*
  Created on 03.08.18.
*/


#include <glez/Font.hpp>
#include <glez/detail/freetype-gl/font-manager.hpp>

static std::unique_ptr<ftgl::FontManager> manager{};

static bool isManagerReady{ false };

static void initManager()
{
    manager = std::make_unique<ftgl::FontManager>(1024, 1024, 1);
    isManagerReady = true;
}

glez::Font::Font(std::string filename, float size): size(size)
{
    loader = [filename = std::move(filename), size]() -> ftgl::TextureFont * {
        return manager->get_from_filename(filename, size);
    };
}

glez::Font::Font(ftgl::Markup markup, float size): size(size)
{
    loader = [markup = std::move(markup), size]() -> ftgl::TextureFont * {
        return manager->get_from_markup(markup);
    };
}

glez::Font::Font(std::string family, float size, bool bold, bool italic): size(size)
{
    loader = [family = std::move(family), size, bold, italic]() -> ftgl::TextureFont * {
        return manager->get_from_description(family, size, bold, italic);
    };
}

ftgl::vec2 glez::Font::getStringSize(const std::string &string)
{
    ftgl::vec2 result{};
    return result;
}

void glez::Font::prepare()
{

}

bool glez::Font::load()
{
    font = loader();
    return font != nullptr;
}

