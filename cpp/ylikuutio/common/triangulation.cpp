#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#define DEGREES_TO_RADIANS(x) (x * PI / 180.0f)
#endif

#include "triangulation.hpp"
#include "triangulation_macros.cpp"
#include "transformation.hpp"
#include "indexing.hpp"
#include "bilinear_interpolation.hpp"

// Include GLEW
#ifndef __GL_GLEW_H_INCLUDED
#define __GL_GLEW_H_INCLUDED
#include <GL/glew.h> // GLfloat, GLuint etc.
#endif

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <cmath>    // NAN
#include <cstring>  // strcmp
#include <iostream> // std::cout, std::cin, std::cerr
#include <string>   // std::string
#include <string.h> // strcmp
#include <vector>   // std::vector
#include <stdint.h> // uint32_t etc.

namespace geometry
{
    // for southeast-northwest edges.
    enum SoutheastNorthwestEdgesDirections { SSE_CODE_FOR_SE_NW, WNW_CODE_FOR_SE_NW, ESE_CODE_FOR_SE_NW, NNW_CODE_FOR_SE_NW, SW_CODE_FOR_SE_NW, NE_CODE_FOR_SE_NW };

    // for southeast-northwest edges.
    glm::vec3 get_face_normal_for_SE_NW(
            std::vector<glm::vec3> &face_normal_data,
            uint32_t x,
            uint32_t z,
            SoutheastNorthwestEdgesDirections compass_point_code,
            uint32_t image_width)
    {
        uint32_t face_normal_i;

        switch (compass_point_code)
        {
            case SSE_CODE_FOR_SE_NW:
                face_normal_i = 2 * (z - 1) * (image_width - 1) + 2 * x;
                break;
            case WNW_CODE_FOR_SE_NW:
                face_normal_i = 2 * z * (image_width - 1) + 2 * x - 2;
                break;
            case ESE_CODE_FOR_SE_NW:
                face_normal_i = 2 * (z - 1) * (image_width - 1) + 2 * x + 1;
                break;
            case NNW_CODE_FOR_SE_NW:
                face_normal_i = 2 * z * (image_width - 1) + 2 * x - 1;
                break;
            case SW_CODE_FOR_SE_NW:
                face_normal_i = 2 * (z - 1) * (image_width - 1) + 2 * (x - 1) + 1;
                break;
            case NE_CODE_FOR_SE_NW:
                face_normal_i = 2 * z * (image_width - 1) + 2 * x;
                break;
            default:
                std::cerr << "invalid compass point code!\n";
                return glm::vec3(NAN, NAN, NAN);
        }
        return face_normal_data[face_normal_i];
    }

    void output_triangle_vertices(
            std::vector<glm::vec3> &temp_vertices,
            std::vector<glm::vec2> &temp_UVs,
            std::vector<glm::vec3> &temp_normals,
            GLuint vertexIndex[3],
            GLuint uvIndex[3],
            GLuint normalIndex[3],
            std::vector<glm::vec3> &out_vertices,
            std::vector<glm::vec2> &out_UVs,
            std::vector<glm::vec3> &out_normals,
            uint32_t triangle_i)
    {
        out_vertices.push_back(temp_vertices[vertexIndex[0]]);
        out_UVs.push_back(temp_UVs[uvIndex[0]]);
        out_normals.push_back(temp_normals[normalIndex[0]]);

        // std::cout << "triangle " << triangle_i << ", vertex 0: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ").\n";

        out_vertices.push_back(temp_vertices[vertexIndex[1]]);
        out_UVs.push_back(temp_UVs[uvIndex[1]]);
        out_normals.push_back(temp_normals[normalIndex[1]]);

        // std::cout << "triangle " << triangle_i << ", vertex 1: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ").\n";

        out_vertices.push_back(temp_vertices[vertexIndex[2]]);
        out_UVs.push_back(temp_UVs[uvIndex[2]]);
        out_normals.push_back(temp_normals[normalIndex[2]]);

        // std::cout << "triangle " << triangle_i << ", vertex 2: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ").\n\n";
    }

    bool triangulate_quads(
            TriangulateQuadsStruct triangulate_quads_struct,
            std::vector<glm::vec3> &out_vertices,
            std::vector<glm::vec2> &out_UVs,
            std::vector<glm::vec3> &out_normals)
    {
        GLuint* input_vertex_pointer = triangulate_quads_struct.input_vertex_pointer;
        uint32_t image_width = triangulate_quads_struct.image_width;
        uint32_t image_height = triangulate_quads_struct.image_height;
        std::string triangulation_type = triangulate_quads_struct.triangulation_type;
        double sphere_radius = triangulate_quads_struct.sphere_radius;
        SphericalWorldStruct spherical_world_struct = triangulate_quads_struct.spherical_world_struct;

        const char* char_triangulation_type = triangulation_type.c_str();

        bool is_bilinear_interpolation_in_use = false;
        bool is_southwest_northeast_edges_in_use = false;
        bool is_southeast_northwest_edges_in_use = false;
        bool is_triangulation_type_valid = false;

        if (strcmp(char_triangulation_type, "bilinear_interpolation") == 0)
        {
            is_bilinear_interpolation_in_use = true;
            is_triangulation_type_valid = true;
        }
        else if ((strcmp(char_triangulation_type, "southwest_northeast_edges") == 0) || (strcmp(char_triangulation_type, "northeast_southwest_edges") == 0))
        {
            is_southwest_northeast_edges_in_use = true;
            is_triangulation_type_valid = true;
        }
        else if ((strcmp(char_triangulation_type, "southeast_northwest_edges") == 0) || (strcmp(char_triangulation_type, "northwest_southeast_edges") == 0))
        {
            is_southeast_northwest_edges_in_use = true;
            is_triangulation_type_valid = true;
        }

        std::cout << "triangulation type in use: " << triangulation_type << "\n";

        if (!is_triangulation_type_valid)
        {
            std::cerr << "invalid triangulation type!\n";
            return false;
        }

        std::vector<GLuint> vertexIndices, uvIndices, normalIndices;
        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec2> temp_UVs;
        std::vector<glm::vec3> temp_normals;

        uint32_t texture_x = 0;
        uint32_t texture_y = 0;

        // Processing stages:
        // 1. Define the (GLfloat) vertices for vertices loaded from file, `push_back` to `temp_vertices`.
        // 2. Interpolate the (GLfloat) vertices between, using bilinear interpolation, `push_back` to `temp_vertices` and `temp_UVs`.
        // 3. Transform spherical coordinates loaded from file (and computed this far as being in horizontal plane) to a curved surface.
        // 4. For bilinear interpolation: Transform interpolated coordinates (and computed this far as being in horizontal plane) to a curved surface.
        // 5. Compute the face normals, `push_back` to `face_normals`.
        // 6. Compute the vertex normals for vertices loaded from file and for interpolated vertices (for `"bilinear_interpolation"`), `push_back` to `temp_normals`.
        // 7. Loop through all vertices and `geometry::output_triangle_vertices`.

        // 1. Define the vertices for vertices loaded from file, `push_back` to `temp_vertices`.
        // First, define the temporary vertices in a double loop.
        for (uint32_t z = 0; z < image_height; z++)
        {
            for (uint32_t x = 0; x < image_width; x++)
            {
                // current x,z coordinates).
                GLfloat y = (GLfloat) geometry::get_y(input_vertex_pointer, x, z, image_width);

                // This corresponds to "v": specify one vertex.
                glm::vec3 vertex;
                vertex.x = (GLfloat) x;
                vertex.y = y;
                vertex.z = (GLfloat) z;
                temp_vertices.push_back(vertex);

                // This corresponds to "vt": specify texture coordinates of one vertex.
                glm::vec2 uv;

                if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                {
                    uv.x = round((GLfloat) texture_x);
                    uv.y = round((GLfloat) texture_y);
                }
                else
                {
                    uv.x = y / 256;
                    uv.y = 0.0f;
                }

                temp_UVs.push_back(uv);

                texture_x ^= 1;
            }
            texture_y ^= 1;
        }

        uint32_t n_faces_for_each_vertex;
        uint32_t n_faces;

        if (is_bilinear_interpolation_in_use)
        {
            n_faces_for_each_vertex = 4;
        }
        else if (is_southwest_northeast_edges_in_use || is_southeast_northwest_edges_in_use)
        {
            n_faces_for_each_vertex = 2;
        }

        n_faces = n_faces_for_each_vertex * (image_width - 1) * (image_height - 1);

        std::cout << "image width: " << image_width << " pixels.\n";
        std::cout << "image height: " << image_height << " pixels.\n";
        std::cout << "number of faces: " << n_faces << ".\n";

        uint32_t vertexIndex[3], uvIndex[3], normalIndex[3];

        uint32_t triangle_i = 0;

        // 2. Interpolate the vertices between, using bilinear interpolation, `push_back` to `temp_vertices`.
        if (is_bilinear_interpolation_in_use)
        {
            BilinearInterpolationStruct bilinear_interpolation_struct;
            bilinear_interpolation_struct.input_vertex_pointer = input_vertex_pointer;
            bilinear_interpolation_struct.image_width = image_width;
            bilinear_interpolation_struct.image_height = image_height;
            bilinear_interpolation_struct.should_ylikuutio_use_real_texture_coordinates = triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates;
            geometry::interpolate_vertices_using_bilinear_interpolation(
                    bilinear_interpolation_struct,
                    temp_vertices,
                    temp_UVs);
        }
        else if (is_southwest_northeast_edges_in_use || is_southeast_northwest_edges_in_use)
        {
            // Then, define the faces in a double loop.
            // Begin from index 1.
            for (uint32_t z = 1; z < image_height; z++)
            {
                // Begin from index 1.
                for (uint32_t x = 1; x < image_width; x++)
                {
                    // This corresponds to "f": specify a face (but here we specify 2 faces instead!).
                    // std::cout << "Processing coordinate (" << x << ", " << z << ").\n";

                    uint32_t current_vertex_i = image_width * z + x;

                    if (is_southwest_northeast_edges_in_use)
                    {
                        // Define the triangles (2 faces).
                        // Triangle order: SE - NW.
                        //
                        // First triangle: 1, 4, 2 (southwest, northeast, southeast).
                        // Second triangle: 1, 3, 4 (southwest, northwest, northeast).
                        // 1, 4, 2 are relative vertex indices (base 1).
                        // 1, 3, 4 are relative vertex indices (base 1).

                        // Define the first triangle, SE: 1, 4, 2 (southwest, northeast, southeast).
                        // southwest: down and left from current coordinate.
                        // southeast: down from current coordinate.
                        // northeast: current coordinate.

                        vertexIndex[0] = SOUTHWEST;
                        vertexIndex[1] = NORTHEAST;
                        vertexIndex[2] = SOUTHEAST;

                        if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                        {
                            uvIndex[0] = SOUTHWEST;
                            uvIndex[1] = NORTHEAST;
                            uvIndex[2] = SOUTHEAST;
                        }
                        else
                        {
                            uvIndex[0] = SOUTHWEST_Y;
                            uvIndex[1] = NORTHEAST_Y;
                            uvIndex[2] = SOUTHEAST_Y;
                        }
                    }
                    else if (is_southeast_northwest_edges_in_use)
                    {

                        // Define the triangles (2 faces).
                        // Triangle order: SW - NE.
                        //
                        // First triangle: 2, 1, 3 (southeast, southwest, northwest).
                        // Second triangle: 2, 3, 4 (southeast, northwest, northeast).
                        // 2, 1, 3 are relative vertex indices (base 1).
                        // 2, 3, 4 are relative vertex indices (base 1).

                        // Define the first triangle, SW: 2, 1, 3 (southeast, southwest, northwest).
                        // southeast: down from current coordinate.
                        // southwest: down and left from current coordinate.
                        // northwest: left from current coordinate.

                        vertexIndex[0] = SOUTHEAST;
                        vertexIndex[1] = SOUTHWEST;
                        vertexIndex[2] = NORTHWEST;

                        if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                        {
                            uvIndex[0] = SOUTHEAST;
                            uvIndex[1] = SOUTHWEST;
                            uvIndex[2] = NORTHWEST;
                        }
                        else
                        {
                            uvIndex[0] = SOUTHEAST_Y;
                            uvIndex[1] = SOUTHWEST_Y;
                            uvIndex[2] = NORTHWEST_Y;

                        }
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    if (is_southwest_northeast_edges_in_use)
                    {
                        // Define the second triangle, NW: 1, 3, 4 (southwest, northwest, northeast).
                        // southwest: down and left from current coordinate.
                        // northwest: left from current coordinate.
                        // northeast: current coordinate.

                        vertexIndex[0] = SOUTHWEST;
                        vertexIndex[1] = NORTHWEST;
                        vertexIndex[2] = NORTHEAST;

                        if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                        {
                            uvIndex[0] = SOUTHWEST;
                            uvIndex[1] = NORTHWEST;
                            uvIndex[2] = NORTHEAST;
                        }
                        else
                        {
                            uvIndex[0] = SOUTHWEST_Y;
                            uvIndex[1] = NORTHWEST_Y;
                            uvIndex[2] = NORTHEAST_Y;
                        }
                    }
                    else if (is_southeast_northwest_edges_in_use)
                    {
                        // Define the second triangle, NE: 2, 3, 4 (southeast, northwest, northeast).
                        // southeast: down from current coordinate.
                        // northwest: left from current coordinate.
                        // northeast: current coordinate.

                        vertexIndex[0] = SOUTHEAST;
                        vertexIndex[1] = NORTHWEST;
                        vertexIndex[2] = NORTHEAST;

                        if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                        {
                            uvIndex[0] = SOUTHEAST;
                            uvIndex[1] = NORTHWEST;
                            uvIndex[2] = NORTHEAST;
                        }
                        else
                        {
                            uvIndex[0] = SOUTHEAST_Y;
                            uvIndex[1] = NORTHWEST_Y;
                            uvIndex[2] = NORTHEAST_Y;
                        }
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.
                }
            }
        }
        else
        {
            std::cerr << "quad triangulation type " << triangulation_type << " not yet implemented!\n";
            return false;
        }

        if (!std::isnan(sphere_radius))
        {
            // 3. Transform spherical coordinates loaded from file (and computed this far as being in horizontal plane) to a curved surface.
            //
            // Wikipedia:
            // https://en.wikipedia.org/wiki/List_of_common_coordinate_transformations#From_spherical_coordinates
            //
            // x = rho * sin(theta) * cos(phi)
            // y = rho * sin(theta) * sin(phi)
            // z = rho * cos(theta)

            TransformationStruct transformation_struct;
            transformation_struct.image_width = image_width;
            transformation_struct.image_height = image_height;
            transformation_struct.sphere_radius = sphere_radius;
            transformation_struct.is_bilinear_interpolation_in_use = is_bilinear_interpolation_in_use;
            transformation_struct.spherical_world_struct = spherical_world_struct;
            geometry::transform_coordinates_to_curved_surface(transformation_struct, temp_vertices);
        }
        else
        {
            std::cout << "no coordinate transformation is needed.\n";
        }

        // 5. Compute the face normals, `push_back` to `face_normals`.
        // Triangle order: S - W - N - E.
        //
        // First triangle: center, southeast, southwest.
        // Second triangle: center, southwest, northwest.
        // Third triangle: center, northwest, northeast.
        // Fourth triangle: center, northeast, southeast.
        std::cout << "computing face normals.\n";

        std::vector<glm::vec3> face_normal_vector_vec3;

        uint32_t current_interpolated_vertex_i;

        current_interpolated_vertex_i = image_width * image_height;

        for (uint32_t z = 1; z < image_height; z++)
        {
            for (uint32_t x = 1; x < image_width; x++)
            {
                uint32_t current_vertex_i = image_width * z + x;

                // Computing of face normals depends on triangulation type.
                if (is_bilinear_interpolation_in_use)
                {
                    glm::vec3 edge1;
                    glm::vec3 edge2;
                    glm::vec3 face_normal;

                    // Compute the normal of S face.
                    edge1 = temp_vertices[SOUTHEAST] - temp_vertices[CENTER];
                    edge2 = temp_vertices[SOUTHWEST] - temp_vertices[CENTER];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);

                    // Compute the normal of W face.
                    edge1 = temp_vertices[SOUTHWEST] - temp_vertices[CENTER];
                    edge2 = temp_vertices[NORTHWEST] - temp_vertices[CENTER];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);

                    // Compute the normal of N face.
                    edge1 = temp_vertices[NORTHWEST] - temp_vertices[CENTER];
                    edge2 = temp_vertices[NORTHEAST] - temp_vertices[CENTER];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);

                    // Compute the normal of E face.
                    edge1 = temp_vertices[NORTHEAST] - temp_vertices[CENTER];
                    edge2 = temp_vertices[SOUTHEAST] - temp_vertices[CENTER];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);
                }
                else if (is_southwest_northeast_edges_in_use)
                {
                    glm::vec3 edge1;
                    glm::vec3 edge2;
                    glm::vec3 face_normal;

                    // Compute the normal of SE face.
                    edge1 = temp_vertices[SOUTHEAST] - temp_vertices[NORTHEAST];
                    edge2 = temp_vertices[SOUTHWEST] - temp_vertices[NORTHEAST];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);

                    // Compute the normal of NW face.
                    edge1 = temp_vertices[SOUTHWEST] - temp_vertices[NORTHEAST];
                    edge2 = temp_vertices[NORTHWEST] - temp_vertices[NORTHEAST];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);
                }
                else if (is_southeast_northwest_edges_in_use)
                {
                    glm::vec3 edge1;
                    glm::vec3 edge2;
                    glm::vec3 face_normal;

                    // Compute the normal of SW face.
                    edge1 = temp_vertices[SOUTHEAST] - temp_vertices[NORTHWEST];
                    edge2 = temp_vertices[SOUTHWEST] - temp_vertices[NORTHWEST];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);

                    // Compute the normal of NE face.
                    edge1 = temp_vertices[NORTHEAST] - temp_vertices[NORTHWEST];
                    edge2 = temp_vertices[SOUTHEAST] - temp_vertices[NORTHWEST];
                    face_normal = glm::cross(edge1, edge2);
                    face_normal_vector_vec3.push_back(face_normal);
                }
                else
                {
                    std::cerr << "invalid triangulation type!\n";
                    return false;
                }
                current_interpolated_vertex_i++;
            }
        }

        // 6. Compute the vertex normals for vertices loaded from file, `push_back` to `temp_normals`.
        std::cout << "computing vertex normals for vertices loaded from file.\n";

        if (is_bilinear_interpolation_in_use)
        {
            current_interpolated_vertex_i = image_width * image_height;
            uint32_t x = 0;
            uint32_t z = 0;

            // Compute the normal of the southwesternmost vertex.
            // Number of adjacent faces: 2.
            glm::vec3 vertex_normal;

            vertex_normal = NNE_FACE_NORMAL + ENE_FACE_NORMAL;
            temp_normals.push_back(vertex_normal);

            // Compute the normals of southern vertices.
            for (x = 1; x < (image_width - 1); x++)
            {
                // Compute the normal of a southern vertex.
                // Number of adjacent faces: 4.
                vertex_normal = WNW_FACE_NORMAL + NNW_FACE_NORMAL + NNE_FACE_NORMAL + ENE_FACE_NORMAL;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the southeasternmost vertex.
            // Number of adjacent faces: 2.
            x = image_width - 1;
            vertex_normal = WNW_FACE_NORMAL + NNW_FACE_NORMAL;
            temp_normals.push_back(vertex_normal);

            // Then, define most normals in a double loop.
            for (z = 1; z < (image_height - 1); z++)
            {
                // Compute the normal of a western vertex.
                // Number of adjacent faces: 4.
                x = 0;
                vertex_normal = NNE_FACE_NORMAL + ENE_FACE_NORMAL + ESE_FACE_NORMAL + SSE_FACE_NORMAL;
                temp_normals.push_back(vertex_normal);

                for (x = 1; x < (image_width - 1); x++)
                {
                    uint32_t current_vertex_i = image_width * z + x;

                    // Compute the normal of a central vertex.
                    // Number of adjacent faces: 8.
                    vertex_normal = SSW_FACE_NORMAL + WSW_FACE_NORMAL + WNW_FACE_NORMAL + NNW_FACE_NORMAL + NNE_FACE_NORMAL + ENE_FACE_NORMAL + ESE_FACE_NORMAL + SSE_FACE_NORMAL;
                    temp_normals.push_back(vertex_normal);
                }

                x = image_width - 1;

                // Compute the normal of an eastern vertex.
                // Number of adjacent faces: 4.
                vertex_normal = SSW_FACE_NORMAL + WSW_FACE_NORMAL + WNW_FACE_NORMAL + NNW_FACE_NORMAL;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the northwesternmost vertex.
            // Number of adjacent faces: 2.
            x = 0;
            vertex_normal = SSE_FACE_NORMAL + ESE_FACE_NORMAL;
            temp_normals.push_back(vertex_normal);

            // Compute the normals of northern vertices.
            for (x = 1; x < (image_width - 1); x++)
            {
                // Compute the normal of a northern vertex.
                // Number of adjacent faces: 4.
                vertex_normal = SSW_FACE_NORMAL + WSW_FACE_NORMAL + ESE_FACE_NORMAL + SSE_FACE_NORMAL;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the northeasternmost vertex.
            // Number of adjacent faces: 2.
            x = image_width - 1;
            vertex_normal = SSW_FACE_NORMAL + WSW_FACE_NORMAL;
            temp_normals.push_back(vertex_normal);

            // 6. Compute the vertices between, `push_back` to `temp_normals`.
            std::cout << "computing vertex normals for interpolated vertices.\n";

            for (z = 1; z < image_height; z++)
            {
                for (x = 1; x < image_width; x++)
                {
                    vertex_normal = S_FACE_NORMAL + W_FACE_NORMAL + N_FACE_NORMAL + E_FACE_NORMAL;
                    temp_normals.push_back(vertex_normal);
                }
            }
        }
        else if (is_southwest_northeast_edges_in_use)
        {
            // TODO: compute vertex normals for `"southwest_northeast_edges"`.
        }
        else if (is_southeast_northwest_edges_in_use)
        {
            uint32_t x = 0;
            uint32_t z = 0;

            // Compute the normal of the southwesternmost vertex.
            // Number of adjacent faces: 1.
            glm::vec3 vertex_normal;
            vertex_normal = NE_FACE_NORMAL_FOR_SE_NW;
            temp_normals.push_back(vertex_normal);

            // Compute the normals of southern vertices.
            for (x = 1; x < (image_width - 1); x++)
            {
                // Compute the normal of a southern vertex.
                // Number of adjacent faces: 3.
                vertex_normal = WNW_FACE_NORMAL_FOR_SE_NW + NNW_FACE_NORMAL_FOR_SE_NW + NE_FACE_NORMAL_FOR_SE_NW;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the southeasternmost vertex.
            // Number of adjacent faces: 2.
            x = image_width - 1;
            vertex_normal = WNW_FACE_NORMAL_FOR_SE_NW + NNW_FACE_NORMAL_FOR_SE_NW;
            temp_normals.push_back(vertex_normal);

            // Then, define most normals in a double loop.
            for (z = 1; z < (image_height - 1); z++)
            {
                // Compute the normal of a western vertex.
                // Number of adjacent faces: 3.
                x = 0;
                vertex_normal = NE_FACE_NORMAL_FOR_SE_NW + ESE_FACE_NORMAL_FOR_SE_NW + SSE_FACE_NORMAL_FOR_SE_NW;
                temp_normals.push_back(vertex_normal);

                for (x = 1; x < (image_width - 1); x++)
                {
                    uint32_t current_vertex_i = image_width * z + x;

                    // Compute the normal of a central vertex.
                    // Number of adjacent faces: 6.
                    vertex_normal = SSE_FACE_NORMAL_FOR_SE_NW + SW_FACE_NORMAL_FOR_SE_NW + WNW_FACE_NORMAL_FOR_SE_NW + NNW_FACE_NORMAL_FOR_SE_NW + NE_FACE_NORMAL_FOR_SE_NW + ESE_FACE_NORMAL_FOR_SE_NW;
                    temp_normals.push_back(vertex_normal);
                }

                x = image_width - 1;

                // Compute the normal of an eastern vertex.
                // Number of adjacent faces: 4.
                vertex_normal = SW_FACE_NORMAL_FOR_SE_NW + WNW_FACE_NORMAL_FOR_SE_NW + NNW_FACE_NORMAL_FOR_SE_NW;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the northwesternmost vertex.
            // Number of adjacent faces: 2.
            x = 0;
            vertex_normal = SSE_FACE_NORMAL_FOR_SE_NW + ESE_FACE_NORMAL_FOR_SE_NW;
            temp_normals.push_back(vertex_normal);

            // Compute the normals of northern vertices.
            for (x = 1; x < (image_width - 1); x++)
            {
                // Compute the normal of a northern vertex.
                // Number of adjacent faces: 3.
                vertex_normal = SW_FACE_NORMAL_FOR_SE_NW + ESE_FACE_NORMAL_FOR_SE_NW + SSE_FACE_NORMAL_FOR_SE_NW;
                temp_normals.push_back(vertex_normal);
            }

            // Compute the normal of the northeasternmost vertex.
            // Number of adjacent faces: 1.
            x = image_width - 1;
            vertex_normal = SW_FACE_NORMAL_FOR_SE_NW;
            temp_normals.push_back(vertex_normal);
        }

        // 7. Loop through all vertices and `geometry::output_triangle_vertices`.
        std::cout << "defining output vertices, UVs and normals.\n";

        if (is_bilinear_interpolation_in_use)
        {
            int32_t current_percent = 0;

            triangle_i = 0;
            current_interpolated_vertex_i = image_width * image_height;

            for (uint32_t z = 1; z < image_height; z++)
            {
                // show progress in percents.
                current_percent = (int32_t) floor(100.0f * ((double) z / (double) (image_height - 1)));
                std::cout << current_percent << "% ";

                for (uint32_t x = 1; x < image_width; x++)
                {
                    uint32_t current_vertex_i = image_width * z + x;

                    // This corresponds to "vn": specify normal of one vertex.

                    // Then, define the triangles (4 faces).
                    // Triangle order: S - W - N - E.
                    //
                    // First triangle: center, southeast, southwest.
                    // Second triangle: center, southwest, northwest.
                    // Third triangle: center, northwest, northeast.
                    // Fourth triangle: center, northeast, southeast.

                    // Define the first triangle, S: center, southeast, southwest.
                    vertexIndex[0] = CENTER;
                    vertexIndex[1] = SOUTHEAST;
                    vertexIndex[2] = SOUTHWEST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = CENTER;
                        uvIndex[1] = SOUTHEAST;
                        uvIndex[2] = SOUTHWEST;
                    }
                    else
                    {
                        uvIndex[0] = CENTER_Y;
                        uvIndex[1] = SOUTHEAST_Y;
                        uvIndex[2] = SOUTHWEST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;

                    // Define the second triangle, W: center, southwest, northwest.
                    vertexIndex[0] = CENTER;
                    vertexIndex[1] = SOUTHWEST;
                    vertexIndex[2] = NORTHWEST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = CENTER;
                        uvIndex[1] = SOUTHWEST;
                        uvIndex[2] = NORTHWEST;
                    }
                    else
                    {
                        uvIndex[0] = CENTER_Y;
                        uvIndex[1] = SOUTHWEST_Y;
                        uvIndex[2] = NORTHWEST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;

                    // Define the third triangle, N: center, northwest, northeast.
                    vertexIndex[0] = CENTER;
                    vertexIndex[1] = NORTHWEST;
                    vertexIndex[2] = NORTHEAST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = CENTER;
                        uvIndex[1] = NORTHWEST;
                        uvIndex[2] = NORTHEAST;
                    }
                    else
                    {
                        uvIndex[0] = CENTER_Y;
                        uvIndex[1] = NORTHWEST_Y;
                        uvIndex[2] = NORTHEAST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;

                    // Define the fourth triangle, E: center, northeast, southeast.
                    vertexIndex[0] = CENTER;
                    vertexIndex[1] = NORTHEAST;
                    vertexIndex[2] = SOUTHEAST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = CENTER;
                        uvIndex[1] = NORTHEAST;
                        uvIndex[2] = SOUTHEAST;
                    }
                    else
                    {
                        uvIndex[0] = CENTER_Y;
                        uvIndex[1] = NORTHEAST_Y;
                        uvIndex[2] = SOUTHEAST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;

                    current_interpolated_vertex_i++;
                }
            }
            std::cout << "\n"; // a newline after percent progress meter.
        }
        else if (is_southwest_northeast_edges_in_use)
        {
            // TODO: define output vertices, UVs and normals for `"southwest_northeast_edges"`.
        }
        else if (is_southeast_northwest_edges_in_use)
        {
            uint32_t x;
            uint32_t z;

            triangle_i = 0;

            for (z = 1; z < image_height; z++)
            {
                for (x = 1; x < image_width; x++)
                {
                    uint32_t current_vertex_i = image_width * z + x;

                    // This corresponds to "vn": specify normal of one vertex.

                    // Then, define the triangles (2 faces).
                    // Triangle order: SW - NE.
                    //
                    // First triangle: southwest, northwest, southeast.
                    // Second triangle: northeast, southeast, northwest.

                    // Define the first triangle, SW: southwest, northwest, southeast.
                    vertexIndex[0] = SOUTHWEST;
                    vertexIndex[1] = NORTHWEST;
                    vertexIndex[2] = SOUTHEAST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = SOUTHWEST;
                        uvIndex[1] = NORTHWEST;
                        uvIndex[2] = SOUTHEAST;
                    }
                    else
                    {
                        uvIndex[0] = SOUTHWEST_Y;
                        uvIndex[1] = NORTHWEST_Y;
                        uvIndex[2] = SOUTHEAST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;

                    // Define the second triangle, NW: northeast, southeast, northwest.
                    vertexIndex[0] = NORTHEAST;
                    vertexIndex[1] = SOUTHEAST;
                    vertexIndex[2] = NORTHWEST;

                    if (triangulate_quads_struct.should_ylikuutio_use_real_texture_coordinates)
                    {
                        uvIndex[0] = NORTHEAST;
                        uvIndex[1] = SOUTHEAST;
                        uvIndex[2] = NORTHWEST;
                    }
                    else
                    {
                        uvIndex[0] = NORTHEAST_Y;
                        uvIndex[1] = SOUTHEAST_Y;
                        uvIndex[2] = NORTHWEST_Y;
                    }

                    normalIndex[0] = 0; // TODO: add proper normal index.
                    normalIndex[1] = 0; // TODO: add proper normal index.
                    normalIndex[2] = 0; // TODO: add proper normal index.

                    geometry::output_triangle_vertices(
                            temp_vertices,
                            temp_UVs,
                            temp_normals,
                            vertexIndex,
                            uvIndex,
                            normalIndex,
                            out_vertices,
                            out_UVs,
                            out_normals,
                            triangle_i);
                    triangle_i++;
                }
            }
        }
        std::cout << "number of vertices: " << out_vertices.size() << ".\n";
        std::cout << "number of UVs: " << out_UVs.size() << ".\n";
        std::cout << "number of normals: " << out_normals.size() << ".\n";

        return true;
    }
}