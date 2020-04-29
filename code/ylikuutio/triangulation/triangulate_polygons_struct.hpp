// Ylikuutio - A 3D game and simulation engine.
//
// Copyright (C) 2015-2020 Antti Nuortimo.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef __TRIANGULATE_POLYGONS_STRUCT_HPP_INCLUDED
#define __TRIANGULATE_POLYGONS_STRUCT_HPP_INCLUDED

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <vector>   // std::vector

namespace yli::triangulation
{
    struct TriangulatePolygonsStruct
    {
        TriangulatePolygonsStruct()
            : input_vertices(nullptr), use_real_texture_coordinates(true)
        {
            // constructor.
        }
        std::vector<std::vector<glm::vec2>>* input_vertices;
        bool use_real_texture_coordinates;
    };
}

#endif
