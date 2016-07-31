#ifndef __CONSOLE_CALLBACK_OBJECT_HPP_INCLUDED
#define __CONSOLE_CALLBACK_OBJECT_HPP_INCLUDED

#include "console.hpp"
#include "cpp/ylikuutio/callback_system/callback_object.hpp"
#include "cpp/ylikuutio/callback_system/callback_engine.hpp"
#include "cpp/ylikuutio/common/globals.hpp"

namespace callback_system
{
    class CallbackParameter;
}

namespace console
{
    class ConsoleCallbackObject : public callback_system::CallbackObject
    {
        public:
            // constructor.
            ConsoleCallbackObject(InputParametersToAnyValueCallbackWithConsole console_callback,
                    callback_system::CallbackEngine* parent_pointer, console::Console* console_pointer)
                : callback_system::CallbackObject(nullptr, parent_pointer)
            {
                // constructor.
                this->callback = nullptr;
                this->console_callback = console_callback;
                this->console_pointer = console_pointer;
            }

            // destructor.
            ~ConsoleCallbackObject();

        private:
            // execute this callback.
            datatypes::AnyValue* execute();

            InputParametersToAnyValueCallbackWithConsole console_callback;
            console::Console* console_pointer;
    };
}

#endif
