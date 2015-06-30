#ifndef __GLOBALS_HPP_INCLUDED
#define __GLOBALS_HPP_INCLUDED

#include <string>
#include <vector>

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

#ifndef ASPECT_RATIO
#define ASPECT_RATIO (4/3)
#endif

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 1600
#endif

#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT (WINDOW_WIDTH / ASPECT_RATIO)
#endif

#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef EARTH_RADIUS
#define EARTH_RADIUS 6371000.0f
#endif

#ifndef TEXT_SIZE
#define TEXT_SIZE 40
#endif

#ifndef FONT_SIZE
#define FONT_SIZE 16
#endif

#ifndef MAX_FPS
#define MAX_FPS 60
#endif

extern glm::vec3 position;
extern glm::vec3 spherical_position;
extern GLfloat horizontalAngle;
extern GLfloat verticalAngle;
extern GLfloat initialFoV;
extern GLfloat earth_radius;
extern bool hasMouseEverMoved;
extern bool is_invert_mouse_in_use;
extern bool is_key_I_released;
extern bool is_world_loaded; // no more than one world object can be loaded. TODO: check that no more than one world is loaded!
extern bool is_world_spherical;

typedef struct
{
    std::string texture_file_format;
    std::string image_path;
} TextureStruct;

typedef struct
{
    void *world_pointer;                     // pointer to the world (draw list).
    std::string vertex_shader;               // filename of vertex shader.
    std::string fragment_shader;             // filename of fragment shader.
} ShaderStruct;

typedef struct
{
    uint32_t *node_data;
} GraphStruct;

typedef struct
{
    GLuint nodeID;
    void* graph_pointer;
    glm::vec3 coordinate_vector;
    std::vector<uint32_t> neighbor_nodeIDs;
} NodeStruct;

typedef struct
{
    glm::vec3 coordinate_vector;     // coordinate vector.
    GLfloat rotate_angle;            // rotate angle.
    glm::vec3 rotate_vector;         // rotate vector.
    glm::vec3 translate_vector;      // translate vector.
    glm::mat4 model_matrix;          // model matrix.
    glm::mat4 MVP_matrix;            // model view projection matrix.
    void *species_pointer;           // pointer to the species.
} ObjectStruct;

typedef struct
{
    // used for all files (for all species).
    void *shader_pointer;                    // pointer to the shader object.
    std::string model_file_format;           // type of the model file. supported file formats so far: `"bmp"`/`"BMP"`, `"obj"`/`"OBJ"`.
                                             // TODO: add support for `"SRTM"`.
    std::string texture_file_format;         // type of the texture file. supported file formats so far: `"bmp"`/`"BMP"`, `"dds"`/`"DDS"`.
    std::string texture_filename;            // filename of the model file.
    std::string vertex_shader;               // filename of vertex shader.
    std::string fragment_shader;             // filename of fragment shader.
    glm::vec3 lightPos;                      // light position.
    std::vector<ObjectStruct> object_vector; // vector of individual objects of this species.
    bool is_world;                           // worlds currently do not rotate nor translate.
    std::string coordinate_system;           // used only for worlds (`is_world` == `true`). valid values: `"cartesian"`.
                                             // TODO: add support for `"spherical"`. `"spherical"` is used eg. in SRTM heightmaps.
    GLfloat world_radius;                    // radius of sea level in meters. used only for worlds.

    // for `"bmp"` model files.
    std::string model_filename;              // filename of the model file.
    std::string color_channel;               // color channel to use for altitude data.
} SpeciesStruct;

typedef struct
{
    GLuint screen_width;
    GLuint screen_height;
    GLuint x;
    GLuint y;
    GLuint text_size;
    GLuint font_size;
    const char *text;
    const char *char_font_texture_file_format;
    const char *horizontal_alignment;
    const char *vertical_alignment;
} PrintingStruct;

typedef struct
{
    double rho;
    double theta;
    double phi;
} SphericalCoordinatesStruct;

#endif
