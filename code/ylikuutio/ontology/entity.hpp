#ifndef __ENTITY_HPP_INCLUDED
#define __ENTITY_HPP_INCLUDED

#include "code/ylikuutio/common/globals.hpp"

// Include standard headers
#include <cstddef>  // std::size_t
#include <string>   // std::string
#include <vector>   // std::vector

namespace yli
{
    namespace config
    {
        class SettingMaster;
    }

    namespace ontology
    {
        class Universe;

        class Entity
        {
            public:
                // constructor.
                Entity(yli::ontology::Universe* const universe);

                // destructor.
                virtual ~Entity();

                std::size_t get_childID() const;
                std::string get_type() const;

                bool get_can_be_erased() const;

                yli::ontology::Universe* get_universe() const;
                yli::config::SettingMaster* get_setting_master() const;
                void set_setting_master(yli::config::SettingMaster* const setting_master);

                virtual yli::ontology::Entity* get_parent() const = 0;
                virtual std::size_t get_number_of_children() const = 0;
                virtual std::size_t get_number_of_descendants() const = 0;

                void set_name(const std::string& name);

            protected:
                void prerender() const;
                void postrender() const;

                yli::ontology::Universe* universe;          // pointer to `Universe`.
                std::shared_ptr<yli::config::SettingMaster> setting_master; // pointer to `SettingMaster`.
                std::vector<void*> child_vector_pointers_vector;
                std::size_t childID; // TODO: add checks for `std::numeric_limits<std::size_t>::max();` (invalid value).

                std::string type;

                std::string name; // name of this entity.

                bool can_be_erased;

                PreRenderCallback prerender_callback;
                PostRenderCallback postrender_callback;
        };
    }
}
#endif
