#ifndef __BMP_HEIGHTMAP_LOADER_HPP_INCLUDED
#define __BMP_HEIGHTMAP_LOADER_HPP_INCLUDED

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <stdint.h> // uint32_t etc.
#include <string>   // std::string
#include <vector>   // std::vector

namespace loaders
{
    bool load_BMP_world(
            std::string image_path,
            std::vector<glm::vec3>& out_vertices,
            std::vector<glm::vec2>& out_UVs,
            std::vector<glm::vec3>& out_normals,
            uint32_t &image_width,
            uint32_t &image_height,
            std::string color_channel,
            std::string triangulation_type);
}

#endif