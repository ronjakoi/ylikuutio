#ifndef __INDEXING_HPP_INCLUDED
#define __INDEXING_HPP_INCLUDED

#include "triangulation_enums.hpp"
#include "code/ylikuutio/common/globals.hpp"

// Include standard headers
#include <stdint.h> // uint32_t etc.
#include <vector>   // std::vector

namespace geometry
{
    GLuint southwest(uint32_t current_vertex_i, uint32_t image_width);
    GLuint southeast(uint32_t current_vertex_i, uint32_t image_width);
    GLuint northwest(uint32_t current_vertex_i, uint32_t image_width);
    GLuint northeast(uint32_t current_vertex_i, uint32_t image_width);

    int32_t get_face_normal_i(
            uint32_t x,
            uint32_t z,
            BilinearDirections compass_point_code,
            uint32_t actual_image_width);
}

#endif
