# Unity Structures (Engine Components & Math)

BNM provides native C++20 wrappers for Unity Engine objects, 3D math structures, physics queries, rendering, audio, animation, and hierarchy management.

---

## 1. GameObject & Transform

```cpp
#include <BNM/UnityStructures/GameObject.hpp>
#include <BNM/UnityStructures/Transform.hpp>
using namespace BNM::UnityEngine;

// 1. Spawning a new GameObject
GameObject *myObj = GameObject::Create("MyCustomModObject");

// 2. Finding an existing GameObject by Name or Tag
GameObject *player = GameObject::Find("Player");
GameObject *tagged = GameObject::FindWithTag("MainCamera");

// 3. Transform operations & Parenting
Transform *transform = myObj->GetTransform();
transform->SetPosition({0.0f, 5.0f, -10.0f});
transform->SetLocalPosition({0.0f, 0.0f, 0.0f});
transform->SetLocalScale({1.0f, 1.0f, 1.0f});
transform->SetParent(player->GetTransform(), false);

// 4. Enumerating Children & Finding Sub-nodes
std::vector<Transform *> children = transform->GetChildren();
for (auto child : children) {
    BNM_LOG_INFO("Child object: %s", child->GetGameObject()->GetName()->str().c_str());
}

Transform *childNode = transform->Find("Armature/Hand");

// 5. Adding & Getting Components
auto audioSource = myObj->AddComponent<AudioSource *>();
auto rb = myObj->GetComponent<Rigidbody *>();
```

---

## 2. Global Scene Hierarchy Discovery (`SceneManager`)

Enumerate all root game objects in the active scene and persistent objects stored in `DontDestroyOnLoad`:

```cpp
#include <BNM/UnityStructures/SceneManager.hpp>
using namespace BNM::UnityEngine::SceneManagement;

// Enumerates all root objects across active scenes and DontDestroyOnLoad
std::vector<BNM::UnityEngine::GameObject *> allRoots = SceneManager::GetAllRootGameObjects(true /* includeDontDestroyOnLoad */);

for (auto rootGo : allRoots) {
    BNM_LOG_INFO("Root Entity: %s (Active: %d)", rootGo->GetName()->str().c_str(), rootGo->GetActive());
}

// Scene inspection
Scene activeScene = SceneManager::GetActiveScene();
BNM_LOG_INFO("Active Scene: %s (Build Index: %d)", activeScene.GetName().c_str(), activeScene.GetBuildIndex());
```

---

## 3. 3D Math Vectors, Matrices & Geometry

BNM includes IL2CPP-compatible structs for standard 3D math:

```cpp
#include <BNM/UnityStructures/Vector2.hpp>
#include <BNM/UnityStructures/Vector3.hpp>
#include <BNM/UnityStructures/Vector4.hpp>
#include <BNM/UnityStructures/Quaternion.hpp>
#include <BNM/UnityStructures/Color.hpp>
#include <BNM/UnityStructures/Matrix3x3.hpp>
#include <BNM/UnityStructures/Matrix4x4.hpp>
#include <BNM/UnityStructures/Rect.hpp>
#include <BNM/UnityStructures/Ray.hpp>
#include <BNM/UnityStructures/Bounds.hpp>
using namespace BNM::Structures::Unity;

// Vector arithmetic
Vector3 pos1(0.f, 10.f, 0.f);
Vector3 pos2(5.f, 0.f, 5.f);
float dist = Vector3::Distance(pos1, pos2);
Vector3 norm = pos1.Normalized();

// Quaternion rotations
Quaternion rot = Quaternion::Euler(0.0f, 90.0f, 0.0f);
Quaternion look = Quaternion::LookRotation({0.f, 0.f, 1.f}, {0.f, 1.f, 0.f});

// Colors
Color red = Color::Red();
Color transparentBlue(0.0f, 0.5f, 1.0f, 0.8f);

// Geometry
Rect screenRect(0.f, 0.f, 1920.f, 1080.f);
Bounds boxBounds(Vector3::Zero(), Vector3::One());
```

---

## 4. 3D & 2D Physics & Raycasting

```cpp
#include <BNM/UnityStructures/Physics.hpp>
#include <BNM/UnityStructures/RaycastHit.hpp>
#include <BNM/UnityStructures/Physics2D.hpp>
using namespace BNM::UnityEngine;

// 3D Physics Raycast
Structures::Unity::RaycastHit hit;
Structures::Unity::Vector3 origin(0.f, 10.f, 0.f);
Structures::Unity::Vector3 direction(0.f, -1.f, 0.f);

if (Physics::Raycast(origin, direction, &hit, 100.0f)) {
    Collider *hitCol = hit.GetCollider();
    Structures::Unity::Vector3 point = hit.GetPoint();
    BNM_LOG_INFO("3D Raycast Hit at Y: %.2f on GameObject: %s", 
        point.y, 
        hitCol->GetGameObject()->GetName()->str().c_str()
    );
}

// 2D Physics Raycast
Structures::Unity::RaycastHit2D hit2D;
if (Physics2D::Raycast({0.f, 10.f}, {0.f, -1.f}, &hit2D, 100.0f)) {
    BNM_LOG_INFO("2D Raycast Hit: %s", hit2D.GetCollider()->GetGameObject()->GetName()->str().c_str());
}
```

---

## 5. Rendering & Materials

```cpp
#include <BNM/UnityStructures/Camera.hpp>
#include <BNM/UnityStructures/MeshRenderer.hpp>
#include <BNM/UnityStructures/Material.hpp>
#include <BNM/UnityStructures/Shader.hpp>
#include <BNM/UnityStructures/Texture2D.hpp>
using namespace BNM::UnityEngine;

// Main Camera
Camera *mainCam = Camera::GetMain();
Structures::Unity::Vector3 screenPos = mainCam->WorldToScreenPoint({0.f, 0.f, 0.f});

// Materials and Shaders
Shader *chamsShader = Shader::Find("Hidden/Internal-Colored");
Material *customMat = Material::Create(chamsShader);
customMat->SetColor("_Color", Structures::Unity::Color::Green());

// MeshRenderer
auto renderer = myObj->GetComponent<MeshRenderer *>();
renderer->SetMaterial(customMat);
```

---

## 6. Audio & Animation

```cpp
#include <BNM/UnityStructures/Audio.hpp>
#include <BNM/UnityStructures/Animation.hpp>
using namespace BNM::UnityEngine;

// Audio Playback
auto audio = myObj->GetComponent<AudioSource *>();
audio->SetVolume(1.0f);
audio->SetPitch(1.2f);
audio->Play();

// Animator
auto animator = myObj->GetComponent<Animator *>();
animator->Play("Walk");
animator->SetSpeed(1.5f);
```

---

## 7. Engine Services (Time, Screen, Input, PlayerPrefs, SystemInfo)

```cpp
#include <BNM/UnityStructures/Time.hpp>
#include <BNM/UnityStructures/Screen.hpp>
#include <BNM/UnityStructures/Input.hpp>
#include <BNM/UnityStructures/PlayerPrefs.hpp>
#include <BNM/UnityStructures/Application.hpp>
#include <BNM/UnityStructures/SystemInfo.hpp>
#include <BNM/UnityStructures/Resources.hpp>
using namespace BNM::UnityEngine;

// Time & Framerate
float dt = Time::GetDeltaTime();
Time::SetTimeScale(2.0f); // Fast forward

// Screen dimensions
int width = Screen::GetWidth();
int height = Screen::GetHeight();

// Input
bool spacePressed = Input::GetKeyDown(32 /* KeyCode.Space */);
Structures::Unity::Vector3 mousePos = Input::GetMousePosition();

// PlayerPrefs persistent storage
PlayerPrefs::SetInt("HighScore", 99999);
PlayerPrefs::Save();

// System Information
std::string deviceModel = SystemInfo::GetDeviceModel();
std::string gpuName = SystemInfo::GetGraphicsDeviceName();

// Asset & Resource Loading
Material *loadedMat = Resources::Load<Material *>("Materials/Highlight");
```
