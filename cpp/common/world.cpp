#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#define DEGREES_TO_RADIANS(x) (x * PI / 180.0f)
#endif

#include "world.hpp"
#include "shader.hpp"

extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

namespace model
{
    World::World()
    {
        // constructor.
    }

    World::~World()
    {
        // destructor.
        std::cout << "This world will be destroyed.\n";

        // destroy all shaders of this world.
        std::cout << "All shaders of this world will be destroyed.\n";
        model::delete_children<model::Shader*>(this->shader_pointer_vector);
    }

    void World::render()
    {
        this->compute_matrices_from_inputs();

        // render World by calling `render()` function of each Shader.
        model::render_children<model::Shader*>(this->shader_pointer_vector);
    }

    void World::set_shader_pointer(GLuint childID, void* parent_pointer)
    {
        set_child_pointer(childID, parent_pointer, this->shader_pointer_vector, this->free_shaderID_queue);
    }

    void World::set_world_species_pointer(void* world_species_pointer)
    {
        this->world_species_pointer = world_species_pointer;
    }

    void World::compute_matrices_from_inputs()
    {
        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);

        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Reset mouse position for next frame
        glfwSetCursorPos(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

        if (hasMouseEverMoved || (abs(xpos) > 0.0001) || (abs(ypos) > 0.0001))
        {
            hasMouseEverMoved = true;

            // Compute new orientation
            horizontalAngle += mouseSpeed * GLfloat(WINDOW_WIDTH/2 - xpos);
            horizontalAngle = remainder(horizontalAngle, (2.0f * PI));

            if (is_invert_mouse_in_use)
            {
                // invert mouse.
                verticalAngle   -= mouseSpeed * GLfloat(WINDOW_HEIGHT/2 - ypos);
            }
            else
            {
                // don't invert mouse.
                verticalAngle   += mouseSpeed * GLfloat(WINDOW_HEIGHT/2 - ypos);
            }
            verticalAngle = remainder(verticalAngle, (2.0f * PI));
        }

        // Direction : Spherical coordinates to Cartesian coordinates conversion
        glm::vec3 direction(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
                );

        // Right vector
        glm::vec3 right = glm::vec3(
                sin(horizontalAngle - PI/2.0f),
                0,
                cos(horizontalAngle - PI/2.0f)
                );

        // Up vector
        glm::vec3 up = glm::cross(right, direction);

        GLfloat temp_speed;

        // Turbo.
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
        {
            temp_speed = twin_turbo_factor * speed;
        }
        else if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
        {
            temp_speed = turbo_factor * speed;
        }
        else
        {
            temp_speed = speed;
        }

        // Move forward
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            position += direction * deltaTime * temp_speed;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            position -= direction * deltaTime * temp_speed;
        }
        // Strafe right
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            position += right * deltaTime * temp_speed;
        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            position -= right * deltaTime * temp_speed;
        }

        // Move up.
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            position.y += deltaTime * temp_speed;
        }

        // Move down.
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            position.y -= deltaTime * temp_speed;
        }

        // Move west.
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            position.x -= deltaTime * temp_speed;
        }

        // Move east.
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            position.x += deltaTime * temp_speed;
        }

        // Move north.
        if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            position.z -= deltaTime * temp_speed;
        }

        // Move south.
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            position.z += deltaTime * temp_speed;
        }

        // Flight mode on.
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            fallSpeed = 0.0f;
            inFlightmode = true;
        }

        // Run mode on.
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            fallSpeed = 0.0f;
            inFlightmode = false;
        }

        if (!inFlightmode)
        {
            fallSpeed += gravity;
            position.y -= fallSpeed;
        }

        GLfloat FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            if (is_key_I_released)
            {
                if (is_invert_mouse_in_use)
                {
                    is_invert_mouse_in_use = false;
                }
                else
                {
                    is_invert_mouse_in_use = true;
                }
                is_key_I_released = false;
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
        {
            is_key_I_released = true;
        }

        // adjust position according to the ground.
        if (!inFlightmode)
        {
            if (this->world_species_pointer != NULL)
            {
                GLfloat ground_y = model::get_floor_level(static_cast<model::Species*>(this->world_species_pointer), position);

                if (!std::isnan(ground_y))
                {
                    if (position.y < ground_y)
                    {
                        position.y = ground_y;
                        fallSpeed = 0.0f;
                    }
                }
            }
        }

#ifdef TESTING_SPHERICAL_WORLD_IN_USE
        // compute spherical coordinates.
        spherical_position.rho = sqrt((position.x * position.x) + (position.y * position.y) + (position.z * position.z));
        spherical_position.theta = RADIANS_TO_DEGREES(atan2(sqrt((position.x * position.x) + (position.y * position.y)), position.z));
        spherical_position.phi = RADIANS_TO_DEGREES(atan2(position.y, position.x));
#endif

        earth_radius = EARTH_RADIUS;

        camera_position = position;
        camera_position.y += 2.0f;

        // Projection matrix : 45° Field of View, aspect ratio, display range : 0.1 unit <-> 100 units
        ProjectionMatrix = glm::perspective(FoV, ASPECT_RATIO, 0.001f, 5000.0f + 2.0f * (GLfloat) earth_radius);
        // Camera matrix
        ViewMatrix = glm::lookAt(
                camera_position,           // Camera is here
                camera_position+direction, // and looks here : at the same position, plus "direction"
                up                         // Head is up (set to 0,-1,0 to look upside-down)
                );

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
    }
}