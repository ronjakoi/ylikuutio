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

#ifndef __CALLBACK_OBJECT_HPP_INCLUDED
#define __CALLBACK_OBJECT_HPP_INCLUDED

#include "callback_engine.hpp"
#include "input_parameters_and_any_value_to_any_value_callback_with_universe.hpp"
#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <cstddef>       // std::size_t
#include <memory>        // std::make_shared, std::shared_ptr
#include <queue>         // std::queue
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <vector>        // std::vector

namespace yli
{
    namespace callback
    {
        class CallbackParameter;

        class CallbackObject
        {
            // CallbackObject is an object that contains a single callback.

            public:
                void bind_CallbackParameter(yli::callback::CallbackParameter* const callback_parameter);
                void unbind_CallbackParameter(const std::size_t childID);

                // destructor.
                virtual ~CallbackObject();

                yli::callback::CallbackParameter* create_CallbackParameter(
                        const std::string& name,
                        std::shared_ptr<yli::common::AnyValue> any_value,
                        const bool is_reference);

                // this method changes the callback without changing the parameters of CallbackObject.
                void set_new_callback(const InputParametersAndAnyValueToAnyValueCallbackWithUniverse callback);

                // getter functions for callbacks and callback objects.
                std::shared_ptr<yli::common::AnyValue> get_any_value(const std::string& name) const;
                std::shared_ptr<yli::common::AnyValue> get_arg(const std::size_t arg_i) const;

                // setter function for callbacks and callback objects.
                void set_any_value(const std::string& name, std::shared_ptr<yli::common::AnyValue> any_value);

                friend class CallbackEngine;
                friend class CallbackParameter;
                template<class T1>
                    friend void yli::hierarchy::bind_child_to_parent(T1 child_pointer, std::vector<T1>& child_pointer_vector, std::queue<std::size_t>& free_childID_queue, std::size_t& number_of_children);

            protected:
                yli::callback::CallbackEngine* parent; // pointer to the callback engine.

                std::vector<yli::callback::CallbackParameter*> callback_parameter_pointer_vector;
                std::queue<std::size_t> free_callback_parameterID_queue;
                std::size_t number_of_callback_parameters;

                InputParametersAndAnyValueToAnyValueCallbackWithUniverse callback;

                // constructor.
                CallbackObject(yli::callback::CallbackEngine* const parent);

                // constructor.
                CallbackObject(const InputParametersAndAnyValueToAnyValueCallbackWithUniverse callback, yli::callback::CallbackEngine* const parent);

            private:
                void bind_to_parent();

                // this method sets a callback parameter pointer.
                void set_callback_parameter_pointer(const std::size_t childID, yli::callback::CallbackParameter* const child_pointer);

                void bind_child_to_parent(yli::callback::CallbackParameter* child_pointer);

                // execute this callback with a parameter.
                virtual std::shared_ptr<yli::common::AnyValue> execute(std::shared_ptr<yli::common::AnyValue> any_value);

                std::size_t childID;                          // callback object ID, returned by `yli::callback::CallbackEngine->get_callback_objectID()`.

                // A hash map used to store variables.
                std::unordered_map<std::string, yli::common::AnyValue> anyvalue_hashmap;
        };
    }
}

#endif
