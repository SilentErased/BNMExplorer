#pragma once

#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_UI

#include <string>
#include <string_view>
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "Transform.hpp"
#include "Behaviour.hpp"
#include "MonoBehaviour.hpp"
#include "Material.hpp"
#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Rect.hpp"
#include "Camera.hpp"
#include "Events.hpp"

namespace BNM::UnityEngine::UI {

    struct RectTransform;
    struct Canvas;
    struct CanvasScaler;
    struct UIBehaviour;
    struct Graphic;
    struct GraphicRaycaster;
    struct MaskableGraphic;
    struct Text;
    struct Image;
    struct Selectable;
    struct Button;
    struct Slider;
    struct Toggle;
    struct InputField;
    struct EventSystem;
    struct CanvasHelper;

    /**
        @brief Position, size, anchor and pivot information for UI elements.
    */
    struct RectTransform : public Transform {

        /**
            @brief The position of the pivot of this RectTransform relative to the anchor reference point.
            @return Vector2 anchored position.
        */
        inline Structures::Unity::Vector2 GetAnchoredPosition() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_anchoredPosition"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets anchored position.
            @param value Vector2 position.
        */
        inline void SetAnchoredPosition(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_anchoredPosition"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The 3D position of the pivot of this RectTransform relative to the anchor reference point.
            @return Vector3 anchored position.
        */
        inline Structures::Unity::Vector3 GetAnchoredPosition3D() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_anchoredPosition3D"), 0).cast<Structures::Unity::Vector3>();
            return method[(void *)this]();
        }

        /**
            @brief Sets 3D anchored position.
            @param value Vector3 position.
        */
        inline void SetAnchoredPosition3D(Structures::Unity::Vector3 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_anchoredPosition3D"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The size of this RectTransform relative to the distances between the anchors.
            @return Vector2 sizeDelta (width, height).
        */
        inline Structures::Unity::Vector2 GetSizeDelta() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_sizeDelta"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets size delta.
            @param value Vector2 size (width, height).
        */
        inline void SetSizeDelta(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_sizeDelta"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The normalized position in the parent RectTransform that the lower left corner is anchored to.
            @return Vector2 anchorMin (0 to 1).
        */
        inline Structures::Unity::Vector2 GetAnchorMin() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_anchorMin"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets anchorMin.
            @param value Vector2 anchorMin.
        */
        inline void SetAnchorMin(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_anchorMin"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The normalized position in the parent RectTransform that the upper right corner is anchored to.
            @return Vector2 anchorMax (0 to 1).
        */
        inline Structures::Unity::Vector2 GetAnchorMax() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_anchorMax"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets anchorMax.
            @param value Vector2 anchorMax.
        */
        inline void SetAnchorMax(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_anchorMax"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The normalized position in this RectTransform that it rotates around.
            @return Vector2 pivot (0 to 1, default 0.5, 0.5).
        */
        inline Structures::Unity::Vector2 GetPivot() const {
            if (!IsValid()) return {0.5f, 0.5f};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_pivot"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets pivot.
            @param value Vector2 pivot.
        */
        inline void SetPivot(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_pivot"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The calculated rectangle in the local space of the Transform.
            @return Rect local rectangle.
        */
        inline Structures::Unity::Rect GetRect() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_rect"), 0).cast<Structures::Unity::Rect>();
            return method[(void *)this]();
        }

        /**
            @brief The offset of the lower left corner of the rectangle relative to the lower left anchor.
            @return Vector2 offsetMin.
        */
        inline Structures::Unity::Vector2 GetOffsetMin() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_offsetMin"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets offsetMin.
            @param value Vector2 offsetMin.
        */
        inline void SetOffsetMin(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_offsetMin"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The offset of the upper right corner of the rectangle relative to the upper right anchor.
            @return Vector2 offsetMax.
        */
        inline Structures::Unity::Vector2 GetOffsetMax() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("get_offsetMax"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets offsetMax.
            @param value Vector2 offsetMax.
        */
        inline void SetOffsetMax(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("set_offsetMax"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Makes the RectTransform dimensions match the given size along a given axis (0 = Horizontal, 1 = Vertical).
            @param axis Axis index (0 = Horizontal, 1 = Vertical).
            @param size Size float.
        */
        inline void SetSizeWithCurrentAnchors(int axis, float size) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("SetSizeWithCurrentAnchors"), 2).cast<void>();
            method[(void *)this](axis, size);
        }

        /**
            @brief Reapply the rules of the RectTransform to all children.
        */
        static inline void ForceUpdateRectTransforms() {
            static auto method = BNM::Defaults::Get<RectTransform>().ToClass().GetMethod(BNM_OBFUSCATE("ForceUpdateRectTransforms"), 0).cast<void>();
            if (method.IsValid()) method();
        }
    };

    /**
        @brief Element that can be used for screen rendering in Canvas.
    */
    struct Canvas : public Behaviour {

        /**
            @brief Is this the root Canvas?
            @return True if root canvas.
        */
        inline bool GetIsRootCanvas() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("get_isRootCanvas"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief The render mode of the Canvas (0 = ScreenSpaceOverlay, 1 = ScreenSpaceCamera, 2 = WorldSpace).
            @return RenderMode integer.
        */
        inline int GetRenderMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("get_renderMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets render mode of the Canvas.
            @param value RenderMode integer (0 = ScreenSpaceOverlay, 1 = ScreenSpaceCamera, 2 = WorldSpace).
        */
        inline void SetRenderMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("set_renderMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Camera used for generating events or rendering ScreenSpaceCamera/WorldSpace Canvas.
            @return Camera pointer.
        */
        inline Camera *GetWorldCamera() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("get_worldCamera"), 0).cast<Camera *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets world camera.
            @param camera Camera pointer.
        */
        inline void SetWorldCamera(Camera *camera) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("set_worldCamera"), 1).cast<void>();
            method[(void *)this](camera);
        }

        /**
            @brief Canvas' order within a sorting layer.
            @return Sorting order integer.
        */
        inline int GetSortingOrder() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingOrder"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets canvas sorting order.
            @param value Sorting order integer.
        */
        inline void SetSortingOrder(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingOrder"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Used to scale the entire canvas (scaleFactor).
            @return Scale factor float.
        */
        inline float GetScaleFactor() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("get_scaleFactor"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets canvas scale factor.
            @param value Scale factor float.
        */
        inline void SetScaleFactor(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("set_scaleFactor"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Force all canvases to update their content immediately.
        */
        static inline void ForceUpdateCanvases() {
            static auto method = BNM::Defaults::Get<Canvas>().ToClass().GetMethod(BNM_OBFUSCATE("ForceUpdateCanvases"), 0).cast<void>();
            if (method.IsValid()) method();
        }
    };

    /**
        @brief Base class for all UI components.
    */
    struct UIBehaviour : public MonoBehaviour {

        /**
            @brief Returns true if the GameObject and the Component are active.
            @return True if active.
        */
        inline bool IsActive() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<UIBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("IsActive"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Returns true if the Component is destroyed.
            @return True if destroyed.
        */
        inline bool IsDestroyed() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<UIBehaviour>().ToClass().GetMethod(BNM_OBFUSCATE("IsDestroyed"), 0).cast<bool>();
            return method[(void *)this]();
        }
    };

    /**
        @brief Controls the overall scale and pixel density of UI elements in the Canvas.
    */
    struct CanvasScaler : public UIBehaviour {

        /**
            @brief Determines how UI elements in the Canvas are scaled (0 = ConstantPixelSize, 1 = ScaleWithScreenSize, 2 = ConstantPhysicalSize).
            @return ScaleMode integer.
        */
        inline int GetUiScaleMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("get_uiScaleMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets scale mode of CanvasScaler.
            @param value ScaleMode integer.
        */
        inline void SetUiScaleMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("set_uiScaleMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The resolution the UI layout is designed for.
            @return Vector2 reference resolution.
        */
        inline Structures::Unity::Vector2 GetReferenceResolution() const {
            if (!IsValid()) return {800.f, 600.f};
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("get_referenceResolution"), 0).cast<Structures::Unity::Vector2>();
            return method[(void *)this]();
        }

        /**
            @brief Sets reference resolution.
            @param value Vector2 resolution.
        */
        inline void SetReferenceResolution(Structures::Unity::Vector2 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("set_referenceResolution"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Setting to scale the Canvas with the width or height as reference, or a mix (0 to 1).
            @return Match width or height float.
        */
        inline float GetMatchWidthOrHeight() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("get_matchWidthOrHeight"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets match width or height (0 = width, 1 = height).
            @param value Float (0 to 1).
        */
        inline void SetMatchWidthOrHeight(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<CanvasScaler>().ToClass().GetMethod(BNM_OBFUSCATE("set_matchWidthOrHeight"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief Base class for all visual UI components (Text, Image, etc.).
    */
    struct Graphic : public UIBehaviour {

        /**
            @brief The color of the Graphic.
            @return Color structure.
        */
        inline Structures::Unity::Color GetColor() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_color"), 0).cast<Structures::Unity::Color>();
            return method[(void *)this]();
        }

        /**
            @brief Sets graphic color.
            @param value New color.
        */
        inline void SetColor(Structures::Unity::Color value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("set_color"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Should this Graphic be considered a target for raycasting?
            @return True if raycast target.
        */
        inline bool GetRaycastTarget() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_raycastTarget"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether graphic is a raycast target.
            @param value True to enable raycasting.
        */
        inline void SetRaycastTarget(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("set_raycastTarget"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The Material used by the Graphic.
            @return Material pointer.
        */
        inline Material *GetMaterial() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_material"), 0).cast<Material *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets Material.
            @param material Material pointer.
        */
        inline void SetMaterial(Material *material) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("set_material"), 1).cast<void>();
            method[(void *)this](material);
        }

        /**
            @brief The RectTransform associated with this Graphic.
            @return RectTransform pointer.
        */
        inline RectTransform *GetRectTransform() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_rectTransform"), 0).cast<RectTransform *>();
            return method[(void *)this]();
        }

        /**
            @brief The Canvas this Graphic belongs to.
            @return Canvas pointer.
        */
        inline Canvas *GetCanvas() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_canvas"), 0).cast<Canvas *>();
            return method[(void *)this]();
        }

        /**
            @brief Mark the Graphic as dirty and needing re-rendering.
        */
        inline void SetAllDirty() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("SetAllDirty"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Tweens the alpha of the Graphic to target alpha over duration.
            @param alpha Target alpha (0 to 1).
            @param duration Duration in seconds.
            @param ignoreTimeScale When true, uses unscaled time.
        */
        inline void CrossFadeAlpha(float alpha, float duration, bool ignoreTimeScale = false) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFadeAlpha"), 3).cast<void>();
            method[(void *)this](alpha, duration, ignoreTimeScale);
        }

        /**
            @brief Tweens the color of the Graphic.
            @param targetColor Target color.
            @param duration Duration in seconds.
            @param ignoreTimeScale When true, uses unscaled time.
            @param useAlpha When true, fades alpha as well.
        */
        inline void CrossFadeColor(Structures::Unity::Color targetColor, float duration, bool ignoreTimeScale = false, bool useAlpha = true) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Graphic>().ToClass().GetMethod(BNM_OBFUSCATE("CrossFadeColor"), 4).cast<void>();
            method[(void *)this](targetColor, duration, ignoreTimeScale, useAlpha);
        }
    };

    /**
        @brief A Graphic that is capable of being clipped or masked.
    */
    struct MaskableGraphic : public Graphic {

        /**
            @brief Does this graphic allow masking?
            @return True if maskable.
        */
        inline bool GetMaskable() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<MaskableGraphic>().ToClass().GetMethod(BNM_OBFUSCATE("get_maskable"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether this graphic allows masking.
            @param value True to allow masking.
        */
        inline void SetMaskable(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<MaskableGraphic>().ToClass().GetMethod(BNM_OBFUSCATE("set_maskable"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief The standard Text rendering component for uGUI.
    */
    struct Text : public MaskableGraphic {

        /**
            @brief Gets the text string rendered by the component.
            @return Mono String pointer.
        */
        inline Structures::Mono::String *GetText() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_text"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text string from string_view.
            @param value String text.
        */
        inline void SetText(const std::string_view &value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(value));
        }

        /**
            @brief Sets text string from Mono String pointer.
            @param value Mono String pointer.
        */
        inline void SetText(Structures::Mono::String *value) {
            if (!IsValid() || !value) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the font size of the text.
            @return Font size integer.
        */
        inline int GetFontSize() const {
            if (!IsValid()) return 14;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_fontSize"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the font size of the text.
            @param value Font size integer.
        */
        inline void SetFontSize(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_fontSize"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets text alignment (TextAnchor enum).
            @return TextAnchor integer.
        */
        inline int GetAlignment() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_alignment"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text alignment.
            @param value TextAnchor integer.
        */
        inline void SetAlignment(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_alignment"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets line spacing multiplier.
            @return Line spacing float.
        */
        inline float GetLineSpacing() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_lineSpacing"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets line spacing multiplier.
            @param value Line spacing float.
        */
        inline void SetLineSpacing(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_lineSpacing"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is rich text markup enabled?
            @return True if rich text enabled.
        */
        inline bool GetSupportRichText() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_supportRichText"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether rich text markup is supported.
            @param value True to support rich text.
        */
        inline void SetSupportRichText(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_supportRichText"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The preferred width that the text would like to be given the string.
            @return Preferred width float.
        */
        inline float GetPreferredWidth() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_preferredWidth"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief The preferred height that the text would like to be given the string.
            @return Preferred height float.
        */
        inline float GetPreferredHeight() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_preferredHeight"), 0).cast<float>();
            return method[(void *)this]();
        }
    };

    /**
        @brief Image rendering component for uGUI (displays sprites).
    */
    struct Image : public MaskableGraphic {

        /**
            @brief Gets the Sprite rendered by the Image component.
            @return Sprite Object pointer.
        */
        inline Object *GetSprite() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("get_sprite"), 0).cast<Object *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the Sprite rendered by the Image component.
            @param sprite Sprite Object pointer.
        */
        inline void SetSprite(Object *sprite) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("set_sprite"), 1).cast<void>();
            method[(void *)this](sprite);
        }

        /**
            @brief Gets image display type (0 = Simple, 1 = Sliced, 2 = Tiled, 3 = Filled).
            @return Type enum integer.
        */
        inline int GetType() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("get_type"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets image display type.
            @param value Type enum integer (0 = Simple, 1 = Sliced, 2 = Tiled, 3 = Filled).
        */
        inline void SetType(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("set_type"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Amount of the Image that is showing (0.0 to 1.0) when type is Filled.
            @return Fill amount float.
        */
        inline float GetFillAmount() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("get_fillAmount"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets fill amount (0.0 to 1.0).
            @param value Fill amount float.
        */
        inline void SetFillAmount(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("set_fillAmount"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Controls whether the image aspect ratio is preserved.
            @return True if aspect ratio preserved.
        */
        inline bool GetPreserveAspect() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("get_preserveAspect"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether aspect ratio is preserved.
            @param value True to preserve aspect ratio.
        */
        inline void SetPreserveAspect(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("set_preserveAspect"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Adjusts the image size to make it match the native sprite dimensions.
        */
        inline void SetNativeSize() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Image>().ToClass().GetMethod(BNM_OBFUSCATE("SetNativeSize"), 0).cast<void>();
            method[(void *)this]();
        }
    };

    /**
        @brief Base class for interactive UI components (Buttons, Sliders, Toggles, etc.).
    */
    struct Selectable : public UIBehaviour {

        /**
            @brief Is this selectable interactable?
            @return True if interactable.
        */
        inline bool GetInteractable() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<Selectable>().ToClass().GetMethod(BNM_OBFUSCATE("get_interactable"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets interactable state.
            @param value True to make interactable.
        */
        inline void SetInteractable(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Selectable>().ToClass().GetMethod(BNM_OBFUSCATE("set_interactable"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Selects this selectable component.
        */
        inline void Select() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Selectable>().ToClass().GetMethod(BNM_OBFUSCATE("Select"), 0).cast<void>();
            method[(void *)this]();
        }
    };

    /**
        @brief Standard clickable Button component.
    */
    struct Button : public Selectable {

        /**
            @brief The onClick UnityEvent triggered when the button is clicked.
            @return UnityEvent<> pointer.
        */
        inline UnityEvent<> *GetOnClick() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Button>().ToClass().GetMethod(BNM_OBFUSCATE("get_onClick"), 0).cast<UnityEvent<> *>();
            return method[(void *)this]();
        }
    };

    /**
        @brief Slider control component for selecting numerical values.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Slider : public Selectable {

        /**
            @brief Gets the current value of the slider.
            @return Value float.
        */
        inline float GetValue() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("get_value"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets current value of the slider (invokes onValueChanged).
            @param value New value.
        */
        inline void SetValue(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("set_value"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Sets slider value without triggering onValueChanged events.
            @note Unity Version Aware: Uses SetValueWithoutNotify on Unity 2019.1+, fallback to set_value on older.
            @param value New value.
        */
        inline void SetValueWithoutNotify(float value) {
            if (!IsValid()) return;
            static auto methodNew = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("SetValueWithoutNotify"), 1).cast<void>();
            if (methodNew.IsValid()) {
                methodNew[(void *)this](value);
                return;
            }
            SetValue(value);
        }

        /**
            @brief Minimum allowed value of the slider.
            @return Min value float.
        */
        inline float GetMinValue() const {
            if (!IsValid()) return 0.0f;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("get_minValue"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets minimum allowed value.
            @param value Min value float.
        */
        inline void SetMinValue(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("set_minValue"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Maximum allowed value of the slider.
            @return Max value float.
        */
        inline float GetMaxValue() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("get_maxValue"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets maximum allowed value.
            @param value Max value float.
        */
        inline void SetMaxValue(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("set_maxValue"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Should whole numbers (integers) only be used?
            @return True if whole numbers only.
        */
        inline bool GetWholeNumbers() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("get_wholeNumbers"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether whole numbers only should be used.
            @param value True for integers only.
        */
        inline void SetWholeNumbers(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("set_wholeNumbers"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The onValueChanged UnityEvent triggered when slider value changes.
            @return UnityEvent<float>* pointer.
        */
        inline UnityEvent<float> *GetOnValueChanged() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Slider>().ToClass().GetMethod(BNM_OBFUSCATE("get_onValueChanged"), 0).cast<UnityEvent<float> *>();
            return method[(void *)this]();
        }
    };

    /**
        @brief Toggle (Checkbox) component.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct Toggle : public Selectable {

        /**
            @brief Is the toggle currently on (checked)?
            @return True if checked.
        */
        inline bool GetIsOn() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<Toggle>().ToClass().GetMethod(BNM_OBFUSCATE("get_isOn"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether toggle is on (invokes onValueChanged).
            @param value True for on, false for off.
        */
        inline void SetIsOn(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<Toggle>().ToClass().GetMethod(BNM_OBFUSCATE("set_isOn"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Sets toggle state without invoking onValueChanged events.
            @note Unity Version Aware: Uses SetIsOnWithoutNotify on Unity 2019.1+, fallback to set_isOn on older.
            @param value True for on.
        */
        inline void SetIsOnWithoutNotify(bool value) {
            if (!IsValid()) return;
            static auto methodNew = BNM::Defaults::Get<Toggle>().ToClass().GetMethod(BNM_OBFUSCATE("SetIsOnWithoutNotify"), 1).cast<void>();
            if (methodNew.IsValid()) {
                methodNew[(void *)this](value);
                return;
            }
            SetIsOn(value);
        }

        /**
            @brief The onValueChanged UnityEvent triggered when toggle state changes.
            @return UnityEvent<bool>* pointer.
        */
        inline UnityEvent<bool> *GetOnValueChanged() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<Toggle>().ToClass().GetMethod(BNM_OBFUSCATE("get_onValueChanged"), 0).cast<UnityEvent<bool> *>();
            return method[(void *)this]();
        }
    };

    /**
        @brief InputField component for text input.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct InputField : public Selectable {

        /**
            @brief Current text string in the input field.
            @return Mono String pointer.
        */
        inline Structures::Mono::String *GetText() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_text"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text string from string_view (invokes onValueChanged).
            @param value Text string.
        */
        inline void SetText(const std::string_view &value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(value));
        }

        /**
            @brief Sets text string without triggering onValueChanged events.
            @note Unity Version Aware: Uses SetTextWithoutNotify on Unity 2019.1+, fallback to set_text on older.
            @param value Text string.
        */
        inline void SetTextWithoutNotify(const std::string_view &value) {
            if (!IsValid()) return;
            static auto methodNew = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("SetTextWithoutNotify"), 1).cast<void>();
            if (methodNew.IsValid()) {
                methodNew[(void *)this](Structures::Mono::String::Create(value));
                return;
            }
            SetText(value);
        }

        /**
            @brief Maximum number of characters allowed in the input field (0 = unlimited).
            @return Character limit integer.
        */
        inline int GetCharacterLimit() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_characterLimit"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets maximum number of characters allowed in the input field.
            @param value Character limit integer.
        */
        inline void SetCharacterLimit(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("set_characterLimit"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is the input field read-only?
            @return True if read-only.
        */
        inline bool GetReadOnly() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_readOnly"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether input field is read-only.
            @param value True for read-only.
        */
        inline void SetReadOnly(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("set_readOnly"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The Text component used to render the text.
            @return Text component pointer.
        */
        inline Text *GetTextComponent() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_textComponent"), 0).cast<Text *>();
            return method[(void *)this]();
        }

        /**
            @brief Is the input field currently focused by the user?
            @return True if focused.
        */
        inline bool GetIsFocused() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_isFocused"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Activates the input field for text editing.
        */
        inline void ActivateInputField() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("ActivateInputField"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Deactivates the input field and stops text editing.
        */
        inline void DeactivateInputField() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("DeactivateInputField"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Event triggered when text value changes.
            @return UnityEvent<Structures::Mono::String *>* pointer.
        */
        inline UnityEvent<Structures::Mono::String *> *GetOnValueChanged() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_onValueChanged"), 0).cast<UnityEvent<Structures::Mono::String *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Event triggered when user finishes text editing (presses Enter or unfocuses).
            @return UnityEvent<Structures::Mono::String *>* pointer.
        */
        inline UnityEvent<Structures::Mono::String *> *GetOnEndEdit() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_onEndEdit"), 0).cast<UnityEvent<Structures::Mono::String *> *>();
            return method[(void *)this]();
        }
    };

    /**
        @brief Used for raycasting against a Canvas for touch and click interactions.
    */
    struct GraphicRaycaster : public UIBehaviour {

        /**
            @brief Should reversed graphics be ignored for raycasting?
            @return True if reversed graphics are ignored.
        */
        inline bool GetIgnoreReversedGraphics() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<GraphicRaycaster>().ToClass().GetMethod(BNM_OBFUSCATE("get_ignoreReversedGraphics"), 0).cast<bool>();
            if (method.IsValid()) return method[(void *)this]();
            return true;
        }

        /**
            @brief Sets whether reversed graphics should be ignored.
            @param value True to ignore reversed graphics.
        */
        inline void SetIgnoreReversedGraphics(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<GraphicRaycaster>().ToClass().GetMethod(BNM_OBFUSCATE("set_ignoreReversedGraphics"), 1).cast<void>();
            if (method.IsValid()) method[(void *)this](value);
        }
    };

    /**
        @brief Handles and routes input events (touch, click, drag) in the scene.
    */
    struct EventSystem : public UIBehaviour {

        /**
            @brief Gets the currently active EventSystem in the scene.
            @return EventSystem pointer, or nullptr if none exists.
        */
        static inline EventSystem *GetCurrent() {
            static auto method = BNM::Defaults::Get<EventSystem>().ToClass().GetMethod(BNM_OBFUSCATE("get_current"), 0).cast<EventSystem *>();
            if (method.IsValid()) return method();
            return nullptr;
        }

        /**
            @brief Sets the current EventSystem.
            @param eventSystem Target EventSystem.
        */
        static inline void SetCurrent(EventSystem *eventSystem) {
            static auto method = BNM::Defaults::Get<EventSystem>().ToClass().GetMethod(BNM_OBFUSCATE("set_current"), 1).cast<void>();
            if (method.IsValid()) method(eventSystem);
        }

        /**
            @brief Ensures an active EventSystem and InputModule exist in the scene (auto-creates persistent one if needed).
            Essential for Android touch input to work on injected UI.
            @return Active EventSystem pointer.
        */
        static inline EventSystem *EnsureEventSystem() {
            auto current = GetCurrent();
            if (current && current->IsValid()) return current;

            auto go = GameObject::Create(BNM_OBFUSCATE("BNM_EventSystem"));
            if (!go || !go->IsValid()) return nullptr;
            Object::DontDestroyOnLoad((Object *)go);

            auto es = (EventSystem *) go->AddComponent(BNM::Defaults::Get<EventSystem>().ToClass());
            go->AddComponent(BNM::Class(BNM_OBFUSCATE("UnityEngine.EventSystems"), BNM_OBFUSCATE("StandaloneInputModule")));

            return es;
        }
    };

    /**
        @brief Factory helper for creating full-featured runtime overlay Canvases.
    */
    struct CanvasHelper {

        /**
            @brief Creates a persistent ScreenSpaceOverlay Canvas configured for touch and interaction.
            @param name Canvas GameObject name.
            @param sortingOrder Sorting layer order.
            @param ensureEventSystem When true, automatically ensures an EventSystem exists.
            @return Created Canvas pointer.
        */
        static inline Canvas *CreateOverlayCanvas(const std::string_view &name = "BNM_OverlayCanvas", int sortingOrder = 1000, bool ensureEventSystem = true) {
            auto go = GameObject::Create(name);
            if (!go || !go->IsValid()) return nullptr;
            Object::DontDestroyOnLoad((Object *)go);

            auto canvas = go->AddComponent<Canvas *>();
            if (canvas) {
                canvas->SetRenderMode(0); // ScreenSpaceOverlay
                canvas->SetSortingOrder(sortingOrder);
            }

            auto scaler = go->AddComponent<CanvasScaler *>();
            if (scaler) {
                scaler->SetUiScaleMode(0); // ConstantPixelSize
            }

            go->AddComponent(BNM::Defaults::Get<GraphicRaycaster>().ToClass());

            if (ensureEventSystem) {
                EventSystem::EnsureEventSystem();
            }

            return canvas;
        }
    };
}

#endif
