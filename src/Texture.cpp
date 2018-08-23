/*
  Created on 23.08.18.
*/

#include <glez/Texture.hpp>
#include <rgba_pixel.hpp>
#include <image.hpp>

glez::Texture::Texture(std::string filename): filename(filename)
{
}

glez::Texture::~Texture()
{
    delete[] data;
    if (id)
        glDeleteTextures(1, &id);
}

void glez::Texture::load()
{
    if (loaded)
        return;

    png::image<png::rgba_pixel> image(filename);
    width      = image.get_width();
    height     = image.get_height();
    loaded     = true;
    auto bytes = image.get_width() * image.get_height() * 4;
    data       = new GLubyte[bytes];

    for (int i = 0u; i < image.get_height(); ++i)
    {
        memcpy(data + image.get_width() * 4 * i,
               image.get_pixbuf().get_row(i).data(), image.get_width() * 4);
    }
}

void glez::Texture::upload()
{
    if (uploaded)
        return;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    uploaded = true;

}

void glez::Texture::bind()
{
    if (!loaded)
        load();
    if (!uploaded)
        upload();

    glBindTexture(GL_TEXTURE_2D, id);
}

GLuint glez::Texture::getTexture() const
{
    return id;
}

int glez::Texture::getWidth() const
{
    return width;
}

int glez::Texture::getHeight() const
{
    return height;
}

bool glez::Texture::isUploaded() const
{
    return uploaded;
}

