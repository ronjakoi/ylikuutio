#ifndef __SRTM_HEIGHTMAP_LOADER_HPP_INCLUDED
#define __SRTM_HEIGHTMAP_LOADER_HPP_INCLUDED

#include "heightmap_loader_struct.hpp"

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <string>   // std::string
#include <vector>   // std::vector

namespace yli
{
    namespace load
    {
        bool load_SRTM_terrain(
                const HeightmapLoaderStruct& heightmap_loader_struct,
                const std::string& image_path,
                std::vector<glm::vec3>& out_vertices,
                std::vector<glm::vec2>& out_UVs,
                std::vector<glm::vec3>& out_normals,
                const std::string& triangulation_type);
    }
}

#endif
