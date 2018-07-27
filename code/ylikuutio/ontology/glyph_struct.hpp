#ifndef __GLYPH_STRUCT_HPP_INCLUDED
#define __GLYPH_STRUCT_HPP_INCLUDED

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <vector>   // std::vector

namespace yli
{
    namespace ontology
    {
        class Universe;
        class Shader;
        class VectorFont;
    }
}

typedef struct GlyphStruct
{
    GlyphStruct()
        : glyph_vertex_data(nullptr), glyph_name_pointer(nullptr), unicode_char_pointer(nullptr), universe(nullptr), shader_pointer(nullptr), parent(nullptr), light_position(glm::vec3(0.0f, 0.0f, 0.0f)), vram_buffer_in_use(true)
    {
        // constructor.
    }
    // used for all files (for all glyph).
    std::vector<std::vector<glm::vec2>>* glyph_vertex_data;
    const char* glyph_name_pointer;       // we need only a pointer, because glyphs are always created by the `VectorFont` constructor.
    const char* unicode_char_pointer;     // we need only a pointer, because glyphs are always created by the `VectorFont` constructor.
    yli::ontology::Universe* universe; // pointer to the `Universe`.
    yli::ontology::Shader* shader_pointer;     // pointer to the `Shader`.
    yli::ontology::VectorFont* parent; // pointer to the font object.
    glm::vec3 light_position;             // light position.
    bool vram_buffer_in_use;
} GlyphStruct;

#endif
