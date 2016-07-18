#ifndef __CALLBACK_ENGINE_HPP_INCLUDED
#define __CALLBACK_ENGINE_HPP_INCLUDED

#include "custom_namespace.hpp"
#include "cpp/ylikuutio/common/any_value.hpp"
#include "cpp/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <queue>         // std::queue
#include <stdint.h>      // uint32_t etc.
#include <string>        // std::string
#include <vector>        // std::vector

namespace ontology
{
    class Universe;
}

// callback typedefs in alphabetical order.
typedef void (*VoidToVoidCallback)(void);
typedef void (*AnyValueToVoidCallback)(datatypes::AnyValue);
typedef datatypes::AnyValue (*VoidToAnyValueCallback)(void);
typedef datatypes::AnyValue (*AnyValueToAnyValueCallback)(datatypes::AnyValue);

namespace callback_system
{
    class CallbackObject;

    class CallbackEngine: public CustomNamespace
    {
        // `CallbackEngine` is an object that contains some callbacks and hashmaps that are used for input and output parameters.
        // `CallbackEngine` provides a way to create callback chains.
        //
        // Hierarchy of callbacks:
        //
        //     CallbackEngine
        //           ^
        //     CallbackObject
        //           ^
        //   CallbackParameter
        //
        // How to use.
        // 1. Create a new `CallbackEngine`. No callbacks have been
        //    defined yet. Calling `CallbackEngine.execute()` at this
        //    point will simply go through an empty vector and
        //    practically won't do anything interesting.
        // 2. Create a new `CallbackObject`, give pointer to the
        //    recently created `CallbackEngine` as input parameter.
        // 3. If the callback has parameter[s], create a new
        //    `CallbackParameter` for each parameter, give `CallbackObject`
        //    as input parameter for the `CallbackParameter` constructor.

        public:
            // constructor.
            CallbackEngine();

            // destructor.
            ~CallbackEngine();

            // execute all callbacks.
            datatypes::AnyValue* execute();

            friend class CallbackObject;
            template<class T1>
                friend void hierarchy::bind_child_to_parent(T1 child_pointer, std::vector<void*> &child_pointer_vector, std::queue<uint32_t> &free_childID_queue);
            template<class T1, class T2>
                friend void hierarchy::bind_child_to_new_parent(T1 child_pointer, T2 new_parent_pointer, std::vector<void*> &old_child_pointer_vector, std::queue<uint32_t> &old_free_childID_queue);

        private:
            // this method sets a callback object pointer.
            void set_callback_object_pointer(uint32_t childID, void* parent_pointer);

            // getter functions for callbacks and callback objects.
            datatypes::AnyValue get_any_value(std::string name);
            bool get_bool(std::string name);
            float get_float(std::string name);
            double get_double(std::string name);
            int32_t get_int32_t(std::string name);
            uint32_t get_uint32_t(std::string name);
            void* get_void_pointer(std::string name);

            // setter functions for callbacks and callback objects.
            void set_bool(std::string name, bool value);
            void set_float(std::string name, float value);
            void set_double(std::string name, double value);
            void set_int32_t(std::string name, int32_t value);
            void set_uint32_t(std::string name, uint32_t value);
            void set_void_pointer(std::string name, void* value);
            void set_universe_pointer(std::string name, ontology::Universe* value);

            std::vector<void*> callback_object_pointer_vector;
            std::queue<uint32_t> free_callback_objectID_queue;
    };
}

#endif
