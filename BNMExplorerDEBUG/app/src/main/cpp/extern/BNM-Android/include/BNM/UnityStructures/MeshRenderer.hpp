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
#include "Renderer.hpp"
#include "Mesh.hpp"

namespace BNM::UnityEngine {

    /**
        @brief Renders meshes inserted by the MeshFilter or TextMesh.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct MeshRenderer : public Renderer {

        /**
            @brief Vertex attributes in this mesh will override attributes in the MeshFilter when rendered.
            @return Additional vertex stream Mesh pointer.
        */
        inline Mesh *GetAdditionalVertexStreams() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<MeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_additionalVertexStreams"), 0).cast<Mesh *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets additional vertex streams for this renderer.
            @param mesh Mesh pointer with overriding attributes.
        */
        inline void SetAdditionalVertexStreams(Mesh *mesh) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_additionalVertexStreams"), 1).cast<void>();
            method[(void *)this]((void *)mesh);
        }

        /**
            @brief Enlighten dynamic vertex stream for real-time GI.
            @return Enlighten vertex stream Mesh pointer.
        */
        inline Mesh *GetEnlightenVertexStream() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<MeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_enlightenVertexStream"), 0).cast<Mesh *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets enlighten vertex stream for real-time GI.
            @param mesh Mesh pointer.
        */
        inline void SetEnlightenVertexStream(Mesh *mesh) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("set_enlightenVertexStream"), 1).cast<void>();
            method[(void *)this]((void *)mesh);
        }

        /**
            @brief Index of the first sub-mesh to render.
            @return Sub-mesh start index.
        */
        inline int GetSubMeshStartIndex() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<MeshRenderer>().ToClass().GetMethod(BNM_OBFUSCATE("get_subMeshStartIndex"), 0).cast<int>();
            return method[(void *)this]();
        }
    };
}

#endif
