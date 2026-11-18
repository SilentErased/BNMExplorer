#include <BNM/Defaults.hpp>
#include <Internals.hpp>

namespace BNM::Defaults::Internal {
    ClassType Void{}, Boolean{}, Byte{}, SByte{}, Int16{}, UInt16{}, Int32{}, UInt32{}, IntPtr{}, UIntPtr{}, Int64{}, UInt64{}, Single{}, Double{}, Decimal{}, String{}, Object{};
    ClassType Vector2{}, Vector3{}, Vector4{}, Color{}, Color32{}, Ray{}, Quaternion{}, Matrix3x3{}, Matrix4x4{}, RaycastHit{}, RaycastHit2D{}, Bounds{};
    ClassType UnityObject{}, Component{}, Transform{}, GameObject{}, Behaviour{}, MonoBehaviour{}, ScriptableObject{};
    ClassType Application{}, Camera{}, Light{}, Shader{}, Material{}, Texture{}, Texture2D{}, RenderTexture{};
    ClassType Time{}, Screen{}, Touch{}, Input{}, SystemInfo{}, PlayerPrefs{};
    ClassType YieldInstruction{}, AsyncOperation{}, Resources{}, Scene{}, SceneManager{};
#ifdef BNM_UNITY_RENDERERS
    ClassType Renderer{}, Mesh{}, MeshRenderer{}, SkinnedMeshRenderer{}, SpriteRenderer{};
#endif
#ifdef BNM_UNITY_AUDIO
    ClassType AudioClip{}, AudioSource{}, AudioListener{};
#endif
#ifdef BNM_UNITY_ANIMATION
    ClassType AnimationClip{}, Animation{}, Animator{};
#endif
#ifdef BNM_UNITY_PHYSICS
    ClassType Collider{}, BoxCollider{}, SphereCollider{}, CapsuleCollider{}, MeshCollider{}, Rigidbody{}, Physics{};
#endif
#ifdef BNM_UNITY_PHYSICS2D
    ClassType Collider2D{}, BoxCollider2D{}, CircleCollider2D{}, Rigidbody2D{}, Physics2D{};
#endif
#ifdef BNM_UNITY_UI
    ClassType RectTransform{}, Canvas{}, CanvasScaler{}, UIBehaviour{}, Graphic{}, GraphicRaycaster{}, MaskableGraphic{}, Text{}, UIImage{}, Selectable{}, Button{}, Slider{}, Toggle{}, InputField{}, EventSystem{};
#endif
#ifdef BNM_UNITY_TEXTMESHPRO
    ClassType TMP_Text{}, TextMeshPro{}, TextMeshProUGUI{}, TMP_InputField{};
#endif
}

void BNM::Internal::LoadDefaults() {
    using namespace BNM::Defaults::Internal;

    // mscorlib
    if (!il2cppMethods.il2cpp_get_corlib) return;
    auto image = il2cppMethods.il2cpp_get_corlib();
    auto SystemStr = BNM_OBFUSCATE_TMP("System");
    auto ObjectStr = BNM_OBFUSCATE_TMP("Object");
    Void = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Void"));
    Boolean = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Boolean"));
    Byte = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Byte"));
    SByte = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("SByte"));
    Int16 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int16"));
    UInt16 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt16"));
    Int32 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int32"));
    UInt32 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt32"));
    IntPtr = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("IntPtr"));
    UIntPtr = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UIntPtr"));
    Int64 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Int64"));
    UInt64 = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("UInt64"));
    Single = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Single"));
    Double = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Double"));
    Decimal = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("Decimal"));
    String = TryGetClassInImage(image, SystemStr, BNM_OBFUSCATE_TMP("String"));
    Object = TryGetClassInImage(image, SystemStr, ObjectStr);

    // Unity Core
    auto UnityEngineStr = BNM_OBFUSCATE_TMP("UnityEngine");
    image = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.CoreModule.dll"));
    if (!image) image = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.dll"));

    Vector2 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector2"));
    Vector3 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector3"));
    Vector4 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Vector4"));
    Color = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Color"));
    Color32 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Color32"));
    Ray = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Ray"));
    Quaternion = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Quaternion"));
    Matrix3x3 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Matrix3x3"));
    Matrix4x4 = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Matrix4x4"));
    Bounds = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Bounds"));

    // UnityEngine Core Classes
    UnityObject = TryGetClassInImage(image, UnityEngineStr, ObjectStr);
    Component = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Component"));
    Transform = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Transform"));
    GameObject = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("GameObject"));
    Behaviour = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Behaviour"));
    MonoBehaviour = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("MonoBehaviour"));
    ScriptableObject = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("ScriptableObject"));

    // Rendering & Scene Classes
    Application = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Application"));
    Camera = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Camera"));
    Light = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Light"));
    Shader = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Shader"));
    Material = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Material"));
    Texture = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Texture"));
    Texture2D = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Texture2D"));
    RenderTexture = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("RenderTexture"));

    // Core Utilities & Input
    Time = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Time"));
    Screen = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Screen"));
    SystemInfo = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("SystemInfo"));
    PlayerPrefs = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("PlayerPrefs"));

    auto inputImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.InputLegacyModule.dll"));
    if (!inputImage) inputImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.InputModule.dll"));
    if (!inputImage) inputImage = image;
    Input = TryGetClassInImage(inputImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Input"));
    Touch = TryGetClassInImage(inputImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Touch"));

    // Scene Management & Assets
    YieldInstruction = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("YieldInstruction"));
    AsyncOperation = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("AsyncOperation"));
    Resources = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Resources"));

    auto sceneImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.SceneManagementModule.dll"));
    if (!sceneImage) sceneImage = image;
    auto SceneManagementStr = BNM_OBFUSCATE_TMP("UnityEngine.SceneManagement");
    SceneManager = TryGetClassInImage(sceneImage, SceneManagementStr, BNM_OBFUSCATE_TMP("SceneManager"));
    Scene = TryGetClassInImage(sceneImage, SceneManagementStr, BNM_OBFUSCATE_TMP("Scene"));

#ifdef BNM_UNITY_RENDERERS
    // Visual Renderers & Mesh
    auto rendererImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.CoreModule.dll"));
    if (!rendererImage) rendererImage = image;
    Renderer = TryGetClassInImage(rendererImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Renderer"));
    Mesh = TryGetClassInImage(rendererImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Mesh"));
    MeshRenderer = TryGetClassInImage(rendererImage, UnityEngineStr, BNM_OBFUSCATE_TMP("MeshRenderer"));
    SkinnedMeshRenderer = TryGetClassInImage(rendererImage, UnityEngineStr, BNM_OBFUSCATE_TMP("SkinnedMeshRenderer"));
    SpriteRenderer = TryGetClassInImage(rendererImage, UnityEngineStr, BNM_OBFUSCATE_TMP("SpriteRenderer"));
#endif

#ifdef BNM_UNITY_AUDIO
    // Audio Module
    auto audioImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.AudioModule.dll"));
    if (!audioImage) audioImage = image;
    AudioClip = TryGetClassInImage(audioImage, UnityEngineStr, BNM_OBFUSCATE_TMP("AudioClip"));
    AudioSource = TryGetClassInImage(audioImage, UnityEngineStr, BNM_OBFUSCATE_TMP("AudioSource"));
    AudioListener = TryGetClassInImage(audioImage, UnityEngineStr, BNM_OBFUSCATE_TMP("AudioListener"));
#endif

#ifdef BNM_UNITY_ANIMATION
    // Animation Module
    auto animImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.AnimationModule.dll"));
    if (!animImage) animImage = image;
    AnimationClip = TryGetClassInImage(animImage, UnityEngineStr, BNM_OBFUSCATE_TMP("AnimationClip"));
    Animation = TryGetClassInImage(animImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Animation"));
    Animator = TryGetClassInImage(animImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Animator"));
#endif

#ifdef BNM_UNITY_PHYSICS
    // Physics 3D
    auto physImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.PhysicsModule.dll"));
    if (!physImage) physImage = image;
    RaycastHit = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("RaycastHit"));
    Collider = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Collider"));
    BoxCollider = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("BoxCollider"));
    SphereCollider = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("SphereCollider"));
    CapsuleCollider = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("CapsuleCollider"));
    MeshCollider = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("MeshCollider"));
    Rigidbody = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Rigidbody"));
    Physics = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Physics"));
#else
    auto physImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.PhysicsModule.dll"));
    if (!physImage) physImage = image;
    RaycastHit = TryGetClassInImage(physImage, UnityEngineStr, BNM_OBFUSCATE_TMP("RaycastHit"));
#endif

#ifdef BNM_UNITY_PHYSICS2D
    // Physics 2D
    auto phys2DImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.Physics2DModule.dll"));
    if (!phys2DImage) phys2DImage = image;
    RaycastHit2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("RaycastHit2D"));
    Collider2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Collider2D"));
    BoxCollider2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("BoxCollider2D"));
    CircleCollider2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("CircleCollider2D"));
    Rigidbody2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Rigidbody2D"));
    Physics2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Physics2D"));
#else
    auto phys2DImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.Physics2DModule.dll"));
    if (!phys2DImage) phys2DImage = image;
    RaycastHit2D = TryGetClassInImage(phys2DImage, UnityEngineStr, BNM_OBFUSCATE_TMP("RaycastHit2D"));
#endif

#ifdef BNM_UNITY_UI
    // UnityEngine.UI & UIModule
    auto uiModuleImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.UIModule.dll"));
    if (!uiModuleImage) uiModuleImage = image;

    auto uiImage = TryGetImage(BNM_OBFUSCATE_TMP("UnityEngine.UI.dll"));
    if (!uiImage) uiImage = uiModuleImage;

    auto UIStr = BNM_OBFUSCATE_TMP("UnityEngine.UI");
    auto EventSystemsStr = BNM_OBFUSCATE_TMP("UnityEngine.EventSystems");

    RectTransform = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("RectTransform"));
    if (!RectTransform) RectTransform = TryGetClassInImage(uiModuleImage, UnityEngineStr, BNM_OBFUSCATE_TMP("RectTransform"));

    Canvas = TryGetClassInImage(uiModuleImage, UnityEngineStr, BNM_OBFUSCATE_TMP("Canvas"));
    if (!Canvas) Canvas = TryGetClassInImage(image, UnityEngineStr, BNM_OBFUSCATE_TMP("Canvas"));

    CanvasScaler = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("CanvasScaler"));
    UIBehaviour = TryGetClassInImage(uiImage, EventSystemsStr, BNM_OBFUSCATE_TMP("UIBehaviour"));
    Graphic = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Graphic"));
    MaskableGraphic = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("MaskableGraphic"));
    Text = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Text"));
    UIImage = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Image"));
    Selectable = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Selectable"));
    Button = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Button"));
    Slider = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Slider"));
    Toggle = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("Toggle"));
    InputField = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("InputField"));
    GraphicRaycaster = TryGetClassInImage(uiImage, UIStr, BNM_OBFUSCATE_TMP("GraphicRaycaster"));
    EventSystem = TryGetClassInImage(uiImage, EventSystemsStr, BNM_OBFUSCATE_TMP("EventSystem"));
#endif

#ifdef BNM_UNITY_TEXTMESHPRO
    // TextMeshPro dynamic version assembly resolution
    auto tmpImage = TryGetImage(BNM_OBFUSCATE_TMP("Unity.TextMeshPro.dll"));
    if (!tmpImage) tmpImage = TryGetImage(BNM_OBFUSCATE_TMP("TextMeshPro-2017.3-Runtime.dll"));
    if (!tmpImage) tmpImage = TryGetImage(BNM_OBFUSCATE_TMP("TextMeshPro-2017.2-Runtime.dll"));
    if (!tmpImage) tmpImage = TryGetImage(BNM_OBFUSCATE_TMP("TextMeshPro-2017.1-Runtime.dll"));
    if (!tmpImage) tmpImage = TryGetImage(BNM_OBFUSCATE_TMP("TextMeshPro-5.6-Runtime.dll"));

    auto TMProStr = BNM_OBFUSCATE_TMP("TMPro");
    if (tmpImage) {
        TMP_Text = TryGetClassInImage(tmpImage, TMProStr, BNM_OBFUSCATE_TMP("TMP_Text"));
        TextMeshPro = TryGetClassInImage(tmpImage, TMProStr, BNM_OBFUSCATE_TMP("TextMeshPro"));
        TextMeshProUGUI = TryGetClassInImage(tmpImage, TMProStr, BNM_OBFUSCATE_TMP("TextMeshProUGUI"));
        TMP_InputField = TryGetClassInImage(tmpImage, TMProStr, BNM_OBFUSCATE_TMP("TMP_InputField"));
    }
#endif
}

BNM::Defaults::DefaultTypeRef::operator BNM::CompileTimeClass() const { return {_reference}; }
BNM::Defaults::DefaultTypeRef::operator BNM::Class() const { return _reference ? *_reference : nullptr; }
BNM::Class BNM::Defaults::DefaultTypeRef::ToClass() const { return _reference ? *_reference : nullptr; }
