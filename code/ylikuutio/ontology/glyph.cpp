#include "glyph.hpp"
#include "species_or_glyph.hpp"
#include "object.hpp"
#include "glyph_struct.hpp"
#include "render_templates.hpp"
#include "code/ylikuutio/triangulation/polygon_triangulation.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

// Include GLFW
#ifndef __GLFW3_H_INCLUDED
#define __GLFW3_H_INCLUDED
#include <glfw3.h>
#endif

// Include standard headers
#include <stdint.h> // uint32_t etc.
#include <iostream> // std::cout, std::cin, std::cerr

namespace ontology
{
    void Glyph::bind_to_parent()
    {
        // get `childID` from `VectorFont` and set pointer to this `Glyph`.
        hierarchy::bind_child_to_parent<ontology::Glyph*>(this, this->parent_pointer->glyph_pointer_vector, this->parent_pointer->free_glyphID_queue, &this->parent_pointer->number_of_glyphs);
    }

    Glyph::Glyph(const GlyphStruct glyph_struct)
    {
        // constructor.
        this->parent_pointer = glyph_struct.parent_pointer;
        this->universe_pointer = this->parent_pointer->universe_pointer;

        this->glyph_vertex_data = glyph_struct.glyph_vertex_data;
        this->glyph_name_pointer = glyph_struct.glyph_name_pointer;
        this->unicode_char_pointer = glyph_struct.unicode_char_pointer;
        this->light_position = glyph_struct.light_position;

        // get `childID` from `VectorFont` and set pointer to this `Glyph`.
        this->bind_to_parent();

        // TODO: implement triangulation of `Glyph` objects!
        TriangulatePolygonsStruct triangulate_polygons_struct;
        triangulate_polygons_struct.input_vertices = this->glyph_vertex_data;
        bool triangulating_result = geometry::triangulate_polygons(
                triangulate_polygons_struct,
                this->vertices,
                this->UVs,
                this->normals);

        if (!triangulating_result)
        {
            std::cerr << "triangulation failed!\n";
        }

        // Get a handle for our buffers.
        this->vertexPosition_modelspaceID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexPosition_modelspace");
        this->vertexUVID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexUV");
        this->vertexNormal_modelspaceID = glGetAttribLocation(this->parent_pointer->parent_pointer->parent_pointer->programID, "vertexNormal_modelspace");

        // TODO: triangulate the vertex data!

        // TODO: load the vertex data the same way as in `ontology::Species::Species(SpeciesStruct species_struct)`!
    }

    Glyph::~Glyph()
    {
        // destructor.
        std::string glyph_name_string = this->glyph_name_pointer;
        std::string unicode_string = this->unicode_char_pointer;
        std::cout << "This glyph (\"" << glyph_name_string << "\", Unicode: \"" << std::dec << unicode_string << "\") will be destroyed.\n";

        // TODO: Cleanup VBO, shader and texture (copy these from `Species::~Species()`).

        // set pointer to this `Glyph` to nullptr.
        this->parent_pointer->set_glyph_pointer(this->childID, nullptr);

        if (!this->name.empty() && this->universe_pointer != nullptr)
        {
            delete this->universe_pointer->entity_anyvalue_map[this->name];
            this->universe_pointer->entity_anyvalue_map[this->name] = nullptr;
        }
    }

    void Glyph::render()
    {
        this->prerender();

        // render this `Glyph`.
        ontology::render_species_or_glyph<ontology::Glyph*>(this);

        this->postrender();
    }

    void Glyph::set_object_pointer(const uint32_t childID, ontology::Object* const child_pointer)
    {
        hierarchy::set_child_pointer(childID, child_pointer, this->object_pointer_vector, this->free_objectID_queue, &this->number_of_objects);
    }

    void Glyph::set_name(const std::string name)
    {
        ontology::set_name(name, this);
    }
}
