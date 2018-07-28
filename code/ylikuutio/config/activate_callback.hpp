#ifndef __ACTIVATE_CALLBACK_HPP_INCLUDED
#define __ACTIVATE_CALLBACK_HPP_INCLUDED

#include "code/ylikuutio/common/any_value.hpp"

// Include standard headers
#include <memory> // std::make_shared, std::shared_ptr

namespace yli
{
    namespace config
    {
        class SettingMaster;
    }

    namespace ontology
    {
        class Entity;
    }
}

typedef std::shared_ptr<yli::datatypes::AnyValue> (*ActivateCallback) (yli::ontology::Entity* entity, yli::config::SettingMaster* setting_master);

#endif
