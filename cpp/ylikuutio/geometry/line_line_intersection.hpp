#ifndef __LINE_LINE_INTERSECTION_HPP_INCLUDED
#define __LINE_LINE_INTERSECTION_HPP_INCLUDED

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

namespace geometry
{
    glm::vec2 do_lines_intersect(glm::vec2 line_1_vertex_1, glm::vec2 line_1_vertex_2, glm::vec2 line_2_vertex_1, glm::vec2 line_2_vertex_2);
}

#endif
