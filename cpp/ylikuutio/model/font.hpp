#ifndef __FONT_HPP_INCLUDED
#define __FONT_HPP_INCLUDED

#include "cpp/ylikuutio/common/globals.hpp"
#include "render_templates.hpp"
#include "material.hpp"
#include "cpp/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <string>   // std::string
#include <vector>   // std::vector
#include <queue>    // std::queue

namespace model
{
    class Material;

    class Font
    {
        public:
            // constructor.
            Font(FontStruct font_struct);

            // destructor.
            ~Font();

            // this method sets a glyph pointer.
            void set_glyph_pointer(uint32_t childID, void* parent_pointer);

            // this method sets pointer to this species to nullptr, sets `parent_pointer` according to the input, and requests a new `childID` from the new texture.
            void bind_to_new_parent(model::Material *new_texture_pointer);

            // The rest fields are created in the constructor.
            uint32_t image_width;
            uint32_t image_height;

            model::Material* parent_pointer;       // pointer to the texture.

            friend class Glyph;
            template<class T1>
                friend void render_children(std::vector<void*> &child_pointer_vector);
            template<class T1>
                friend void hierarchy::bind_child_to_parent(T1 child_pointer, std::vector<void*> &child_pointer_vector, std::queue<uint32_t> &free_childID_queue);
            template<class T1, class T2>
                friend void hierarchy::bind_child_to_new_parent(T1 child_pointer, T2 new_parent_pointer, std::vector<void*> &old_child_pointer_vector, std::queue<uint32_t> &old_free_childID_queue);

        private:
            void bind_to_parent();

            // this method renders all objects of this species.
            void render();

            std::string font_file_format;          // type of the model file, eg. `"bmp"`.
            std::string font_filename;             // filename of the model file.
            GLfloat vertex_scaling_factor;
            uint32_t childID;                      // species ID, returned by `model::Material->get_speciesID()`.
            const char* char_font_file_format;
            const char* char_font_filename;

            std::vector<std::vector<std::vector<glm::vec2>>> glyph_vertex_data;
            std::vector<std::vector<glm::vec2>> glyph_UV_data;
            std::vector<std::vector<glm::vec2>> glyph_normal_data;
            std::vector<std::string> glyph_names;
            std::vector<std::string> unicode_strings;

            std::vector<void*> glyph_pointer_vector;
            std::queue<uint32_t> free_glyphID_queue;
    };
}

#endif
