#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#define DEGREES_TO_RADIANS(x) (x * PI / 180.0f)
#endif

#ifndef RADIANS_TO_DEGREES
#define RADIANS_TO_DEGREES(x) (x * 180.0f / PI)
#endif

// Include standard headers
#include <iostream>
#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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

GLFWwindow* window;

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp>
#endif

#include <glm/gtc/matrix_transform.hpp>

#include "cpp/common/shader.hpp"
#include "cpp/common/texture.hpp"

#include "cpp/common/globals.hpp"
#include "cpp/common/model.hpp"
#include "cpp/common/vboindexer.hpp"
#include "cpp/common/text2D.hpp"
#include "cpp/common/text3D.hpp"

// #define TESTING_SPHERICAL_WORLD_IN_USE

// model file format: obj/bmp/...
// std::string g_model_file_format = "bmp";
std::string g_model_file_format = "bmp";

// model filename.
// std::string g_model_filename = "cube.obj";
// std::string g_model_filename = "oma_icosphere.obj";
// std::string g_model_filename = "kirjainkuutio.obj";
// std::string g_model_filename = "noise1024x1024.bmp";
std::string g_model_filename = "noise256x256.bmp";
// std::string g_model_filename = "noise128x128.bmp";

// texture file format: bmp/...
std::string g_texture_file_format = "bmp";

// color channel to use for height data.
// std::string g_height_data_color_channel = "red";
// std::string g_height_data_color_channel = "green";
// std::string g_height_data_color_channel = "blue";
std::string g_height_data_color_channel = "mean"; // "all" is equivalent to "mean".

// texture filename.
// std::string_g_texture_filename = "kuutio-uv-numerot.bmp";
// std::string g_texture_filename = "punamusta.bmp";
std::string g_texture_filename = "GrassGreenTexture0002.bmp";

// font texture file format: bmp/...
std::string g_font_texture_file_format = "bmp";

// font texture filename.
// std::string g_font_texture_filename = "Holstein.DDS";
std::string g_font_texture_filename = "Holstein.bmp";

int main(void)
{
    // Initial position : on +Z
    // position = glm::vec3(100, 100, 100);
    // position = glm::vec3(100, 3900, 100);
    // position = glm::vec3(100.0f, 5000.0f, 100.0f);
#ifdef TESTING_SPHERICAL_WORLD_IN_USE
    // position = glm::vec3(100.0f, 5000.0f + EARTH_RADIUS, 100.0f);
    // position = glm::vec3(100.0f, 5000.0f, 100.0f);
    // position = glm::vec3(-5658223.00f, -1700825.12f, 2322764.00f);
    position = glm::vec3(-5659377.50f, -1696985.38f, 2358353.25f);
#else
    position = glm::vec3(100.0f, 100.0f, 100.0f);
#endif
    // Initial horizontal angle : toward -Z
    // horizontalAngle = 0.0f;
    horizontalAngle = 42.42f;
    // Initial vertical angle : none
    // verticalAngle = PI / 2;
    verticalAngle = 7.44f;
    // Initial Field of View
    // initialFoV = 45.0f;
    initialFoV = 60.0f;

    bool does_suzanne_species_exist = true;
    bool does_suzanne_species_have_uvmap_texture = true;
    bool has_suzanne_2_transformed_into_terrain = false;

    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context.
    window = glfwCreateWindow((GLuint) WINDOW_WIDTH, (GLuint) WINDOW_HEIGHT, "Ylikuutio", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to open GLFW window.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW.
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW.\n";
        return -1;
    }

    // Ensure we can capture the escape key being pressed below.
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(window, ((GLuint) WINDOW_WIDTH / 2), ((GLuint) WINDOW_HEIGHT / 2));

    // Dark blue background.
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one.
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera.
    glEnable(GL_CULL_FACE);

    // Create the world, store it in `my_world`.
    model::World *my_world = new model::World();

    // Create the shader, store it in 'my_shader`.
    SHADERSTRUCT(shader_struct);
    shader_struct.world_pointer = my_world;
    shader_struct.vertex_shader = "StandardShading.vertexshader";
    shader_struct.fragment_shader = "StandardShading.fragmentshader";
    model::Shader *my_shader = new model::Shader(shader_struct);

    // Create the texture, store it in 'my_grass_texture`.
    TEXTURESTRUCT(grass_texture_struct);
    grass_texture_struct.shader_pointer = my_shader;
    grass_texture_struct.texture_file_format = g_texture_file_format;
    grass_texture_struct.texture_filename = g_texture_filename;
    model::Texture *my_grass_texture = new model::Texture(grass_texture_struct);

    // Create the texture, store it in 'my_uvmap_texture`.
    TEXTURESTRUCT(uvmap_texture_struct);
    uvmap_texture_struct.shader_pointer = my_shader;
    uvmap_texture_struct.texture_file_format = "dds";
    uvmap_texture_struct.texture_filename = "uvmap.DDS";
    model::Texture *my_uvmap_texture = new model::Texture(uvmap_texture_struct);

#ifdef TESTING_SPHERICAL_WORLD_IN_USE
    // Create the species, store it in `terrain_species`.
    SPECIESSTRUCT(SRTM_terrain_species_struct);
    SRTM_terrain_species_struct.texture_pointer = my_grass_texture;
    SRTM_terrain_species_struct.model_file_format = "SRTM";
    // SRTM_terrain_species_struct.model_filename = "/media/laatikko_4TB/satelliittikuvat/srtm/version3/data/";
    SRTM_terrain_species_struct.model_filename = "./"; // for testing
    SRTM_terrain_species_struct.color_channel = g_height_data_color_channel;
    SRTM_terrain_species_struct.lightPos = glm::vec3(4, 4, 4);
    SRTM_terrain_species_struct.is_world = true;
    model::Species *terrain_species = new model::Species(SRTM_terrain_species_struct);
#else
    // Create the species, store it in `terrain_species`.
    SPECIESSTRUCT(bmp_terrain_species_struct);
    bmp_terrain_species_struct.texture_pointer = my_grass_texture;
    bmp_terrain_species_struct.model_file_format = g_model_file_format;
    bmp_terrain_species_struct.model_filename = g_model_filename;
    bmp_terrain_species_struct.color_channel = g_height_data_color_channel;
    bmp_terrain_species_struct.lightPos = glm::vec3(4, 4, 4);
    bmp_terrain_species_struct.is_world = true;
    model::Species *terrain_species = new model::Species(bmp_terrain_species_struct);
#endif

    // Create terrain1, store it in `terrain1`.
    OBJECTSTRUCT(terrain_object_struct1);
    terrain_object_struct1.species_pointer = terrain_species;
    terrain_object_struct1.coordinate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain_object_struct1.rotate_angle = 0.0f;
    terrain_object_struct1.rotate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain_object_struct1.translate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    model::Object *terrain1 = new model::Object(terrain_object_struct1);

    SPECIESSTRUCT(suzanne_species_struct);
    suzanne_species_struct.texture_pointer = my_uvmap_texture;
    suzanne_species_struct.model_file_format = "obj";
    suzanne_species_struct.model_filename = "suzanne.obj";
    suzanne_species_struct.lightPos = glm::vec3(4, 4, 4);
    model::Species *suzanne_species = new model::Species(suzanne_species_struct);

    // Create suzanne1, store it in `suzanne1`.
    OBJECTSTRUCT(suzanne_object_struct1);
    suzanne_object_struct1.species_pointer = suzanne_species;
    suzanne_object_struct1.coordinate_vector = glm::vec3(82.50f, 119.00f, 95.50f);
    suzanne_object_struct1.rotate_angle = 0.10f;
    suzanne_object_struct1.rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    suzanne_object_struct1.translate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    model::Object *suzanne1 = new model::Object(suzanne_object_struct1);

    OBJECTSTRUCT(suzanne_object_struct2);
    suzanne_object_struct2.species_pointer = suzanne_species;
    suzanne_object_struct2.coordinate_vector = glm::vec3(112.90f, 113.90f, 75.50f);
    suzanne_object_struct2.rotate_angle = 0.20f;
    suzanne_object_struct2.rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    suzanne_object_struct2.translate_vector = glm::vec3(0.0f, 1.0f, 0.0f);
    model::Object *suzanne2 = new model::Object(suzanne_object_struct2);

    OBJECTSTRUCT(suzanne_object_struct3);
    suzanne_object_struct3.species_pointer = suzanne_species;
    suzanne_object_struct3.coordinate_vector = glm::vec3(126.90f, 162.90f, 103.00f);
    suzanne_object_struct3.rotate_angle = 0.05f;
    suzanne_object_struct3.rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    suzanne_object_struct3.translate_vector = glm::vec3(0.0f, 0.0f, 1.0f);
    model::Object *suzanne3 = new model::Object(suzanne_object_struct3);

    OBJECTSTRUCT(suzanne_object_struct4);
    suzanne_object_struct4.species_pointer = suzanne_species;
    suzanne_object_struct4.coordinate_vector = glm::vec3(96.00f, 130.00f, 109.00f);
    suzanne_object_struct4.rotate_angle = 0.15f;
    suzanne_object_struct4.rotate_vector = glm::vec3(1.0f, 0.0f, 0.0f);
    suzanne_object_struct4.translate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    model::Object *suzanne4 = new model::Object(suzanne_object_struct4);

    OBJECTSTRUCT(suzanne_object_struct5);
    suzanne_object_struct5.species_pointer = suzanne_species;
    suzanne_object_struct5.original_scale_vector = glm::vec3(10.0f, 10.0f, 10.0f);
    suzanne_object_struct5.coordinate_vector = glm::vec3(103.00f, 105.00f, 109.00f);
    suzanne_object_struct5.rotate_angle = 0.03f;
    suzanne_object_struct5.rotate_vector = glm::vec3(1.0f, 1.0f, 1.0f);
    suzanne_object_struct5.translate_vector = glm::vec3(0.0f, 0.0f, 0.0f);
    model::Object *suzanne5 = new model::Object(suzanne_object_struct5);

    std::cout << "number of vertices: " << terrain_species->vertices.size() << ".\n";
    std::cout << "number of UVs: " << terrain_species->UVs.size() << ".\n";
    std::cout << "number of normals: " << terrain_species->normals.size() << ".\n";

    std::cout << "number of indexed vertices: " << terrain_species->indexed_vertices.size() << ".\n";
    std::cout << "number of indexed UVs: " << terrain_species->indexed_UVs.size() << ".\n";
    std::cout << "number of indexed normals: " << terrain_species->indexed_normals.size() << ".\n";

    // Initialize our little text library with the Holstein font
    const char *char_g_font_texture_filename = g_font_texture_filename.c_str();
    const char *char_g_font_texture_file_format = g_font_texture_file_format.c_str();
    text2D::initText2D((GLuint) WINDOW_WIDTH, (GLuint) WINDOW_HEIGHT, char_g_font_texture_filename, char_g_font_texture_file_format);

    // For speed computation
    double lastTime = glfwGetTime();
    double last_time_for_display_sync = glfwGetTime();
    int nbFrames = 0;

    bool ms_frame_text_ready = false;

    do
    {
        // Measure speed
        double currentTime = glfwGetTime();

        if (currentTime - last_time_for_display_sync >= (1.0f / MAX_FPS))
        {
            last_time_for_display_sync = glfwGetTime();

            char ms_frame_text[256];
            nbFrames++;

            if (currentTime - lastTime >= 1.0f)
            {
                // If last `printf()` was more than 1 sec ago,
                // `printf` and reset.
                sprintf(ms_frame_text, "%.02f ms/frame; %.02f Hz", 1000.0f / ((double) nbFrames), 1000.0f / (1000.0f / ((double) nbFrames)));
                ms_frame_text_ready = true;
                nbFrames = 0;
                lastTime += 1.0;
            }

            // Clear the screen.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Render the world.
            my_world->render();

            PrintingStruct printing_struct;
            printing_struct.screen_width = (GLuint) WINDOW_WIDTH;
            printing_struct.screen_height = (GLuint) WINDOW_HEIGHT;
            printing_struct.text_size = TEXT_SIZE;
            printing_struct.font_size = FONT_SIZE;
            printing_struct.char_font_texture_file_format = "bmp";

            char coordinates_text[256];
            sprintf(coordinates_text, "(%.2f,%.2f,%.2f)", position.x, position.y, position.z);

            char angles_text[256];
            sprintf(
                    angles_text,
                    "%.2f,%.2f rad; %.2f,%.2f deg",
                    horizontalAngle,
                    verticalAngle,
                    RADIANS_TO_DEGREES(horizontalAngle),
                    RADIANS_TO_DEGREES(verticalAngle));

            char time_text[256];
            sprintf(time_text, "%.2f sec", glfwGetTime());

#ifdef TESTING_SPHERICAL_WORLD_IN_USE
            char spherical_coordinates_text[256];
            sprintf(spherical_coordinates_text, "rho:%.2f theta:%.2f phi:%.2f", spherical_position.rho, spherical_position.theta, spherical_position.phi);
#endif

            // print cartesian coordinates on bottom left corner.
            printing_struct.x = 0;
            printing_struct.y = 0;
            printing_struct.text = coordinates_text;
            printing_struct.horizontal_alignment = "left";
            printing_struct.vertical_alignment = "bottom";
            text2D::printText2D(printing_struct);

            // print horizontal and vertical angles on bottom left corner.
            printing_struct.x = 0;
            printing_struct.y += TEXT_SIZE;
            printing_struct.text = angles_text;
            printing_struct.horizontal_alignment = "left";
            printing_struct.vertical_alignment = "bottom";
            text2D::printText2D(printing_struct);

#ifdef TESTING_SPHERICAL_WORLD_IN_USE
            // print spherical coordinates on bottom left corner.
            printing_struct.x = 0;
            printing_struct.y += TEXT_SIZE;
            printing_struct.text = spherical_coordinates_text;
            printing_struct.horizontal_alignment = "left";
            printing_struct.vertical_alignment = "bottom";
            text2D::printText2D(printing_struct);
#endif

            // print time data on top left corner.
            printing_struct.x = 0;
            printing_struct.y = (GLuint) WINDOW_HEIGHT;
            printing_struct.text = time_text;
            printing_struct.horizontal_alignment = "left";
            printing_struct.vertical_alignment = "top";
            text2D::printText2D(printing_struct);

            if (ms_frame_text_ready)
            {
                // print frame rate data on top right corner.
                printing_struct.x = WINDOW_WIDTH;
                printing_struct.y = WINDOW_HEIGHT;
                printing_struct.text = ms_frame_text;
                printing_struct.horizontal_alignment = "right";
                printing_struct.vertical_alignment = "top";
                text2D::printText2D(printing_struct);
            }

            // Swap buffers.
            glfwSwapBuffers(window);
        }

        if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) && does_suzanne_species_exist)
        {
            delete suzanne_species;
            does_suzanne_species_exist = false;
        }

        if (does_suzanne_species_exist && does_suzanne_species_have_uvmap_texture && (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS))
        {
            // switch to grass texture.
            suzanne_species->switch_to_new_texture(my_grass_texture);
            does_suzanne_species_have_uvmap_texture = false;
        }
        else if (does_suzanne_species_exist && !does_suzanne_species_have_uvmap_texture && (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS))
        {
            // switch to uvmap texture.
            suzanne_species->switch_to_new_texture(my_uvmap_texture);
            does_suzanne_species_have_uvmap_texture = true;
        }

        if (does_suzanne_species_exist && has_suzanne_2_transformed_into_terrain && (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS))
        {
            // switch to suzanne species.
            suzanne2->switch_to_new_species(suzanne_species);
            has_suzanne_2_transformed_into_terrain = false;
        }
        else if (does_suzanne_species_exist && !has_suzanne_2_transformed_into_terrain && (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS))
        {
            // switch to terrain species.
            suzanne2->switch_to_new_species(terrain_species);
            has_suzanne_2_transformed_into_terrain = true;
        }

        glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while ((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
            && (glfwWindowShouldClose(window) == 0));

    delete my_world;

    // Delete the text's VBO, the shader and the texture
    text2D::cleanupText2D();

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}