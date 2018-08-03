/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "glez/detail/freetype-gl/vec234.hpp"
#include "glez/detail/freetype-gl/vertex-attribute.hpp"

namespace ftgl
{

VertexAttribute::VertexAttribute(GLchar *name, GLint size, GLenum type,
                                 GLboolean normalized, GLsizei stride,
                                 GLvoid *pointer)
     : size(size), type(type), normalized(normalized), stride(stride)
{
    assert(size > 0);

    name = (GLchar *) strdup(name);
    index = 0xFFFFFFFF;
    pointer = pointer;
}

std::optional<VertexAttribute> VertexAttribute::parse(char *format)
{
    GLenum type = 0;
    int size;
    GLboolean normalized = GL_FALSE;
    char ctype;
    char *name;
    char *p = strchr(format, ':');
    if (p != nullptr)
    {
        name = strndup(format, p - format);
        if (*(++p) == '\0')
        {
            fprintf(stderr, "No size specified for '%s' attribute\n", name);
            free(name);
            return std::nullopt;
        }
        size = *p - '0';

        if (*(++p) == '\0')
        {
            fprintf(stderr, "No format specified for '%s' attribute\n", name);
            free(name);
            return std::nullopt;
        }
        ctype = *p;

        if (*(++p) != '\0')
        {
            if (*p == 'n')
            {
                normalized = GL_TRUE;
            }
        }
    } else
    {
        fprintf(stderr, "Vertex attribute format not understood ('%s')\n",
                format);
        return std::nullopt;
    }

    switch (ctype)
    {
    case 'b':
        type = GL_BYTE;
        break;
    case 'B':
        type = GL_UNSIGNED_BYTE;
        break;
    case 's':
        type = GL_SHORT;
        break;
    case 'S':
        type = GL_UNSIGNED_SHORT;
        break;
    case 'i':
        type = GL_INT;
        break;
    case 'I':
        type = GL_UNSIGNED_INT;
        break;
    case 'f':
        type = GL_FLOAT;
        break;
    default:
        type = 0;
        break;
    }

    return VertexAttribute(name, size, type, normalized, 0, nullptr);
}

void VertexAttribute::enable()
{
    if (index == 0xFFFFFFFF)
    {
        GLint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &program);
        if (program == 0)
        {
            return;
        }
        auto idx = glGetAttribLocation((GLuint)program, name);
        if (idx == -1)
        {
            return;
        }
        index = (GLuint) idx;
    }

    glEnableVertexAttribArray(index);
    switch (type)
    {
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_INT:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_INT:
    case GL_BYTE:
        glVertexAttribIPointer(index, size, type, stride, pointer);
        break;
    default:
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }
}

}