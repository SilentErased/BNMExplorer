#pragma once
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Class.hpp>
#include <BNM/ClassesManagement.hpp>
#include <BNM/ComplexMonoStructures.hpp>
#include <BNM/Coroutine.hpp>
#include <BNM/DebugMessages.hpp>
#include <BNM/Defaults.hpp>
#include <BNM/Delegates.hpp>
#include <BNM/Event.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/EventBase.hpp>
#include <BNM/Exceptions.hpp>
#include <BNM/Field.hpp>
#include <BNM/FieldBase.hpp>
#include <BNM/Il2CppHeaders.hpp>
#include <BNM/Image.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Method.hpp>
#include <BNM/MethodBase.hpp>
#include <BNM/Operators.hpp>
#include <BNM/Property.hpp>
#include <BNM/PropertyBase.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/Utils.hpp>

#define O(str) BNM_OBFUSCATE(str)

using namespace BNM;
using namespace Structures::Unity;
using namespace Structures::Mono;
using namespace UnityEngine;

struct NamedObject;
struct Component;
struct GameObject;
struct Transform;
struct Behaviour;
struct Canvas;
struct CanvasScaler;
struct Camera;
struct UIBehavior;
struct BaseRaycaster;
struct GraphicRaycaster;
struct Shader;
struct Renderer;
struct Material;
struct RectTransform;
struct Graphic;
struct MaskableGraphic;
struct Text;
struct Font;
struct LineRenderer;
struct Rigidbody;
struct Time;
struct Collider;
struct SphereCollider;
struct BoxCollider;
struct MeshRenderer;
struct Resources;
struct AssetBundle;
struct Physics;
struct LightmapData;
struct LightmapSettings;
struct Texture2D;
struct GradientColorKey;
struct GradientAlphaKey;
struct Gradient;
struct Skybox;
struct Sprite;
struct QualitySettings;
struct ParticleSystem;
struct Light;
struct AudioClip;
struct AudioSource;
struct LODGroup;
struct Matrix4x4;
struct Animator;
struct MonoBehaviour;

//enums
enum GradientMode
{
    Blend,
    Fixed,
    PerceptualBlend
};
enum RenderMode
{
    ScreenSpaceOverlay,
    ScreenSpaceCamera,
    WorldSpace
};
enum PrimitiveType{
    Sphere,
    Capsule,
    Cylinder,
    Cube,
    Plane,
    Quad
};
enum TextAnchor
{
    UpperLeft,
    UpperCenter,
    UpperRight,
    MiddleLeft,
    MiddleCenter,
    MiddleRight,
    LowerLeft,
    LowerCenter,
    LowerRight
};

enum ForceMode
{
    Force = 0,
    Acceleration = 5,
    Impulse = 1,
    VelocityChange = 2
};

enum FontStyle
{
    Normal,
    Bold,
    Italic,
    BoldAndItalic
};

enum SpriteMeshType
{
    FullRect,
    Tight
};

// idk if this is correct or not
enum TextureFormat
{
    Alpha8 = 1,
    ARGB4444 = 2,
    RGB24 = 3,
    RGBA32 = 4,
    ARGB32 = 5,
    RGB565 = 7,
    R16 = 9,
    DXT1 = 10,
    DXT5 = 12,
    RGBA4444 = 13,
    BGRA32 = 14,
    RHalf = 15,
    RGHalf = 16,
    RGBAHalf = 17,
    RFloat = 18,
    RGFloat = 19,
    RGBAFloat = 20,
    YUY2 = 21,
    RGB9e5Float = 22,
    BC4 = 26,
    BC5 = 27,
    BC6H = 24,
    BC7 = 25,
    DXT1Crunched = 28,
    DXT5Crunched = 29,
    PVRTC_RGB2 = 30,
    PVRTC_RGBA2 = 31,
    PVRTC_RGB4 = 32,
    PVRTC_RGBA4 = 33,
    ETC_RGB4 = 34,
    ATC_RGB4 = 35,
    ATC_RGBA8 = 36,
    EAC_R = 41,
    EAC_R_SIGNED = 42,
    EAC_RG = 43,
    EAC_RG_SIGNED = 44,
    ETC2_RGB4 = 45,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA = 46,
    ETC2_RGBA8 = 47,
    ASTC_4x4 = 48,
    ASTC_5x5 = 49,
    ASTC_6x6 = 50,
    ASTC_8x8 = 51,
    ASTC_10x10 = 52,
    ASTC_12x12 = 53,
    ETC_RGB4_3DS = 60,
    ETC_RGBA8_3DS = 61,
    RG16 = 62,
    R8 = 63,
    ETC_RGB4Crunched = 64,
    ETC2_RGBA8Crunched = 65,
    ASTC_HDR_4x4 = 66,
    ASTC_HDR_5x5 = 67,
    ASTC_HDR_6x6 = 68,
    ASTC_HDR_8x8 = 69,
    ASTC_HDR_10x10 = 70,
    ASTC_HDR_12x12 = 71,
    EAC_R_UNSIGNED = 72,
    EAC_RG_UNSIGNED = 74,
    ETC2_RGB4_UNSIGNED = 76,
    ETC2_RGB4_SIGNED = 77,
    ETC2_RGBA8_UNSIGNED = 78,
    ETC2_RGBA8_SIGNED = 79,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA_UNSIGNED = 80,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA_SIGNED = 81,
    ETC2_RGBA8_PUNCHTHROUGH_ALPHA_UNSIGNED = 82,
    ETC2_RGBA8_PUNCHTHROUGH_ALPHA_SIGNED = 83,
    PBGRA32 = 84,
    PVRTC_2BPP_RGB = 96,
    PVRTC_2BPP_RGBA = 97,
    PVRTC_4BPP_RGB = 98,
    PVRTC_4BPP_RGBA = 99,
    ATC_RGB4_3DS = 100,
    ATC_RGBA8_3DS = 101,
    ETC_RGB4Crunched_3DS = 102,
    ETC_RGBA8Crunched_3DS = 103,
    ETC2_RGB4_3DS = 104,
    ETC2_RGBA8_3DS = 105,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA_3DS = 106,
    ETC2_RGBA8_PUNCHTHROUGH_ALPHA_3DS = 107,
    EAC_R_3DS = 108,
    EAC_R_SIGNED_3DS = 109,
    EAC_RG_3DS = 110,
    EAC_RG_SIGNED_3DS = 111,
    ETC2_RGB4_UNSIGNED_3DS = 112,
    ETC2_RGB4_SIGNED_3DS = 113,
    ETC2_RGBA8_UNSIGNED_3DS = 114,
    ETC2_RGBA8_SIGNED_3DS = 115,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA_UNSIGNED_3DS = 116,
    ETC2_RGB4_PUNCHTHROUGH_ALPHA_SIGNED_3DS = 117,
    ETC2_RGBA8_PUNCHTHROUGH_ALPHA_UNSIGNED_3DS = 118,
    ETC2_RGBA8_PUNCHTHROUGH_ALPHA_SIGNED_3DS = 119,
    RGBA5551 = 123,
};

//structs
struct GradientColorKey {
    Color color;
    float time;
};

struct GradientAlphaKey {
    float alpha;
    float time;
};
struct Gradient : BNM::IL2CPP::Il2CppObject {
    static Class GetClass() {
        static Class gradient = Class("UnityEngine", "Gradient");
        return gradient;
    }
    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    void SetColorKeys(std::vector<GradientColorKey> keys) {
        static auto set_colorKeys = (void(*)(void*, Array<GradientColorKey>*))GetExternMethod("UnityEngine.Gradient::set_colorKeys");
        Array<GradientColorKey>* keyArr = Array<GradientColorKey>::Create(keys);
        set_colorKeys(this, keyArr);
    }

    void SetAlphaKeys(std::vector<GradientAlphaKey> keys) {
        static auto set_alphaKeys = (void(*)(void*, Array<GradientAlphaKey>*))GetExternMethod("UnityEngine.Gradient::set_alphaKeys");
        Array<GradientAlphaKey>* keyArr = Array<GradientAlphaKey>::Create(keys);
        set_alphaKeys(this, keyArr);
    }

    void SetKeys(std::vector<GradientColorKey> colorKeys, std::vector<GradientAlphaKey> alphaKeys) {
        static auto SetKeysMethod = (void(*)(void*, Array<GradientColorKey>*, Array<GradientAlphaKey>*))GetExternMethod("UnityEngine.Gradient::SetKeys");
        Array<GradientColorKey>* colorKeyArray = Array<GradientColorKey>::Create(colorKeys);
        Array<GradientAlphaKey>* alphaKeyArray = Array<GradientAlphaKey>::Create(alphaKeys);
        SetKeysMethod(this, colorKeyArray, alphaKeyArray);
    }

    Array<GradientColorKey>* GetColorKeys() {
        static auto get_colorKeys = (Array<GradientColorKey>*(*)(void*))GetExternMethod("UnityEngine.Gradient::get_colorKeys");
        return get_colorKeys(this);
    }

    Array<GradientAlphaKey>* GetAlphaKeys() {
        static auto get_alphaKeys = (Array<GradientAlphaKey>*(*)(void*))GetExternMethod("UnityEngine.Gradient::get_alphaKeys");
        return get_alphaKeys(this);
    }

    Color Evaluate(float time) {
        static auto evaluate = (Color(*)(void*, float))GetExternMethod("UnityEngine.Gradient::Evaluate");
        return evaluate(this, time);
    }

    void SetMode(GradientMode mode) {
        static auto set_mode = (void(*)(void*, int))GetExternMethod("UnityEngine.Gradient::set_mode");
        set_mode(this, (int)mode);
    }

    GradientMode GetMode() {
        static auto get_mode = (int(*)(void*))GetExternMethod("UnityEngine.Gradient::get_mode");
        return (GradientMode)get_mode(this);
    }
};
struct NamedObject : Object{ // pretty much Object but for some reason BNMDev didn't add .name for it
    static Class GetClass() {
        static Class mclass = Class("UnityEngine", "Object");
        return mclass;
    }
    static MonoType* GetType() {
        static MonoType* type = GetClass().GetMonoType();
        return type;
    }

    std::string GetName() {
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        String* name = get_name[this]();
        return name->str();
    }
    void SetName(std::string name) {
        static Method<void> set_name = GetClass().GetMethod("set_name", 1);
        set_name[this](CreateMonoString(name));
    }
};

struct Component : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Component").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Component");
        return mclass;
    }

    Transform* GetTransform(){
        static auto get_transform = (Transform*(*)(void*))GetExternMethod("UnityEngine.Component::get_transform");
        return get_transform(this);
    }
    GameObject* GetGameObject(){
        static auto get_gameObject = (GameObject*(*)(void*))GetExternMethod("UnityEngine.Component::get_gameObject");
        return get_gameObject(this);
    }
    std::string GetTag(){
        static Method<String*> get_tag = GetClass().GetMethod("get_tag");
        auto tag = get_tag[this]();
        return tag->str();
    }
    void SetTag(std::string tag){
        static Method<void> set_tag = GetClass().GetMethod("set_tag");
        set_tag[this](CreateMonoString(tag));
    }

    std::string GetName(){
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto tag = get_name[this]();
        return tag->str();
    }
    void SetName(std::string tag){
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(tag));
    }

    Component* GetComponent(MonoType* type) {
        static Method<Component*> GetComponent = GetClass().GetMethod("GetComponent", 1);
        GetComponent.SetInstance(this);
        return GetComponent(type);
    }
    Component* GetComponentInChildren(MonoType* type, bool includeInactive) {
        static Method<Component*> GetComponentInChildren = GetClass().GetMethod("GetComponentInChildren", 2);
        GetComponentInChildren.SetInstance(this);
        return GetComponentInChildren(type, includeInactive);
    }
    Component* GetComponentInParent(MonoType* type, bool includeInactive) {
        static Method<Component*> GetComponentInParent = GetClass().GetMethod("GetComponentInParent", 2);
        GetComponentInParent.SetInstance(this);
        return GetComponentInParent(type, includeInactive);
    }
};
struct GameObject : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "GameObject").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "GameObject");
        return mclass;
    }
    static void Destroy(Object* obj){
        static Method<void> Destroy = Class("UnityEngine", "Object").GetMethod("Destroy", 1);
        Destroy(obj);
    }
    static void Destroy(Object* obj, float t) {
        static Method<void> Destroy = Class("UnityEngine", "Object").GetMethod("Destroy", 2);
        Destroy(obj, t);
    }
    static GameObject *CreatePrimitive(PrimitiveType primitiveType){
        static auto CreatePrimitive = (GameObject*(*)(PrimitiveType))GetExternMethod("UnityEngine.GameObject::CreatePrimitive");
        return CreatePrimitive(primitiveType);
    }

    static Object *Instantiate(Object* original, Vector3 position, Quaternion rotation) {
        static Method<Object*> Instantiate = GetClass().GetMethod("Instantiate", { "original", "position", "rotation" });
        return Instantiate(original, position, rotation);
    }
    static Object* Instantiate(Object* original) {
        static Method<Object*> Instantiate = GetClass().GetMethod("Instantiate", { "original" });
        return Instantiate(original);
    }
    static Object* Instantiate(Object* original, Transform* parent, bool instantiateInWorldSpace) {
        static Method<Object*> Instantiate = GetClass().GetMethod("Instantiate", { "original", "parent", "instantiateInWorldSpace" });
        return Instantiate(original, parent, instantiateInWorldSpace);
    }

    static void DontDestroyOnLoad(Object* object){
        static Method<void> DontDestroyOnLoad = GetClass().GetMethod("DontDestroyOnLoad");
        DontDestroyOnLoad(object);
    }

    Array<Component*>* GetComponentsInChildren(MonoType* type){
        static Method<Array<Component*>*> GetComponentsInChildren = GetClass().GetMethod("GetComponentsInChildren");
        return GetComponentsInChildren[this](type);
    }

    Array<Component*>* GetComponentsInParent(MonoType* type){
        static Method<Array<Component*>*> GetComponentsInParent = GetClass().GetMethod("GetComponentsInParent");
        return GetComponentsInParent[this](type);
    }
    Array<Component*>* GetComponents(MonoType* type){
        static Method<Array<Component*>*> GetComponents = GetClass().GetMethod("GetComponents");
        return GetComponents[this](type);
    }
    Component* GetComponentInParent(MonoType* type, bool includeInactive){
        static auto GetComponentInParent = (Component*(*)(void*, MonoType*, bool))GetExternMethod("UnityEngine.GameObject::GetComponentInParent"); // consistently extern :(
        return GetComponentInParent(this, type, includeInactive);
    }
    static Array<Object*>* FindObjectsOfType(MonoType * type){
        static Method<Array<Object*>*> FindObjectsOfType = GetClass().GetMethod("FindObjectsOfType", {"type"});
        return FindObjectsOfType(type);
    }
    static Object* FindObjectOfType(MonoType* type){
        static Method<Object*> FindObjectOfType = GetClass().GetMethod("FindObjectOfType", {"type"});
        return FindObjectOfType(type);
    }
    Component* GetComponent(MonoType* type){
        static auto GetComponent = (Component*(*)(void*, MonoType*))GetExternMethod("UnityEngine.GameObject::GetComponent");
        return GetComponent(this, type);
    }
    Component* AddComponent(MonoType* type){
        static Method<Component*> AddComponent = GetClass().GetMethod("AddComponent");
        return AddComponent[this](type);
    }
    Transform* GetTransform(){
        static auto get_transform = (Transform*(*)(void*))GetExternMethod("UnityEngine.GameObject::get_transform");
        return get_transform(this);
    }
    std::string GetTag(){
        static auto get_tag = (String*(*)(void*))GetExternMethod("UnityEngine.GameObject::get_tag");
        auto tag = get_tag(this);
        return tag->str();
    }
    void SetTag(std::string str){
        static auto set_tag = (void (*)(void*, String*))GetExternMethod("UnityEngine.GameObject::set_tag");
        set_tag(this, CreateMonoString(str));
    }
    static GameObject* Find(std::string str){
        static auto Find = (GameObject*(*)(String*)) GetExternMethod("UnityEngine.GameObject::Find");
        return Find(CreateMonoString(str));
    }
    void SetActive(bool active){
        static auto SetActive = (void (*)(void*, bool))GetExternMethod("UnityEngine.GameObject::SetActive");
        SetActive(this, active);
    }
    bool GetActiveSelf(){
        static auto get_activeSelf = (bool (*)(void*))GetExternMethod("UnityEngine.GameObject::get_activeSelf");
        return get_activeSelf(this);
    }
    void SetLayer(int layer) {
        static auto set_layer = (void (*)(void*, int))GetExternMethod("UnityEngine.GameObject::set_layer");
        set_layer(this, layer);
    }
    int GetLayer() {
        static auto get_layer = (int (*)(void*))GetExternMethod("UnityEngine.GameObject::get_layer");
        return get_layer(this);
    }

    std::string GetName(){
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto tag = get_name[this]();
        return tag->str();
    }
    void SetName(std::string name){
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(name));
    }
};
struct Transform : Component{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Transform").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Transform");
        return mclass;
    }

    Vector3 GetPosition(){
        static Method<Vector3> get_position = GetClass().GetMethod("get_position");
        return get_position[this]();
    }
    void SetPosition(Vector3 position){
        static Method<void> set_position = GetClass().GetMethod("set_position");
        set_position[this](position);
    }
    Vector3 GetLocalPosition(){
        static Method<Vector3> get_position = GetClass().GetMethod("get_localPosition");
        return get_position[this]();
    }
    void SetLocalPosition(Vector3 position){
        static Method<void> set_position = GetClass().GetMethod("set_localPosition");
        set_position[this](position);
    }
    void LookAt(Transform* target){
        static Method<void> LookAt = GetClass().GetMethod("LookAt", {"target"});
        LookAt[this](target);
    }
    void LookAt(Vector3 worldPosition){
        static Method<void> LookAt = GetClass().GetMethod("LookAt", {"worldPosition"});
        LookAt[this](worldPosition);
    }
    void* GetParent(){
        static Method<void*> get_parent = GetClass().GetMethod("get_parent");
        return get_parent[this]();
    }
    void SetParent(Transform* parent){
        static Method<void> set_parent = GetClass().GetMethod("set_parent");
        set_parent[this](parent);
    }
    void SetParent(Transform* parent, bool worldPositionStays){
        static Method<void*> set_parent = GetClass().GetMethod("SetParent", 2);
        set_parent[this](parent, worldPositionStays);
    }
    Vector3 GetForward(){
        static Method<Vector3> get_forward = GetClass().GetMethod("get_forward");
        return get_forward[this]();
    }
    void SetForward(Vector3 forward){
        static Method<void> set_forward = GetClass().GetMethod("set_forward");
        set_forward[this](forward);
    }
    Vector3 GetRight(){
        static Method<Vector3> get_right = GetClass().GetMethod("get_right");
        return get_right[this]();
    }
    Vector3 GetUp() {
        static Method<Vector3> get_up = GetClass().GetMethod("get_up");
        return get_up[this]();
    }
    Vector3 GetLocalScale(){
        static Method<Vector3> get_localScale = GetClass().GetMethod("get_localScale");
        return get_localScale[this]();
    }
    void SetLocalScale(Vector3 scale){
        static Method<void> set_localScale = GetClass().GetMethod("set_localScale");
        set_localScale[this](scale);
    }
    Quaternion GetRotation(){
        static Method<Quaternion> get_rotation = GetClass().GetMethod("get_rotation");
        return get_rotation[this]();
    }
    void SetRotation(Quaternion rotation){
        static Method<void> set_rotation = GetClass().GetMethod("set_rotation");
        set_rotation[this](rotation);
    }
    Quaternion GetLocalRotation(){
        static Method<Quaternion> get_rotation = GetClass().GetMethod("get_localRotation");
        return get_rotation[this]();
    }
    void SetLocalRotation(Quaternion rotation){
        static Method<void> set_rotation = GetClass().GetMethod("set_localRotation");
        set_rotation[this](rotation);
    }
    Vector3 GetLocalEulerAngles() {
        static Method<Vector3> get_localEulerAngles = GetClass().GetMethod("get_localEulerAngles");
        return get_localEulerAngles[this]();
    }
    void SetLocalEulerAngles(Vector3 val) {
        static Method<void> set_localEulerAngles = GetClass().GetMethod("set_localEulerAngles");
        set_localEulerAngles[this](val);
    }
    Vector3 GetEulerAngles() {
        static Method<Vector3> get_eulerAngles = GetClass().GetMethod("get_eulerAngles");
        return get_eulerAngles[this]();
    }
    void SetEulerAngles(Vector3 val) {
        static Method<void> set_eulerAngles = GetClass().GetMethod("set_eulerAngles");
        set_eulerAngles[this](val);
    }
};
struct Behaviour : Component{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Behaviour").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Behaviour");
        return mclass;
    }
    void SetEnabled(bool enabled){
        static auto set_enabled = (void(*)(void*, bool))GetExternMethod("UnityEngine.Behaviour::set_enabled");
        set_enabled(this, enabled);
    }
    bool GetEnabled(){
        static auto get_enabled = (bool(*)(void*))GetExternMethod("UnityEngine.Behaviour::get_enabled");
        return get_enabled(this);
    }
};
struct Camera : Behaviour{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Camera").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Camera");
        return mclass;
    }

    static Camera* GetMain(){
        static auto get_main = (Camera*(*)())GetExternMethod("UnityEngine.Camera::get_main");
        return get_main();
    }
};
struct Canvas : Behaviour{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Canvas").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Canvas");
        return mclass;
    }
    void SetRenderMode(RenderMode mode){
        static auto set_renderMode = (void(*)(void*, RenderMode))GetExternMethod("UnityEngine.Canvas::set_renderMode");
        set_renderMode(this, mode);
    }
    RenderMode GetRenderMode(){
        static auto get_renderMode = (RenderMode(*)(void*))GetExternMethod("UnityEngine.Canvas::get_renderMode");
        return get_renderMode(this);
    }
    void SetWorldCamera(Camera* camera){
        static auto set_worldCamera = (void(*)(void*, Camera*))GetExternMethod("UnityEngine.Canvas::set_worldCamera");
        set_worldCamera(this, camera);
    }
    Camera GetWorldCamera(){
        static auto get_worldCamera = (Camera(*)(void*))GetExternMethod("UnityEngine.Canvas::get_worldCamera");
        return get_worldCamera(this);
    }
};
struct UIBehavior : Behaviour{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.EventSystems", "UIBehavior").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.EventSystems", "UIBehavior");
        return mclass;
    }
};
struct CanvasScaler : UIBehavior{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.UI", "CanvasScaler").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.UI", "CanvasScaler");
        return mclass;
    }
    float GetDynamicPixelsPerUnit() {
        static Method<float> get_dynamicPixelsPerUnit = GetClass().GetMethod("get_dynamicPixelsPerUnit");
        return get_dynamicPixelsPerUnit[this]();
    }
    void SetDynamicPixelsPerUnit(float canvasScaleFloat){
        static Method<void> set_dynamicPixelsPerUnit = GetClass().GetMethod("set_dynamicPixelsPerUnit");
        set_dynamicPixelsPerUnit[this](canvasScaleFloat);
    }
    float GetReferencePixelsPerUnit() {
        static Method<float> get_referencePixelsPerUnit = GetClass().GetMethod("get_referencePixelsPerUnit");
        return get_referencePixelsPerUnit[this]();
    }
    void SetReferencePixelsPerUnit(float canvasScaleFloat){
        static Method<void> set_referencePixelsPerUnit = GetClass().GetMethod("set_referencePixelsPerUnit");
        set_referencePixelsPerUnit[this](canvasScaleFloat);
    }
};
struct BaseRaycaster : UIBehavior{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.EventSystems", "BaseRaycaster").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.EventSystems", "BaseRaycaster");
        return mclass;
    }
};
struct GraphicRaycaster : BaseRaycaster{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.UI", "GraphicRaycaster").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.UI", "GraphicRaycaster");
        return mclass;
    }
};
struct Shader : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Shader").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Shader");
        return mclass;
    }
    static Shader* Find(std::string shaderName){
        static Method<Shader*> Find = GetClass().GetMethod("Find");
        return Find(CreateMonoString(shaderName));
    }
    std::string GetName(){
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto tag = get_name[this]();
        return tag->str();
    }
    void SetName(std::string name){
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(name));
    }

};
struct Material : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Material").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Material");
        return mclass;
    }
    Shader* GetShader(){
        static auto getShader = (Shader*(*)(void*)) GetExternMethod("UnityEngine.Material::get_shader");
        return getShader(this);
    }
    void SetShader(Shader* shader){
        static auto setShader = (void(*)(void*, Shader*)) GetExternMethod("UnityEngine.Material::set_shader");
        setShader(this, shader);
    }
    Color GetColor(){
        static Method<Color> get_color = GetClass().GetMethod("get_color");
        return get_color[this]();
    }
    void SetColor(Color color){
        static Method<void> set_color = GetClass().GetMethod("set_color");
        set_color[this](color);
    }
};
struct Renderer : Component{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Renderer").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Renderer");
        return mclass;
    }

    Material* GetMaterial(){
        static auto getMaterial = (Material*(*)(void*)) GetExternMethod("UnityEngine.Renderer::GetMaterial");
        return getMaterial(this);
    }
    void SetMaterial(Material* material){
        static auto setMaterial = (void(*)(void*, Material*)) GetExternMethod("UnityEngine.Renderer::SetMaterial");
        setMaterial(this, material);
    }
};
struct RectTransform : Transform{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "RectTransform").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "RectTransform");
        return mclass;
    }

    void SetSizeDelta(Vector2 sizeDelta){
        static Method<void> set_sizeDelta = GetClass().GetMethod("set_sizeDelta");
        set_sizeDelta[this](sizeDelta);
    }
    Vector2 GetSizeDelta(){
        static Method<Vector2> get_sizeDelta = GetClass().GetMethod("get_sizeDelta");
        return get_sizeDelta[this]();
    }
};
struct Graphic : UIBehavior{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.UI", "Graphic").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.UI", "Graphic");
        return mclass;
    }
    Color GetColor(){
        static Method<Color> get_color = GetClass().GetMethod("get_color");
        return get_color[this]();
    }
    void SetColor(Color color){
        static Method<void> set_color = GetClass().GetMethod("set_color");
        set_color[this](color);
    }
    Canvas* GetCanvas(){
        static Method<Canvas*> get_canvas = GetClass().GetMethod("get_canvas");
        return get_canvas[this]();
    }
    Material* GetMaterial(){
        static Method<Material*> get_material = GetClass().GetMethod("get_material");
        return get_material[this]();
    }
    void SetMaterial(Material* material){
        static Method<void> set_material = GetClass().GetMethod("set_material");
        set_material[this](material);
    }
    RectTransform* GetRectTransform() {
        static Method<RectTransform*> get_rectTransform = GetClass().GetMethod("get_rectTransform");
        return get_rectTransform[this]();
    }
};
struct MaskableGraphic : Graphic{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.UI", "MaskableGraphic").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.UI", "MaskableGraphic");
        return mclass;
    }
};
struct Font : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Font").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Font");
        return mclass;
    }
};
struct Text : MaskableGraphic{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine.UI", "Text").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine.UI", "Text");
        return mclass;
    }
    TextAnchor GetAlignment(){
        static Method<TextAnchor> get_alignment = GetClass().GetMethod("get_alignment");
        return get_alignment[this]();
    }
    void SetAlignment(TextAnchor alignment){
        static Method<TextAnchor> set_alignment = GetClass().GetMethod("set_alignment");
        set_alignment[this](alignment);
    }
    Font* GetFont(){
        static Method<Font*> get_font = GetClass().GetMethod("get_font");
        return get_font[this]();
    }
    void SetFont(Font* font){
        static Method<void> set_font = GetClass().GetMethod("set_font");
        return set_font[this](font);
    }
    int GetFontSize(){
        static Method<int> get_fontSize = GetClass().GetMethod("get_fontSize");
        return get_fontSize[this]();
    }
    void SetFontSize(int size){
        static Method<void> set_fontSize = GetClass().GetMethod("set_fontSize");
        set_fontSize[this](size);
    }
    std::string GetText(){
        static Method<String*> get_text = GetClass().GetMethod("get_text");
        auto text = get_text[this]();
        return text->str();
    }
    void SetText(std::string text){
        static Method<void> set_text = GetClass().GetMethod("set_text");
        set_text[this](CreateMonoString(text));
    }
    bool GetTextResizeForBestFit(){
        static Method<bool> get_resizeTextForBestFit = GetClass().GetMethod("get_resizeTextForBestFit");
        return get_resizeTextForBestFit[this]();
    }
    void SetTextResizeForBestFit(bool val){
        static Method<void> set_resizeTextForBestFit = GetClass().GetMethod("set_resizeTextForBestFit");
        set_resizeTextForBestFit[this](val);
    }

    int GetResizeTextMaxSize(){
        static Method<int> get_resizeTextMaxSize = GetClass().GetMethod("get_resizeTextMaxSize");
        return get_resizeTextMaxSize[this]();
    }
    void SetResizeTextMaxSize(int val){
        static Method<void> set_resizeTextMaxSize = GetClass().GetMethod("set_resizeTextMaxSize");
        set_resizeTextMaxSize[this](val);
    }
    int GetResizeTextMinSize(){
        static Method<int> get_resizeTextMinSize = GetClass().GetMethod("get_resizeTextMinSize");
        return get_resizeTextMinSize[this]();
    }
    void SetResizeTextMinSize(int val){
        static Method<void> set_resizeTextMinSize = GetClass().GetMethod("set_resizeTextMinSize");
        set_resizeTextMinSize[this](val);
    }
    bool GetSupportRichText() {
        static Method<bool> get_supportRichText = GetClass().GetMethod("get_supportRichText");
        return get_supportRichText[this]();
    }
    void SetSupportRichText(bool val) {
        static Method<void> set_supportRichText = GetClass().GetMethod("set_supportRichText");
        set_supportRichText[this](val);
    }

    FontStyle GetFontStyle() {
        static Method<FontStyle> get_fontStyle = GetClass().GetMethod("get_fontStyle");
        return get_fontStyle[this]();
    }
    void SetFontStyle(FontStyle val) {
        static Method<void> set_fontStyle = GetClass().GetMethod("set_fontStyle");
        set_fontStyle[this](val);
    }
};
struct LineRenderer : Renderer{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "LineRenderer").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "LineRenderer");
        return mclass;
    }

    void SetUseWorldScape(bool val){
        static auto set_useWorldSpace = (void (*)(void*, bool))GetExternMethod("UnityEngine.LineRenderer::set_useWorldSpace");
        set_useWorldSpace(this, val);
    }
    bool GetUseWorldScape(){
        static auto get_useWorldSpace = (bool (*)(void*))GetExternMethod("UnityEngine.LineRenderer::get_useWorldSpace");
        return get_useWorldSpace(this);
    }
    void SetStartWidth(float val){
        static auto set_startWidth = (void (*)(void*, float))GetExternMethod("UnityEngine.LineRenderer::set_startWidth");
        set_startWidth(this, val);
    }
    float GetStartWidth(){
        static auto get_startWidth = (float (*)(void*))GetExternMethod("UnityEngine.LineRenderer::get_startWidth");
        return get_startWidth(this);
    }
    void SetEndWidth(float val){
        static auto set_endWidth = (void (*)(void*, float))GetExternMethod("UnityEngine.LineRenderer::set_endWidth");
        set_endWidth(this, val);
    }
    void SetStartColor(Color val){
        static Method<void> set_startColor = GetClass().GetMethod("set_startColor");
        set_startColor[this](val);
    }
    Color GetStartColor(){
        static Method<Color> get_startColor = GetClass().GetMethod("get_startColor");
        Color color = get_startColor[this]();
        return color;
    }
    void SetEndColor(Color val){
        static Method<void> set_startColor = GetClass().GetMethod("set_startColor");
        set_startColor[this](val);
    }
    Color GetEndColor(){
        static Method<Color> get_endColor = GetClass().GetMethod("get_endColor");
        Color color = get_endColor[this]();
        return color;
    }
    void SetPosition(int index, Vector3 position){
        static Method<void> SetPosition = GetClass().GetMethod("SetPosition");
        SetPosition[this](index, position);
    }
    void SetColorGradient(Gradient* gradient) {
        static auto setColorGradient = (void(*)(void*, Gradient*))GetExternMethod("UnityEngine.LineRenderer::SetColorGradient");
        setColorGradient(this, gradient);
    }
    void SetPositionCount(int positionCount) {
        static auto set_positionCount = (void(*)(void*, int))GetExternMethod("UnityEngine.LineRenderer::set_positionCount");
        set_positionCount(this, positionCount);
    }
};

struct Rigidbody : Component{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Rigidbody").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Rigidbody");
        return mclass;
    }
    void SetDetectCollisions(bool detectCollisions){
        static auto set_detectCollisions = (void(*)(void*, bool))GetExternMethod("UnityEngine.Rigidbody::set_detectCollisions");
        set_detectCollisions(this, detectCollisions);
    }
    bool GetDetectCollisions(bool detectCollisions){
        static auto get_detectCollisions = (bool(*)(void*))GetExternMethod("UnityEngine.Rigidbody::get_detectCollisions");
        return get_detectCollisions(this);
    }
    void SetUseGravity(bool useGravity){
        static auto set_useGravity = (void(*)(void*, bool))GetExternMethod("UnityEngine.Rigidbody::set_useGravity");
        set_useGravity(this, useGravity);
    }
    bool GetUseGravity(){
        static auto get_useGravity = (bool(*)(void*))GetExternMethod("UnityEngine.Rigidbody::get_useGravity");
        return get_useGravity(this);
    }
    void SetVelocity(Vector3 velocity){
        static Method<void> set_velocity = GetClass().GetMethod("set_velocity");
        set_velocity[this](velocity);
    }
    Vector3 GetVelocity(){
        static Method<Vector3> get_velocity = GetClass().GetMethod("get_velocity");
        return get_velocity[this]();
    }
    void AddForce(Vector3 force, ForceMode mode){
        static Method<void> AddForce = GetClass().GetMethod("AddForce", {"force", "mode"});
        AddForce[this](force, mode);
    }
    void SetIsKinematic(bool kinematic) {
        static auto set_isKinematic = (void(*)(void*, bool))GetExternMethod("UnityEngine.Rigidbody::set_isKinematic");
        set_isKinematic(this, kinematic);
    }
    bool GetIsKinematic() {
        static auto get_isKinematic = (bool(*)(void*))GetExternMethod("UnityEngine.Rigidbody::get_isKinematic");
        return get_isKinematic(this);
    }
};
struct Time{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Time").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Time");
        return mclass;
    }
    static float GetDeltaTime(){
        static auto get_deltaTime = (float(*)())GetExternMethod("UnityEngine.Time::get_deltaTime");
        return get_deltaTime();
    }
    static float GetTimeScale(){
        static auto get_timeScale = (float(*)())GetExternMethod("UnityEngine.Time::get_timeScale");
        return get_timeScale();
    }
    static void SetTimeScale(float scale){
        static auto set_timeScale = (void(*)(float))GetExternMethod("UnityEngine.Time::set_timeScale");
        set_timeScale(scale);
    }
    static int GetFrameCount(){
        static auto get_frameCount = (int(*)())GetExternMethod("UnityEngine.Time::get_frameCount");
        return get_frameCount();
    }
    static float GetTime() {
        static auto get_time = (float(*)())GetExternMethod("UnityEngine.Time::get_time");
        return get_time();
    }
    static float GetUnscaledDeltaTime() {
        static auto get_time = (float(*)())GetExternMethod("UnityEngine.Time::get_unscaledDeltaTime");
        return get_time();
    }
};
struct Collider : Component{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Collider").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Collider");
        return mclass;
    }

    bool GetIsTrigger(){
        static auto get_isTrigger = (bool(*)(void*))GetExternMethod("UnityEngine.Collider::get_isTrigger");
        return get_isTrigger(this);
    }
    void SetIsTrigger(bool val){
        static auto set_isTrigger = (void(*)(void*, bool))GetExternMethod("UnityEngine.Collider::set_isTrigger");
        set_isTrigger(this, val);
    }
    Rigidbody* GetAttachedRigidbody(){
        static auto get_attachedRigidbody = (Rigidbody*(*)(void*))GetExternMethod("UnityEngine.Collider::get_attachedRigidbody");
        return get_attachedRigidbody(this);
    }
    void SetEnabled(bool val){
        static auto set_enabled = (void(*)(void*, bool))GetExternMethod("UnityEngine.Collider::set_enabled");
        set_enabled(this, val);
    }
    bool GetEnabled(){
        static auto get_enabled = (bool(*)(void*))GetExternMethod("UnityEngine.Collider::get_enabled");
        return get_enabled(this);
    }
};
struct SphereCollider : Collider{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "SphereCollider").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "SphereCollider");
        return mclass;
    }
    Vector3 GetCenter(){
        static Method<Vector3> get_center = GetClass().GetMethod("get_center");
        return get_center[this]();
    }
    float GetRadius(){
        static auto get_radius = (float(*)(void*)) GetExternMethod("UnityEngine.SphereCollider::get_radius");
        return get_radius(this);
    }
    void SetRadius(float val){
        static auto set_radius = (void(*)(void*, float)) GetExternMethod("UnityEngine.SphereCollider::set_radius");
        set_radius(this, val);
    }
};
struct BoxCollider : Collider{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "BoxCollider").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "BoxCollider");
        return mclass;
    }
    Vector3 GetCenter(){
        static Method<Vector3> get_center = GetClass().GetMethod("get_center");
        return get_center[this]();
    }
    Vector3 GetSize(){
        static Method<Vector3> get_size = GetClass().GetMethod("get_size");
        return get_size[this]();
    }
    void SetSize(Vector3 val){
        static Method<void> set_size = GetClass().GetMethod("set_size");
        return set_size[this](val);
    }
};
struct MeshCollider : Collider{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "MeshCollider").GetMonoType();
        return type;
    }
    static Class GetClass() {
        static Class mclass = Class("UnityEngine", "MeshCollider");
        return mclass;
    }
};
struct MeshRenderer : Renderer{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "MeshRenderer").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "MeshRenderer");
        return mclass;
    }
};
struct Resources{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Resources").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Resources");
        return mclass;
    }
    static Object* Load(std::string path){
        static Method<Object*> Load = GetClass().GetMethod("Load", 1);
        return Load(path);
    }
    static Object* Load(std::string path, MonoType* systemTypeInstance){
        static Method<Object*> Load = GetClass().GetMethod("Load", 2);
        return Load(CreateMonoString(path), systemTypeInstance);
    }
    static Array<Object*>* LoadAll(std::string path, MonoType* systemTypeInstance){
        static Method<Array<Object*>*> LoadAll = GetClass().GetMethod("LoadAll", 2);
        return LoadAll(CreateMonoString(path), systemTypeInstance);
    }
    static Array<Object*>* FindObjectsOfTypeAll(MonoType* type) {
        static Method<Array<Object*>*> FindObjectsOfTypeAll = GetClass().GetMethod("FindObjectsOfTypeAll", 1);
        return FindObjectsOfTypeAll(type);
    }
    static Object* GetBuiltinResource(MonoType* type, std::string path){
        static auto GetBuiltinResource = (Object *(*)(MonoType*, String*))GetExternMethod("UnityEngine.Resources::GetBuiltinResource");
        return GetBuiltinResource(type, CreateMonoString(path));
    }
};
struct AssetBundle : NamedObject{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "AssetBundle").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "AssetBundle");
        return mclass;
    }
    static AssetBundle* LoadFromFile(std::string path){
        static Method<AssetBundle*> LoadFromFile = GetClass().GetMethod("LoadFromFile", 1);
        return LoadFromFile(CreateMonoString(path));
    }

    static AssetBundle* LoadFromMemory(Array<uint8_t>* binary) {
        static Method<AssetBundle*> LoadFromMemory = GetClass().GetMethod("LoadFromMemory", 1);
        return LoadFromMemory(binary);
    }

    void Unload(bool unloadAllLoadedObjects){
        static auto Unload = (void (*)(AssetBundle *, bool))GetExternMethod("UnityEngine.AssetBundle::Unload");
        Unload(this, unloadAllLoadedObjects);
    }

    Object* LoadAsset(std::string name){
        static Method<Object*> LoadAsset = GetClass().GetMethod("LoadAsset", 1);
        return LoadAsset[this](CreateMonoString(name));
    }

    Object* LoadAsset(std::string name, MonoType* type){
        static Method<Object*> LoadAsset = GetClass().GetMethod("LoadAsset", 2);
        return LoadAsset[this](CreateMonoString(name), type);
    }

    Array<Object*>* LoadAllAssets() {
        static Method<Array<Object*>*> LoadAllAssets = GetClass().GetMethod("LoadAllAssets");
        return LoadAllAssets[this]();
    }

    Array<Object*>* LoadAllAssets(MonoType* type) {
        static Method<Array<Object*>*> LoadAllAssets = GetClass().GetMethod("LoadAllAssets", 1);
        return LoadAllAssets[this](type);
    }

    Array<String*>* GetAllAssetNames() {
        static Method<Array<String*>*> GetAllAssetNames = GetClass().GetMethod("GetAllAssetNames");
        return GetAllAssetNames[this]();
    }

    std::string GetName() {
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto name = get_name[this]();
        return name->str();
    }

    void SetName(std::string name) {
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(name));
    }

    bool GetIsStreamedSceneAssetBundle() {
        static Method<bool> get_isStreamedSceneAssetBundle = GetClass().GetMethod("get_isStreamedSceneAssetBundle");
        return get_isStreamedSceneAssetBundle[this]();
    }

    static void UnloadAllAssetBundles(bool unloadAllObjects) {
        static Method<void> UnloadAllAssetBundles = GetClass().GetMethod("UnloadAllAssetBundles", 1);
        UnloadAllAssetBundles(unloadAllObjects);
    }
};
struct Physics{
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Physics").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Physics");
        return mclass;
    }
    static bool Raycast(Vector3 origin, Vector3 direction, RaycastHit& hitInfo){
        static Method<bool> Raycast = GetClass().GetMethod("Raycast", {"origin","direction","hitInfo"});
        return Raycast(origin, direction, &hitInfo);
    }
    static bool Raycast(Vector3 origin, Vector3 direction, RaycastHit& hitInfo, float maxDistance){
        static Method<bool> Raycast = GetClass().GetMethod("Raycast", {"origin","direction","hitInfo", "maxDistance"});
        return Raycast(origin, direction, &hitInfo, maxDistance);
    }
    static bool Raycast(Vector3 origin, Vector3 direction, RaycastHit& hitInfo, float maxDistance, int layerMask){
        static Method<bool> Raycast = GetClass().GetMethod("Raycast", {"origin","direction","hitInfo", "maxDistance", "layerMask"});
        return Raycast(origin, direction, &hitInfo, maxDistance, layerMask);
    }

    static void SetGravity(Vector3 gravity){
        static Method<void> set_gravity = GetClass().GetMethod("set_gravity");
        set_gravity(gravity);
    }
    static Vector3 GetGravity(){
        static Method<Vector3> get_gravity = GetClass().GetMethod("get_gravity");
        return get_gravity();
    }
};

struct Texture2D : NamedObject {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Texture2D").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Texture2D");
        return mclass;
    }

    static Texture2D* Create(int width, int height) {
        static Method<Texture2D*> Create = GetClass().GetMethod("Create", 2);
        return Create(width, height);
    }

    static Texture2D* Create(int width, int height, TextureFormat format) {
        static Method<Texture2D*> Create = GetClass().GetMethod("Create", 3);
        return Create(width, height, format);
    }
};

struct LightmapData : Object {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "LightmapData").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "LightmapData");
        return mclass;
    }

    Texture2D* GetLightmapColor() {
        static Method<Texture2D*> get_lightmapColor = GetClass().GetMethod("get_lightmapColor");
        return get_lightmapColor[this]();
    }

    void SetLightmapColor(Texture2D* texture) {
        static Method<void> set_lightmapColor = GetClass().GetMethod("set_lightmapColor");
        set_lightmapColor[this](texture);
    }

    Texture2D* GetLightmapDir() {
        static Method<Texture2D*> get_lightmapDir = GetClass().GetMethod("get_lightmapDir");
        return get_lightmapDir[this]();
    }

    void SetLightmapDir(Texture2D* texture) {
        static Method<void> set_lightmapDir = GetClass().GetMethod("set_lightmapDir");
        set_lightmapDir[this](texture);
    }

    Texture2D* GetShadowMask() {
        static Method<Texture2D*> get_shadowMask = GetClass().GetMethod("get_shadowMask");
        return get_shadowMask[this]();
    }

    void SetShadowMask(Texture2D* texture) {
        static Method<void> set_shadowMask = GetClass().GetMethod("set_shadowMask");
        set_shadowMask[this](texture);
    }
};

struct LightmapSettings {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "LightmapSettings").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "LightmapSettings");
        return mclass;
    }

    static Array<LightmapData*>* GetLightmaps() {
        static Method<Array<LightmapData*>*> get_lightmaps = GetClass().GetMethod("get_lightmaps");
        return get_lightmaps();
    }

    static void SetLightmaps(Array<LightmapData*>* lightmaps) {
        static Method<void> set_lightmaps = GetClass().GetMethod("set_lightmaps");
        set_lightmaps(lightmaps);
    }
};

// Structs
struct LayerMask{
    int m_Mask;

    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "LayerMask").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "LayerMask");
        return mclass;
    }

    int GetValue() {
        return m_Mask;
    };
    static String* LayerToName(int layer){
        static auto layerToName = (String*(*)(int))GetExternMethod("UnityEngine.LayerMask::LayerToName");
        return layerToName(layer);
    };
    static int NameToLayer(BNM::Structures::Mono::String * name) {
        static auto nameToLayer = (int(*)(String*))GetExternMethod("UnityEngine.LayerMask::NameToLayer");
        return nameToLayer(name);
    }
};

struct Skybox : Behaviour {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Skybox").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Skybox");
        return mclass;
    }

    Material* GetMaterial() {
        static Method<Material*> get_material = GetClass().GetMethod("get_material");
        return get_material[this]();
    }

    void SetMaterial(Material* material) {
        static Method<void> set_material = GetClass().GetMethod("set_material");
        set_material[this](material);
    }
};

struct Sprite : NamedObject {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Sprite").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Sprite");
        return mclass;
    }

    static Sprite* Create(Texture2D* texture) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 1);
        return Create(texture);
    }

    static Sprite* Create(Texture2D* texture, Rect rect, Vector2 pivot) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 3);
        return Create(texture, rect, pivot);
    }

    static Sprite* Create(Texture2D* texture, Rect rect, Vector2 pivot, float pixelsPerUnit) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 4);
        return Create(texture, rect, pivot, pixelsPerUnit);
    }

    static Sprite* Create(Texture2D* texture, Rect rect, Vector2 pivot, float pixelsPerUnit, uint extrude) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 5);
        return Create(texture, rect, pivot, pixelsPerUnit, extrude);
    }

    static Sprite* Create(Texture2D* texture, Rect rect, Vector2 pivot, float pixelsPerUnit, uint extrude, SpriteMeshType meshType) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 6);
        return Create(texture, rect, pivot, pixelsPerUnit, extrude, meshType);
    }

    static Sprite* Create(Texture2D* texture, Rect rect, Vector2 pivot, float pixelsPerUnit, uint extrude, SpriteMeshType meshType, Vector4 border) {
        static Method<Sprite*> Create = GetClass().GetMethod("Create", 7);
        return Create(texture, rect, pivot, pixelsPerUnit, extrude, meshType, border);
    }

    Texture2D* GetTexture() {
        static Method<Texture2D*> get_texture = GetClass().GetMethod("get_texture");
        return get_texture[this]();
    }

    Rect GetRect() {
        static Method<Rect> get_rect = GetClass().GetMethod("get_rect");
        return get_rect[this]();
    }

    Vector2 GetPivot() {
        static Method<Vector2> get_pivot = GetClass().GetMethod("get_pivot");
        return get_pivot[this]();
    }

    float GetPixelsPerUnit() {
        static Method<float> get_pixelsPerUnit = GetClass().GetMethod("get_pixelsPerUnit");
        return get_pixelsPerUnit[this]();
    }

    Vector4 GetBorder() {
        static Method<Vector4> get_border = GetClass().GetMethod("get_border");
        return get_border[this]();
    }

    Vector2 GetBounds() {
        static Method<Vector2> get_bounds = GetClass().GetMethod("get_bounds");
        return get_bounds[this]();
    }

    std::string GetName() {
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto name = get_name[this]();
        return name->str();
    }

    void SetName(std::string name) {
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(name));
    }
};

struct Animator : Behaviour {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Animator").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Animator");
        return mclass;
    }

    void SetEnabled(bool enabled){
        static auto set_enabled = (void(*)(void*, bool))GetExternMethod("UnityEngine.Behaviour::set_enabled");
        set_enabled(this, enabled);
    }
    bool GetEnabled(){
        static auto get_enabled = (bool(*)(void*))GetExternMethod("UnityEngine.Behaviour::get_enabled");
        return get_enabled(this);
    }
    void Play(std::string stateName) {
        static Method<void> Play = GetClass().GetMethod("Play", 1);
        Play[this](CreateMonoString(stateName));
    }

    void Stop() {
        static Method<void> Stop = GetClass().GetMethod("Stop");
        Stop[this]();
    }
};

struct Matrix4x4 {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;

    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Matrix4x4").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Matrix4x4");
        return mclass;
    }

    static Matrix4x4 identity() {
        static auto identity = (Matrix4x4(*)())GetExternMethod("UnityEngine.Matrix4x4::get_identity");
        return identity();
    }

    static Matrix4x4 zero() {
        static auto zero = (Matrix4x4(*)())GetExternMethod("UnityEngine.Matrix4x4::get_zero");
        return zero();
    }
};

struct LODGroup : Component {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "LODGroup").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "LODGroup");
        return mclass;
    }
};

struct MonoBehaviour : Behaviour {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "MonoBehaviour").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "MonoBehaviour");
        return mclass;
    }

    void* StartCoroutine(String* methodName) {
        static Method<void*> StartCoroutine = GetClass().GetMethod("StartCoroutine", 1);
        return StartCoroutine[this](methodName);
    }

    void* StartCoroutine(String* methodName, Object* value) {
        static Method<void*> StartCoroutine = GetClass().GetMethod("StartCoroutine", 2);
        return StartCoroutine[this](methodName, value);
    }

    void StopCoroutine(String* methodName) {
        static Method<void> StopCoroutine = GetClass().GetMethod("StopCoroutine", 1);
        StopCoroutine[this](methodName);
    }

    void StopCoroutine(void* routine) {
        static Method<void> StopCoroutine = GetClass().GetMethod("StopCoroutine", 1);
        StopCoroutine[this](routine);
    }

    void StopCoroutine(MonoType* routineType) {
        static Method<void> StopCoroutine = GetClass().GetMethod("StopCoroutine", 1);
        StopCoroutine[this](routineType);
    }

    void StopAllCoroutines() {
        static Method<void> StopAllCoroutines = GetClass().GetMethod("StopAllCoroutines");
        StopAllCoroutines[this]();
    }

    void Invoke(String* methodName, float time) {
        static Method<void> Invoke = GetClass().GetMethod("Invoke", 2);
        Invoke[this](methodName, time);
    }

    void CancelInvoke() {
        static Method<void> CancelInvoke = GetClass().GetMethod("CancelInvoke");
        CancelInvoke[this]();
    }

    void CancelInvoke(String* methodName) {
        static Method<void> CancelInvoke = GetClass().GetMethod("CancelInvoke", 1);
        CancelInvoke[this](methodName);
    }

    bool IsInvoking(String* methodName) {
        static Method<bool> IsInvoking = GetClass().GetMethod("IsInvoking", 1);
        return IsInvoking[this](methodName);
    }

    bool IsInvoking() {
        static Method<bool> IsInvoking = GetClass().GetMethod("IsInvoking");
        return IsInvoking[this]();
    }
};

struct QualitySettings {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "QualitySettings").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "QualitySettings");
        return mclass;
    }
};

struct AudioSource : Behaviour {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "AudioSource").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "AudioSource");
        return mclass;
    }

    void Play() {
        static Method<void> Play = GetClass().GetMethod("Play");
        Play[this]();
    }

    void Play(float delay) {
        static Method<void> Play = GetClass().GetMethod("Play", 1);
        Play[this](delay);
    }

    void Stop() {
        static Method<void> Stop = GetClass().GetMethod("Stop");
        Stop[this]();
    }

    void Pause() {
        static Method<void> Pause = GetClass().GetMethod("Pause");
        Pause[this]();
    }

    void UnPause() {
        static Method<void> UnPause = GetClass().GetMethod("UnPause");
        UnPause[this]();
    }

    float GetVolume() {
        static Method<float> get_volume = GetClass().GetMethod("get_volume");
        return get_volume[this]();
    }

    void SetVolume(float volume) {
        static Method<void> set_volume = GetClass().GetMethod("set_volume");
        set_volume[this](volume);
    }

    AudioClip* GetClip() {
        static Method<AudioClip*> get_clip = GetClass().GetMethod("get_clip");
        return get_clip[this]();
    }

    void SetClip(AudioClip* clip) {
        static Method<void> set_clip = GetClass().GetMethod("set_clip");
        set_clip[this](clip);
    }

    bool GetIsPlaying() {
        static Method<bool> get_isPlaying = GetClass().GetMethod("get_isPlaying");
        return get_isPlaying[this]();
    }

    bool GetLoop() {
        static Method<bool> get_loop = GetClass().GetMethod("get_loop");
        return get_loop[this]();
    }

    void SetLoop(bool loop) {
        static Method<void> set_loop = GetClass().GetMethod("set_loop");
        set_loop[this](loop);
    }
};

struct AudioClip : NamedObject {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "AudioClip").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "AudioClip");
        return mclass;
    }

    std::string GetName() {
        static Method<String*> get_name = GetClass().GetMethod("get_name");
        auto name = get_name[this]();
        return name->str();
    }

    void SetName(std::string name) {
        static Method<void> set_name = GetClass().GetMethod("set_name");
        set_name[this](CreateMonoString(name));
    }

    void SetData(Array<float>* data, int offsetSamples) {
        static Method<void> SetData = GetClass().GetMethod("SetData", 2);
        SetData[this](data, offsetSamples);
    }

    void SetData(Array<float>* data, int offsetSamples, int channel) {
        static Method<void> SetData = GetClass().GetMethod("SetData", 3);
        SetData[this](data, offsetSamples, channel);
    }

    bool GetData(Array<float>* data, int offsetSamples) {
        static Method<bool> GetData = GetClass().GetMethod("GetData", 2);
        return GetData[this](data, offsetSamples);
    }

    bool GetData(Array<float>* data, int offsetSamples, int channel) {
        static Method<bool> GetData = GetClass().GetMethod("GetData", 3);
        return GetData[this](data, offsetSamples, channel);
    }

    int GetSamples() {
        static Method<int> get_samples = GetClass().GetMethod("get_samples");
        return get_samples[this]();
    }

    int GetChannels() {
        static Method<int> get_channels = GetClass().GetMethod("get_channels");
        return get_channels[this]();
    }

    int GetFrequency() {
        static Method<int> get_frequency = GetClass().GetMethod("get_frequency");
        return get_frequency[this]();
    }

    float GetLength() {
        static Method<float> get_length = GetClass().GetMethod("get_length");
        return get_length[this]();
    }
};

struct Light : Behaviour {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "Light").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "Light");
        return mclass;
    }
};

struct ParticleSystem : Component {
    static MonoType* GetType(){
        static MonoType* type = Class("UnityEngine", "ParticleSystem").GetMonoType();
        return type;
    }
    static Class GetClass(){
        static Class mclass = Class("UnityEngine", "ParticleSystem");
        return mclass;
    }
};