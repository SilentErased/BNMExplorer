#pragma once

#include "UserSettings/GlobalSettings.hpp"

// --- Math & Basic Structures ---
#include "UnityStructures/Color.hpp"
#include "UnityStructures/Quaternion.hpp"
#include "UnityStructures/Ray.hpp"
#include "UnityStructures/RaycastHit.hpp"
#include "UnityStructures/RaycastHit2D.hpp"
#include "UnityStructures/Rect.hpp"
#include "UnityStructures/Bounds.hpp"
#include "UnityStructures/Vector2.hpp"
#include "UnityStructures/Vector3.hpp"
#include "UnityStructures/Vector4.hpp"
#include "UnityStructures/Matrix3x3.hpp"
#include "UnityStructures/Matrix4x4.hpp"

// --- Core UnityEngine Classes ---
#include "UnityStructures/Object.hpp"
#include "UnityStructures/Component.hpp"
#include "UnityStructures/Transform.hpp"
#include "UnityStructures/GameObject.hpp"
#include "UnityStructures/Behaviour.hpp"
#include "UnityStructures/MonoBehaviour.hpp"
#include "UnityStructures/ScriptableObject.hpp"
#include "UnityStructures/Application.hpp"
#include "UnityStructures/Events.hpp"
#include "CustomEvent.hpp"

// --- Core Utilities, Input & System ---
#include "UnityStructures/Time.hpp"
#include "UnityStructures/Screen.hpp"
#include "UnityStructures/Input.hpp"
#include "UnityStructures/SystemInfo.hpp"
#include "UnityStructures/PlayerPrefs.hpp"

// --- Scene Management & Assets ---
#include "UnityStructures/AsyncOperation.hpp"
#include "UnityStructures/SceneManager.hpp"
#include "UnityStructures/Resources.hpp"

// --- Rendering, Camera & Lighting ---
#include "UnityStructures/Shader.hpp"
#include "UnityStructures/Texture.hpp"
#include "UnityStructures/Texture2D.hpp"
#include "UnityStructures/RenderTexture.hpp"
#include "UnityStructures/Material.hpp"
#include "UnityStructures/Light.hpp"
#include "UnityStructures/Camera.hpp"

// --- Optional Visual Renderers & Mesh ---
#ifdef BNM_UNITY_RENDERERS
#include "UnityStructures/Renderer.hpp"
#include "UnityStructures/Mesh.hpp"
#include "UnityStructures/MeshRenderer.hpp"
#include "UnityStructures/SkinnedMeshRenderer.hpp"
#include "UnityStructures/SpriteRenderer.hpp"
#endif

// --- Optional Audio ---
#ifdef BNM_UNITY_AUDIO
#include "UnityStructures/Audio.hpp"
#endif

// --- Optional Animation ---
#ifdef BNM_UNITY_ANIMATION
#include "UnityStructures/Animation.hpp"
#endif

// --- Optional Physics Modules ---
#ifdef BNM_UNITY_PHYSICS
#include "UnityStructures/Physics.hpp"
#endif

#ifdef BNM_UNITY_PHYSICS2D
#include "UnityStructures/Physics2D.hpp"
#endif

// --- Optional UI & TextMeshPro Modules ---
#ifdef BNM_UNITY_UI
#include "UnityStructures/UI.hpp"
#endif

#ifdef BNM_UNITY_TEXTMESHPRO
#include "UnityStructures/TextMeshPro.hpp"
#endif
