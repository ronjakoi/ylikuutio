#include "callback_object.hpp"
#include "callback_parameter.hpp"
#include "code/ylikuutio/common/any_value.hpp"
#include "code/ylikuutio/common/globals.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <string>   // std::string
#include <vector>   // std::vector

namespace callback_system
{
    // getter function for callbacks and callback objects.
    datatypes::AnyValue* CallbackObject::get_any_value(std::string name)
    {
        return &this->anyvalue_hashmap[name];
    }

    // setter function for callbacks and callback objects.
    void CallbackObject::set_any_value(std::string name, datatypes::AnyValue* any_value)
    {
        this->anyvalue_hashmap[name] = *any_value;
    }

    void CallbackObject::bind_to_parent()
    {
        hierarchy::bind_child_to_parent<callback_system::CallbackObject*>(this, this->parent_pointer->callback_object_pointer_vector, this->parent_pointer->free_callback_objectID_queue);
    }

    void CallbackObject::set_new_callback(InputParametersToAnyValueCallback callback)
    {
        this->callback = callback;
    }

    uint32_t CallbackObject::get_childID()
    {
        uint32_t childID;

        while (!this->free_callback_parameterID_queue.empty())
        {
            // return the first (oldest) free childID.
            childID = this->free_callback_parameterID_queue.front();
            this->free_callback_parameterID_queue.pop();

            // check that the child index does not exceed current child pointer vector.
            if (childID < this->callback_parameter_pointer_vector.size())
            {
                // OK, it does not exceed current child pointer vector.
                return childID;
            }
        }
        // OK, the queue is empty.
        // A new child index must be created.
        childID = this->callback_parameter_pointer_vector.size();

        // child pointer vector must also be extended with an appropriate nullptr pointer.
        this->callback_parameter_pointer_vector.push_back(nullptr);

        return childID;
    }

    void CallbackObject::bind_child_to_parent(callback_system::CallbackParameter* child_pointer)
    {
        // get childID from the parent, because every child deserves a unique ID!
        child_pointer->childID = this->get_childID();
        // set pointer to the child in parent's child pointer vector so that parent knows about children's whereabouts!
        this->set_callback_parameter_pointer(child_pointer->childID, child_pointer);
    }

    CallbackObject::CallbackObject(callback_system::CallbackEngine* parent_pointer)
    {
        // constructor.
        this->parent_pointer = parent_pointer;

        // get childID from the CallbackEngine and set pointer to this CallbackObject.
        this->bind_to_parent();
    }

    CallbackObject::CallbackObject(InputParametersToAnyValueCallback callback, callback_system::CallbackEngine* parent_pointer)
    {
        // constructor.
        this->callback = callback;
        this->parent_pointer = parent_pointer;

        // get childID from the CallbackEngine and set pointer to this CallbackObject.
        this->bind_to_parent();
    }

    CallbackObject::~CallbackObject()
    {
        std::cout << "Callback object with childID " << this->childID << " will be destroyed.\n";

        // destroy all callback parameters of this callback object.
        std::cout << "All callback parameters of this callback object will be destroyed.\n";
        for (uint32_t child_i = 0; child_i < this->callback_parameter_pointer_vector.size(); child_i++)
        {
            delete this->callback_parameter_pointer_vector[child_i];
        }
    }

    uint32_t CallbackObject::get_callback_parameterID(std::vector<callback_system::CallbackParameter*>& child_pointer_vector, std::queue<uint32_t>& free_childID_queue)
    {
        uint32_t childID;

        while (!free_childID_queue.empty())
        {
            // return the first (oldest) free childID.
            childID = free_childID_queue.front();
            free_childID_queue.pop();

            // check that the child index does not exceed current child pointer vector.
            if (childID < child_pointer_vector.size())
            {
                // OK, it does not exceed current child pointer vector.
                return childID;
            }
        }
        // OK, the queue is empty.
        // A new child index must be created.
        childID = child_pointer_vector.size();

        // child pointer vector must also be extended with an appropriate nullptr pointer.
        child_pointer_vector.push_back(nullptr);

        return childID;
    }

    void CallbackObject::set_callback_parameter_pointer(uint32_t childID, callback_system::CallbackParameter* child_pointer)
    {
        this->callback_parameter_pointer_vector[childID] = child_pointer;

        if (child_pointer == nullptr)
        {
            if (childID == this->callback_parameter_pointer_vector.size() - 1)
            {
                // OK, this is the biggest childID of all childID's of this 'object'.
                // We can reduce the size of the child pointer vector at least by 1.
                while ((!this->callback_parameter_pointer_vector.empty()) && (this->callback_parameter_pointer_vector.back() == nullptr))
                {
                    // Reduce the size of child pointer vector by 1.
                    this->callback_parameter_pointer_vector.pop_back();
                }
            }
        }
    }

    datatypes::AnyValue* CallbackObject::execute()
    {
        if (this->callback != nullptr)
        {
            return this->callback(this->parent_pointer, this, this->callback_parameter_pointer_vector);
        }
        return nullptr;
    }
}