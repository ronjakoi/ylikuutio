#include "gl_attrib_locations.hpp"
#include "shader.hpp"
#include "glyph.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

namespace ontology
{
    void store_gl_attrib_locations(ontology::Shader* shader, ontology::Glyph* const glyph)
    {
        glyph->store_vertexPosition_modelspaceID(glGetAttribLocation(shader->get_programID(), "vertexPosition_modelspace"));
        glyph->store_vertexUVID(glGetAttribLocation(shader->get_programID(), "vertexUV"));
        glyph->store_vertexNormal_modelspaceID(glGetAttribLocation(shader->get_programID(), "vertexNormal_modelspace"));
    }
}
