# Runtime UI & Touch Events (uGUI & Android Input)

BNM provides comprehensive support for constructing graphical user interfaces (GUI) at runtime on Android using **uGUI** (`UnityEngine.UI`) and **TextMeshPro**, including automated touch input routing and persistent overlay canvases.

---

## 1. 1-Line Overlay Canvas Creation (`CanvasHelper`)

To display a mod menu, developer overlay, or UI inspector on an Android screen:

```cpp
#include <BNM/UnityStructures/UI.hpp>
using namespace BNM::UnityEngine::UI;

// Creates a persistent ScreenSpaceOverlay Canvas in DontDestroyOnLoad
// Automatically attaches Canvas, CanvasScaler, GraphicRaycaster, and ensures EventSystem is active
Canvas *myCanvas = CanvasHelper::CreateOverlayCanvas("ModMenuCanvas", 2000 /* sortingOrder */);
```

---

## 2. Automated Android Touch Input Routing (`EventSystem`)

If the target game scene lacks an `EventSystem` or `StandaloneInputModule`, on-screen UI buttons will not register finger touches. BNM handles this automatically:

```cpp
// Finds an existing active EventSystem or spawns a persistent one with StandaloneInputModule in DontDestroyOnLoad
EventSystem *es = EventSystem::EnsureEventSystem();
```

---

## 3. Creating Buttons & Text

```cpp
using namespace BNM::UnityEngine;
using namespace BNM::UnityEngine::UI;

// 1. Create Button GameObject
auto btnGo = GameObject::Create("ModButton");
btnGo->GetTransform()->SetParent(myCanvas->GetTransform(), false);

// 2. Configure RectTransform, Image (background), and Button component
auto rect = btnGo->GetComponent<RectTransform *>();
rect->SetSizeDelta({200.f, 60.f});
rect->SetAnchoredPosition({0.f, 100.f});

auto btnImage = btnGo->AddComponent<Image *>();
btnImage->SetColor({0.2f, 0.6f, 1.0f, 1.0f}); // Blue background

auto button = btnGo->AddComponent<Button *>();

// 3. Add OnClick event listener via BNM::CreateUnityAction
auto clickEvent = button->GetOnClick();
auto action = BNM::CreateUnityAction([]() {
    BNM_LOG_INFO("Mod Button tapped on Android screen!");
});
clickEvent->AddListener(action);
```

---

## 4. InputField, Slider & Toggle

### A. Slider
```cpp
auto slider = sliderGo->GetComponent<Slider *>();
slider->SetMinValue(0.0f);
slider->SetMaxValue(100.0f);
slider->SetValue(50.0f);

// Value change event listener
slider->GetOnValueChanged()->AddListener(BNM::CreateUnityAction<float>([](float val) {
    BNM_LOG_INFO("Slider value changed: %.2f", val);
}));
```

### B. Toggle (Checkbox)
```cpp
auto toggle = toggleGo->GetComponent<Toggle *>();
toggle->SetIsOn(true);
toggle->GetOnValueChanged()->AddListener(BNM::CreateUnityAction<bool>([](bool active) {
    BNM_LOG_INFO("Toggle status: %s", active ? "ON" : "OFF");
}));
```

### C. InputField (Text Input)
```cpp
auto input = inputGo->GetComponent<InputField *>();
input->SetText("Enter command...");
input->GetOnEndEdit()->AddListener(BNM::CreateUnityAction<BNM::Structures::Mono::String *>([](auto str) {
    BNM_LOG_INFO("User input committed: %s", str->str().c_str());
}));
```

---

## 5. TextMeshPro (TMP)

```cpp
#include <BNM/UnityStructures/TextMeshPro.hpp>
using namespace BNM::UnityEngine::TMPro;

auto tmpText = textGo->GetComponent<TextMeshProUGUI *>();
if (tmpText && tmpText->IsValid()) {
    tmpText->SetText("<b><color=#00FF00>UniverseLib Android Active</color></b>");
    tmpText->SetFontSize(24.0f);
}
```
