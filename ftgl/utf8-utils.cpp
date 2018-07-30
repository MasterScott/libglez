/* Freetype GL - A C OpenGL Freetype engine
 *
 * Distributed under the OSI-approved BSD 2-Clause License.  See accompanying
 * file `LICENSE` for more details.
 */
#include <string.h>
#include "utf8-utils.hpp"

namespace ftgl
{

// ----------------------------------------------------- utf8_surrogate_len ---
size_t utf8_surrogate_len(const char *character)
{
    size_t result = 0;
    char test_char;

    if (!character)
        return 0;

    test_char = character[0];

    if ((test_char & 0x80) == 0)
        return 1;

    while (test_char & 0x80)
    {
        test_char <<= 1;
        result++;
    }

    return result;
}

// ------------------------------------------------------------ utf8_strlen ---
size_t utf8_strlen(const char *string)
{
    const char *ptr = string;
    size_t result = 0;

    while (*ptr)
    {
        ptr += utf8_surrogate_len(ptr);
        result++;
    }

    return result;
}

uint32_t utf8_to_utf32(const char *character)
{
    if (!character)
    {
        return 0xFFFFFFFF;
    }

    uint32_t result{0xFFFFFFFF};

    /*
     *  0000
     *  1000 8
     *  1100 C
     *  1110 E
     *  1111 F
     *  11111 F8
     */

    if (!(character[0] & 0x80))
    {
        return (uint32_t) character[0];
    }

    if (character[0] & 0x40)
    {
        result = ((character[0] & 0x3F) << 6) | (character[1] & 0x3F);
    }

    if ((character[0] & 0x60) == 0x60)
    {
        result = ((character[0] & 0x1F) << (6 + 6)) |
                 ((character[1] & 0x3F) << 6) | (character[2] & 0x3F);
    }

    if ((character[0] & 0x70) == 0x70)
    {
        result = ((character[0] & 0x0F) << (6 + 6 + 6)) |
                 ((character[1] & 0x3F) << (6 + 6)) |
                 ((character[2] & 0x3F) << 6) | (character[3] & 0x3F);
    }

    if ((character[0] & 0x78) == 0x78)
    {
        result = ((character[0] & 0x07) << (6 + 6 + 6 + 6)) |
                 ((character[1] & 0x3F) << (6 + 6 + 6)) |
                 ((character[2] & 0x3F) << (6 + 6)) |
                 ((character[3] & 0x3F) << 6) | (character[4] & 0x3F);
    }

    return result;
}

}