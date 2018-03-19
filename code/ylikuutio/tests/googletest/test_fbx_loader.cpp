#include "gtest/gtest.h"
#include "code/ylikuutio/loaders/fbx_species_loader.hpp"
#include "code/ylikuutio/loaders/fbx_species_loader.cpp"
#include <miniz.c>
#include <miniz.h>
#include <ofbx.cpp>
#include <ofbx.h>

// Include GLM
#ifndef __GLM_GLM_HPP_INCLUDED
#define __GLM_GLM_HPP_INCLUDED
#include <glm/glm.hpp> // glm
#endif

// Include standard headers
#include <iostream> // std::cout, std::cin, std::cerr
#include <stdint.h> // uint32_t etc.
#include <string>   // std::string
#include <vector>   // std::vector

typedef unsigned char u8;

TEST(OpenFBX_must_function_appropriately, rigged_and_animated_cat)
{
    const std::string filename = "cat.fbx";
    std::vector<unsigned char> data_vector = file::binary_slurp(filename);
    ASSERT_EQ(data_vector.size(), 7550684);                                // size of `cat.fbx` in bytes.

    const u8* data = reinterpret_cast<unsigned char*>(data_vector.data());
    ASSERT_NE(data, nullptr);

    const int size = data_vector.size();
    ASSERT_EQ(size, 7550684);                                             // size of `cat.fbx` in bytes.

    ofbx::IScene* ofbx_iscene = ofbx::load(data, size);
    ASSERT_NE(ofbx_iscene, nullptr);

    const ofbx::IElement* ofbx_ielement = ofbx_iscene->getRootElement();
    ASSERT_NE(ofbx_ielement, nullptr);

    const ofbx::Object* ofbx_object = ofbx_iscene->getRoot();
    ASSERT_NE(ofbx_object, nullptr);
    ofbx::Object::Type ofbx_object_type = ofbx_object->getType();
    ASSERT_EQ(ofbx_object_type, ofbx::Object::Type::ROOT);

    const int ofbx_mesh_count = ofbx_iscene->getMeshCount();
    std::cout << filename << ": getMeshCount(): " << ofbx_mesh_count << "\n";
    ASSERT_GT(ofbx_mesh_count, 0);

    // FBX file may be several meshes (`ofbx::Mesh`).
    // Each mesh may have several materials (`ofbx::Material`).
    // Each material has a texture (`ofbx::Texture`).

    for (int i = 0; i < ofbx_mesh_count; i++)
    {
        std::cout << "mesh " << i << "\n";
        const ofbx::Mesh* mesh = ofbx_iscene->getMesh(i);
        ASSERT_NE(mesh, nullptr);

        const ofbx::Geometry* geometry = mesh->getGeometry();
        ASSERT_NE(geometry, nullptr);

        const ofbx::Vec3* vertices = geometry->getVertices();

        const int vertex_count = geometry->getVertexCount();
        std::cout << "mesh " << i << ": getVertexCount(): " << vertex_count << "\n";
        ASSERT_GE(vertex_count, 0);

        const ofbx::Vec3* normals = geometry->getNormals();

        const ofbx::Vec2* uvs = geometry->getUVs();

        const ofbx::Vec4* colors = geometry->getColors();

        const ofbx::Vec3* tangents = geometry->getTangents();

        const ofbx::Skin* skin = geometry->getSkin();

        const int* materials = geometry->getMaterials();

        ofbx::Matrix geometric_matrix = mesh->getGeometricMatrix();

        const int material_count = mesh->getMaterialCount();
        std::cout << "mesh " << i << ": getMaterialCount(): " << material_count << "\n";
        ASSERT_GT(material_count, 0);

        for (int j = 0; j < material_count; j++)
        {
            std::cout << "mesh " << i << ", material " << j << "\n";
            const ofbx::Material* material = mesh->getMaterial(j);
            ASSERT_NE(material, nullptr);

            static const ofbx::Object::Type material_type = material->s_type;
            ASSERT_EQ(material_type, ofbx::Object::Type::MATERIAL);

            const ofbx::Texture* diffuse_texture = material->getTexture(ofbx::Texture::DIFFUSE);
            if (diffuse_texture == nullptr)
            {
                std::cout << "mesh " << i << ", DIFFUSE texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", DIFFUSE texture at " << std::hex << (void*) diffuse_texture << std::dec << "\n";
                ofbx::DataView dataview_filename = diffuse_texture->getFileName();
                ofbx::DataView dataview_rel_filename = diffuse_texture->getRelativeFileName();
            }

            const ofbx::Texture* normal_texture = material->getTexture(ofbx::Texture::NORMAL);
            if (normal_texture == nullptr)
            {
                std::cout << "mesh " << i << ", NORMAL texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", NORMAL texture at " << std::hex << (void*) normal_texture << std::dec << "\n";
            }

            const ofbx::Texture* count_texture = material->getTexture(ofbx::Texture::COUNT);
            if (count_texture == nullptr)
            {
                std::cout << "mesh " << i << ", COUNT texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", COUNT texture at " << std::hex << (void*) count_texture << std::dec << "\n";
            }

            ofbx::Color color = material->getDiffuseColor();

            const ofbx::Object* parent = material->getParent();
            ASSERT_NE(parent, nullptr);

            const ofbx::RotationOrder rotation_order = material->getRotationOrder();
            ASSERT_EQ(rotation_order, ofbx::RotationOrder::EULER_XYZ);
        }

        // Base struct `Object` functions for `const ofbx::Mesh* mesh`.

        const ofbx::RotationOrder rotation_order = mesh->getRotationOrder();
        ASSERT_EQ(rotation_order, ofbx::RotationOrder::EULER_XYZ);

        const bool mesh_is_node = mesh->isNode();
        ASSERT_TRUE(mesh_is_node);
    }

    int ofbx_animation_stack_count = ofbx_iscene->getAnimationStackCount();
    std::cout << filename << ": getAnimationStackCount(): " << ofbx_animation_stack_count << "\n";
    ASSERT_GT(ofbx_mesh_count, 0);

    const ofbx::Object* const* ofbx_all_objects = ofbx_iscene->getAllObjects();
    ASSERT_NE(ofbx_all_objects, nullptr);

    const int all_object_count = ofbx_iscene->getAllObjectCount();
    std::cout << filename << ": getAllObjectCount(): " << all_object_count << "\n";
    ASSERT_GT(all_object_count, 0);
}
TEST(OpenFBX_must_function_appropriately, turbo_polizei)
{
    const std::string filename = "turbo_polizei.fbx";
    std::vector<unsigned char> data_vector = file::binary_slurp(filename);
    ASSERT_EQ(data_vector.size(), 364972);                                // size of `turbo_polizei.fbx` in bytes.

    const u8* data = reinterpret_cast<unsigned char*>(data_vector.data());
    ASSERT_NE(data, nullptr);

    const int size = data_vector.size();
    ASSERT_EQ(size, 364972);                                             // size of `turbo_polizei.fbx` in bytes.

    ofbx::IScene* ofbx_iscene = ofbx::load(data, size);
    ASSERT_NE(ofbx_iscene, nullptr);

    const ofbx::IElement* ofbx_ielement = ofbx_iscene->getRootElement();
    ASSERT_NE(ofbx_ielement, nullptr);

    const ofbx::Object* ofbx_object = ofbx_iscene->getRoot();
    ASSERT_NE(ofbx_object, nullptr);
    ofbx::Object::Type ofbx_object_type = ofbx_object->getType();
    ASSERT_EQ(ofbx_object_type, ofbx::Object::Type::ROOT);

    const int ofbx_mesh_count = ofbx_iscene->getMeshCount();
    std::cout << filename << ": getMeshCount(): " << ofbx_mesh_count << "\n";
    ASSERT_GT(ofbx_mesh_count, 0);

    // FBX file may be several meshes (`ofbx::Mesh`).
    // Each mesh may have several materials (`ofbx::Material`).
    // Each material has a texture (`ofbx::Texture`).

    for (int i = 0; i < ofbx_mesh_count; i++)
    {
        std::cout << "mesh " << i << "\n";
        const ofbx::Mesh* mesh = ofbx_iscene->getMesh(i);
        ASSERT_NE(mesh, nullptr);

        const ofbx::Geometry* geometry = mesh->getGeometry();
        ASSERT_NE(geometry, nullptr);

        const ofbx::Vec3* vertices = geometry->getVertices();

        const int vertex_count = geometry->getVertexCount();
        std::cout << "mesh " << i << ": getVertexCount(): " << vertex_count << "\n";
        ASSERT_GE(vertex_count, 0);

        const ofbx::Vec3* normals = geometry->getNormals();

        const ofbx::Vec2* uvs = geometry->getUVs();

        const ofbx::Vec4* colors = geometry->getColors();

        const ofbx::Vec3* tangents = geometry->getTangents();

        const ofbx::Skin* skin = geometry->getSkin();

        const int* materials = geometry->getMaterials();

        ofbx::Matrix geometric_matrix = mesh->getGeometricMatrix();

        const int material_count = mesh->getMaterialCount();
        std::cout << "mesh " << i << ": getMaterialCount(): " << material_count << "\n";
        ASSERT_GT(material_count, 0);

        for (int j = 0; j < material_count; j++)
        {
            std::cout << "mesh " << i << ", material " << j << "\n";
            const ofbx::Material* material = mesh->getMaterial(j);
            ASSERT_NE(material, nullptr);

            static const ofbx::Object::Type material_type = material->s_type;
            ASSERT_EQ(material_type, ofbx::Object::Type::MATERIAL);

            const ofbx::Texture* diffuse_texture = material->getTexture(ofbx::Texture::DIFFUSE);
            if (diffuse_texture == nullptr)
            {
                std::cout << "mesh " << i << ", DIFFUSE texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", DIFFUSE texture at " << std::hex << (void*) diffuse_texture << std::dec << "\n";
                ofbx::DataView dataview_filename = diffuse_texture->getFileName();
                ofbx::DataView dataview_rel_filename = diffuse_texture->getRelativeFileName();
            }

            const ofbx::Texture* normal_texture = material->getTexture(ofbx::Texture::NORMAL);
            if (normal_texture == nullptr)
            {
                std::cout << "mesh " << i << ", NORMAL texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", NORMAL texture at " << std::hex << (void*) normal_texture << std::dec << "\n";
            }

            const ofbx::Texture* count_texture = material->getTexture(ofbx::Texture::COUNT);
            if (count_texture == nullptr)
            {
                std::cout << "mesh " << i << ", COUNT texture is nullptr\n";
            }
            else
            {
                std::cout << "mesh " << i << ", COUNT texture at " << std::hex << (void*) count_texture << std::dec << "\n";
            }

            ofbx::Color color = material->getDiffuseColor();

            const ofbx::Object* parent = material->getParent();
            ASSERT_NE(parent, nullptr);

            const ofbx::RotationOrder rotation_order = material->getRotationOrder();
            ASSERT_EQ(rotation_order, ofbx::RotationOrder::EULER_XYZ);
        }

        // Base struct `Object` functions for `const ofbx::Mesh* mesh`.

        const ofbx::RotationOrder rotation_order = mesh->getRotationOrder();
        ASSERT_EQ(rotation_order, ofbx::RotationOrder::EULER_XYZ);

        const bool mesh_is_node = mesh->isNode();
        ASSERT_TRUE(mesh_is_node);
    }

    int ofbx_animation_stack_count = ofbx_iscene->getAnimationStackCount();
    std::cout << filename << ": getAnimationStackCount(): " << ofbx_animation_stack_count << "\n";
    ASSERT_GT(ofbx_mesh_count, 0);

    const ofbx::Object* const* ofbx_all_objects = ofbx_iscene->getAllObjects();
    ASSERT_NE(ofbx_all_objects, nullptr);

    const int all_object_count = ofbx_iscene->getAllObjectCount();
    std::cout << filename << ": getAllObjectCount(): " << all_object_count << "\n";
    ASSERT_GT(all_object_count, 0);
}
TEST(fbx_file_must_be_loaded_appropriately, rigged_and_animated_cat)
{
    std::string filename = "cat.fbx";
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_UVs;
    std::vector<glm::vec3> out_normals;

    int32_t mesh_i = 0;
    bool result = loaders::load_FBX(filename, mesh_i, out_vertices, out_UVs, out_normals);
    ASSERT_TRUE(result);
}