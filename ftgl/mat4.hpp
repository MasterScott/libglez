/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */

#pragma once

#include <cstring>
#include <math.h>

namespace ftgl
{

class mat4
{
public:
    mat4() = default;

    mat4(const mat4& other)
    {
        memcpy(data, other.data, sizeof(data));
    }

    inline void set_zero()
    {
        memset(data, 0, sizeof(data));
    }

    inline void set_identity()
    {
        set_zero();
        m00 = 1.0;
        m11 = 1.0;
        m22 = 1.0;
        m33 = 1.0;
    }

    inline void multiply(const mat4& other)
    {
        mat4 m{};

        for (auto i = 0; i < 4; ++i)
        {
            m.data[i * 4 + 0] = (data[i * 4 + 0] * other.data[0 * 4 + 0]) +
                                (data[i * 4 + 1] * other.data[1 * 4 + 0]) +
                                (data[i * 4 + 2] * other.data[2 * 4 + 0]) +
                                (data[i * 4 + 3] * other.data[3 * 4 + 0]);

            m.data[i * 4 + 1] = (data[i * 4 + 0] * other.data[0 * 4 + 1]) +
                                (data[i * 4 + 1] * other.data[1 * 4 + 1]) +
                                (data[i * 4 + 2] * other.data[2 * 4 + 1]) +
                                (data[i * 4 + 3] * other.data[3 * 4 + 1]);

            m.data[i * 4 + 2] = (data[i * 4 + 0] * other.data[0 * 4 + 2]) +
                                (data[i * 4 + 1] * other.data[1 * 4 + 2]) +
                                (data[i * 4 + 2] * other.data[2 * 4 + 2]) +
                                (data[i * 4 + 3] * other.data[3 * 4 + 2]);

            m.data[i * 4 + 3] = (data[i * 4 + 0] * other.data[0 * 4 + 3]) +
                                (data[i * 4 + 1] * other.data[1 * 4 + 3]) +
                                (data[i * 4 + 2] * other.data[2 * 4 + 3]) +
                                (data[i * 4 + 3] * other.data[3 * 4 + 3]);
        }

        *this = m;
    }

    inline void set_orthographic(float left, float right, float bottom, float top, float znear, float zfar)
    {
        if (left == right || bottom == top || znear == zfar)
            return;

        set_zero();

        m00 = +2.0f / (right - left);
        m30 = -(right + left) / (right - left);
        m11 = +2.0f / (top - bottom);
        m31 = -(top + bottom) / (top - bottom);
        m22 = -2.0f / (zfar - znear);
        m32 = -(zfar + znear) / (zfar - znear);
        m33 = 1.0f;
    }

    inline void set_perspective(float fovy, float aspect, float znear, float zfar)
    {
        if (znear == zfar)
            return;

        float h, w;

        h = (float) tan(fovy / 360.0 * M_PI) * znear;
        w = h * aspect;

        set_frustum(-w, w, -h, h, znear, zfar);
    }

    inline void set_frustum(float left, float right, float bottom, float top, float znear, float zfar)
    {
        if (left == right || bottom == top || znear == zfar)
            return;

        set_zero();

        m00 = (2.0f * znear) / (right - left);
        m20 = (right + left) / (right - left);

        m11 = (2.0f * znear) / (top - bottom);
        m21 = (top + bottom) / (top - bottom);

        m22 = -(zfar + znear) / (zfar - znear);
        m32 = -(2.0f * zfar * znear) / (zfar - znear);

        m23 = -1.0f;
    }

    inline void set_rotation(float angle, float x, float y, float z)
    {
        float c, s, norm;

        c = (float) cos(M_PI * angle / 180.0);
        s = (float) sin(M_PI * angle / 180.0);
        norm = (float) sqrt(x * x + y * y + z * z);

        x /= norm;
        y /= norm;
        z /= norm;

        set_identity();

        m00 = x * x * (1 - c) + c;
        m10 = y * x * (1 - c) - z * s;
        m20 = z * x * (1 - c) + y * s;

        m01 = x * y * (1 - c) + z * s;
        m11 = y * y * (1 - c) + c;
        m21 = z * y * (1 - c) - x * s;

        m02 = x * z * (1 - c) - y * s;
        m12 = y * z * (1 - c) + x * s;
        m22 = z * z * (1 - c) + c;
    }

    inline void set_translation(float x, float y, float z)
    {
        set_identity();
        m30 = x;
        m31 = y;
        m32 = z;
    }

    inline void set_scaling(float x, float y, float z)
    {
        set_identity();
        m00 = x;
        m11 = y;
        m22 = z;
    }

    inline void rotate(float angle, float x, float y, float z)
    {
        mat4 m{};
        m.set_rotation(angle, x, y, z);
        multiply(m);
    }

    inline void translate(float x, float y, float z)
    {
        mat4 m{};
        m.set_translation(x, y, z);
        multiply(m);
    }

    inline void scale(float x, float y, float z)
    {
        mat4 m{};
        m.set_scaling(x, y, z);
        multiply(m);
    }

    union
    {
        float data[16]; /**< All compoments at once     */
        struct
        {
            float m00, m01, m02, m03;
            float m10, m11, m12, m13;
            float m20, m21, m22, m23;
            float m30, m31, m32, m33;
        };
    };
};

}