#include "gtest/gtest.h"
#include "cpp/ylikuutio/common/triangulation.hpp"

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <stdint.h> // uint32_t etc.
#include <vector>   // std::vector

TEST(a_2x2_world_must_be_triangulated_appropriately, bilinear_interpolation)
{
    uint32_t image_width = 2;
    uint32_t image_height = 2;
    uint32_t world_size = image_width * image_height;

    GLuint* vertex_data;
    vertex_data = new GLuint [world_size];
    GLuint* vertex_pointer = vertex_data;
    // x, z: height (y).
    GLuint southwest_height = 1;
    GLuint southeast_height = 2;
    GLuint northwest_height = 4;
    GLuint northeast_height = 8;
    GLfloat center_x = 0.5f;
    GLfloat center_z = 0.5f;

    // 0, 0: 1.
    *vertex_pointer++ = southwest_height;
    // 1, 0: 2.
    *vertex_pointer++ = southeast_height;
    // 0, 1: 4.
    *vertex_pointer++ = northwest_height;
    // 1, 1: 8.
    *vertex_pointer++ = northeast_height;

    std::vector<glm::vec3> vertices; // vertices of the object.
    std::vector<glm::vec2> UVs;      // UVs of the object.
    std::vector<glm::vec3> normals;  // normals of the object.

    TriangulateQuadsStruct triangulate_quads_struct;
    triangulate_quads_struct.input_vertex_pointer = vertex_data;
    triangulate_quads_struct.image_width = image_width;
    triangulate_quads_struct.image_height = image_height;
    triangulate_quads_struct.sphere_radius = NAN;
    triangulate_quads_struct.spherical_world_struct = SphericalWorldStruct(); // not used, but is needed in the function call.

    triangulate_quads_struct.triangulation_type = "bilinear_interpolation";

    bool is_success = geometry::triangulate_quads(triangulate_quads_struct, vertices, UVs, normals);
    ASSERT_TRUE(is_success);
    ASSERT_EQ(vertices.size(), 12);
    ASSERT_EQ(UVs.size(), 12);
    ASSERT_EQ(normals.size(), 12);

    std::cout << " 0: " << vertices[0].x << ", " << vertices[0].y << ", " << vertices[0].z << "\n";
    std::cout << " 1: " << vertices[1].x << ", " << vertices[1].y << ", " << vertices[1].z << "\n";
    std::cout << " 2: " << vertices[2].x << ", " << vertices[2].y << ", " << vertices[2].z << "\n";
    std::cout << " 3: " << vertices[3].x << ", " << vertices[3].y << ", " << vertices[3].z << "\n";
    std::cout << " 4: " << vertices[4].x << ", " << vertices[4].y << ", " << vertices[4].z << "\n";
    std::cout << " 5: " << vertices[5].x << ", " << vertices[5].y << ", " << vertices[5].z << "\n";
    std::cout << " 6: " << vertices[6].x << ", " << vertices[6].y << ", " << vertices[6].z << "\n";
    std::cout << " 7: " << vertices[7].x << ", " << vertices[7].y << ", " << vertices[7].z << "\n";
    std::cout << " 8: " << vertices[8].x << ", " << vertices[8].y << ", " << vertices[8].z << "\n";
    std::cout << " 9: " << vertices[9].x << ", " << vertices[9].y << ", " << vertices[9].z << "\n";
    std::cout << "10: " << vertices[10].x << ", " << vertices[10].y << ", " << vertices[10].z << "\n";
    std::cout << "11: " << vertices[11].x << ", " << vertices[11].y << ", " << vertices[11].z << "\n";

    // 1st vertex is the center vertex of the 1st triangle.
    ASSERT_EQ(vertices[0].x, center_x);
    ASSERT_EQ(vertices[0].z, center_z);
    ASSERT_EQ(vertices[0].y, (GLfloat) (southwest_height + southeast_height + northwest_height + northeast_height) / 4);

    // 2nd vertex is the southwest vertex of the 1st triangle.
    ASSERT_EQ(vertices[1].x, 1.0f);
    ASSERT_EQ(vertices[1].z, 0.0f);
    ASSERT_EQ(vertices[1].y, southeast_height);

    // 3rd vertex is the northwest vertex of the 1st triangle.
    ASSERT_EQ(vertices[2].x, 0.0f);
    ASSERT_EQ(vertices[2].z, 0.0f);
    ASSERT_EQ(vertices[2].y, southwest_height);

    // 4th vertex is the center vertex of the 2nd triangle.
    ASSERT_EQ(vertices[3].x, center_x);
    ASSERT_EQ(vertices[3].z, center_z);
    ASSERT_EQ(vertices[3].y, (GLfloat) (southwest_height + southeast_height + northwest_height + northeast_height) / 4);

    // 5th vertex is the southwest vertex of the 2nd triangle.
    ASSERT_EQ(vertices[4].x, 0.0f);
    ASSERT_EQ(vertices[4].z, 0.0f);
    ASSERT_EQ(vertices[4].y, southwest_height);

    // 6th vertex is the northwest vertex of the 2nd triangle.
    ASSERT_EQ(vertices[5].x, 0.0f);
    ASSERT_EQ(vertices[5].z, 1.0f);
    ASSERT_EQ(vertices[5].y, northwest_height);

    // 7th vertex is the center vertex of the 3rd triangle.
    ASSERT_EQ(vertices[6].x, center_x);
    ASSERT_EQ(vertices[6].z, center_z);
    ASSERT_EQ(vertices[6].y, (GLfloat) (southwest_height + southeast_height + northwest_height + northeast_height) / 4);

    // 8th vertex is the northwest vertex of the 3rd triangle.
    ASSERT_EQ(vertices[7].x, 0.0f);
    ASSERT_EQ(vertices[7].z, 1.0f);
    ASSERT_EQ(vertices[7].y, northwest_height);

    // 9th vertex is the northeast vertex of the 3rd triangle.
    ASSERT_EQ(vertices[8].x, 1.0f);
    ASSERT_EQ(vertices[8].z, 1.0f);
    ASSERT_EQ(vertices[8].y, northeast_height);

    // 10th vertex is the center vertex of the 4th triangle.
    ASSERT_EQ(vertices[9].x, center_x);
    ASSERT_EQ(vertices[9].z, center_z);
    ASSERT_EQ(vertices[9].y, (GLfloat) (southwest_height + southeast_height + northwest_height + northeast_height) / 4);

    // 11th vertex is the northeast vertex of the 4th triangle.
    ASSERT_EQ(vertices[10].x, 1.0f);
    ASSERT_EQ(vertices[10].z, 1.0f);
    ASSERT_EQ(vertices[10].y, northeast_height);

    // 12th vertex is the southeast vertex of the 4th triangle.
    ASSERT_EQ(vertices[11].x, 1.0f);
    ASSERT_EQ(vertices[11].z, 0.0f);
    ASSERT_EQ(vertices[11].y, southeast_height);
}
TEST(a_2x2_world_must_be_triangulated_appropriately, southeast_northwest_edges)
{
    uint32_t image_width = 2;
    uint32_t image_height = 2;
    uint32_t world_size = image_width * image_height;

    GLuint* vertex_data;
    vertex_data = new GLuint [world_size];
    GLuint* vertex_pointer = vertex_data;
    // x, z: height (y).
    GLuint southwest_height = 1;
    GLuint southeast_height = 2;
    GLuint northwest_height = 4;
    GLuint northeast_height = 8;
    GLfloat center_x = 0.5f;
    GLfloat center_z = 0.5f;

    // 0, 0: 1.
    *vertex_pointer++ = southwest_height;
    // 1, 0: 2.
    *vertex_pointer++ = southeast_height;
    // 0, 1: 4.
    *vertex_pointer++ = northwest_height;
    // 1, 1: 8.
    *vertex_pointer++ = northeast_height;

    std::vector<glm::vec3> vertices; // vertices of the object.
    std::vector<glm::vec2> UVs;      // UVs of the object.
    std::vector<glm::vec3> normals;  // normals of the object.

    TriangulateQuadsStruct triangulate_quads_struct;
    triangulate_quads_struct.input_vertex_pointer = vertex_data;
    triangulate_quads_struct.image_width = image_width;
    triangulate_quads_struct.image_height = image_height;
    triangulate_quads_struct.sphere_radius = NAN;
    triangulate_quads_struct.spherical_world_struct = SphericalWorldStruct(); // not used, but is needed in the function call.

    triangulate_quads_struct.triangulation_type = "southeast_northwest_edges";

    bool is_success = geometry::triangulate_quads(triangulate_quads_struct, vertices, UVs, normals);
    ASSERT_TRUE(is_success);
    ASSERT_EQ(vertices.size(), 6);
    ASSERT_EQ(UVs.size(), 6);
    ASSERT_EQ(normals.size(), 6);

    std::cout << " 0: " << vertices[0].x << ", " << vertices[0].y << ", " << vertices[0].z << "\n";
    std::cout << " 1: " << vertices[1].x << ", " << vertices[1].y << ", " << vertices[1].z << "\n";
    std::cout << " 2: " << vertices[2].x << ", " << vertices[2].y << ", " << vertices[2].z << "\n";
    std::cout << " 3: " << vertices[3].x << ", " << vertices[3].y << ", " << vertices[3].z << "\n";
    std::cout << " 4: " << vertices[4].x << ", " << vertices[4].y << ", " << vertices[4].z << "\n";
    std::cout << " 5: " << vertices[5].x << ", " << vertices[5].y << ", " << vertices[5].z << "\n";

    // 1st vertex is the southwest vertex of the 1st triangle.
    ASSERT_EQ(vertices[0].x, 0.0f);
    ASSERT_EQ(vertices[0].z, 0.0f);
    ASSERT_EQ(vertices[0].y, southwest_height);

    // 2nd vertex is the northwest vertex of the 1st triangle.
    ASSERT_EQ(vertices[1].x, 0.0f);
    ASSERT_EQ(vertices[1].z, 1.0f);
    ASSERT_EQ(vertices[1].y, northwest_height);

    // 3rd vertex is the southeast vertex of the 1st triangle.
    ASSERT_EQ(vertices[2].x, 1.0f);
    ASSERT_EQ(vertices[2].z, 0.0f);
    ASSERT_EQ(vertices[2].y, southeast_height);

    // 4th vertex is the northeast vertex of the 2nd triangle.
    ASSERT_EQ(vertices[3].x, 1.0f);
    ASSERT_EQ(vertices[3].z, 1.0f);
    ASSERT_EQ(vertices[3].y, northeast_height);

    // 5th vertex is the southeast vertex of the 2nd triangle.
    ASSERT_EQ(vertices[4].x, 1.0f);
    ASSERT_EQ(vertices[4].z, 0.0f);
    ASSERT_EQ(vertices[4].y, southeast_height);

    // 6th vertex is the northwest vertex of the 2nd triangle.
    ASSERT_EQ(vertices[5].x, 0.0f);
    ASSERT_EQ(vertices[5].z, 1.0f);
    ASSERT_EQ(vertices[5].y, northwest_height);
}
