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

#include "shader_compare.hpp"
#include "shader.hpp"

namespace yli
{
    namespace ontology
    {
        bool ShaderCompare::operator() (yli::ontology::Shader* first, yli::ontology::Shader* second)
        {
            if (first->is_gpgpu_shader)
            {
                if (!second->is_gpgpu_shader)
                {
                    return false;
                }

                if (first->get_childID() < second->get_childID())
                {
                    return true;
                }

                return false;
            }

            if (second->is_gpgpu_shader)
            {
                return false;
            }

            if (first->get_childID() < second->get_childID())
            {
                return true;
            }

            return false;
        }
    }
}
