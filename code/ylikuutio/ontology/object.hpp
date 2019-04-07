#ifndef __OBJECT_HPP_INCLUDED
#define __OBJECT_HPP_INCLUDED

#include "entity.hpp"
#include "movable.hpp"
#include "object_struct.hpp"
#include "render_templates.hpp"
#include "family_templates.hpp"
#include "code/ylikuutio/hierarchy/hierarchy_templates.hpp"

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

#ifndef __GLM_GTC_MATRIX_TRANSFORM_HPP_INCLUDED
#define __GLM_GTC_MATRIX_TRANSFORM_HPP_INCLUDED
#include <glm/gtc/matrix_transform.hpp>
#endif

#ifndef __GLM_GTC_QUATERNION_HPP_INCLUDED
#define __GLM_GTC_QUATERNION_HPP_INCLUDED
#include <glm/gtc/quaternion.hpp> // glm::quat
#endif

// Include standard headers
#include <cstddef>  // std::size_t
#include <queue>    // std::queue
#include <string>   // std::string
#include <vector>   // std::vector

namespace yli
{
    namespace ontology
    {
        class Shader;
        class Species;
        class Text3D;
        class Glyph;

        class Object: public yli::ontology::Movable
        {
            public:
                // this method sets pointer to this `Object` to `nullptr`, sets `parent` according to the input,
                // and requests a new `childID` from the new `Species` or from the new `Text3D`.
                void bind_to_new_parent(yli::ontology::Species* const new_parent);
                void bind_to_new_parent(yli::ontology::Text3D* const new_parent);
                void bind_to_new_parent(yli::ontology::Entity* const new_parent) override;

                // constructor.
                Object(yli::ontology::Universe* const universe, const ObjectStruct& object_struct)
                    : Movable(universe, object_struct.cartesian_coordinates)
                {
                    // constructor.
                    this->original_scale_vector = object_struct.original_scale_vector;
                    this->rotate_vector         = object_struct.rotate_vector;
                    this->translate_vector      = object_struct.translate_vector;
                    this->initial_rotate_vector = object_struct.initial_rotate_vector;

                    this->initial_rotate_angle  = object_struct.initial_rotate_angle;
                    this->rotate_angle          = object_struct.rotate_angle;

                    this->has_entered           = false;

                    // enable rendering of a recently entered `Object`.
                    // TODO: enable entering without enabling rendering.
                    this->should_ylikuutio_render_this_object = true;

                    this->is_character          = object_struct.is_character;
                    this->quaternions_in_use    = object_struct.quaternions_in_use;

                    if (this->is_character)
                    {
                        this->text3D_parent  = object_struct.text3D_parent;
                        this->species_parent = nullptr;
                        this->glyph          = object_struct.glyph;
                    }
                    else
                    {
                        this->species_parent = object_struct.species_parent;
                        this->text3D_parent  = nullptr;
                        this->glyph          = nullptr;
                    }

                    // get `childID` from `Species` or `Text3D` and set pointer to this `Object`.
                    this->bind_to_parent();

                    // `yli::ontology::Entity` member variables begin here.
                    this->type_string = "yli::ontology::Object*";
                    this->can_be_erased = true;
                }

                // destructor.
                virtual ~Object();

                yli::ontology::Entity* get_parent() const override;

                std::size_t get_number_of_children() const override;
                std::size_t get_number_of_descendants() const override;

                template<class T1>
                    friend void yli::hierarchy::bind_child_to_parent(T1 child_pointer, std::vector<T1>& child_pointer_vector, std::queue<std::size_t>& free_childID_queue, std::size_t& number_of_children);
                template<class T1>
                    friend void yli::ontology::render_children(const std::vector<T1>& child_pointer_vector);

            private:
                void bind_to_parent();

                // this method renders this `Object`.
                void render();
                void render_this_object(yli::ontology::Shader* const shader_pointer);

                yli::ontology::Species* species_parent; // pointer to the `Species` parent.
                yli::ontology::Text3D* text3D_parent;   // pointer to the `Text3D` parent.
                yli::ontology::Glyph* glyph;            // pointer to the `Glyph` (not a parent!).

                bool is_character;
                bool quaternions_in_use;
                bool has_entered;
                bool should_ylikuutio_render_this_object;

                glm::vec3 original_scale_vector;        // original scale vector.
                glm::vec3 rotate_vector;                // rotate vector.
                glm::vec3 translate_vector;             // translate vector.
                glm::vec3 initial_rotate_vector;        // initial rotate vector.

                float initial_rotate_angle;             // initial rotate angle.
                float rotate_angle;                     // rotate angle.
        };
    }
}

#endif
