/*
  Created on 03.08.18.
*/

#pragma once

#include <string>
#include <GL/glew.h>

namespace glez
{

class Texture
{
public:
    explicit Texture(std::string filename);

    ~Texture();

    void load();

    void upload();

    void bind();

    GLuint getTexture() const;

    int getWidth() const;

    int getHeight() const;

    bool isUploaded() const;

protected:
    std::string filename;
    GLuint id{ 0 };
    unsigned char *data{ nullptr };
    bool loaded{ false };
    bool uploaded{ false };
    int width{ 0 };
    int height{ 0 };
};

}