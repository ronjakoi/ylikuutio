#ifndef __MODEL_HPP_INCLUDED
#define __MODEL_HPP_INCLUDED

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h>
#endif

// Include GLFW
#ifndef __GLFW3_H_INCLUDED
#define __GLFW3_H_INCLUDED
#include <glfw3.h>
#endif

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp>
#endif

#include <iostream>
#include <string.h>
#include "globals.hpp"
#include "shader.hpp"
#include "bmploader.hpp"
#include "objloader.hpp"

namespace model
{
    class Species
    {
        public:
            // constructor.
            Species(SpeciesStruct species_struct);

            // First, render the species.
            // Then, render each individual object of this species.
            void render();

            std::string model_file_format;           // type of the model file, eg. `"bmp"`.
            std::string model_filename;              // filename of the model file.
            std::string texture_file_format;         // type of the model file, eg. `"bmp"`.
            std::string texture_filename;            // filename of the model file.
            std::string color_channel;               // filename of fragment shader.
            std::string vertex_shader;               // filename of vertex shader.
            std::string fragment_shader;             // filename of fragment shader.
            std::vector<ObjectStruct> object_vector; // vector of individual objects of this species.
            glm::vec3 lightPos;                      // light position.

            // The rest fields are created in the constructor.
            GLuint programID;                      // shaders' programID, returned by `LoadShaders`.
            GLuint lightID;                        // light ID, returned by `glGetUniformLocation(programID, "LightPosition_worldspace");`.
            GLuint texture;                        // Texture, returned by `loadDDS` or `loadBMP_custom`.
            GLuint textureID;                      // texture ID, returned by `glGetUniformLocation(programID, "myTextureSampler");`.
            GLuint MatrixID;
            GLuint ViewMatrixID;
            GLuint ModelMatrixID;
            GLuint vertexPosition_modelspaceID;
            GLuint vertexUVID;
            GLuint vertexNormal_modelspaceID;

            std::vector<glm::vec3> vertices;       // vertices of the object.
            std::vector<glm::vec2> UVs;            // UVs of the object.
            std::vector<glm::vec3> normals;        // normals of the object. not used at the moment.

            std::vector<GLuint> indices;
            std::vector<glm::vec3> indexed_vertices;
            std::vector<glm::vec2> indexed_UVs;
            std::vector<glm::vec3> indexed_normals;

            GLuint vertexbuffer;
            GLuint uvbuffer;
            GLuint normalbuffer;
            GLuint elementbuffer;

            glm::mat4 ProjectionMatrix;
            glm::mat4 ViewMatrix;

        private:
            const char *char_model_file_format;
            const char *char_model_filename;
            const char *char_texture_file_format;
            const char *char_texture_filename;
            const char *char_color_channel;
            const char *char_vertex_shader;
            const char *char_fragment_shader;
    };

    class Object
    {
        public:
            // constructor.
            Object(ObjectStruct object_struct);

            // First, render the species.
            // Then, render each individual object.
            void render();

            glm::mat4 model_matrix;                // model matrix.
            glm::vec3 translate_vector;            // translate vector.
            model::Species *species_ptr;           // pointer to the species.

            model::Species *shader_species_ptr;    // pointer to the shader species (not yet in use!).
            model::Species *vertex_UV_species_ptr; // pointer to the vertex & UV species (not yet in use!).
            model::Species *texture_species_ptr;   // pointer to the texture species (not yet in use!).

            // The rest fields are created in the constructor.
            std::vector<glm::vec3> vertices;       // vertices of the object.
            std::vector<glm::vec2> UVs;            // UVs of the object.
            std::vector<glm::vec3> normals;        // normals of the object. not used at the moment.

            glm::mat4 MVP_matrix;                  // model view projection matrix.
    };
}

#endif