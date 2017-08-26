#ifndef __SETTING_MASTER_HPP_INCLUDED
#define __SETTING_MASTER_HPP_INCLUDED

#include "code/ylikuutio/console/console.hpp"
#include "code/ylikuutio/ontology/universe.hpp"
#include "code/ylikuutio/common/any_value.hpp"

// Include standard headers
#include <memory>        // std::make_shared, std::shared_ptr
#include <queue>         // std::queue
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace config
{
    class Setting;

    class SettingMaster
    {
        public:
            // constructor.
            SettingMaster(ontology::Universe* const universe_pointer);

            // destructor.
            ~SettingMaster();

            // does setting `setting` exist?
            bool is_setting(const std::string setting_name);

            // this function returns general help.
            std::string help();

            // this function returns help string for setting `setting`.
            std::string help(const std::string setting_name);

            bool set(std::string& setting_name, std::shared_ptr<datatypes::AnyValue> setting_new_any_value);

            config::Setting* get(std::string& setting_name);

            // Public callbacks.

            static std::shared_ptr<datatypes::AnyValue> set_and_print(
                    console::Console* console,
                    ontology::Universe* universe,
                    std::vector<std::string>& command_parameters);

            static std::shared_ptr<datatypes::AnyValue> get_and_print(
                    console::Console* console,
                    ontology::Universe* universe,
                    std::vector<std::string>& command_parameters);

            static std::shared_ptr<datatypes::AnyValue> activate_world_radius(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_background_color(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_spherical_coordinates(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_rho(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_theta(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_phi(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_cartesian_coordinates(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_x(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_y(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> activate_z(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_horizontal_angle(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_vertical_angle(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_speed(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_turbo_factor(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_twin_turbo_factor(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_mouse_speed(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_is_flight_mode_in_use(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> activate_testing_spherical_world_in_use(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> read_x(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_y(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_z(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> read_rho(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_theta(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_phi(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> read_up(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_right(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> read_horizontal_angle(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_vertical_angle(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            static std::shared_ptr<datatypes::AnyValue> read_is_flight_mode_in_use(ontology::Universe* const universe, config::SettingMaster* const setting_master);
            static std::shared_ptr<datatypes::AnyValue> read_testing_spherical_world_in_use(ontology::Universe* const universe, config::SettingMaster* const setting_master);

            // Public callbacks end here.

            friend class Setting;
            friend class ontology::Universe;
            friend class console::Console;

        private:
            ontology::Universe* parent_pointer;

            std::vector<config::Setting*> setting_pointer_vector;
            std::queue<uint32_t> free_settingID_queue;
            int32_t number_of_settings;

            std::unordered_map<std::string, config::Setting*> setting_pointer_map;
    };
}

#endif