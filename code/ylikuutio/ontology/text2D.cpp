#include "text2D.hpp"
#include "font2D.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <cstddef>  // std::size_t
#include <iostream> // std::cout, std::cin, std::cerr
#include <string>   // std::string
#include <vector>   // std::vector

namespace yli
{
    namespace ontology
    {
        void Text2D::bind_to_parent()
        {
            // requirements:
            // `this->parent` must not be `nullptr`.
            yli::ontology::Font2D* const font2D = this->parent;

            if (font2D == nullptr)
            {
                std::cerr << "ERROR: `Text2D::bind_to_parent`: `font2D` is `nullptr`!\n";
                return;
            }

            // get `childID` from the `Font2D` and set pointer to this `Text2D`.
            font2D->bind_text2D(this);
        }

        void Text2D::bind_to_new_parent(yli::ontology::Font2D* const new_parent)
        {
            // this method sets pointer to this `Text2D` to `nullptr`, sets `parent` according to the input,
            // and requests a new `childID` from the new `Font2D`.
            //
            // requirements:
            // `this->parent` must not be `nullptr`.
            // `new_parent` must not be `nullptr`.

            yli::ontology::Font2D* const font2D = this->parent;

            if (font2D == nullptr)
            {
                std::cerr << "ERROR: `Text2D::bind_to_new_parent`: `font2D` is `nullptr`!\n";
                return;
            }

            if (new_parent == nullptr)
            {
                std::cerr << "ERROR: `Text2D::bind_to_new_parent`: `new_parent` is `nullptr`!\n";
                return;
            }

            // unbind from the old parent `Font2D`.
            this->parent->unbind_text2D(this->childID);

            // get `childID` from `Font2D` and set pointer to this `Text2D`.
            this->parent = new_parent;
            this->parent->bind_text2D(this);
        }

        Text2D::~Text2D()
        {
            // destructor.
            std::cout << "This text will be destroyed.\n";

            // Delete buffers
            glDeleteBuffers(1, &this->vertexbuffer);
            glDeleteBuffers(1, &this->uvbuffer);

            // Delete shader
            glDeleteProgram(this->programID);
        }

        void Text2D::render()
        {
            // If horizontal alignment is `"left"`, each line begins from the same x coordinate.
            // If horizontal alignment is `"left"` and vertical alignment is `"top"`,
            // then there is no need to check the text beforehand for newlines.
            // Otherwise newlines need to be checked beforehand.
            //
            // If horizontal alignment is right, each line ends in the same x coordinate.
            // Newlines need to be checked beforehand.
            const std::size_t length = this->text.size();

            // Count the number of lines.
            std::size_t number_of_lines = 1;

            std::size_t i = 0;

            while (i < length)
            {
                char character = text[i++];

                if (i >= length)
                {
                    // Backslash `\` is the last character of string.
                    // End processing to avoid buffer over-read.
                    break;
                }

                if (character == (char) '\\')
                {
                    // OK, this character was backslash, so read the next character.
                    character = text[i++];

                    if (character == 'n')
                    {
                        number_of_lines++;
                    }
                }
            }

            std::size_t current_left_x;
            std::size_t current_top_y;

            if (this->horizontal_alignment == "left")
            {
                current_left_x = this->x;
            }
            else if (this->horizontal_alignment == "center")
            {
                current_left_x = this->x - 0.5f * length * this->text_size;
            }
            else if (this->horizontal_alignment == "right")
            {
                current_left_x = this->x - length * this->text_size;
            }
            else
            {
                std::cerr << "Invalid horizontal alignment: " << horizontal_alignment << "\n";
                return;
            }

            if (this->vertical_alignment == "top")
            {
                current_top_y = this->y;
            }
            else if (this->vertical_alignment == "center")
            {
                current_top_y = this->y + 0.5f * number_of_lines * this->text_size;
            }
            else if (this->vertical_alignment == "bottom")
            {
                current_top_y = this->y + number_of_lines * this->text_size;
            }
            else
            {
                std::cerr << "Invalid vertical alignment: " << this->vertical_alignment << "\n";
                return;
            }

            // Fill buffers
            std::vector<glm::vec2> vertices;
            std::vector<glm::vec2> UVs;

            i = 0;

            while (i < length)
            {
                // Print to the right side of X (so far there is no check for input length).
                // Print up of Y.
                std::size_t vertex_up_left_x;
                std::size_t vertex_up_left_y;
                std::size_t vertex_up_right_x;
                std::size_t vertex_up_right_y;
                std::size_t vertex_down_left_x;
                std::size_t vertex_down_left_y;
                std::size_t vertex_down_right_x;
                std::size_t vertex_down_right_y;

                char character = text[i++];

                if (character == (char) '\\')
                {
                    // OK, this character was backslash, so read the next character.
                    character = text[i++];

                    if (i >= length)
                    {
                        // Backslash `\` is the last character of string.
                        // End processing to avoid buffer over-read.
                        break;
                    }

                    if (character == 'n')
                    {
                        // jump to the beginning of the next line.
                        // `"left"` horizontal alignment and `"top"` vertical alignment are assumed.
                        // TODO: implement newline for other horizontal and vertical alignments too!
                        current_left_x = this->x;
                        current_top_y -= this->text_size;
                        continue;
                    }
                }

                vertex_up_left_x = vertex_down_left_x = current_left_x;
                vertex_up_right_x = vertex_down_right_x = current_left_x + this->text_size;
                current_left_x += this->text_size;

                vertex_down_left_y = vertex_down_right_y = current_top_y - this->text_size;
                vertex_up_left_y = vertex_up_right_y = current_top_y;

                glm::vec2 vertex_up_left = glm::vec2(vertex_up_left_x, vertex_up_left_y);
                glm::vec2 vertex_up_right = glm::vec2(vertex_up_right_x, vertex_up_right_y);
                glm::vec2 vertex_down_left = glm::vec2(vertex_down_left_x, vertex_down_left_y);
                glm::vec2 vertex_down_right = glm::vec2(vertex_down_right_x, vertex_down_right_y);

                vertices.push_back(vertex_up_left);
                vertices.push_back(vertex_down_left);
                vertices.push_back(vertex_up_right);

                vertices.push_back(vertex_down_right);
                vertices.push_back(vertex_up_right);
                vertices.push_back(vertex_down_left);

                float uv_x = (character % this->font_size) / static_cast<float>(this->font_size);
                float uv_y;

                const std::string& font_texture_file_format = this->parent->get_font_texture_file_format();

                if (font_texture_file_format == "dds" || font_texture_file_format == "DDS")
                {
                    uv_y = (character / this->font_size) / static_cast<float>(this->font_size);
                }
                else if (font_texture_file_format == "bmp" || font_texture_file_format == "BMP")
                {
                    // BMP is stored in the file beginning from the bottom line.
                    uv_y = 1 - (character / this->font_size) / static_cast<float>(this->font_size);
                }
                else
                {
                    std::cerr << "invalid font_texture_file_format " << std::string(font_texture_file_format) << "\n";
                    return;
                }

                glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
                glm::vec2 uv_up_right = glm::vec2(uv_x + (1.0f / static_cast<float>(this->font_size)), uv_y);
                glm::vec2 uv_down_right;
                glm::vec2 uv_down_left;

                if (font_texture_file_format == "dds" || font_texture_file_format == "DDS")
                {
                    uv_down_right = glm::vec2(uv_x + (1.0f / static_cast<float>(this->font_size)), (uv_y + 1.0f / static_cast<float>(this->font_size)));
                    uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / static_cast<float>(this->font_size)));
                }
                else if (font_texture_file_format == "bmp" || font_texture_file_format == "BMP")
                {
                    // BMP is stored in the file beginning from the bottom line.
                    uv_down_right = glm::vec2(uv_x + (1.0f / static_cast<float>(this->font_size)), (uv_y - 1.0f / static_cast<float>(this->font_size)));
                    uv_down_left = glm::vec2(uv_x, (uv_y - 1.0f / static_cast<float>(this->font_size)));
                }
                UVs.push_back(uv_up_left);
                UVs.push_back(uv_down_left);
                UVs.push_back(uv_up_right);

                UVs.push_back(uv_down_right);
                UVs.push_back(uv_up_right);
                UVs.push_back(uv_down_left);
            }

            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

            // 1st attribute buffer : vertices
            glEnableVertexAttribArray(this->vertex_position_in_screenspaceID);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glVertexAttribPointer(this->vertex_position_in_screenspaceID, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            // 2nd attribute buffer : UVs
            glEnableVertexAttribArray(this->vertexUVID);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glVertexAttribPointer(this->vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            // 1st attribute buffer: vertices
            glEnableVertexAttribArray(this->vertex_position_in_screenspaceID);
            glBindBuffer(GL_ARRAY_BUFFER, this->vertexbuffer);
            glVertexAttribPointer(this->vertex_position_in_screenspaceID, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            // 2nd attribute buffer: UVs
            glEnableVertexAttribArray(this->vertexUVID);
            glBindBuffer(GL_ARRAY_BUFFER, this->uvbuffer);
            glVertexAttribPointer(this->vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);

            // Draw call
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            glDisableVertexAttribArray(this->vertex_position_in_screenspaceID);
            glDisableVertexAttribArray(this->vertexUVID);
        }

        yli::ontology::Entity* Text2D::get_parent() const
        {
            return this->parent;
        }

        std::size_t Text2D::get_number_of_children() const
        {
            return 0; // `Text2D` has no children.
        }

        std::size_t Text2D::get_number_of_descendants() const
        {
            return 0; // `Text2D` has no children.
        }

        void Text2D::change_string(const std::string& text)
        {
            this->text = text;
        }

    }
}
