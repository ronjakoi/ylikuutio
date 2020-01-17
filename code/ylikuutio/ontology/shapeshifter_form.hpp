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

#ifndef __SHAPESHIFTER_FORM_HPP_INCLUDED
#define __SHAPESHIFTER_FORM_HPP_INCLUDED

#include "model.hpp"
#include "species_struct.hpp"

// Include standard headers
#include <cstddef>  // std::size_t

// `ShapeshifterForm` is a `Model` and a part of `ShapeshifterTransformation`.

namespace yli
{
    namespace ontology
    {
        class Entity;
        class Universe;
        class ShapeshifterTransformation;

        class ShapeshifterForm: public yli::ontology::Model
        {
            public:
                // constructor.
                ShapeshifterForm(yli::ontology::Universe* const universe, const yli::ontology::SpeciesStruct& species_struct)
                    : Model(universe, species_struct.opengl_in_use)
                {
                    // constructor.
                    this->parent = species_struct.shapeshifter_transformation;

                    // get `childID` from `ShapeshifterTransformation` and set pointer to this `ShapeshifterForm`.
                    this->bind_to_parent();

                    // `yli::ontology::Entity` member variables begin here.
                    this->type_string = "yli::ontology::ShapeshifterForm*";
                }

                // destructor.
                virtual ~ShapeshifterForm();

                yli::ontology::Entity* get_parent() const override;
                std::size_t get_number_of_children() const override;
                std::size_t get_number_of_descendants() const override;

            private:
                void bind_to_parent();

                void render();

                yli::ontology::ShapeshifterTransformation* parent;
        };
    }
}

#endif