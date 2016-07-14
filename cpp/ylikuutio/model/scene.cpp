#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#define DEGREES_TO_RADIANS(x) (x * PI / 180.0f)
#endif

#include "scene.hpp"
#include "ground_level.hpp"
#include "shader.hpp"
#include "render_templates.hpp"
#include "cpp/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <stdint.h> // uint32_t etc.

extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

namespace model
{
    void Scene::bind_to_parent()
    {
        hierarchy::bind_child_to_parent<model::Scene*>(this, this->parent_pointer->scene_pointer_vector, this->parent_pointer->free_sceneID_queue);
    }

    Scene::Scene(model::World* parent_pointer)
    {
        // constructor.
        this->parent_pointer = parent_pointer;

        // get childID from the World and set pointer to this Scene.
        this->bind_to_parent();
    }

    Scene::~Scene()
    {
        // destructor.
        std::cout << "Scene with childID " << std::dec << this->childID << " will be destroyed.\n";

        // destroy all shaders of this scene.
        std::cout << "All shaders of this scene will be destroyed.\n";
        hierarchy::delete_children<model::Shader*>(this->shader_pointer_vector);
    }

    void Scene::render()
    {
        // render Scene by calling `render()` function of each Shader.
        model::render_children<model::Shader*>(this->shader_pointer_vector);
    }

    void Scene::bind_to_new_parent(model::World* new_world_pointer)
    {
        hierarchy::bind_child_to_new_parent<model::Scene*, model::World*>(this, new_world_pointer, this->parent_pointer->scene_pointer_vector, this->parent_pointer->free_sceneID_queue);
    }

    void Scene::set_shader_pointer(uint32_t childID, void* parent_pointer)
    {
        hierarchy::set_child_pointer(childID, parent_pointer, this->shader_pointer_vector, this->free_shaderID_queue);
    }
}
