#ifndef __UNIVERSE_HPP_INCLUDED
#define __UNIVERSE_HPP_INCLUDED

#include "entity.hpp"
#include "entity_factory.hpp"
#include "universe_struct.hpp"
#include "camera_struct.hpp"
#include "code/ylikuutio/opengl/opengl.hpp"
#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/common/globals.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

#include "SDL.h"

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <cmath>         // NAN, std::isnan, std::pow
#include <cstddef>       // std::size_t
#include <iostream>      // std::cout, std::cin, std::cerr
#include <limits>        // std::numeric_limits
#include <memory>        // std::make_shared, std::shared_ptr
#include <queue>         // std::queue
#include <stdint.h>      // uint32_t etc.
#include <unordered_map> // std::unordered_map
#include <string>        // std::string
#include <vector>        // std::vector

// `Universe`, `Scene`, `Shader`, `Material`, `Species`, `Object`.
// `Universe`, `Scene`, `Shader`, `Material`, `VectorFont`, `Glyph`, `Object`.
// `Universe` must be created before any `Scene`. `parent` must be given to each `Scene`.
// `Scene` must be created before any `Shader`. `parent` must be given to each `Shader`.
// `Shader` must be created before any `Material`. `parent` must be given to each `Material`.
// `Material` must be created before any `Species`. `parent` must be given to each `Species`.
// `Species` must be create before any `Object` of that `Species`. `parent` must be given to each `Object` of the `Species`.
//
//
// Hierarchy of regular `Object`s (including terrain `Species`):
//
//    Universe
//       ^
//     World
//       ^
//     Scene
//       ^
//     Shader
//       ^
//    Material
//       ^
//    Species : Model
//       ^
//     Object : Movable
//
// Please note that for regular `Object`s the hierarchy above is both the ontological hierarchy and the rendering hierarchy.
//
// Ontological hierarchy of `Glyph` (character) entities:
//
//    Universe
//       ^
//     World
//       ^
//     Scene
//       ^
//     Shader
//       ^
//    Material
//       ^
//   VectorFont < Glyph : Model
//       ^
//     Text3D
//       ^
//     Object : Movable
//
// Ontological hierarchy affects how objects can be created and how they can be destroyed,
// though the precise ways how objects can be created depends on the functions available.
//
// Rendering hierarchy of `Glyph` (character) entities:
//
//    Universe
//       ^
//     World
//       ^
//     Scene
//       ^
//     Shader
//       ^
//    Material
//       ^
//   VectorFont
//       ^
//     Glyph : Model
//       ^
//     Object : Movable
//
// Please note that rendering hierarchy does not include `Text3D` at all, as each `Glyph` points directly to `VectorFont`.
// So, `render_species_or_glyph` is called only once for each glyph, and that call renders all the children of that `Glyph`,
// even if the children (which are of type `Object`) may belong to many different `Text3D` objects.
// `Text3D` is anyway needed in the ontological hierarchy, so that complete 3D texts can be destroyed and manipulated at once.
//
// Ontological hierarchy of `Symbiosis` entities:
//
//    Universe
//       ^
//     World
//       ^
//     Scene
//       ^
//     Shader
//       ^
//   Symbiosis : Entity  < SymbiontMaterial : Material
//       ^                        ^
//   Holobiont : Movable   SymbiontSpecies : Species
//       ^
//     Biont : Movable
//
// Each `Holobiont` is a composite organism which consists of 0 more `Bionts`.
// The `Biont`s of the `Holobiont` each belong to their corresponding
// `SymbiontSpecies`.
//
// For more information about holobionts, check Wikipedia:
// https://en.wikipedia.org/wiki/Holobiont
//
// Ontological hierarchy affects how objects can be created and how they can be destroyed,
// though the precise ways how objects can be created depends on the functions available.
//
// Rendering hierarchy of `Symbiosis` entities:
//
//    Universe
//       ^
//     World
//       ^
//     Scene
//       ^
//     Shader
//       ^
//   Symbiosis : Entity  > SymbiontMaterial : Material
//       ^                        v
//   Holobiont : Movable   SymbiontSpecies : Species
//       ^
//     Biont : Movable
//
// Optimized rendering hierarchy of `Symbiosis` entities:
//
//      Universe
//         ^
//       World
//         ^
//       Scene
//         ^
//       Shader
//         ^
//     Symbiosis
//         ^
//  SymbiontMaterial : Material
//         ^
//  SymbiontSpecies : Species
//         ^
//       Biont : Movable
//
// TODO: implement optimized rendering hierarchy for `Symbiosis` entities!
//
// Ontological hierarchy of `ShaderSymbiosis` entities:
//
//      Universe
//         ^
//       World
//         ^
//       Scene
//         ^
//  ShaderSymbiosis : Symbiosis < SymbiontShader : Shader
//         ^                            ^
//     Holobiont : Movable       SymbiontMaterial : Material
//         ^                            ^
//       Biont : Movable         SymbiontSpecies : Species
//
// Rendering hierarchy of `ShaderSymbiosis` entities:
//
//      Universe
//         ^
//       World
//         ^
//       Scene
//         ^
//  ShaderSymbiosis : Symbiosis  > SymbiontShader : Shader
//         ^                             v
//     Holobiont : Movable        SymbiontMaterial : Material
//         ^                             v
//       Biont : Movable          SymbiontSpecies : Species
//
// TODO: implement `ShaderSymbiosis` entities!
//
// Optimized rendering hierarchy of `ShaderSymbiosis` entities:
//
//      Universe
//         ^
//       World
//         ^
//       Scene
//         ^
//  ShaderSymbiosis
//         ^
//   SymbiontShader : Shader
//         ^
//  SymbiontMaterial : Material
//         ^
//  SymbiontSpecies : Species
//         ^
//       Biont : Movable
//
// TODO: implement optimized rendering hierarchy for `ShaderSymbiosis` entities!
//
// Deleting a `Universe` also deletes all worlds, scenes, all shaders, materials, species, fonts, glyphs and objects that are bound to the same `Universe`.
// Deleting a `World` also deletes all scenes, all shaders, materials, species, fonts, glyphs and objects that are bound to the same `World`.
// Deleting a `Scene` also deletes all shaders, materials, species, fonts, glyphs and objects that are bound to the same `Scene`.
// Deleting a `Shader` also deletes all materials, species, fonts, glyphs and objects that are bound to the same `Shader`.
// Deleting a `Material` also deletes all species, fonts, glyphs and objects that are bound to the same `Material`.
// Deleting a `Species` also deletes all objects that are bound to the same `Species`.
// Deleting an `Object` only deletes the object.

// Characteristics of object type graphs: TODO: implement object type graphs!
// 1. Each object must be an undirected graph.
// 2. Each edge must be a link in the graph.
// 3. The faces of each object must form a closed surface. The only exception is the terrain object, which may have borders.
//
// Modifying object type graphs:
// 1. Translation of a vertex does not require changes in any other nodes of the graph.
// 2. Adding a vertex always requires changes in some other nodes of the graph (unless the graph is empty before adding the vertex).
// 3. Deleting a vertex always requires deletion of edges from some other nodes of the graph (unless the vertex is the only vertex of the graph).
// 4. Deleting a vertex or vertices usually also requires appropriate vertex additions. These changes are called 'complex modifications'.
//
// Adding a vertex or several vertices:
// 1. The new edges must be connected to the existing graph with appropriate links.
// 2. Each new face must be a triangle.
//
// Deleting a vertex or several vertices:
// 1. When a vertex or several vertices are deleted, their links must be deleted too (`Node` destructor handles this).
// 2. If the vertex to be deleted is on the border of a [terrain] object, it can be deleted.
// 3. If the vertices that are neighbors to the vertex to be deleted form only triangeles, the vertex can be deleted without vertex additions.
// 4. Otherwise the vertex cannot be deleted without appropriate vertex and edge additions.
//
// Complex modifications:
// 1. In complex modifications one or more vertices and edges are deleted and one or more vertices and edges are added.
// 2. Complex modifications may also change the topology of the object (tunnels, arcs, etc.).
// 3. If a complex modification causes splitting the object in two or more pieces, each piece becomes a separate object.
// 4. If the splitted object is a terrain object, then the lowest vertex (any vertex with smallest y-coordinate) of each piece is searched and the
//    y-coordinates of these are compared. The piece with the smallest y-coordinate (lowest altitude) remains terrain, other pieces become
//    regular objects. The pieces that become regular objects will be subject to gravity the same way as any regular object.

namespace yli
{
    namespace config
    {
        class Setting;
    }

    namespace console
    {
        class Console;
    }

    namespace ontology
    {
        class World;
        class Scene;
        class Species;

        class Universe: public yli::ontology::Entity
        {
            public:
                void bind_world(yli::ontology::World* const world);

                // constructor.
                Universe(const UniverseStruct& universe_struct)
                    : Entity(this) // `Universe` has no parent.
                {
                    this->entity_factory = std::make_shared<yli::ontology::EntityFactory>(this);

                    this->current_camera_cartesian_coordinates = glm::vec3(NAN, NAN, NAN); // dummy coordinates.

                    this->current_camera_spherical_coordinates.rho   = NAN; // dummy coordinates.
                    this->current_camera_spherical_coordinates.theta = NAN; // dummy coordinates.
                    this->current_camera_spherical_coordinates.phi   = NAN; // dummy coordinates.

                    this->planet_radius    = NAN;     // world radius is NAN as long it doesn't get `set` by `SettingMaster`.
                    this->terrain_species  = nullptr;
                    this->active_world     = nullptr;
                    this->console_pointer  = nullptr;

                    this->background_red   = NAN;
                    this->background_green = NAN;
                    this->background_blue  = NAN;
                    this->background_alpha = NAN;

                    // Variables related to the window.
                    this->window        = nullptr;
                    this->window_width  = universe_struct.window_width;
                    this->window_height = universe_struct.window_height;
                    this->window_title  = universe_struct.window_title;
                    this->is_headless   = universe_struct.is_headless;

                    this->current_camera_projection_matrix = glm::mat4(1.0f); // identity matrix (dummy value).
                    this->current_camera_view_matrix       = glm::mat4(1.0f);       // identity matrix (dummy value).
                    this->current_camera_horizontal_angle  = NAN;
                    this->current_camera_vertical_angle    = NAN;

                    // Variables related to the camera.
                    this->aspect_ratio = static_cast<GLfloat>(this->window_width / this->window_height);
                    this->initialFoV   = 60.0f;

                    this->text_size = universe_struct.text_size;
                    this->font_size = universe_struct.font_size;

                    this->max_FPS                              = universe_struct.max_FPS;
                    this->delta_time                           = NAN;

                    // `std::numeric_limits<std::size_t>::max()` means that `last_time_before_reading_keyboard` is not defined.
                    this->last_time_before_reading_keyboard    = std::numeric_limits<uint32_t>::max();

                    // `std::numeric_limits<std::size_t>::max()` means that `current_time_before_reading_keyboard` is not defined.
                    this->current_time_before_reading_keyboard = std::numeric_limits<uint32_t>::max();

                    this->has_mouse_ever_moved    = false;
                    this->can_toggle_invert_mouse = false;
                    this->is_invert_mouse_in_use  = false;
                    this->can_toggle_flight_mode  = false;
                    this->is_flight_mode_in_use   = false;
                    this->is_first_turbo_pressed  = false;
                    this->is_second_turbo_pressed = false;

                    this->turbo_factor      = NAN;
                    this->twin_turbo_factor = NAN;

                    this->speed       = universe_struct.speed;
                    this->mouse_speed = universe_struct.mouse_speed;

                    this->gravity    = universe_struct.gravity;
                    this->fall_speed = this->gravity;

                    this->znear      = universe_struct.znear;
                    this->zfar       = universe_struct.zfar;

                    this->testing_spherical_terrain_in_use = false;
                    this->in_help_mode                     = true;
                    this->can_toggle_help_mode             = false;
                    this->can_display_help_screen          = true;

                    this->number_of_worlds = 0;

                    this->context = nullptr;
                    this->window  = nullptr;

                    // Initialise SDL
                    if (!yli::opengl::init_window())
                    {
                        std::cerr << "Failed to initialize SDL.\n";
                        return;
                    }

                    // Open a window and create its OpenGL context.
                    std::cout << "Opening a window and creating its OpenGL context...\n";
                    this->window = yli::opengl::create_window(
                            static_cast<int>(this->window_width),
                            static_cast<int>(this->window_height),
                            this->window_title.c_str());

                    if (this->window == nullptr)
                    {
                        std::cerr << "SDL Window could not be created!\n";
                    }

                    this->create_context();
                    this->make_context_current();

                    // Disable vertical sync.
                    // TODO: add option to enable/disable vsync in the console.
                    SDL_GL_SetSwapInterval(0);

                    // `yli::ontology::Entity` member variables begin here.
                    this->child_vector_pointers_vector.push_back(&this->world_pointer_vector);
                    this->type_string = "yli::ontology::Universe*";
                }

                // destructor.
                virtual ~Universe();

                // this method processes the physics.
                void do_physics();

                // this method renders the active `Scene` of this `Universe`.
                void render();

                // this method stes the active `World`.
                void set_active_world(yli::ontology::World* const world);

                // this method stes the active `Scene`.
                void set_active_scene(yli::ontology::Scene* const world);

                yli::console::Console* get_console() const;
                void set_console(yli::console::Console* console);

                float get_planet_radius() const;
                void set_planet_radius(float planet_radius);

                // this method returns a terrain `Species` pointer.
                yli::ontology::Species* get_terrain_species() const;

                // this method sets a terrain `Species` pointer.
                void set_terrain_species(yli::ontology::Species* const terrain_species);

                std::size_t get_number_of_worlds() const;

                yli::ontology::World* get_active_world() const;

                yli::ontology::Entity* get_parent() const override;
                std::size_t get_number_of_children() const override;
                std::size_t get_number_of_descendants() const override;

                void create_context();
                void make_context_current();

                // this method returns current `window`.
                SDL_Window* get_window() const;

                // this method returns current `window_width`.
                std::size_t get_window_width() const;

                // this method returns current `window_height`.
                std::size_t get_window_height() const;

                // this method returns current `text_size`.
                std::size_t get_text_size() const;

                // this method returns current `font_size`.
                std::size_t get_font_size() const;

                // this method computes the new delta time and returns it.
                float compute_delta_time();

                // this method returns the last computed delta time.
                float get_delta_time() const;

                // this method stores `current_time_before_reading_keyboard` into `last_time_before_reading_keyboard`.
                void finalize_delta_time_loop();

                // this method returns current `max_FPS`.
                std::size_t get_max_FPS() const;

                void set(std::string& setting_name, std::shared_ptr<yli::datatypes::AnyValue> setting_any_value);

                // this method returns a pointer to `yli::config::Setting` corresponding to the given `key`.
                yli::config::Setting* get(std::string key) const;

                bool is_entity(const std::string& name) const;
                yli::ontology::Entity* get_entity(const std::string& name) const;
                std::string get_entity_names() const;

                void add_entity(const std::string& name, yli::ontology::Entity* const entity);
                void erase_entity(const std::string& name);

                yli::ontology::EntityFactory* get_entity_factory() const;

                glm::mat4& get_projection_matrix();
                void set_projection_matrix(glm::mat4& projection_matrix);

                glm::mat4& get_view_matrix();
                void set_view_matrix(glm::mat4& view_matrix);

                GLfloat get_aspect_ratio() const;
                GLfloat get_initialFoV() const;

                // Public callbacks.

                static std::shared_ptr<yli::datatypes::AnyValue> activate(
                        yli::console::Console* const console,
                        yli::ontology::Entity* const universe_entity,
                        std::vector<std::string>& command_parameters);

                static std::shared_ptr<yli::datatypes::AnyValue> delete_entity(
                        yli::console::Console* const console,
                        yli::ontology::Entity* const universe_entity,
                        std::vector<std::string>& command_parameters);

                static std::shared_ptr<yli::datatypes::AnyValue> info(
                        yli::console::Console* const console,
                        yli::ontology::Entity* const universe_entity,
                        std::vector<std::string>& command_parameters);

                static std::shared_ptr<yli::datatypes::AnyValue> bind(
                        yli::console::Console* const console,
                        yli::ontology::Entity* const universe_entity,
                        std::vector<std::string>& command_parameters);

                // Public callbacks end here.

                // Variables related to location and orientation.

                // `cartesian_coordinates` can be accessed as a vector or as single coordinates `x`, `y`, `z`.
                glm::vec3 current_camera_cartesian_coordinates;

                // `spherical_coordinates` can be accessed as a vector or as single coordinates `rho`, `theta`, `phi`.
                SphericalCoordinatesStruct current_camera_spherical_coordinates;

                // `direction` can be accessed as a vector or as single coordinates `pitch`, `roll`, `yaw`.
                glm::vec3 current_camera_direction;

                glm::vec3 current_camera_right; // note: `right` can not be set directly using console.
                glm::vec3 current_camera_up;    // note: `up` can not be set directly using console.

                double current_camera_horizontal_angle;
                double current_camera_vertical_angle;

                float speed;
                float turbo_factor;
                float twin_turbo_factor;
                float mouse_speed;
                bool has_mouse_ever_moved;
                bool can_toggle_invert_mouse;
                bool is_invert_mouse_in_use;
                bool can_toggle_flight_mode;
                bool is_flight_mode_in_use;
                bool is_first_turbo_pressed;
                bool is_second_turbo_pressed;

                // Variables related to physics.
                float gravity;
                float fall_speed;

                // Variables related to graphics.
                float znear;
                float zfar;

                // Variables related to the current `Scene`.
                bool testing_spherical_terrain_in_use;

                // Variables related to help mode.
                bool in_help_mode;
                bool can_toggle_help_mode;
                bool can_display_help_screen;

            private:
                bool compute_and_update_matrices_from_inputs();

                yli::ontology::Species* terrain_species; // pointer to terrain `Species` (used in collision detection).

                float planet_radius;

                std::shared_ptr<yli::ontology::EntityFactory> entity_factory;

                std::vector<yli::ontology::World*> world_pointer_vector;
                std::queue<std::size_t> free_worldID_queue;
                std::size_t number_of_worlds;

                yli::ontology::World* active_world;

                yli::console::Console* console_pointer;     // pointer to `Console`.

                // Named entities are stored here so that they can be recalled, if needed.
                std::unordered_map<std::string, yli::ontology::Entity*> entity_map;

                GLclampf background_red;
                GLclampf background_green;
                GLclampf background_blue;
                GLclampf background_alpha;

                // Variables related to the window.
                std::shared_ptr<SDL_GLContext> context;
                SDL_Window* window;
                std::size_t window_width;
                std::size_t window_height;
                std::string window_title;
                bool is_headless;

                // Variables related to the camera.
                glm::mat4 current_camera_projection_matrix;
                glm::mat4 current_camera_view_matrix;
                GLfloat aspect_ratio; // at the moment all cameras use the same aspect ratio.
                GLfloat initialFoV;   // at the moment all cameras use the same FoV.

                // Variables related to the fonts and texts used.
                std::size_t text_size;
                std::size_t font_size;

                // Variables related to timing of events.
                std::size_t max_FPS;
                double delta_time;

                uint32_t last_time_before_reading_keyboard;
                uint32_t current_time_before_reading_keyboard;
        };
    }
}

#endif
