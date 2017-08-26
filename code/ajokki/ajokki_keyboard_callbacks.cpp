#include "ajokki_keyboard_callbacks.hpp"
#include "code/ylikuutio/callback_system/callback_parameter.hpp"
#include "code/ylikuutio/callback_system/callback_object.hpp"
#include "code/ylikuutio/callback_system/callback_engine.hpp"
#include "code/ylikuutio/callback_system/callback_magic_numbers.hpp"
#include "code/ylikuutio/console/console.hpp"
#include "code/ylikuutio/ontology/object.hpp"
#include "code/ylikuutio/ontology/species.hpp"
#include "code/ylikuutio/ontology/material.hpp"
#include "code/ylikuutio/ontology/universe.hpp"
#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/common/globals.hpp"

// Include GLFW
#ifndef __GLFW3_H_INCLUDED
#define __GLFW3_H_INCLUDED
#include <glfw3.h>
#endif

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <memory>   // std::make_shared, std::shared_ptr
#include <vector>   // std::vector

namespace ajokki
{
    bool move_to_direction(callback_system::CallbackObject* callback_object, glm::vec3 moving_direction)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return false;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return false;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return false;
        }

        GLfloat temp_speed;

        if (universe->is_first_turbo_pressed && universe->is_second_turbo_pressed)
        {
            temp_speed = universe->twin_turbo_factor * universe->speed;
        }
        else if (universe->is_first_turbo_pressed || universe->is_second_turbo_pressed)
        {
            temp_speed = universe->turbo_factor * universe->speed;
        }
        else
        {
            temp_speed = universe->speed;
        }
        *universe->cartesian_coordinates += temp_speed * universe->get_delta_time() * moving_direction;

        return true;
    }

    /*********************************************************************\
     *  Callback engines for action mode keyreleases begin here.         *
    \*********************************************************************/

    std::shared_ptr<datatypes::AnyValue> release_first_turbo(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->is_first_turbo_pressed = false;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> release_second_turbo(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->is_second_turbo_pressed = false;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> enable_toggle_invert_mouse(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->can_toggle_invert_mouse = true;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> enable_toggle_flight_mode(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->can_toggle_flight_mode = true;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> enable_toggle_help_mode(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->can_toggle_help_mode = true;
        return nullptr;
    }

    /*********************************************************************\
     *  Callback engines for action mode keypresses begin here.          *
    \*********************************************************************/

    std::shared_ptr<datatypes::AnyValue> exit_program(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject*,
            std::vector<callback_system::CallbackParameter*>&)
    {
        uint32_t exit_program_magic_number = EXIT_PROGRAM_MAGIC_NUMBER;
        return std::make_shared<datatypes::AnyValue>(exit_program_magic_number);
    }

    std::shared_ptr<datatypes::AnyValue> first_turbo(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->is_first_turbo_pressed = true;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> second_turbo(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        universe->is_second_turbo_pressed = true;
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> move_forward(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, universe->direction);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> move_backward(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, -universe->direction);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> strafe_left(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, -universe->right);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> strafe_right(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, universe->right);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> ascent(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, universe->up);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> descent(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        ajokki::move_to_direction(callback_object, -universe->up);
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> toggle_invert_mouse(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        if (universe->can_toggle_invert_mouse)
        {
            universe->is_invert_mouse_in_use = !universe->is_invert_mouse_in_use;
            universe->can_toggle_invert_mouse = false;
        }
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> toggle_flight_mode(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        if (universe->can_toggle_flight_mode)
        {
            universe->is_flight_mode_in_use = !universe->is_flight_mode_in_use;
            universe->fall_speed = 0.0f;
            universe->can_toggle_flight_mode = false;
        }
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> toggle_help_mode(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_universe_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_arg(0));

        if (any_value_universe_pointer == nullptr)
        {
            std::cerr << "Error: universe_pointer not found!\n";
            return nullptr;
        }

        if (any_value_universe_pointer->type != datatypes::UNIVERSE_POINTER)
        {
            std::cerr << "Invalid datatype: " << any_value_universe_pointer->type << ", should be " << datatypes::UNIVERSE_POINTER << "\n";
            return nullptr;
        }

        ontology::Universe* universe = any_value_universe_pointer->universe_pointer;

        if (universe == nullptr)
        {
            std::cerr << "Error: universe_pointer is nullptr!\n";
            return nullptr;
        }

        if (universe->can_toggle_help_mode)
        {
            universe->in_help_mode = !universe->in_help_mode;
            universe->can_toggle_help_mode = false;
        }
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> delete_suzanne_species(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        // Callbacks' input parameters can be accessed either through
        // `callback_system::CallbackObject* callback_object`or
        // `std::vector<callback_system::CallbackParameter*>& input parameters`.
        //
        // To access named input variables:
        // `datatypes::AnyValue* some_any_value = callback_object->get_any_value("foo");`
        // where `"foo"` is the variable name.
        //
        // To access input variables without name (this works for named input variables too):
        // `datatypes::AnyValue* some_any_value = input_parameters.at(foo)->get_any_value();`
        // where `foo` is the zero-based index of the variable. First `CallbackParameter` of
        // a `CallbackObject` gets index 0, second `CallbackParameter` gets index 1, etc.

        std::shared_ptr<datatypes::AnyValue> any_value_species_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("suzanne_species"));
        std::shared_ptr<datatypes::AnyValue> any_value_bool_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("does_suzanne_species_exist"));

        if (any_value_species_pointer->type != datatypes::SPECIES_POINTER)
        {
            // `any_value_species_pointer` did not contain `ontology::Species*`.
            return nullptr;
        }

        if (any_value_bool_pointer->type != datatypes::BOOL_POINTER)
        {
            // `any_value_bool_pointer` did not contain `bool*`.
            return nullptr;
        }

        // OK, `any_value_species_pointer` contained `ontology::Species*` and
        // `any_value_bool_pointer` contained a `bool*`.
        bool* does_suzanne_species_exist = any_value_bool_pointer->bool_pointer;

        if (*does_suzanne_species_exist)
        {
            ontology::Species* species = any_value_species_pointer->species_pointer;
            delete species;

            *does_suzanne_species_exist = false;
        }
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> switch_to_new_material(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject* callback_object,
            std::vector<callback_system::CallbackParameter*>&)
    {
        std::shared_ptr<datatypes::AnyValue> any_value_species_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("suzanne_species"));
        std::shared_ptr<datatypes::AnyValue> any_value_material_pointer = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("new_material"));
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_exist = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("does_suzanne_species_exist"));
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_have_original_texture = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("does_suzanne_species_have_old_texture"));
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_have_new_texture = std::make_shared<datatypes::AnyValue>(*callback_object->get_any_value("does_suzanne_species_have_new_texture"));

        if (any_value_species_pointer->type != datatypes::SPECIES_POINTER)
        {
            // `any_value_species_pointer` did not contain `ontology::Species*`.
            return nullptr;
        }

        if (any_value_material_pointer->type != datatypes::MATERIAL_POINTER)
        {
            // `any_value_material_pointer` did not contain `ontology::Material*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_exist->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_exist` did not contain `bool*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_have_original_texture->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_have_original_texture` did not contain `bool*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_have_new_texture->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_have_new_texture` did not contain `bool*`.
            return nullptr;
        }

        bool* does_suzanne_species_exist = any_value_does_suzanne_species_exist->bool_pointer;
        bool* does_suzanne_species_have_original_texture = any_value_does_suzanne_species_have_original_texture->bool_pointer;
        bool* does_suzanne_species_have_new_texture = any_value_does_suzanne_species_have_new_texture->bool_pointer;

        if (*does_suzanne_species_exist && *does_suzanne_species_have_original_texture)
        {
            ontology::Species* species = any_value_species_pointer->species_pointer;
            ontology::Material* material = any_value_material_pointer->material_pointer;
            species->bind_to_new_parent(material);

            *does_suzanne_species_have_original_texture = false;
            *does_suzanne_species_have_new_texture = true;
        }
        return nullptr;
    }

    std::shared_ptr<datatypes::AnyValue> transform_into_new_species(
            callback_system::CallbackEngine*,
            callback_system::CallbackObject*,
            std::vector<callback_system::CallbackParameter*>& input_parameters)
    {
        // This serves as an example of how to use indexed input parameters.
        std::shared_ptr<datatypes::AnyValue> any_value_object_pointer = std::make_shared<datatypes::AnyValue>(*input_parameters.at(0)->get_any_value());
        std::shared_ptr<datatypes::AnyValue> any_value_species_pointer = std::make_shared<datatypes::AnyValue>(*input_parameters.at(1)->get_any_value());
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_exist = std::make_shared<datatypes::AnyValue>(*input_parameters.at(2)->get_any_value());
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_belong_to_original_species = std::make_shared<datatypes::AnyValue>(*input_parameters.at(3)->get_any_value());
        std::shared_ptr<datatypes::AnyValue> any_value_does_suzanne_species_belong_to_new_species = std::make_shared<datatypes::AnyValue>(*input_parameters.at(4)->get_any_value());

        if (any_value_object_pointer->type != datatypes::OBJECT_POINTER)
        {
            // `any_value_object_pointer` did not contain `ontology::Object*`.
            return nullptr;
        }

        if (any_value_species_pointer->type != datatypes::SPECIES_POINTER)
        {
            // `any_value_species_pointer` did not contain `ontology::Species*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_exist->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_exist` did not contain `bool*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_belong_to_original_species->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_belong_to_original_species` did not contain `bool*`.
            return nullptr;
        }

        if (any_value_does_suzanne_species_belong_to_new_species->type != datatypes::BOOL_POINTER)
        {
            // `any_value_does_suzanne_species_belong_to_new_species` did not contain `bool*`.
            return nullptr;
        }

        bool* does_suzanne_species_exist = any_value_does_suzanne_species_exist->bool_pointer;
        bool* does_suzanne_species_belong_to_original_species = any_value_does_suzanne_species_belong_to_original_species->bool_pointer;
        bool* does_suzanne_species_belong_to_new_species = any_value_does_suzanne_species_belong_to_new_species->bool_pointer;

        if (*does_suzanne_species_exist && *does_suzanne_species_belong_to_original_species)
        {
            ontology::Object* object = any_value_object_pointer->object_pointer;
            ontology::Species* species = any_value_species_pointer->species_pointer;
            object->bind_to_new_parent(species);

            *does_suzanne_species_belong_to_original_species = false;
            *does_suzanne_species_belong_to_new_species = true;
        }
        return nullptr;
    }
}
