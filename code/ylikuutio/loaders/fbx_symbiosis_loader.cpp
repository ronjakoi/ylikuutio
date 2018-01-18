#include "fbx_symbiosis_loader.hpp"
#include "code/ylikuutio/file/file_loader.hpp"
#include <ofbx.h>

// OpenFBX wants `u8` == `unsigned char`.
typedef unsigned char u8;

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

namespace loaders
{
    bool load_FBX(
            const std::string& filename,
            std::vector<std::vector<glm::vec3>>& out_vertices,
            std::vector<std::vector<glm::vec2>>& out_uvs,
            std::vector<std::vector<glm::vec3>>& out_normals)
    {
        // Functions and data of interest in OpenFBX:
        // struct TakeInfo
        // {
        //     DataView name;
        //     DataView filename;
        //     double local_time_from;
        //     double local_time_to;
        //     double reference_time_from;
        //     double reference_time_to;
        // };
        //
        // IScene* load(const u8* data, int size)
        const std::vector<uint8_t> data_vector = file::binary_slurp(filename);

        // OpenFBX wants `u8` == `unsigned char`.
        const u8* data = reinterpret_cast<const u8*>(data_vector.data());
        const int64_t size = data_vector.size();
        std::cout << "loaded FBX data vector size: " << size << "\n";

        const ofbx::IScene* ofbx_iscene = ofbx::load(data, size);

        if (ofbx_iscene == nullptr)
        {
            std::cerr << "Error: ofbx_iscene is nullptr!\n";
            return false;
        }

        const int32_t mesh_count = static_cast<const int32_t>(ofbx_iscene->getMeshCount()); // `getMeshCount()` returns `int`.

        for (int32_t mesh_i = 0; mesh_i < mesh_count; mesh_i++)
        {
            const ofbx::Mesh* mesh = ofbx_iscene->getMesh(mesh_i);

            if (mesh == nullptr)
            {
                std::cerr << "Error: mesh is nullptr!\n";
                return false;
            }

            const ofbx::Geometry* geometry = mesh->getGeometry();

            if (geometry == nullptr)
            {
                std::cerr << "Error: geometry is nullptr!\n";
                return false;
            }

            // TODO: finalize the implementation of `ontology::Symbiosis`
            // to be able to support for different materials!
            const int material_count = mesh->getMaterialCount(); // TODO: use this in  `ontology::Symbiosis` entities!
            std::cout << filename << ": mesh " << mesh_i << ": getMaterialCount(): " << material_count << "\n";

            const int vertex_count = geometry->getVertexCount();
            std::cout << filename << ": mesh " << mesh_i << ": getVertexCount(): " << vertex_count << "\n";

            const ofbx::Vec3* vertices = geometry->getVertices();

            if (vertices == nullptr)
            {
                std::cerr << "Error: vertices is nullptr!\n";
                return false;
            }

            const ofbx::Vec3* normals = geometry->getNormals();

            if (normals == nullptr)
            {
                std::cerr << "Error: normals is nullptr!\n";
                return false;
            }

            const ofbx::Vec2* uvs = geometry->getUVs();

            if (uvs == nullptr)
            {
                std::cerr << "Error: uvs is nullptr!\n";
                return false;
            }

            std::vector<glm::vec3> mesh_out_vertices;

            for (int i = 0; i < vertex_count; i++)
            {
                // vertices.
                glm::vec3 vertex = { vertices[i].x, vertices[i].y, vertices[i].z };
                mesh_out_vertices.push_back(vertex);
            }

            out_vertices.push_back(mesh_out_vertices);

            std::vector<glm::vec2> mesh_out_uvs;

            for (int i = 0; i < vertex_count; i++)
            {
                // UVs.
                glm::vec2 uv = { uvs[i].x, uvs[i].y };
                mesh_out_uvs.push_back(uv);
            }

            out_uvs.push_back(mesh_out_uvs);

            std::vector<glm::vec3> mesh_out_normals;

            for (int i = 0; i < vertex_count; i++)
            {
                // Normals.
                glm::vec3 normal = { normals[i].x, normals[i].y, normals[i].z };
                mesh_out_normals.push_back(normal);
            }

            out_normals.push_back(mesh_out_normals);
        }

        return true;
    }
}
