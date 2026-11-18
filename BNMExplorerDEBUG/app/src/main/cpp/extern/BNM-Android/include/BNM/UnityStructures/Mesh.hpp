#pragma once

#include <string>
#include <string_view>
#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_RENDERERS

#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Object.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "Bounds.hpp"

namespace BNM::UnityEngine {

    /**
        @brief A class that allows creating or modifying meshes from scripts.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Mesh : public Object {

        /**
            @brief Creates a new empty Mesh instance.
            @return Pointer to new Mesh object.
        */
        static inline Mesh *Create() {
            return (Mesh *) BNM::Defaults::Get<Mesh>().ToClass().CreateNewObjectParameters();
        }

        /**
            @brief Returns a copy of the vertex positions or assigns a new vertex positions array.
            @return Mono Array of Vector3 vertex positions.
        */
        inline Structures::Mono::Array<Structures::Unity::Vector3> *GetVertices() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_vertices"), 0).cast<Structures::Mono::Array<Structures::Unity::Vector3> *>();
            return method[(void *)this]();
        }

        /**
            @brief Assigns a new vertex positions array.
            @param vertices Array of Vector3 vertex positions.
        */
        inline void SetVertices(Structures::Mono::Array<Structures::Unity::Vector3> *vertices) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_vertices"), 1).cast<void>();
            method[(void *)this]((void *)vertices);
        }

        /**
            @brief An array containing all triangles in the Mesh.
            @return Mono Array of triangle index integers.
        */
        inline Structures::Mono::Array<int> *GetTriangles() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_triangles"), 0).cast<Structures::Mono::Array<int> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the triangle list for the Mesh.
            @param triangles Array of triangle vertex indices.
        */
        inline void SetTriangles(Structures::Mono::Array<int> *triangles) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_triangles"), 1).cast<void>();
            method[(void *)this]((void *)triangles);
        }

        /**
            @brief The normals of the Mesh.
            @return Mono Array of Vector3 normal vectors.
        */
        inline Structures::Mono::Array<Structures::Unity::Vector3> *GetNormals() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_normals"), 0).cast<Structures::Mono::Array<Structures::Unity::Vector3> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the normals of the Mesh.
            @param normals Array of Vector3 normal vectors.
        */
        inline void SetNormals(Structures::Mono::Array<Structures::Unity::Vector3> *normals) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_normals"), 1).cast<void>();
            method[(void *)this]((void *)normals);
        }

        /**
            @brief The tangents of the Mesh.
            @return Mono Array of Vector4 tangents.
        */
        inline Structures::Mono::Array<Structures::Unity::Vector4> *GetTangents() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_tangents"), 0).cast<Structures::Mono::Array<Structures::Unity::Vector4> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the tangents of the Mesh.
            @param tangents Array of Vector4 tangents.
        */
        inline void SetTangents(Structures::Mono::Array<Structures::Unity::Vector4> *tangents) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_tangents"), 1).cast<void>();
            method[(void *)this]((void *)tangents);
        }

        /**
            @brief The base texture coordinates of the Mesh (UV channel 0).
            @return Mono Array of Vector2 UV coordinates.
        */
        inline Structures::Mono::Array<Structures::Unity::Vector2> *GetUV() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_uv"), 0).cast<Structures::Mono::Array<Structures::Unity::Vector2> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the base texture coordinates of the Mesh (UV channel 0).
            @param uv Array of Vector2 UV coordinates.
        */
        inline void SetUV(Structures::Mono::Array<Structures::Unity::Vector2> *uv) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_uv"), 1).cast<void>();
            method[(void *)this]((void *)uv);
        }

        /**
            @brief Vertex colors of the Mesh.
            @return Mono Array of Color values.
        */
        inline Structures::Mono::Array<Structures::Unity::Color> *GetColors() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_colors"), 0).cast<Structures::Mono::Array<Structures::Unity::Color> *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets vertex colors of the Mesh.
            @param colors Array of Color values.
        */
        inline void SetColors(Structures::Mono::Array<Structures::Unity::Color> *colors) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_colors"), 1).cast<void>();
            method[(void *)this]((void *)colors);
        }

        /**
            @brief The bounding volume of the mesh.
            @return Bounds structure.
        */
        inline Structures::Unity::Bounds GetBounds() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_bounds"), 0).cast<Structures::Unity::Bounds>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the bounding volume of the mesh.
            @param bounds Bounds structure.
        */
        inline void SetBounds(Structures::Unity::Bounds bounds) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_bounds"), 1).cast<void>();
            method[(void *)this](bounds);
        }

        /**
            @brief The number of sub-meshes. Every material has a separate triangle list.
            @return Sub-mesh count integer.
        */
        inline int GetSubMeshCount() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_subMeshCount"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the number of sub-meshes.
            @param value Sub-mesh count integer.
        */
        inline void SetSubMeshCount(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("set_subMeshCount"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Returns the number of vertices in the Mesh.
            @return Vertex count integer.
        */
        inline int GetVertexCount() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("get_vertexCount"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Recalculates the bounding volume of the Mesh from the vertices.
        */
        inline void RecalculateBounds() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("RecalculateBounds"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Recalculates the normals of the Mesh from the triangles and vertices.
        */
        inline void RecalculateNormals() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("RecalculateNormals"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Recalculates the tangents of the Mesh from the normals and texture coordinates.
        */
        inline void RecalculateTangents() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("RecalculateTangents"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Clears all vertex data and all triangle indices.
            @param keepVertexLayout Set to false to clear vertex layout information.
        */
        inline void Clear(bool keepVertexLayout = true) {
            if (!IsValid()) return;
            static auto method1 = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("Clear"), 1).cast<void>();
            if (method1.IsValid()) {
                method1[(void *)this](keepVertexLayout);
                return;
            }
            static auto method0 = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("Clear"), 0).cast<void>();
            if (method0.IsValid()) method0[(void *)this]();
        }

        /**
            @brief Optimizes the Mesh data to improve rendering performance.
        */
        inline void Optimize() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("Optimize"), 0).cast<void>();
            if (method.IsValid()) method[(void *)this]();
        }

        /**
            @brief Uploads previously done Mesh modifications to the graphics API.
            @param markNoLongerReadable Frees CPU copy of mesh data if true.
        */
        inline void UploadMeshData(bool markNoLongerReadable = false) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Mesh>().ToClass().GetMethod(BNM_OBFUSCATE("UploadMeshData"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](markNoLongerReadable);
        }
    };
}

#endif
