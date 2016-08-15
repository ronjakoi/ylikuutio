#ifndef __CALLBACK_ENGINE_HPP_INCLUDED
#define __CALLBACK_ENGINE_HPP_INCLUDED

#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

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

    class CallbackEngine
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
                friend void hierarchy::bind_child_to_parent(T1 child_pointer, std::vector<T1>& child_pointer_vector, std::queue<uint32_t>& free_childID_queue);
            template<class T1, class T2>
                friend void hierarchy::bind_child_to_new_parent(T1 child_pointer, T2 new_parent_pointer, std::vector<T1>& old_child_pointer_vector, std::queue<uint32_t>& old_free_childID_queue);

        private:
            // this method sets a callback object pointer.
            void set_callback_object_pointer(uint32_t childID, callback_system::CallbackObject* child_pointer);

            std::vector<callback_system::CallbackObject*> callback_object_pointer_vector;
            std::queue<uint32_t> free_callback_objectID_queue;
    };
}

#endif