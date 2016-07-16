#include "any_value.hpp"

// Include standard headers
#include <cmath>    // NAN, std::isnan, std::pow
#include <cstring>  // std::memcmp, std::strcmp, std::strlen, std::strncmp
#include <string>   // std::string
#include <stdint.h> // uint32_t etc.

namespace ontology
{
    class World;
}

namespace datatypes
{
    void AnyValue::set_default_values()
    {
        this->type = datatypes::UNKNOWN;
        this->bool_value = false;
        this->float_value = NAN;
        this->double_value = NAN;
        this->int32_t_value = 0;
        this->uint32_t_value = 0;
        this->void_pointer = nullptr;
        this->world_pointer = nullptr;
    }

    AnyValue::AnyValue()
    {
        // constructor.
        this->set_default_values();
    }

    AnyValue::AnyValue(bool bool_value)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::BOOL;
        this->bool_value = bool_value;
    }

    AnyValue::AnyValue(std::string type, bool bool_value)
    {
        // constructor.
        this->set_default_values();

        if (std::strcmp(type.c_str(), "bool"))
        {
            this->type = datatypes::BOOL;
            this->bool_value = bool_value;
        }
    }

    AnyValue::AnyValue(float float_value)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::FLOAT;
        this->float_value = float_value;
    }

    AnyValue::AnyValue(std::string type, float float_value)
    {
        // constructor.
        this->set_default_values();

        if (std::strcmp(type.c_str(), "float"))
        {
            this->type = datatypes::FLOAT;
            this->float_value = float_value;
        }
    }

    AnyValue::AnyValue(double double_value)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::DOUBLE;
        this->double_value = double_value;
    }

    AnyValue::AnyValue(std::string type, double double_value)
    {
        // constructor.
        this->set_default_values();
        if (std::strcmp(type.c_str(), "double"))
        {
            this->type = datatypes::DOUBLE;
            this->double_value = double_value;
        }
    }

    AnyValue::AnyValue(int32_t int32_t_value)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::INT32_T;
        this->int32_t_value = int32_t_value;
    }

    AnyValue::AnyValue(std::string type, int32_t int32_t_value)
    {
        // constructor.
        this->set_default_values();
        if (std::strcmp(type.c_str(), "int32_t"))
        {
            this->type = datatypes::INT32_T;
            this->int32_t_value = int32_t_value;
        }
    }

    AnyValue::AnyValue(uint32_t uint32_t_value)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::UINT32_T;
        this->uint32_t_value = uint32_t_value;
    }

    AnyValue::AnyValue(std::string type, uint32_t uint32_t_value)
    {
        // constructor.
        this->set_default_values();
        if (std::strcmp(type.c_str(), "uint32_t"))
        {
            this->type = datatypes::UINT32_T;
            this->uint32_t_value = uint32_t_value;
        }
    }

    AnyValue::AnyValue(void* void_pointer)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::VOID_POINTER;
        this->void_pointer = void_pointer;
    }

    AnyValue::AnyValue(std::string type, void* void_pointer)
    {
        // constructor.
        this->set_default_values();
        if (std::strcmp(type.c_str(), "void*"))
        {
            this->type = datatypes::VOID_POINTER;
            this->void_pointer = void_pointer;
        }
    }

    AnyValue::AnyValue(ontology::World* world_pointer)
    {
        // constructor.
        this->set_default_values();
        this->type = datatypes::WORLD_POINTER;
        this->world_pointer = world_pointer;
    }

    AnyValue::AnyValue(std::string type, ontology::World* world_pointer)
    {
        // constructor.
        this->set_default_values();
        if (std::strcmp(type.c_str(), "ontology::World*"))
        {
            this->type = datatypes::WORLD_POINTER;
            this->world_pointer = world_pointer;
        }
    }
}
