#ifndef __GLOBALS_HPP_INCLUDED
#define __GLOBALS_HPP_INCLUDED

#include "code/ylikuutio/callback_system/key_and_callback_struct.hpp"
#include "any_value.hpp"

// GCC (at least g++ 4.7.2) and Visual Studio 2015 do support
// setting default values of a struct using C++11 syntax.
// Clang 3.7.0 and Visual Studio 2013 do not support
// setting default values of a struct using C++11 syntax.
// Visual Studio 2013 fails to compile, whereas Clang-compiled
// executable with code with setting default values of a struct
// causes Segmentation fault upon execution of the program.
// Compilers that don't support setting default values of a struct
// are handled by setting the default values in a macro.
// http://stackoverflow.com/questions/16782103/initializing-default-values-in-a-struct/16783513#16783513
#ifdef __clang__
#elif defined(__GNUC__)
#define __STRUCT_DEFAULT_VALUES_ARE_ACCEPTED
#elif defined(_WIN32)
#if (_MSC_VER >= 1900)
#define __STRUCT_DEFAULT_VALUES_ARE_ACCEPTED
#endif
#endif

// GLEW must be included here, because `globals.hpp` may be compiled
// first, and if `GL/glew.h` is not included before `glfw3.h` (?),
// then g++ prints the following error:
// `error: #error gl.h included before glew.h`
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

// Include GLFW
#ifndef __GLFW3_H_INCLUDED
#define __GLFW3_H_INCLUDED
#include <glfw3.h>
#endif

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <stdint.h> // uint32_t etc.
#include <string>   // std::string
#include <unordered_map> // std::unordered_map
#include <vector>   // std::vector

#ifndef PI
#define PI 3.14159265359f
#endif

namespace config
{
    class SettingMaster;
    class Setting;
}

namespace callback_system
{
    class CallbackEngine;
    class CallbackObject;
    class CallbackParameter;
}

namespace graph
{
    class Graph;
}

namespace ontology
{
    class Universe;
    class Font2D;
}

typedef datatypes::AnyValue* (*ConsoleCommandCallback) (
        console::Console*,
        ontology::Universe*,
        std::vector<std::string>& command_parameters);

typedef struct ConsoleStruct
{
    ConsoleStruct()
        : universe_pointer(nullptr), font2D_pointer(nullptr)
    {
        // constructor.
    }
    std::vector<KeyAndCallbackStruct>** current_keypress_callback_engine_vector_pointer_pointer;
    std::vector<KeyAndCallbackStruct>** current_keyrelease_callback_engine_vector_pointer_pointer;
    std::unordered_map<std::string, ConsoleCommandCallback>* command_callback_map_pointer;
    ontology::Universe* universe_pointer;
    ontology::Font2D* font2D_pointer;
} ConsoleStruct;

typedef datatypes::AnyValue* (*ActivateCallback) (ontology::Universe* universe, config::SettingMaster* setting_master);

typedef struct SettingStruct
{
    SettingStruct(datatypes::AnyValue* initial_value)
        : initial_value(initial_value), should_ylikuutio_call_activate_callback_now(true), setting_master_pointer(nullptr), activate_callback(nullptr)
    {
        // constructor.
    }
    std::string name;
    datatypes::AnyValue* initial_value;
    config::SettingMaster* setting_master_pointer;
    ActivateCallback activate_callback;
    bool should_ylikuutio_call_activate_callback_now;
} SettingStruct;

typedef struct
{
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t x;
    uint32_t y;
    uint32_t text_size;
    uint32_t font_size;
    std::string text;
    const char* text_char;
    const char* char_font_texture_file_format;
    const char* horizontal_alignment;
    const char* vertical_alignment;
} PrintingStruct;

typedef struct SphericalWorldStruct
{
    SphericalWorldStruct()
        : SRTM_latitude_step_in_degrees(1.0f/1200.0f), SRTM_longitude_step_in_degrees(1.0f/1200.0f)
    {
        // constructor.
    }
    double southern_latitude;
    double northern_latitude;
    double western_longitude;
    double eastern_longitude;
    double SRTM_latitude_step_in_degrees;
    double SRTM_longitude_step_in_degrees;
} SphericalWorldStruct;

typedef struct TriangulateQuadsStruct
{
    TriangulateQuadsStruct()
        : should_ylikuutio_use_real_texture_coordinates(true), x_step(1), z_step(1)
    {
        // constructor.
    }
    uint32_t image_width;
    uint32_t image_height;
    uint32_t x_step;
    uint32_t z_step;
    std::string triangulation_type;
    bool should_ylikuutio_use_real_texture_coordinates;
    double sphere_radius;
    SphericalWorldStruct spherical_world_struct;
} TriangulateQuadsStruct;

typedef struct TriangulatePolygonsStruct
{
    TriangulatePolygonsStruct()
        : should_ylikuutio_use_real_texture_coordinates(true)
    {
        // constructor.
    }
    std::vector<std::vector<glm::vec2>>* input_vertices;
    bool should_ylikuutio_use_real_texture_coordinates;
} TriangulatePolygonsStruct;

typedef struct
{
    uint32_t image_width;
    uint32_t image_height;
    bool should_ylikuutio_use_real_texture_coordinates;
} BilinearInterpolationStruct;

typedef struct
{
    uint32_t image_width;
    uint32_t image_height;
    double sphere_radius;
    bool is_bilinear_interpolation_in_use;
    SphericalWorldStruct spherical_world_struct;
} TransformationStruct;

typedef datatypes::AnyValue* (*InputParametersToAnyValueCallback) (
        callback_system::CallbackEngine*,
        callback_system::CallbackObject*,
        std::vector<callback_system::CallbackParameter*>&);

namespace console
{
    class Console;
}
typedef datatypes::AnyValue* (*InputParametersToAnyValueCallbackWithConsole) (
        callback_system::CallbackEngine*,
        callback_system::CallbackObject*,
        std::vector<callback_system::CallbackParameter*>&,
        console::Console*);

typedef datatypes::AnyValue* (*GetContentCallback) (
        callback_system::CallbackEngine*,
        callback_system::CallbackObject*,
        std::vector<callback_system::CallbackParameter*>&,
        uint32_t x_start,
        uint32_t y_start,
        uint32_t z_start,
        uint32_t x_size,
        uint32_t y_size,
        uint32_t z_size);

#endif
