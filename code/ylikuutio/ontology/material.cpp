#include "material.hpp"
#include "vector_font.hpp"
#include "species.hpp"
#include "render_templates.hpp"
#include "code/ylikuutio/loaders/texture_loader.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <cstring>  // std::memcmp, std::strcmp, std::strlen, std::strncmp
#include <iostream> // std::cout, std::cin, std::cerr
#include <stdint.h> // uint32_t etc.

namespace ontology
{
    void Material::bind_to_parent()
    {
        hierarchy::bind_child_to_parent<ontology::Material*>(this, this->parent_pointer->material_pointer_vector, this->parent_pointer->free_materialID_queue);
    }

    Material::Material(MaterialStruct material_struct)
    {
        // constructor.
        this->parent_pointer = material_struct.parent_pointer;

        this->texture_file_format = material_struct.texture_file_format;
        this->texture_filename    = material_struct.texture_filename;

        this->char_texture_file_format = this->texture_file_format.c_str();
        this->char_texture_filename    = this->texture_filename.c_str();

        // get `childID` from `Shader` and set pointer to this `Material`.
        this->bind_to_parent();

        // Load the texture.
        if ((std::strcmp(this->char_texture_file_format, "bmp") == 0) || (std::strcmp(this->char_texture_file_format, "BMP") == 0))
        {
            this->texture = loaders::load_BMP_texture(this->char_texture_filename);
        }
        else if ((std::strcmp(this->char_texture_file_format, "dds") == 0) || (std::strcmp(this->char_texture_file_format, "DDS") == 0))
        {
            this->texture = loaders::load_DDS_texture(this->char_texture_filename);
        }
        else
        {
            std::cerr << "no texture was loaded!\n";
            std::cerr << "texture file format: " << this->texture_file_format << "\n";
        }

        // Get a handle for our "myTextureSampler" uniform.
        this->openGL_textureID = glGetUniformLocation(this->parent_pointer->programID, "myTextureSampler");
    }

    Material::~Material()
    {
        // destructor.
        std::cout << "Material with childID " << std::dec << this->childID << " will be destroyed.\n";

        // destroy all species of this material.
        std::cout << "All species of this material will be destroyed.\n";
        hierarchy::delete_children<ontology::Species*>(this->species_pointer_vector);

        // destroy all fonts of this material.
        std::cout << "All fonts of this material will be destroyed.\n";
        hierarchy::delete_children<ontology::VectorFont*>(this->vector_font_pointer_vector);

        glDeleteTextures(1, &this->texture);

        // set pointer to this material to nullptr.
        this->parent_pointer->set_material_pointer(this->childID, nullptr);
    }

    void Material::render()
    {
        // Bind our texture in Texture Unit 0.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->texture);
        // Set our "myTextureSampler" sampler to user Texture Unit 0.
        glUniform1i(this->openGL_textureID, 0);

        // render Material by calling `render()` function of each Species and of each VectorFont.
        ontology::render_children<ontology::Species*>(this->species_pointer_vector);
        ontology::render_children<ontology::VectorFont*>(this->vector_font_pointer_vector);
    }

    void Material::set_species_pointer(uint32_t childID, ontology::Species* child_pointer)
    {
        hierarchy::set_child_pointer(childID, child_pointer, this->species_pointer_vector, this->free_speciesID_queue);
    }

    void Material::set_vector_font_pointer(uint32_t childID, ontology::VectorFont* child_pointer)
    {
        hierarchy::set_child_pointer(childID, child_pointer, this->vector_font_pointer_vector, this->free_vector_fontID_queue);
    }

    void Material::bind_to_new_parent(ontology::Shader* new_shader_pointer)
    {
        hierarchy::bind_child_to_new_parent<ontology::Material*, ontology::Shader*>(this, new_shader_pointer, this->parent_pointer->material_pointer_vector, this->parent_pointer->free_materialID_queue);
    }

    void Material::set_terrain_species_pointer(ontology::Species* terrain_species_pointer)
    {
        this->terrain_species_pointer = terrain_species_pointer;
        this->parent_pointer->set_terrain_species_pointer(this->terrain_species_pointer);
    }
}