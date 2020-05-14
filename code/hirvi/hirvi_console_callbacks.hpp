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

#ifndef __HIRVI_CONSOLE_CALLBACKS_HPP_INCLUDED
#define __HIRVI_CONSOLE_CALLBACKS_HPP_INCLUDED

// Include standard headers
#include <memory>   // std::make_shared, std::shared_ptr

namespace yli::data
{
    class AnyValue;
}

namespace yli::ontology
{
    class Universe;
    class Console;
}

namespace hirvi
{
    std::shared_ptr<yli::data::AnyValue> version(
            yli::ontology::Universe* const universe,
            yli::ontology::Console* const console);
}

#endif
