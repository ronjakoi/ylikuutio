#include "entity.hpp"
#include "universe.hpp"
#include "code/ylikuutio/config/setting_master.hpp"
#include "code/ylikuutio/config/setting.hpp"
#include "code/ylikuutio/config/setting_struct.hpp"
#include "code/ylikuutio/common/any_value.hpp"

// Include standard headers
#include <iostream>      // std::cout, std::cin, std::cerr
#include <limits>        // std::numeric_limits
#include <memory>        // std::make_shared, std::shared_ptr
#include <string>        // std::string
#include <unordered_map> // std::unordered_map

namespace yli
{
    namespace ontology
    {
        void Entity::bind_to_new_parent(yli::ontology::Entity* new_entity_parent)
        {
            // Do nothing.
            // `yli::ontology` classes which support binding must `override`
            // this `yli::ontology::Entity` base class implementation.
        }

        Entity::Entity(yli::ontology::Universe* const universe)
        {
            // constructor.
            this->childID = std::numeric_limits<std::size_t>::max(); // `std::numeric_limits<std::size_t>::max()` means that `childID` is not defined.
            this->universe = universe;
            this->prerender_callback = nullptr;
            this->postrender_callback = nullptr;
            this->setting_master = std::make_shared<yli::config::SettingMaster>(this);
            this->can_be_erased = false;
            this->should_be_rendered = true;

            SettingStruct should_be_rendered_setting_struct(std::make_shared<yli::datatypes::AnyValue>(this->should_be_rendered));
            should_be_rendered_setting_struct.name = "should_be_rendered";
            should_be_rendered_setting_struct.setting_master = this->get_setting_master();
            should_be_rendered_setting_struct.activate_callback = &yli::config::SettingMaster::activate_should_be_rendered;
            should_be_rendered_setting_struct.read_callback = &yli::config::SettingMaster::read_should_be_rendered;
            should_be_rendered_setting_struct.should_ylikuutio_call_activate_callback_now = true;
            std::cout << "Executing `new yli::config::Setting(should_be_rendered_setting_struct);` ...\n";
            new yli::config::Setting(should_be_rendered_setting_struct);
        }

        Entity::~Entity()
        {
            // destructor.
            std::cout << "Entity destructor called.\n";

            if (this->name.empty())
            {
                std::cerr << "ERROR: `Entity::~Entity`: `this->name` is empty.\n";
                return;
            }

            if (this->universe == nullptr)
            {
                std::cerr << "ERROR: `Entity::~Entity`: `this->universe` is `nullptr`.\n";
                return;
            }

            // OK, this `Entity` had a name, so it's name shall be erased.
            this->universe->erase_entity(this->name);
        }

        std::size_t Entity::get_childID() const
        {
            return this->childID;
        }

        std::string Entity::get_type() const
        {
            return this->type_string;
        }

        bool Entity::get_can_be_erased() const
        {
            return this->can_be_erased;
        }

        yli::ontology::Universe* Entity::get_universe() const
        {
            return this->universe;
        }

        yli::config::SettingMaster* Entity::get_setting_master() const
        {
            if (this->setting_master == nullptr)
            {
                std::cerr << "ERROR: `Entity::get_setting_master`: `this->setting_master` is `nullptr`.\n";
                return nullptr;
            }

            return this->setting_master.get();
        }

        void Entity::prerender() const
        {
            // Requirements:
            // `this->prerender_callback` must not be `nullptr`.
            // `this->universe` must not be `nullptr`.
            // `this->universe->setting_master` must not be `nullptr`.

            if (this->prerender_callback != nullptr &&
                    this->universe != nullptr &&
                    this->universe->setting_master != nullptr)
            {
                this->prerender_callback(this->universe, this->universe->setting_master.get());
            }
        }

        void Entity::postrender() const
        {
            // Requirements:
            // `this->postrender_callback` must not be `nullptr`.
            // `this->universe` must not be `nullptr`.
            // `this->universe->setting_master` must not be `nullptr`.

            if (this->postrender_callback != nullptr &&
                    this->universe != nullptr &&
                    this->universe->setting_master != nullptr)
            {
                this->postrender_callback(this->universe, this->universe->setting_master.get());
            }
        }

        void Entity::set_name(const std::string& name)
        {
            // Requirements:
            // `this->universe` must not be `nullptr`.
            // `name` must not be already in use.

            if (this->universe == nullptr)
            {
                return;
            }

            if (this->universe->is_entity(name))
            {
                // The name is already in use.
                return;
            }

            this->name = name;
            this->universe->add_entity(name, this);
        }
    }
}
