#include "font.hpp"
#include "cpp/hierarchy/hierarchy.hpp"
#include "render_templates.hpp"
#include "cpp/hierarchy/hierarchy_templates.hpp"
#include "font_loader.hpp"
#include "glyph.hpp"

// Include standard headers
#include <cstring>  // strcmp

namespace model
{
    void Font::bind_to_parent()
    {
        model::bind_child_to_parent<model::Font*>(this, this->parent_pointer->font_pointer_vector, this->parent_pointer->free_fontID_queue);
    }

    Font::Font(FontStruct font_struct)
    {
        // constructor.
        this->font_file_format      = font_struct.font_file_format;
        this->font_filename         = font_struct.font_filename;
        this->vertex_scaling_factor = font_struct.vertex_scaling_factor;
        this->parent_pointer        = font_struct.parent_pointer;

        this->char_font_file_format = this->font_file_format.c_str();
        this->char_font_filename    = this->font_filename.c_str();

        // get childID from the Graph and set pointer to this font.
        this->bind_to_parent();

        bool font_loading_result;

        if ((strcmp(this->char_font_file_format, "svg") == 0) || (strcmp(this->char_font_file_format, "SVG") == 0))
        {
            // TODO: implement `load_SVG_font`! It should read all necessary data to this Font object.
            font_loading_result = model::load_SVG_font(
                    this->font_filename,
                    this->glyph_vertex_data,
                    this->glyph_UV_data,
                    this->glyph_normal_data,
                    this->glyph_names,
                    this->unicode_strings,
                    this->vertex_scaling_factor);
        }

        // Font constructor also creates each Glyph and binds them to the Font.
        // TODO: implement creation of each Glyph!
    }

    Font::~Font()
    {
        // destructor.
        std::cout << "This font will be destroyed.\n";

        // destroy all glyphs of this font.
        std::cout << "All glyphs of this font will be destroyed.\n";
        model::delete_children<model::Glyph*>(this->glyph_pointer_vector);
    }

    void Font::render()
    {
        // render Font by calling `render()` function of each Glyph.
        model::render_children<model::Glyph*>(this->glyph_pointer_vector);
    }

    void Font::set_glyph_pointer(uint32_t childID, void* parent_pointer)
    {
        set_child_pointer(childID, parent_pointer, this->glyph_pointer_vector, this->free_glyphID_queue);
    }

    void Font::bind_to_new_parent(model::Material *new_texture_pointer)
    {
        model::bind_child_to_new_parent<model::Font*, model::Material*>(this, new_texture_pointer, this->parent_pointer->font_pointer_vector, this->parent_pointer->free_fontID_queue);
    }
}