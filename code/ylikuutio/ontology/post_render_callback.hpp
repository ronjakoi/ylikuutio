#ifndef __POST_RENDER_CALLBACK_HPP_INCLUDED
#define __POST_RENDER_CALLBACK_HPP_INCLUDED

// Include standard headers
#include <memory>   // std::make_shared, std::shared_ptr

namespace yli
{
    namespace datatypes
    {
        class AnyValue;
    }

    namespace ontology
    {
        class Entity;
    }

    namespace config
    {
        class SettingMaster;
    }
}

typedef std::shared_ptr<yli::datatypes::AnyValue> (*PreRenderCallback) (yli::ontology::Entity* entity, yli::config::SettingMaster* setting_master);

#endif