#include "glyph.hpp"
#include "species_or_glyph.hpp"
#include "object.hpp"
#include "render_templates.hpp"
#include "cpp/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <stdint.h> // uint32_t etc.
#include <iostream> // std::cout, std::cin, std::cerr

namespace model
{
    void Glyph::bind_to_parent()
    {
        hierarchy::bind_child_to_parent<model::Glyph*>(this, this->parent_pointer->glyph_pointer_vector, this->parent_pointer->free_glyphID_queue);
    }

    Glyph::Glyph(GlyphStruct glyph_struct)
    {
        // constructor.
        this->parent_pointer = glyph_struct.parent_pointer;

        this->glyph_vertex_data = glyph_struct.glyph_vertex_data;
        this->glyph_name_pointer = glyph_struct.glyph_name_pointer;
        this->unicode_string_pointer = glyph_struct.unicode_string_pointer;
        this->light_position = glyph_struct.light_position;

        // get `childID` from `VectorFont` and set pointer to this `Glyph`.
        this->bind_to_parent();

        // Get a handle for our buffers.
        this->vertexPosition_modelspaceID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexPosition_modelspace");
        this->vertexUVID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexUV");
        this->vertexNormal_modelspaceID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexNormal_modelspace");

        // TODO: triangulate the vertex data!

        // TODO: load the vertex data the same way as in `model::Species::Species(SpeciesStruct species_struct)`!
    }

    Glyph::~Glyph()
    {
        // destructor.
        std::cout << "This glyph (\"" << *this->glyph_name_pointer << "\", Unicode: \"" << *this->unicode_string_pointer << "\") will be destroyed.\n";

        // TODO: Cleanup VBO, shader and texture (copy these from `Species::~Species()`).

        // set pointer to this `Glyph` to nullptr.
        this->parent_pointer->set_glyph_pointer(this->childID, nullptr);
    }

    void Glyph::render()
    {
        model::render_species_or_glyph<model::Glyph*>(this);
    }

    void Glyph::set_object_pointer(uint32_t childID, void* parent_pointer)
    {
        hierarchy::set_child_pointer(childID, parent_pointer, this->object_pointer_vector, this->free_objectID_queue);
    }
}
