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
#include <queue> // std::queue
#include <string.h>
#include "globals.hpp"
#include "shader.hpp"
#include "heightmap_loader.hpp"
#include "objloader.hpp"

namespace model
{
    class World
    {
        public:
            // constructor.
            World();

            // destructor.
            ~World();

            // this method renders the entire world, one shader at a time.
            void render();

            // this method sets a shader pointer.
            void set_pointer(GLuint shaderID, void* shader_pointer);

            // this method gets a shader pointer.
            void* get_pointer(GLuint shaderID);

            // this method gets a shader ID and removes it from the `free_shaderID_queue` if it was popped from the queue.
            GLuint get_shaderID();

        private:
            std::vector<void*> shader_pointer_vector;
            std::queue<GLuint> free_shaderID_queue;
    };

    class Shader
    {
        public:
            // constructor.
            Shader(ShaderStruct shader_struct);

            // destructor.
            ~Shader();

            // this method renders all species using this shader.
            void render();

            // this method sets a species pointer.
            void set_pointer(GLuint speciesID, void* species_pointer);

            // this method gets a species pointer.
            void* get_pointer(GLuint speciesID);

            // this method gets a species ID and removes it from the `free_speciesID_queue` if it was popped from the queue.
            GLuint get_speciesID();

            // this method sets pointer to this shader to NULL, sets `world_pointer` according to the input, and requests a new `shaderID` from the new world.
            void switch_to_new_world(model::World *new_world_pointer);

            model::World *world_pointer;          // pointer to the world.

            GLuint shaderID;                      // shader ID, returned by `model::World->get_shaderID()`.

            std::string vertex_shader;            // filename of vertex shader.
            std::string fragment_shader;          // filename of fragment shader.

            GLuint programID;                     // shaders' programID, returned by `LoadShaders`.

        private:
            std::vector<void*> species_pointer_vector;
            std::queue<GLuint> free_speciesID_queue;

            const char *char_vertex_shader;
            const char *char_fragment_shader;
    };

    class Graph
    {
        public:
            // constructor.
            Graph();

            // destructor.
            ~Graph();

            // this method sets a node pointer.
            void set_pointer(GLuint nodeID, void* node_pointer);

            // this method gets a node pointer.
            void* get_pointer(GLuint nodeID);

            // this method gets a node ID and removes it from the `free_nodeID_queue` if it was popped from the queue.
            GLuint get_nodeID();

            GLuint *node_data;

        private:
            std::vector<void*> node_pointer_vector;
            std::queue<GLuint> free_nodeID_queue;
    };

    // `Node` is not a subclass of `Graph` because if the graph splits, node may be transferred to an another graph.
    // Transferring a node to a new graph naturally requires appropriate reindexing of `nodeID`s.
    // The graph in which a node belongs is accessible through `void* graph_pointer` (must be cast to `model::Graph*`).
    class Node
    {
        public:
            // constructor.
            Node(NodeStruct node_struct);

            // destructor.
            ~Node();

            // this method creates a bidirectional link.
            // creating of bidirectional links is not possible before all nodes are created.
            void create_bidirectional_link(GLuint nodeID);

            // this method deletes a bidirectional link.
            // deleting of links is not possible before all nodes are created.
            void delete_bidirectional_link(GLuint nodeID);

            // this method transfers this node to a new graph.
            // links will not be changed.
            // all nodes that are to be transferred must be transferred separately.
            // before transfering any node to a new graph,
            // all links to nodes that do not belong to the new graph of this node must be deleted with separate `delete_bidirectional_link` calls.
            void transfer_to_new_graph(model::Graph *new_graph_pointer);

            GLuint nodeID;
            model::Graph *graph_pointer;

            // nodes do not keep pointers to neighbor nodes, because all pointer values are not known yet before all nodes are created.
            std::vector<GLuint> neighbor_nodeIDs;

        private:
            // this method creates an unidirectional link.
            // in the constructor only unidirectional links can be created.
            void create_unidirectional_link(GLuint nodeID);

            // this method deletes an unidirectional link.
            void delete_unidirectional_link(GLuint nodeID);

            glm::vec3 coordinate_vector;
    };

    class Species
    {
        public:
            // constructor.
            Species(SpeciesStruct species_struct);

            // destructor.
            ~Species();

            // this method renders all objects of this species.
            void render();

            // this method sets a object pointer.
            void set_pointer(GLuint objectID, void* object_pointer);

            // this method gets a object pointer.
            void* get_pointer(GLuint objectID);

            // this method gets a object ID and removes it from the `free_objectID_queue` if it was popped from the queue.
            GLuint get_objectID();

            // this method sets pointer to this species to NULL, sets `shader_pointer` according to the input, and requests a new `speciesID` from the new shader.
            void switch_to_new_shader(model::Shader *new_shader_pointer);

            model::Shader *shader_pointer;           // pointer to the shader.

            std::string model_file_format;           // type of the model file, eg. `"bmp"`.
            std::string model_filename;              // filename of the model file.
            std::string texture_file_format;         // type of the model file, eg. `"bmp"`.
            std::string texture_filename;            // filename of the model file.
            std::string color_channel;               // filename of fragment shader.
            std::vector<ObjectStruct> object_vector; // vector of individual objects of this species.
            glm::vec3 lightPos;                      // light position.

            void *vertex_UV_pointer;                 // pointer to the vertex & UV species (not yet in use!).
            void *texture_pointer;                   // pointer to the texture species (not yet in use!).

            // The rest fields are created in the constructor.
            GLuint speciesID;                      // species ID, returned by `model::World->get_speciesID()`.
            GLuint lightID;                        // light ID, returned by `glGetUniformLocation(programID, "LightPosition_worldspace");`.
            GLuint texture;                        // Texture, returned by `load_DDS_texture` or `load_BMP_texture`.
            GLuint textureID;                      // texture ID, returned by `glGetUniformLocation(programID, "myTextureSampler");`.
            GLuint MatrixID;
            GLuint ViewMatrixID;
            GLuint ModelMatrixID;
            GLuint vertexPosition_modelspaceID;
            GLuint vertexUVID;
            GLuint vertexNormal_modelspaceID;

            std::vector<glm::vec3> vertices;       // vertices of the object.
            std::vector<glm::vec2> UVs;            // UVs of the object.
            std::vector<glm::vec3> normals;        // normals of the object.

            std::vector<GLuint> indices;           // the deleted vertices will be reused (though it is not required, if there's enough memory).
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

            std::vector<void*> object_pointer_vector;
            std::queue<GLuint> free_objectID_queue;
    };

    class Object
    {
        public:
            // constructor.
            Object(ObjectStruct object_struct);

            // destructor.
            ~Object();

            // this method renders this object.
            void render();

            // this method sets pointer to this object to NULL, sets `species_pointer` according to the input, and requests a new `objectID` from the new species.
            void switch_to_new_species(model::Species *new_species_pointer);

            model::Species *species_pointer;       // pointer to the species.

            GLuint objectID;                      // object ID, returned by `model::Species->get_objectID()`.

            bool has_entered;
            glm::vec3 coordinate_vector;           // rotate vector.
            GLfloat rotate_angle;                  // rotate angle.
            glm::vec3 rotate_vector;               // rotate vector.
            glm::vec3 translate_vector;            // translate vector.

            // The rest fields are created in the constructor.
            glm::mat4 model_matrix;                // model matrix.
            std::vector<glm::vec3> vertices;       // vertices of the object.
            std::vector<glm::vec2> UVs;            // UVs of the object.
            std::vector<glm::vec3> normals;        // normals of the object. not used at the moment.

            glm::mat4 MVP_matrix;                  // model view projection matrix.
    };
}

#endif
