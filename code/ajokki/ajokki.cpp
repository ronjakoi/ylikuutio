// Ylikuutio - A 3D game and simulation engine.
//
// Copyright (C) 2015-2020 Antti Nuortimo.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#define DEGREES_TO_RADIANS(x) (x * PI / 180.0f)
#endif

#ifndef RADIANS_TO_DEGREES
#define RADIANS_TO_DEGREES(x) (x * 180.0f / PI)
#endif

#include "ajokki_console_callbacks.hpp"
#include "ajokki_altiplano_scene.hpp"
#include "ajokki_helsinki_east_downtown_scene.hpp"
#include "ajokki_joensuu_center_west_scene.hpp"
#include "ajokki_tallinn_scene.hpp"
#include "code/app/app_window.hpp"
#include "code/app/app_framebuffer.hpp"
#include "code/app/app_background_colors.hpp"
#include "code/app/app_console_callbacks.hpp"
#include "code/app/app_keyboard_callbacks.hpp"
#include "code/app/app_debug.hpp"
#include "code/app/app_console.hpp"
#include "code/app/app_movement.hpp"
#include "code/app/app_location_and_orientation.hpp"
#include "code/app/app_wireframe.hpp"
#include "code/app/app_console_callbacks.hpp"
#include "code/ylikuutio/audio/audio_master.hpp"
#include "code/ylikuutio/callback/callback_parameter.hpp"
#include "code/ylikuutio/callback/callback_object.hpp"
#include "code/ylikuutio/callback/callback_engine.hpp"
#include "code/ylikuutio/callback/callback_magic_numbers.hpp"
#include "code/ylikuutio/command_line/command_line_master.hpp"
#include "code/ylikuutio/console/console_callback_engine.hpp"
#include "code/ylikuutio/console/console_callback_object.hpp"
#include "code/ylikuutio/input/input.hpp"
#include "code/ylikuutio/input/input_master.hpp"
#include "code/ylikuutio/input/input_mode.hpp"
#include "code/ylikuutio/ontology/console.hpp"
#include "code/ylikuutio/ontology/font2D.hpp"
#include "code/ylikuutio/ontology/text2D.hpp"
#include "code/ylikuutio/ontology/species.hpp"
#include "code/ylikuutio/ontology/species_struct.hpp"
#include "code/ylikuutio/ontology/material.hpp"
#include "code/ylikuutio/ontology/material_struct.hpp"
#include "code/ylikuutio/ontology/scene.hpp"
#include "code/ylikuutio/ontology/world.hpp"
#include "code/ylikuutio/ontology/universe.hpp"
#include "code/ylikuutio/ontology/universe_struct.hpp"
#include "code/ylikuutio/ontology/entity_factory.hpp"
#include "code/ylikuutio/ontology/text_struct.hpp"
#include "code/ylikuutio/config/setting_master.hpp"
#include "code/ylikuutio/config/setting_struct.hpp"
#include "code/ylikuutio/opengl/opengl.hpp"
#include "code/ylikuutio/opengl/vboindexer.hpp"
#include "code/ylikuutio/string/ylikuutio_string.hpp"
#include "code/ylikuutio/time/time.hpp"
#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/common/pi.hpp"

// Include GLEW
#include "code/ylikuutio/opengl/ylikuutio_glew.hpp" // GLfloat, GLuint etc.

#define SDL_main main

#include "SDL.h"

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <cmath>         // abs, cos, NAN, remainder, sin, std::isnan, std::pow
#include <cstddef>       // std::size_t
#include <iomanip>       // std::setfill, std::setprecision, std::setw
#include <iostream>      // std::cout, std::cin, std::cerr
#include <memory>        // std::make_shared, std::shared_ptr
#include <sstream>       // std::istringstream, std::ostringstream, std::stringstream
#include <stdint.h>      // uint32_t etc.
#include <string>        // std::string
#include <vector>        // std::vector

int main(const int argc, const char* const argv[])
{
    yli::command_line::CommandLineMaster command_line_master(argc, argv);

    if (command_line_master.is_key("version"))
    {
        std::cout << "Ajokki " << yli::ontology::Universe::version << ", powered by Ylikuutio " << yli::ontology::Universe::version << "\n";
        return 0;
    }

    command_line_master.print_keys_and_values();

    if (!command_line_master.get_are_arguments_valid())
    {
        // Some of the arguments do not comply with the Ylikuutio argument syntax.
        std::cerr << "ERROR: Invalid syntax used in command line parameters.\n";
        return 1;
    }

    const std::vector<std::string> valid_keys {
        "help", "version", "window_width", "window_height", "framebuffer_width", "framebuffer_height", "speed", "turbo_factor", "twin_turbo_factor", "mouse_speed" };

    const std::vector<std::string> invalid_keys = command_line_master.get_invalid_keys(valid_keys);

    if (!command_line_master.check_keys(valid_keys))
    {
        std::cerr << "ERROR: 1 or more invalid command line parameters given.\n";

        const std::vector<std::string> invalid_keys = command_line_master.get_invalid_keys(valid_keys);

        for (std::vector<std::string>::const_iterator it = invalid_keys.begin(); it != invalid_keys.end(); it++)
        {
            std::cerr << "Invalid key: " << *it << "\n";
        }

        return 1;
    }

    // Create the `Universe`, store it in `my_universe`.
    std::cout << "Creating yli::ontology::Entity* my_universe_entity ...\n";
    yli::ontology::UniverseStruct universe_struct;
    std::stringstream window_title_stringstream;
    window_title_stringstream << "Ajokki " << yli::ontology::Universe::version << ", powered by Ylikuutio " << yli::ontology::Universe::version;
    universe_struct.window_title = window_title_stringstream.str();

    if (command_line_master.is_key("window_width") &&
            yli::string::check_if_unsigned_integer_string(command_line_master.get_value("window_width")))
    {
        const std::string window_width = command_line_master.get_value("window_width");
        std::size_t index = 0;
        universe_struct.window_width = yli::string::extract_uint32_t_value_from_string(window_width, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("window_height") &&
            yli::string::check_if_unsigned_integer_string(command_line_master.get_value("window_height")))
    {
        const std::string window_height = command_line_master.get_value("window_height");
        std::size_t index = 0;
        universe_struct.window_height = yli::string::extract_uint32_t_value_from_string(window_height, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("framebuffer_width") &&
            yli::string::check_if_unsigned_integer_string(command_line_master.get_value("framebuffer_width")))
    {
        const std::string framebuffer_width = command_line_master.get_value("framebuffer_width");
        std::size_t index = 0;
        universe_struct.framebuffer_width = yli::string::extract_uint32_t_value_from_string(framebuffer_width, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("framebuffer_height") &&
            yli::string::check_if_unsigned_integer_string(command_line_master.get_value("framebuffer_height")))
    {
        const std::string framebuffer_height = command_line_master.get_value("framebuffer_height");
        std::size_t index = 0;
        universe_struct.framebuffer_height = yli::string::extract_uint32_t_value_from_string(framebuffer_height, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("speed") &&
            yli::string::check_if_float_string(command_line_master.get_value("speed")))
    {
        const std::string speed = command_line_master.get_value("speed");
        std::size_t index = 0;
        universe_struct.speed = yli::string::extract_float_value_from_string(speed, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("turbo_factor") &&
            yli::string::check_if_float_string(command_line_master.get_value("turbo_factor")))
    {
        const std::string turbo_factor = command_line_master.get_value("turbo_factor");
        std::size_t index = 0;
        universe_struct.turbo_factor = yli::string::extract_float_value_from_string(turbo_factor, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("twin_turbo_factor") &&
            yli::string::check_if_float_string(command_line_master.get_value("twin_turbo_factor")))
    {
        const std::string twin_turbo_factor = command_line_master.get_value("twin_turbo_factor");
        std::size_t index = 0;
        universe_struct.twin_turbo_factor = yli::string::extract_float_value_from_string(twin_turbo_factor, index, nullptr, nullptr);
    }

    if (command_line_master.is_key("mouse_speed") &&
            yli::string::check_if_float_string(command_line_master.get_value("mouse_speed")))
    {
        const std::string mouse_speed = command_line_master.get_value("mouse_speed");
        std::size_t index = 0;
        universe_struct.mouse_speed = yli::string::extract_float_value_from_string(mouse_speed, index, nullptr, nullptr);
    }

    yli::ontology::Universe* const my_universe = new yli::ontology::Universe(universe_struct);
    my_universe->set_name("universe");

    yli::ontology::EntityFactory* const entity_factory = my_universe->get_entity_factory();

    yli::audio::AudioMaster* const audio_master = my_universe->get_audio_master();

    yli::input::InputMaster* const input_master = my_universe->get_input_master();

    std::cout << "Creating yli::callback::CallbackEngine cleanup_callback_engine ...\n";
    yli::callback::CallbackEngine cleanup_callback_engine = yli::callback::CallbackEngine();
    cleanup_callback_engine.create_CallbackObject(nullptr);

    if (my_universe->get_window() == nullptr)
    {
        std::cerr << "Failed to open SDL window.\n";
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    my_universe->create_context();

    // Initialize GLEW.
    if (!yli::opengl::init_glew())
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    yli::input::disable_cursor();
    yli::input::enable_relative_mouse_mode();

    // Enable depth test.
    yli::opengl::enable_depth_test();
    // Accept fragment if it is closer to the camera than the former one.
    yli::opengl::set_depth_func_to_less();

    // Cull triangles whose normal is not towards the camera.
    yli::opengl::cull_triangles();

    // Create the main `Console`.
    std::cout << "Creating yli::ontology::Entity* my_console_entity ...\n";
    yli::ontology::Entity* const my_console_entity = entity_factory->create_Console();
    std::cout << "Creating yli::ontology::Console* my_console ...\n";
    yli::ontology::Console* const my_console = dynamic_cast<yli::ontology::Console*>(my_console_entity);

    if (my_console == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    my_console->set_name("my_console");
    my_universe->set_active_console(my_console);

    std::cout << "Setting up console ...\n";
    app::set_console(my_universe->get_setting_master(), 15, 0, 0, 39);

    // Create the 'mini' `Console`.
    std::cout << "Creating yli::ontology::Entity* mini_console_entity ...\n";
    yli::ontology::Entity* const mini_console_entity = entity_factory->create_Console();
    std::cout << "Creating yli::ontology::Console* mini_console ...\n";
    yli::ontology::Console* const mini_console = dynamic_cast<yli::ontology::Console*>(mini_console_entity);

    if (mini_console == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    mini_console->set_name("mini_console");
    my_universe->set_active_console(mini_console);

    std::cout << "Setting up console ...\n";
    app::set_console(my_universe->get_setting_master(), 15, 0, 0, 39);

    my_universe->set_active_console(my_console);

    // Create the `World`.

    std::cout << "Creating yli::ontology::Entity* earth_world_entity ...\n";
    yli::ontology::Entity* const earth_world_entity = entity_factory->create_World();
    std::cout << "Creating yli::ontology::World* earth_world ...\n";
    yli::ontology::World* const earth_world = dynamic_cast<yli::ontology::World*>(earth_world_entity);

    if (earth_world == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    earth_world->set_name("earth_world");

    std::cout << "Setting earth_world as the active world ...\n";
    my_universe->set_active_world(earth_world);

    // Create the `Scene`s.
    // The `Scene`s will be created in the following order:
    // 1. Helsinki
    // 2. Joensuu
    // 3. altiplano
    // 4. Tallinn

    // Helsinki `Scene` begins here.

    std::cout << "Creating yli::ontology::Entity* helsinki_east_downtown_scene_entity and its contents ...\n";
    yli::ontology::Entity* const helsinki_east_downtown_scene_entity = ajokki::create_helsinki_east_downtown_scene(entity_factory, earth_world);

    if (helsinki_east_downtown_scene_entity == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    std::cout << "Creating yli::ontology::Scene* helsinki_east_downtown_scene ...\n";
    yli::ontology::Scene* const helsinki_east_downtown_scene = dynamic_cast<yli::ontology::Scene*>(helsinki_east_downtown_scene_entity);

    if (helsinki_east_downtown_scene == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    // Set `helsinki_east_downtown_scene` to be the currently active `Scene` in `earth_world`.
    std::cout << "Setting helsinki_east_downtown_scene as the active scene ...\n";
    earth_world->set_active_scene(helsinki_east_downtown_scene);

    // Helsinki `Scene` ends here.

    // Joensuu `Scene` begins here.

    std::cout << "Creating yli::ontology::Scene* joensuu_center_west_scene and its contents ...\n";
    if (ajokki::create_joensuu_center_west_scene(entity_factory, earth_world) == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    // Joensuu `Scene` ends here.

    // altiplano `Scene` begins here.

    std::cout << "Creating yli::ontology::Scene* altiplano_scene and its contents ...\n";
    if (ajokki::create_altiplano_scene(entity_factory, earth_world) == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    // altiplano `Scene` ends here.

    // Tallinn `Scene` begins here.

    std::cout << "Creating yli::ontology::Scene* tallinn_scene and its contents ...\n";
    if (ajokki::create_tallinn_scene(entity_factory, earth_world) == nullptr)
    {
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    // Tallinn `Scene` ends here.

    // Initialize our little text library with the Holstein font.
    const char* const char_g_font_texture_filename = "Holstein.bmp";
    const char* const char_g_font_texture_file_format = "bmp";

    std::cout << "Creating yli::ontology::Entity* my_font2D_entity ...\n";
    yli::ontology::Entity* const my_font2D_entity = entity_factory->create_Font2D(
            my_universe->get_window_width(),
            my_universe->get_window_height(),
            char_g_font_texture_filename,
            char_g_font_texture_file_format);
    std::cout << "Creating yli::ontology::Font2D* my_font2D ...\n";
    yli::ontology::Font2D* const my_font2D = dynamic_cast<yli::ontology::Font2D*>(my_font2D_entity);

    if (my_font2D == nullptr)
    {
        std::cerr << "Failed to create Font2D.\n";
        cleanup_callback_engine.execute(nullptr);
        return -1;
    }

    std::cout << "Font2D created successfully.\n";
    my_font2D->set_name("my_font2D");

    my_universe->set_active_font2D(my_font2D);
    my_console->print_text("Welcome! Please write \"help\" for more");
    my_console->print_text("information.");

    /*********************************************************************
     *  Callback engines for action mode keyreleases begin here.         *
     *********************************************************************/

    std::cout << "Defining action mode keyrelease callback engines.\n";

    // Callback code for left Control release: release first turbo.
    yli::callback::CallbackEngine release_first_turbo_callback_engine(my_universe);
    release_first_turbo_callback_engine.create_CallbackObject(&app::release_first_turbo);

    // Callback code for right Control release: release second turbo.
    yli::callback::CallbackEngine release_second_turbo_callback_engine(my_universe);
    release_second_turbo_callback_engine.create_CallbackObject(&app::release_second_turbo);

    // Callback code for I release: enable_toggle invert mouse.
    yli::callback::CallbackEngine enable_toggle_invert_mouse_callback_engine(my_universe);
    enable_toggle_invert_mouse_callback_engine.create_CallbackObject(&app::enable_toggle_invert_mouse);

    // Callback code for F release: enable_toggle flight mode.
    yli::callback::CallbackEngine enable_toggle_flight_mode_callback_engine(my_universe);
    enable_toggle_flight_mode_callback_engine.create_CallbackObject(&app::enable_toggle_flight_mode);

    // Callback code for F1 release: enable toggle help mode.
    yli::callback::CallbackEngine enable_toggle_help_mode_callback_engine(my_universe);
    enable_toggle_help_mode_callback_engine.create_CallbackObject(&app::enable_toggle_help_mode);

    /*********************************************************************
     *  Callback engines for action mode keypresses begin here.          *
     *********************************************************************/

    std::cout << "Defining action mode keypress callback engines.\n";

    // Callback code for `SDL_SCANCODE_GRAVE` (tilde key above Tab, usually used for console).
    yli::callback::CallbackEngine enter_console_callback_engine(my_universe);
    enter_console_callback_engine.create_CallbackObject(&yli::ontology::Console::enter_console);

    // Callback code for esc: exit program.
    yli::callback::CallbackEngine exit_program_callback_engine;
    exit_program_callback_engine.create_CallbackObject(&app::exit_program);

    // Callback code for left Control: first turbo.
    yli::callback::CallbackEngine first_turbo_callback_engine(my_universe);
    first_turbo_callback_engine.create_CallbackObject(&app::first_turbo);

    // Callback code for right Control: second turbo.
    yli::callback::CallbackEngine second_turbo_callback_engine(my_universe);
    second_turbo_callback_engine.create_CallbackObject(&app::second_turbo);

    // Callback code for key up: move forward.
    yli::callback::CallbackEngine move_forward_callback_engine(my_universe);
    move_forward_callback_engine.create_CallbackObject(&app::move_forward);

    // Callback code for key down: move backward.
    yli::callback::CallbackEngine move_backward_callback_engine(my_universe);
    move_backward_callback_engine.create_CallbackObject(&app::move_backward);

    // Callback code for key left: strafe left.
    yli::callback::CallbackEngine strafe_left_callback_engine(my_universe);
    strafe_left_callback_engine.create_CallbackObject(&app::strafe_left);

    // Callback code for key right: strafe right.
    yli::callback::CallbackEngine strafe_right_callback_engine(my_universe);
    strafe_right_callback_engine.create_CallbackObject(&app::strafe_right);

    // Callback code for space: ascent.
    yli::callback::CallbackEngine ascent_callback_engine(my_universe);
    ascent_callback_engine.create_CallbackObject(&app::ascent);

    // Callback code for enter: descent.
    yli::callback::CallbackEngine descent_callback_engine(my_universe);
    descent_callback_engine.create_CallbackObject(&app::descent);

    // Callback code for I: toggle invert mouse.
    yli::callback::CallbackEngine toggle_invert_mouse_callback_engine(my_universe);
    toggle_invert_mouse_callback_engine.create_CallbackObject(&app::toggle_invert_mouse);

    // Callback code for F: toggle flight mode.
    yli::callback::CallbackEngine toggle_flight_mode_callback_engine(my_universe);
    toggle_flight_mode_callback_engine.create_CallbackObject(&app::toggle_flight_mode);

    // Callback code for F1: toggle help mode.
    yli::callback::CallbackEngine toggle_help_mode_callback_engine(my_universe);
    toggle_help_mode_callback_engine.create_CallbackObject(&app::toggle_help_mode);

    // Callback code for D: delete Suzanne species.
    const std::string suzanne_species_string = "suzanne_species";
    yli::callback::CallbackEngine delete_suzanne_species_callback_engine(my_universe);
    yli::callback::CallbackObject* const delete_suzanne_species_callback_object = delete_suzanne_species_callback_engine.create_CallbackObject(
            &app::delete_entity);
    delete_suzanne_species_callback_object->create_CallbackParameter("", std::make_shared<yli::common::AnyValue>(&suzanne_species_string), false);

    // Callback code for G: switch to grass material.
    const std::string grass_material_string = "helsinki_east_downtown_grass_material";
    yli::callback::CallbackEngine switch_to_grass_material_callback_engine(my_universe);
    yli::callback::CallbackObject* const switch_to_grass_material_callback_object = switch_to_grass_material_callback_engine.create_CallbackObject(
            &app::switch_to_new_material);
    switch_to_grass_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&suzanne_species_string),
            false);
    switch_to_grass_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&grass_material_string),
            false);

    // Callback code for O: switch to orange fur material.
    const std::string orange_fur_material_string = "helsinki_east_downtown_orange_fur_material";
    yli::callback::CallbackEngine switch_to_orange_fur_material_callback_engine(my_universe);
    yli::callback::CallbackObject* const switch_to_orange_fur_material_callback_object = switch_to_orange_fur_material_callback_engine.create_CallbackObject(
            &app::switch_to_new_material);
    switch_to_orange_fur_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&suzanne_species_string),
            false);
    switch_to_orange_fur_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&orange_fur_material_string),
            false);

    // Callback code for P: switch to pink_geometric_tiles_material.
    const std::string pink_geometric_tiles_material_string = "helsinki_east_downtown_pink_geometric_tiles_material";
    yli::callback::CallbackEngine switch_to_pink_geometric_tiles_material_callback_engine(my_universe);
    yli::callback::CallbackObject* const switch_to_pink_geometric_tiles_material_callback_object = switch_to_pink_geometric_tiles_material_callback_engine.create_CallbackObject(
            &app::switch_to_new_material);
    switch_to_pink_geometric_tiles_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&suzanne_species_string),
            false);
    switch_to_pink_geometric_tiles_material_callback_object->create_CallbackParameter(
            "",
            std::make_shared<yli::common::AnyValue>(&pink_geometric_tiles_material_string),
            false);

    // Callback code for T: transform `suzanne2` into terrain.
    const std::string helsinki_species_string = "helsinki_east_downtown_terrain_species";
    const std::string suzanne2_string = "suzanne2";
    yli::callback::CallbackEngine transform_into_terrain_callback_engine(my_universe);
    yli::callback::CallbackObject* const transform_into_terrain_callback_object = transform_into_terrain_callback_engine.create_CallbackObject(
            &app::transform_into_new_species);
    transform_into_terrain_callback_object->create_CallbackParameter("", std::make_shared<yli::common::AnyValue>(&suzanne2_string), false);
    transform_into_terrain_callback_object->create_CallbackParameter("", std::make_shared<yli::common::AnyValue>(&helsinki_species_string), false);

    // Callback code for A: transform `suzanne2` back into monkey.
    yli::callback::CallbackEngine transform_into_monkey_callback_engine(my_universe);
    yli::callback::CallbackObject* const transform_into_monkey_callback_object = transform_into_monkey_callback_engine.create_CallbackObject(
            &app::transform_into_new_species);
    transform_into_monkey_callback_object->create_CallbackParameter("", std::make_shared<yli::common::AnyValue>(&suzanne2_string), false);
    transform_into_monkey_callback_object->create_CallbackParameter("", std::make_shared<yli::common::AnyValue>(&suzanne_species_string), false);

    /*********************************************************************
     *  Callback engines for console keyreleases begin here.             *
     *********************************************************************/

    std::cout << "Defining console keyrelease callback engines for `my_console`.\n";

    // Callback code for left Control release.
    yli::console::ConsoleCallbackEngine my_release_left_control_in_console_callback_engine;
    my_release_left_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_control_in_console, my_console);

    // Callback code for right Control release.
    yli::console::ConsoleCallbackEngine my_release_right_control_in_console_callback_engine;
    my_release_right_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_control_in_console, my_console);

    // Callback code for left Alt release.
    yli::console::ConsoleCallbackEngine my_release_left_alt_in_console_callback_engine;
    my_release_left_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_alt_in_console, my_console);

    // Callback code for right Alt release.
    yli::console::ConsoleCallbackEngine my_release_right_alt_in_console_callback_engine;
    my_release_right_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_alt_in_console, my_console);

    // Callback code for left Shift release.
    yli::console::ConsoleCallbackEngine my_release_left_shift_in_console_callback_engine;
    my_release_left_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_shift_in_console, my_console);

    // Callback code for right Shift release.
    yli::console::ConsoleCallbackEngine my_release_right_shift_in_console_callback_engine;
    my_release_right_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_shift_in_console, my_console);

    // Callback code for key up release: enable move to previous input.
    yli::console::ConsoleCallbackEngine my_enable_move_to_previous_input_callback_engine;
    my_enable_move_to_previous_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_move_to_previous_input, my_console);

    // Callback code for key down release: enable move to next input.
    yli::console::ConsoleCallbackEngine my_enable_move_to_next_input_callback_engine;
    my_enable_move_to_next_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_move_to_next_input, my_console);

    // Callback code for backspace release: enable backspace.
    yli::console::ConsoleCallbackEngine my_enable_backspace_callback_engine;
    my_enable_backspace_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_backspace, my_console);

    // Callback code for enter release: enable Enter key.
    yli::console::ConsoleCallbackEngine my_enable_enter_key_callback_engine;
    my_enable_enter_key_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_enter_key, my_console);

    // Callback code for C release: enable Control-C.
    yli::console::ConsoleCallbackEngine my_enable_ctrl_c_callback_engine;
    my_enable_ctrl_c_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_ctrl_c, my_console);

    // Callback code for W release: enable Control-W.
    yli::console::ConsoleCallbackEngine my_enable_ctrl_w_callback_engine;
    my_enable_ctrl_w_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_ctrl_w, my_console);

    // Callback code for PgUp release: enable PgUp.
    yli::console::ConsoleCallbackEngine my_enable_page_up_callback_engine;
    my_enable_page_up_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_page_up, my_console);

    // Callback code for PgDn release: enable PgDn.
    yli::console::ConsoleCallbackEngine my_enable_page_down_callback_engine;
    my_enable_page_down_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_page_down, my_console);

    // Callback code for Home release: enable Home.
    yli::console::ConsoleCallbackEngine my_enable_home_callback_engine;
    my_enable_home_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_home, my_console);

    // Callback code for End release: enable End.
    yli::console::ConsoleCallbackEngine my_enable_end_callback_engine;
    my_enable_end_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_end, my_console);

    std::cout << "Defining console keyrelease callback engines for `mini_console`.\n";

    // Callback code for left Control release.
    yli::console::ConsoleCallbackEngine mini_release_left_control_in_console_callback_engine;
    mini_release_left_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_control_in_console, mini_console);

    // Callback code for right Control release.
    yli::console::ConsoleCallbackEngine mini_release_right_control_in_console_callback_engine;
    mini_release_right_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_control_in_console, mini_console);

    // Callback code for left Alt release.
    yli::console::ConsoleCallbackEngine mini_release_left_alt_in_console_callback_engine;
    mini_release_left_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_alt_in_console, mini_console);

    // Callback code for right Alt release.
    yli::console::ConsoleCallbackEngine mini_release_right_alt_in_console_callback_engine;
    mini_release_right_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_alt_in_console, mini_console);

    // Callback code for left Shift release.
    yli::console::ConsoleCallbackEngine mini_release_left_shift_in_console_callback_engine;
    mini_release_left_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_left_shift_in_console, mini_console);

    // Callback code for right Shift release.
    yli::console::ConsoleCallbackEngine mini_release_right_shift_in_console_callback_engine;
    mini_release_right_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::release_right_shift_in_console, mini_console);

    // Callback code for key up release: enable move to previous input.
    yli::console::ConsoleCallbackEngine mini_enable_move_to_previous_input_callback_engine;
    mini_enable_move_to_previous_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_move_to_previous_input, mini_console);

    // Callback code for key down release: enable move to next input.
    yli::console::ConsoleCallbackEngine mini_enable_move_to_next_input_callback_engine;
    mini_enable_move_to_next_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_move_to_next_input, mini_console);

    // Callback code for backspace release: enable backspace.
    yli::console::ConsoleCallbackEngine mini_enable_backspace_callback_engine;
    mini_enable_backspace_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_backspace, mini_console);

    // Callback code for enter release: enable Enter key.
    yli::console::ConsoleCallbackEngine mini_enable_enter_key_callback_engine;
    mini_enable_enter_key_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_enter_key, mini_console);

    // Callback code for C release: enable Control-C.
    yli::console::ConsoleCallbackEngine mini_enable_ctrl_c_callback_engine;
    mini_enable_ctrl_c_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_ctrl_c, mini_console);

    // Callback code for W release: enable Control-W.
    yli::console::ConsoleCallbackEngine mini_enable_ctrl_w_callback_engine;
    mini_enable_ctrl_w_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_ctrl_w, mini_console);

    // Callback code for PgUp release: enable PgUp.
    yli::console::ConsoleCallbackEngine mini_enable_page_up_callback_engine;
    mini_enable_page_up_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_page_up, mini_console);

    // Callback code for PgDn release: enable PgDn.
    yli::console::ConsoleCallbackEngine mini_enable_page_down_callback_engine;
    mini_enable_page_down_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_page_down, mini_console);

    // Callback code for Home release: enable Home.
    yli::console::ConsoleCallbackEngine mini_enable_home_callback_engine;
    mini_enable_home_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_home, mini_console);

    // Callback code for End release: enable End.
    yli::console::ConsoleCallbackEngine mini_enable_end_callback_engine;
    mini_enable_end_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enable_end, mini_console);

    /*********************************************************************
     *  Callback engines for console keypresses begin here.              *
     *********************************************************************/

    std::cout << "Defining console keypress callback engines for `my_console`.\n";

    // Callback code for `SDL_SCANCODE_GRAVE` (tilde key above Tab, usually used for console).
    yli::console::ConsoleCallbackEngine my_exit_console_callback_engine;
    my_exit_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::exit_console, my_console);

    // Callback code for left Control press.
    yli::console::ConsoleCallbackEngine my_press_left_control_in_console_callback_engine;
    my_press_left_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_control_in_console, my_console);

    // Callback code for right Control press.
    yli::console::ConsoleCallbackEngine my_press_right_control_in_console_callback_engine;
    my_press_right_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_control_in_console, my_console);

    // Callback code for left Alt press.
    yli::console::ConsoleCallbackEngine my_press_left_alt_in_console_callback_engine;
    my_press_left_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_alt_in_console, my_console);

    // Callback code for right Alt press.
    yli::console::ConsoleCallbackEngine my_press_right_alt_in_console_callback_engine;
    my_press_right_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_alt_in_console, my_console);

    // Callback code for left Shift press.
    yli::console::ConsoleCallbackEngine my_press_left_shift_in_console_callback_engine;
    my_press_left_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_shift_in_console, my_console);

    // Callback code for right Shift press.
    yli::console::ConsoleCallbackEngine my_press_right_shift_in_console_callback_engine;
    my_press_right_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_shift_in_console, my_console);

    // Callback code for key up: move to previous input.
    yli::console::ConsoleCallbackEngine my_move_to_previous_input_callback_engine;
    my_move_to_previous_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::move_to_previous_input, my_console);

    // Callback code for key down: move to next input.
    yli::console::ConsoleCallbackEngine my_move_to_next_input_callback_engine;
    my_move_to_next_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::move_to_next_input, my_console);

    // Callback code for backspace: delete character left of cursor from current input in console.
    yli::console::ConsoleCallbackEngine my_backspace_callback_engine;
    my_backspace_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::backspace, my_console);

    // Callback code for Enter key.
    yli::console::ConsoleCallbackEngine my_enter_callback_engine;
    my_enter_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enter_key, my_console);

    // Callback code for C: Control-C.
    yli::console::ConsoleCallbackEngine my_ctrl_c_callback_engine;
    my_ctrl_c_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::ctrl_c, my_console);

    // Callback code for W: Control-W.
    yli::console::ConsoleCallbackEngine my_ctrl_w_callback_engine;
    my_ctrl_w_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::ctrl_w, my_console);

    // Callback code for PgUp.
    yli::console::ConsoleCallbackEngine my_page_up_callback_engine;
    my_page_up_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::page_up, my_console);

    // Callback code for PgDn.
    yli::console::ConsoleCallbackEngine my_page_down_callback_engine;
    my_page_down_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::page_down, my_console);

    // Callback code for Home.
    yli::console::ConsoleCallbackEngine my_home_callback_engine;
    my_home_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::home, my_console);

    // Callback code for End.
    yli::console::ConsoleCallbackEngine my_end_callback_engine;
    my_end_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::end, my_console);

    std::cout << "Defining console keypress callback engines for `mini_console`.\n";

    // Callback code for `SDL_SCANCODE_GRAVE` (tilde key above Tab, usually used for console).
    yli::console::ConsoleCallbackEngine mini_exit_console_callback_engine;
    mini_exit_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::exit_console, mini_console);

    // Callback code for left Control press.
    yli::console::ConsoleCallbackEngine mini_press_left_control_in_console_callback_engine;
    mini_press_left_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_control_in_console, mini_console);

    // Callback code for right Control press.
    yli::console::ConsoleCallbackEngine mini_press_right_control_in_console_callback_engine;
    mini_press_right_control_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_control_in_console, mini_console);

    // Callback code for left Alt press.
    yli::console::ConsoleCallbackEngine mini_press_left_alt_in_console_callback_engine;
    mini_press_left_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_alt_in_console, mini_console);

    // Callback code for right Alt press.
    yli::console::ConsoleCallbackEngine mini_press_right_alt_in_console_callback_engine;
    mini_press_right_alt_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_alt_in_console, mini_console);

    // Callback code for left Shift press.
    yli::console::ConsoleCallbackEngine mini_press_left_shift_in_console_callback_engine;
    mini_press_left_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_left_shift_in_console, mini_console);

    // Callback code for right Shift press.
    yli::console::ConsoleCallbackEngine mini_press_right_shift_in_console_callback_engine;
    mini_press_right_shift_in_console_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::press_right_shift_in_console, mini_console);

    // Callback code for key up: move to previous input.
    yli::console::ConsoleCallbackEngine mini_move_to_previous_input_callback_engine;
    mini_move_to_previous_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::move_to_previous_input, mini_console);

    // Callback code for key down: move to next input.
    yli::console::ConsoleCallbackEngine mini_move_to_next_input_callback_engine;
    mini_move_to_next_input_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::move_to_next_input, mini_console);

    // Callback code for backspace: delete character left of cursor from current input in console.
    yli::console::ConsoleCallbackEngine mini_backspace_callback_engine;
    mini_backspace_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::backspace, mini_console);

    // Callback code for Enter key.
    yli::console::ConsoleCallbackEngine mini_enter_callback_engine;
    mini_enter_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::enter_key, mini_console);

    // Callback code for C: Control-C.
    yli::console::ConsoleCallbackEngine mini_ctrl_c_callback_engine;
    mini_ctrl_c_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::ctrl_c, mini_console);

    // Callback code for W: Control-W.
    yli::console::ConsoleCallbackEngine mini_ctrl_w_callback_engine;
    mini_ctrl_w_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::ctrl_w, mini_console);

    // Callback code for PgUp.
    yli::console::ConsoleCallbackEngine mini_page_up_callback_engine;
    mini_page_up_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::page_up, mini_console);

    // Callback code for PgDn.
    yli::console::ConsoleCallbackEngine mini_page_down_callback_engine;
    mini_page_down_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::page_down, mini_console);

    // Callback code for Home.
    yli::console::ConsoleCallbackEngine mini_home_callback_engine;
    mini_home_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::home, mini_console);

    // Callback code for End.
    yli::console::ConsoleCallbackEngine mini_end_callback_engine;
    mini_end_callback_engine.create_ConsoleCallbackObject(&yli::ontology::Console::end, mini_console);

    // Keyrelease callbacks for action mode.
    // Key releases are checked in the order of this struct.
    yli::input::InputMode* const action_mode_input_mode = input_master->create_InputMode();
    action_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LCTRL, &release_first_turbo_callback_engine);
    action_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RCTRL, &release_second_turbo_callback_engine);
    action_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_I, &enable_toggle_invert_mouse_callback_engine);
    action_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_F, &enable_toggle_flight_mode_callback_engine);
    action_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_F1, &enable_toggle_help_mode_callback_engine);

    // Keypress callbacks for action mode.
    // Keypresses are checked in the order of this struct.
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_GRAVE, &enter_console_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_ESCAPE, &exit_program_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LCTRL, &first_turbo_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RCTRL, &second_turbo_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_I, &toggle_invert_mouse_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_F, &toggle_flight_mode_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_F1, &toggle_help_mode_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_D, &delete_suzanne_species_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_G, &switch_to_grass_material_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_O, &switch_to_orange_fur_material_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_P, &switch_to_pink_geometric_tiles_material_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_T, &transform_into_terrain_callback_engine);
    action_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_A, &transform_into_monkey_callback_engine);

    // Continuous keypress callbacks for action mode.
    // Keypresses are checked in the order of this struct.
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_UP, &move_forward_callback_engine);
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_DOWN, &move_backward_callback_engine);
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_LEFT, &strafe_left_callback_engine);
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_RIGHT, &strafe_right_callback_engine);
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_SPACE, &ascent_callback_engine);
    action_mode_input_mode->set_continuous_keypress_callback_engine(SDL_SCANCODE_RETURN, &descent_callback_engine);
    action_mode_input_mode->activate();

    // Keyrelease callbacks for `my_console`.
    // Key releases are checked in the order of this struct.
    yli::input::InputMode* const my_console_mode_input_mode = input_master->create_InputMode();
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LCTRL, &my_release_left_control_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RCTRL, &my_release_right_control_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LALT, &my_release_left_alt_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RALT, &my_release_right_alt_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LSHIFT, &my_release_left_shift_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RSHIFT, &my_release_right_shift_in_console_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_UP, &my_enable_move_to_previous_input_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_DOWN, &my_enable_move_to_next_input_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_BACKSPACE, &my_enable_backspace_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RETURN, &my_enable_enter_key_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_C, &my_enable_ctrl_c_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_W, &my_enable_ctrl_w_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_PAGEUP, &my_enable_page_up_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_PAGEDOWN, &my_enable_page_down_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_HOME, &my_enable_home_callback_engine);
    my_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_END, &my_enable_end_callback_engine);

    // Keypress callbacks for `my_console`.
    // Keypresses are checked in the order of this struct.
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_GRAVE, &my_exit_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LCTRL, &my_press_left_control_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RCTRL, &my_press_right_control_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LALT, &my_press_left_alt_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RALT, &my_press_right_alt_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LSHIFT, &my_press_left_shift_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RSHIFT, &my_press_right_shift_in_console_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_UP, &my_move_to_previous_input_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_DOWN, &my_move_to_next_input_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_BACKSPACE, &my_backspace_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RETURN, &my_enter_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_C, &my_ctrl_c_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_W, &my_ctrl_w_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_PAGEUP, &my_page_up_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_PAGEDOWN, &my_page_down_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_HOME, &my_home_callback_engine);
    my_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_END, &my_end_callback_engine);
    my_console->set_input_mode(my_console_mode_input_mode);

    // Keyrelease callbacks for `mini_console`.
    // Key releases are checked in the order of this struct.
    yli::input::InputMode* const mini_console_mode_input_mode = input_master->create_InputMode();
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LCTRL, &mini_release_left_control_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RCTRL, &mini_release_right_control_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LALT, &mini_release_left_alt_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RALT, &mini_release_right_alt_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_LSHIFT, &mini_release_left_shift_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RSHIFT, &mini_release_right_shift_in_console_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_UP, &mini_enable_move_to_previous_input_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_DOWN, &mini_enable_move_to_next_input_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_BACKSPACE, &mini_enable_backspace_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_RETURN, &mini_enable_enter_key_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_C, &mini_enable_ctrl_c_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_W, &mini_enable_ctrl_w_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_PAGEUP, &mini_enable_page_up_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_PAGEDOWN, &mini_enable_page_down_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_HOME, &mini_enable_home_callback_engine);
    mini_console_mode_input_mode->set_keyrelease_callback_engine(SDL_SCANCODE_END, &mini_enable_end_callback_engine);

    // Keypress callbacks for `mini_console`.
    // Keypresses are checked in the order of this struct.
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_GRAVE, &mini_exit_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LCTRL, &mini_press_left_control_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RCTRL, &mini_press_right_control_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LALT, &mini_press_left_alt_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RALT, &mini_press_right_alt_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_LSHIFT, &mini_press_left_shift_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RSHIFT, &mini_press_right_shift_in_console_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_UP, &mini_move_to_previous_input_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_DOWN, &mini_move_to_next_input_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_BACKSPACE, &mini_backspace_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_RETURN, &mini_enter_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_C, &mini_ctrl_c_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_W, &mini_ctrl_w_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_PAGEUP, &mini_page_up_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_PAGEDOWN, &mini_page_down_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_HOME, &mini_home_callback_engine);
    mini_console_mode_input_mode->set_keypress_callback_engine(SDL_SCANCODE_END, &mini_end_callback_engine);
    mini_console->set_input_mode(mini_console_mode_input_mode);

    /*********************************************************************
     * Callback engines for console commands begin here.                 *
     * These define what commands there are available in the console     *
     * and what each command does.                                       *
     *********************************************************************/

    std::cout << "Defining console command callback engines.\n";

    // Config callbacks.
    my_console->add_command_callback("set", &yli::config::SettingMaster::set_and_print);
    my_console->add_command_callback("get", &yli::config::SettingMaster::get_and_print);

    // `Entity` handling callbacks.
    my_console->add_command_callback("activate", &yli::ontology::Universe::activate);
    my_console->add_command_callback("delete", &yli::ontology::Universe::delete_entity);
    my_console->add_command_callback("info", &yli::ontology::Universe::info);
    my_console->add_command_callback("bind", &yli::ontology::Universe::bind);
    my_console->add_command_callback("AnyValueEntity", &yli::ontology::Universe::create_AnyValue);
    my_console->add_command_callback("AnyStructEntity", &yli::ontology::Universe::create_AnyStruct);

    // Exit program callbacks.
    my_console->add_command_callback("bye", &app::quit);
    my_console->add_command_callback("chau", &app::quit);
    my_console->add_command_callback("ciao", &app::quit);
    my_console->add_command_callback("heippa", &app::quit);
    my_console->add_command_callback("quit", &app::quit);
    my_console->add_command_callback("sayonara", &app::quit);

    // Other callbacks.
    my_console->add_command_callback("eval", &yli::ontology::Universe::eval);
    my_console->add_command_callback("help", &app::help);
    my_console->add_command_callback("version", &ajokki::version);
    my_console->add_command_callback("clear", &yli::ontology::Console::clear);
    my_console->add_command_callback("screenshot", &yli::ontology::Universe::screenshot);

    // mini-console callbacks.
    mini_console->add_command_callback("activate", &yli::ontology::Universe::activate);
    mini_console->add_command_callback("info", &yli::ontology::Universe::info);

    bool has_mouse_focus = true;

    audio_master->add_to_playlist("Ajokki_playlist", "414257__sss-samples__chipland-loop-120-bpm-a-major.wav");
    audio_master->add_to_playlist("Ajokki_playlist", "414270__greek555__sample-97-bpm.wav");
    audio_master->play_playlist("Ajokki_playlist");

    SDL_Event sdl_event;
    std::string ms_frame_text;

    // Print angles and cartesian coordinates on bottom left corner.
    yli::ontology::TextStruct angles_and_coordinates_text_struct;
    angles_and_coordinates_text_struct.font2D_parent = my_font2D;
    angles_and_coordinates_text_struct.screen_width = my_universe->get_window_width();
    angles_and_coordinates_text_struct.screen_height = my_universe->get_window_height();
    angles_and_coordinates_text_struct.x = 0;
    angles_and_coordinates_text_struct.y = 0;
    angles_and_coordinates_text_struct.text_size = my_universe->get_text_size();
    angles_and_coordinates_text_struct.font_size = my_universe->get_font_size();
    angles_and_coordinates_text_struct.font_texture_file_format = "bmp";
    angles_and_coordinates_text_struct.horizontal_alignment = "left";
    angles_and_coordinates_text_struct.vertical_alignment = "bottom";
    yli::ontology::Text2D* angles_and_coordinates_text2D = dynamic_cast<yli::ontology::Text2D*>(entity_factory->create_Text2D(angles_and_coordinates_text_struct));

    // Print spherical coordinates on second line from the bottom left.
    yli::ontology::TextStruct spherical_coordinates_text_struct;
    spherical_coordinates_text_struct.font2D_parent = my_font2D;
    spherical_coordinates_text_struct.screen_width = my_universe->get_window_width();
    spherical_coordinates_text_struct.screen_height = my_universe->get_window_height();
    spherical_coordinates_text_struct.x = 0;
    spherical_coordinates_text_struct.y = 2 * my_universe->get_text_size();
    spherical_coordinates_text_struct.text_size = my_universe->get_text_size();
    spherical_coordinates_text_struct.font_size = my_universe->get_font_size();
    spherical_coordinates_text_struct.horizontal_alignment = "left";
    spherical_coordinates_text_struct.vertical_alignment = "bottom";
    yli::ontology::Text2D* spherical_coordinates_text2D = dynamic_cast<yli::ontology::Text2D*>(entity_factory->create_Text2D(spherical_coordinates_text_struct));

    // Print time data on top left corner.
    yli::ontology::TextStruct time_text_struct;
    time_text_struct.font2D_parent = my_font2D;
    time_text_struct.screen_width = my_universe->get_window_width();
    time_text_struct.screen_height = my_universe->get_window_height();
    time_text_struct.x = 0;
    time_text_struct.y = my_universe->get_window_height();
    time_text_struct.text_size = my_universe->get_text_size();
    time_text_struct.font_size = my_universe->get_font_size();
    time_text_struct.font_texture_file_format = "bmp";
    time_text_struct.horizontal_alignment = "left";
    time_text_struct.vertical_alignment = "top";
    yli::ontology::Text2D* time_text2D = dynamic_cast<yli::ontology::Text2D*>(entity_factory->create_Text2D(time_text_struct));

    // Print help text.
    yli::ontology::TextStruct help_text_struct;
    help_text_struct.font2D_parent = my_font2D;
    help_text_struct.screen_width = my_universe->get_window_width();
    help_text_struct.screen_height = my_universe->get_window_height();
    help_text_struct.x = 0;
    help_text_struct.y = my_universe->get_window_height() - (3 * my_universe->get_text_size());
    help_text_struct.text_size = my_universe->get_text_size();
    help_text_struct.font_size = my_universe->get_font_size();
    help_text_struct.font_texture_file_format = "bmp";
    help_text_struct.horizontal_alignment = "left";
    help_text_struct.vertical_alignment = "top";
    yli::ontology::Text2D* help_text2D = dynamic_cast<yli::ontology::Text2D*>(entity_factory->create_Text2D(help_text_struct));

    // Print frame rate data on top right corner.
    yli::ontology::TextStruct frame_rate_text_struct;
    frame_rate_text_struct.font2D_parent = my_font2D;
    frame_rate_text_struct.screen_width = my_universe->get_window_width();
    frame_rate_text_struct.screen_height = my_universe->get_window_height();
    frame_rate_text_struct.x = my_universe->get_window_width();;
    frame_rate_text_struct.y = my_universe->get_window_height();
    frame_rate_text_struct.text_size = my_universe->get_text_size();
    frame_rate_text_struct.font_size = my_universe->get_font_size();
    frame_rate_text_struct.font_texture_file_format = "bmp";
    frame_rate_text_struct.horizontal_alignment = "right";
    frame_rate_text_struct.vertical_alignment = "top";
    yli::ontology::Text2D* frame_rate_text2D = dynamic_cast<yli::ontology::Text2D*>(entity_factory->create_Text2D(frame_rate_text_struct));

    std::cout << "Setting up window size ...\n";
    app::set_window_size(my_universe->get_setting_master(), my_universe->get_window_width(), my_universe->get_window_height());
    std::cout << "Setting up framebuffer size ...\n";
    app::set_framebuffer_size(my_universe->get_setting_master(), my_universe->get_framebuffer_width(), my_universe->get_framebuffer_height());
    std::cout << "Setting up background colors ...\n";
    app::set_background_colors(my_universe->get_setting_master(), 0.0f, 0.0f, 1.0f, 0.0f);
    std::cout << "Setting up wireframe state ...\n";
    app::set_wireframe(my_universe->get_setting_master(), false);
    std::cout << "Setting up movement ...\n";
    app::set_movement(my_universe->get_setting_master(), my_universe->speed, my_universe->turbo_factor, my_universe->twin_turbo_factor, my_universe->mouse_speed);
    std::cout << "Setting up location and orientation ...\n";
    app::set_location_and_orientation(my_universe->get_setting_master(), -5682.32f, -1641.20f, 2376.45f, 100.0f, 100.0f, 100.0f);
    std::cout << "Setting up debug variables ...\n";
    app::set_flight_mode(my_universe->get_setting_master(), true);

    yli::sdl::flush_sdl_event_queue();

    while (!my_universe->get_is_exit_requested() && input_master != nullptr)
    {
        const double current_time_in_main_loop = yli::time::get_time();

        if (current_time_in_main_loop - my_universe->get_last_time_for_display_sync() >= (1.0f / my_universe->get_max_FPS()))
        {
            my_universe->update_last_time_for_display_sync();

            my_universe->increment_number_of_frames();

            while (current_time_in_main_loop - my_universe->get_last_time_to_display_FPS() >= 1.0f)
            {
                // If last `std::stringstream` here was more than 1 sec ago,
                // std::stringstream` and reset number of frames.
                if (frame_rate_text2D != nullptr && my_universe->get_number_of_frames() > 0)
                {
                    std::stringstream ms_frame_text_stringstream;
                    ms_frame_text_stringstream << std::fixed << std::setprecision(2) << 1000.0f / static_cast<double>(my_universe->get_number_of_frames()) << " ms/frame; " <<
                        my_universe->get_number_of_frames() << " Hz";
                    ms_frame_text = ms_frame_text_stringstream.str();
                    frame_rate_text2D->change_string(ms_frame_text);
                    my_universe->reset_number_of_frames();
                }

                // `last_time_to_display_FPS` needs to be incremented to avoid infinite loop.
                my_universe->increment_last_time_to_display_FPS();

                // Update audio also (in case the sound has reached the end).
                audio_master->update();
            }

            // Clear the screen.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            my_universe->compute_delta_time();

            my_universe->mouse_x = my_universe->get_window_width() / 2;
            my_universe->mouse_y = my_universe->get_window_height() / 2;

            const yli::input::InputMode* const input_mode = input_master->get_active_input_mode();

            // Poll all SDL events.
            while (SDL_PollEvent(&sdl_event))
            {
                if (sdl_event.type == SDL_MOUSEMOTION)
                {
                    my_universe->mouse_x += sdl_event.motion.xrel; // horizontal motion relative to screen center.
                    my_universe->mouse_y += sdl_event.motion.yrel; // vertical motion relative to screen center.
                }
                else if (sdl_event.type == SDL_KEYDOWN && input_mode != nullptr)
                {
                    const uint32_t scancode = static_cast<std::uint32_t>(sdl_event.key.keysym.scancode);

                    yli::callback::CallbackEngine* const callback_engine = input_mode->get_keypress_callback_engine(scancode);

                    if (callback_engine != nullptr)
                    {
                        const std::shared_ptr<yli::common::AnyValue> any_value = callback_engine->execute(nullptr);

                        if (any_value != nullptr &&
                                any_value->type == yli::common::Datatype::UINT32_T &&
                                any_value->uint32_t_value == EXIT_PROGRAM_MAGIC_NUMBER)
                        {
                            my_universe->request_exit();
                        }
                    }

                    yli::ontology::Console* const active_console = my_universe->get_active_console();

                    if (active_console != nullptr)
                    {
                        active_console->process_key_event(sdl_event.key);
                    }
                }
                else if (sdl_event.type == SDL_KEYUP && input_mode != nullptr)
                {
                    const uint32_t scancode = static_cast<std::uint32_t>(sdl_event.key.keysym.scancode);

                    yli::callback::CallbackEngine* const callback_engine = input_mode->get_keyrelease_callback_engine(scancode);

                    if (callback_engine == nullptr)
                    {
                        continue;
                    }

                    const std::shared_ptr<yli::common::AnyValue> any_value = callback_engine->execute(nullptr);

                    if (any_value != nullptr &&
                            any_value->type == yli::common::Datatype::UINT32_T &&
                            any_value->uint32_t_value == EXIT_PROGRAM_MAGIC_NUMBER)
                    {
                        my_universe->request_exit();
                    }
                }
                else if (sdl_event.type == SDL_WINDOWEVENT)
                {
                    if (sdl_event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                        has_mouse_focus = true;
                    }
                    else if (sdl_event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                    {
                        has_mouse_focus = false;
                    }
                }
                else if (sdl_event.type == SDL_QUIT)
                {
                    my_universe->request_exit();
                }
            }

            // mouse position.
            const double xpos = static_cast<double>(my_universe->mouse_x);
            const double ypos = static_cast<double>(my_universe->mouse_y);

            // Reset mouse position for next frame.
            if (has_mouse_focus)
            {
                yli::input::set_cursor_position(
                        my_universe->get_window(),
                        static_cast<double>(my_universe->get_window_width()) / 2,
                        static_cast<double>(my_universe->get_window_height()) / 2);

                if (my_universe->has_mouse_ever_moved || (std::abs(xpos) > 0.0001) || (std::abs(ypos) > 0.0001))
                {
                    my_universe->has_mouse_ever_moved = true;

                    // Compute new orientation.
                    my_universe->current_camera_horizontal_angle += my_universe->mouse_speed * static_cast<float>(my_universe->get_window_width() / 2 - xpos);
                    my_universe->current_camera_horizontal_angle = remainder(my_universe->current_camera_horizontal_angle, (2.0f * PI));

                    if (my_universe->is_invert_mouse_in_use)
                    {
                        // Invert mouse.
                        my_universe->current_camera_vertical_angle -= my_universe->mouse_speed * static_cast<float>(my_universe->get_window_height() / 2 - ypos);
                    }
                    else
                    {
                        // Don't invert mouse.
                        my_universe->current_camera_vertical_angle += my_universe->mouse_speed * static_cast<float>(my_universe->get_window_height() / 2 - ypos);
                    }

                    my_universe->current_camera_vertical_angle = remainder(my_universe->current_camera_vertical_angle, (2.0f * PI));
                }
            }

            // Direction: spherical coordinates to cartesian coordinates conversion.
            my_universe->current_camera_direction = glm::vec3(
                    cos(my_universe->current_camera_vertical_angle) * sin(my_universe->current_camera_horizontal_angle),
                    sin(my_universe->current_camera_vertical_angle),
                    cos(my_universe->current_camera_vertical_angle) * cos(my_universe->current_camera_horizontal_angle));

            // Right vector.
            my_universe->current_camera_right = glm::vec3(
                    sin(my_universe->current_camera_horizontal_angle - PI/2.0f),
                    0,
                    cos(my_universe->current_camera_horizontal_angle - PI/2.0f));

            // Up vector.
            my_universe->current_camera_up = glm::cross(my_universe->current_camera_right, my_universe->current_camera_direction);

            if (!my_universe->in_console && input_mode != nullptr)
            {
                const uint8_t* const current_key_states = SDL_GetKeyboardState(nullptr);
                const std::vector<yli::callback::CallbackEngine*>* const continuous_keypress_callback_engines = input_mode->get_continuous_keypress_callback_engines();
                if (continuous_keypress_callback_engines == nullptr)
                {
                    continue;
                }

                // Check for keypresses and call corresponding callbacks.
                for (std::size_t i = 0; i < continuous_keypress_callback_engines->size(); i++)
                {
                    bool is_pressed = false;

                    if (my_universe->get_input_method() == yli::input::InputMethod::KEYBOARD)
                    {
                        if (current_key_states[i] == 1) // 1 = pressed, 0 = not pressed.
                        {
                            is_pressed = true;
                        }
                    }
                    else if (my_universe->get_input_method() == yli::input::InputMethod::INPUT_FILE)
                    {
                        // TODO: implement optionally loading keyreleases from a file (do not execute `SDL_GetKeyboardState` in that case).
                        if (false)
                        {
                            is_pressed = true;
                        }
                    }
                    else
                    {
                        std::cerr << "ERROR: unsupported input method.\n";
                    }

                    if (is_pressed)
                    {
                        yli::callback::CallbackEngine* const callback_engine = continuous_keypress_callback_engines->at(i);

                        if (callback_engine == nullptr)
                        {
                            continue;
                        }

                        const std::shared_ptr<yli::common::AnyValue> any_value = callback_engine->execute(nullptr);

                        if (any_value != nullptr &&
                                any_value->type == yli::common::Datatype::UINT32_T &&
                                any_value->uint32_t_value == EXIT_PROGRAM_MAGIC_NUMBER)
                        {
                            my_universe->request_exit();
                        }
                    }
                }
            }

            // Gravity etc. physical phenomena.
            my_universe->do_physics();

            // Intentional actors (AIs and keyboard controlled ones).
            my_universe->act();

            if (angles_and_coordinates_text2D != nullptr)
            {
                std::stringstream angles_and_coordinates_stringstream;
                angles_and_coordinates_stringstream << std::fixed << std::setprecision(2) <<
                    my_universe->current_camera_horizontal_angle << "," <<
                    my_universe->current_camera_vertical_angle << " rad; " <<
                    RADIANS_TO_DEGREES(my_universe->current_camera_horizontal_angle) << "," <<
                    RADIANS_TO_DEGREES(my_universe->current_camera_vertical_angle) << " deg\n" <<
                    "(" <<
                    my_universe->current_camera_cartesian_coordinates.x << "," <<
                    my_universe->current_camera_cartesian_coordinates.y << "," <<
                    my_universe->current_camera_cartesian_coordinates.z << ")";
                const std::string angles_and_coordinates_string = angles_and_coordinates_stringstream.str();
                angles_and_coordinates_text2D->change_string(angles_and_coordinates_string);
            }

            if (time_text2D != nullptr)
            {
                std::stringstream time_stringstream;
                time_stringstream << std::fixed << std::setprecision(2) << yli::time::get_time() << " sec";
                const std::string time_string = time_stringstream.str();
                time_text2D->change_string(time_string);
            }

            const std::string on_string = "on";
            const std::string off_string = "off";

            if (help_text2D != nullptr)
            {
                if (my_universe->in_help_mode && my_universe->can_display_help_screen)
                {
                    std::stringstream help_text_stringstream;
                    yli::ontology::World* const world = my_universe->get_active_world();
                    yli::ontology::Scene* const scene = (world == nullptr ? nullptr : world->get_active_scene());
                    help_text_stringstream <<
                        "Ajokki " << yli::ontology::Universe::version << "\n"
                        "\n"
                        "arrow keys\n"
                        "space jump\n"
                        "enter duck\n"
                        "F1 help mode\n"
                        "`  enter console\n"
                        "I  invert mouse (" << (my_universe->is_invert_mouse_in_use ? on_string : off_string) << ")\n"
                        "F  flight mode (" << (scene == nullptr || scene->get_is_flight_mode_in_use() ? on_string : off_string) << ")\n"
                        "Ctrl      turbo\n" <<
                        "Ctrl+Ctrl extra turbo\n" <<
                        "for debugging:\n" <<
                        "G  grass texture\n" <<
                        "O  orange fur texture\n" <<
                        "P  pink geometric tiles texture\n" <<
                        "T  terrain species\n" <<
                        "A  suzanne species\n";
                    const std::string help_text_string = help_text_stringstream.str();
                    help_text2D->change_string(help_text_string);
                }
                else
                {
                    help_text2D->change_string("");
                }
            }

            if (spherical_coordinates_text2D != nullptr)
            {
                if (my_universe->testing_spherical_terrain_in_use)
                {
                    std::stringstream spherical_coordinates_stringstream;
                    spherical_coordinates_stringstream << std::fixed << std::setprecision(2) <<
                        "rho:" << my_universe->current_camera_spherical_coordinates.rho <<
                        "theta:" << my_universe->current_camera_spherical_coordinates.theta <<
                        "phi:" << my_universe->current_camera_spherical_coordinates.phi;
                    std::string spherical_coordinates_string = spherical_coordinates_stringstream.str();
                    spherical_coordinates_text2D->change_string(spherical_coordinates_string);
                }
                else
                {
                    spherical_coordinates_text2D->change_string("");
                }
            }

            // Render the `Universe`.
            my_universe->render();

            my_universe->finalize_delta_time_loop();
        }
    }

    // Do cleanup.
    cleanup_callback_engine.execute(nullptr);

    return 0;
}
