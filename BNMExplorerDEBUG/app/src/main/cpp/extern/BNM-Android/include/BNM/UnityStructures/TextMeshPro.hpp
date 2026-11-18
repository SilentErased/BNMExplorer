#pragma once

#include "../UserSettings/GlobalSettings.hpp"

#ifdef BNM_UNITY_TEXTMESHPRO

#include <string>
#include <string_view>
#include "../Il2CppHeaders.hpp"
#include "../Utils.hpp"
#include "../BasicMonoStructures.hpp"
#include "../Class.hpp"
#include "../Method.hpp"
#include "UI.hpp"
#include "Color.hpp"
#include "Vector4.hpp"

namespace BNM::UnityEngine::TMPro {

    struct TMP_Text;
    struct TextMeshPro;
    struct TextMeshProUGUI;
    struct TMP_InputField;

    /**
        @brief Base class for TextMeshPro text rendering components.
        @note Fully Unity Version Aware across various TextMeshPro versions (Unity 5.6 to 2023+).
    */
    struct TMP_Text : public BNM::UnityEngine::UI::MaskableGraphic {

        /**
            @brief Gets the string rendered by TextMeshPro.
            @return Mono String pointer.
        */
        inline Structures::Mono::String *GetText() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_text"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text string from string_view.
            @param value Text string.
        */
        inline void SetText(const std::string_view &value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(value));
        }

        /**
            @brief Sets text string from Mono String pointer.
            @param value Mono String pointer.
        */
        inline void SetText(Structures::Mono::String *value) {
            if (!IsValid() || !value) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the point size (font size) of the text.
            @return Font size float.
        */
        inline float GetFontSize() const {
            if (!IsValid()) return 36.0f;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_fontSize"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets font size.
            @param value Font size float.
        */
        inline void SetFontSize(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_fontSize"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief The TMP_FontAsset assigned to this text component.
            @return Font asset Object pointer.
        */
        inline Object *GetFont() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_font"), 0).cast<Object *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets the TMP_FontAsset.
            @param font Font asset Object pointer.
        */
        inline void SetFont(Object *font) {
            if (!IsValid() || !font) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_font"), 1).cast<void>();
            method[(void *)this](font);
        }

        /**
            @brief Text alignment (TextAlignmentOptions enum).
            @return Alignment integer.
        */
        inline int GetAlignment() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_alignment"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text alignment.
            @param value Alignment integer.
        */
        inline void SetAlignment(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_alignment"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets text alpha transparency (0 to 1).
            @return Alpha float.
        */
        inline float GetAlpha() const {
            if (!IsValid()) return 1.0f;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_alpha"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text alpha transparency.
            @param value Alpha float.
        */
        inline void SetAlpha(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_alpha"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is rich text tags parsing enabled?
            @return True if rich text enabled.
        */
        inline bool GetRichText() const {
            if (!IsValid()) return true;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_richText"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets rich text tags parsing.
            @param value True to enable rich text.
        */
        inline void SetRichText(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_richText"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets the margins of the text container (left, top, right, bottom).
            @return Vector4 margins.
        */
        inline Structures::Unity::Vector4 GetMargin() const {
            if (!IsValid()) return {};
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_margin"), 0).cast<Structures::Unity::Vector4>();
            return method[(void *)this]();
        }

        /**
            @brief Sets margins of the text container.
            @param value Vector4 margins (left, top, right, bottom).
        */
        inline void SetMargin(Structures::Unity::Vector4 value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_margin"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is word wrapping enabled?
            @return True if word wrapping enabled.
        */
        inline bool GetEnableWordWrapping() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_enableWordWrapping"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Sets whether word wrapping is enabled.
            @param value True to enable word wrapping.
        */
        inline void SetEnableWordWrapping(bool value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_enableWordWrapping"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets overflow mode (0 = Overflow, 1 = Ellipsis, 2 = Masking, 3 = Truncate, 4 = ScrollRect, 5 = Page).
            @return TextOverflowModes integer.
        */
        inline int GetOverflowMode() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_overflowMode"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets overflow mode.
            @param value TextOverflowModes integer.
        */
        inline void SetOverflowMode(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_overflowMode"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is the text overflowing its container?
            @return True if text overflowing.
        */
        inline bool GetIsTextOverflowing() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_isTextOverflowing"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Maximum number of characters to display.
            @return Max visible characters integer.
        */
        inline int GetMaxVisibleCharacters() const {
            if (!IsValid()) return 99999;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("get_maxVisibleCharacters"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets maximum number of visible characters.
            @param value Max visible characters integer.
        */
        inline void SetMaxVisibleCharacters(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("set_maxVisibleCharacters"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Forces immediate recalculation and update of the text mesh geometry.
            @note Unity Version Aware: Handles 2-arguments, 1-argument, and parameterless overloads.
            @param ignoreActiveState True to ignore active state of gameObject.
            @param forceTextReparsing True to force re-parsing text tags and strings.
        */
        inline void ForceMeshUpdate(bool ignoreActiveState = false, bool forceTextReparsing = false) {
            if (!IsValid()) return;
            static auto method2 = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("ForceMeshUpdate"), 2).cast<void>();
            if (method2.IsValid()) {
                method2[(void *)this](ignoreActiveState, forceTextReparsing);
                return;
            }
            static auto method1 = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("ForceMeshUpdate"), 1).cast<void>();
            if (method1.IsValid()) {
                method1[(void *)this](ignoreActiveState);
                return;
            }
            static auto method0 = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("ForceMeshUpdate"), 0).cast<void>();
            if (method0.IsValid()) method0[(void *)this]();
        }

        /**
            @brief Clears text mesh geometry.
            @param updateMesh True to upload cleared mesh to GPU.
        */
        inline void ClearMesh(bool updateMesh = false) {
            if (!IsValid()) return;
            static auto method1 = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("ClearMesh"), 1).cast<void>();
            if (method1.IsValid()) {
                method1[(void *)this](updateMesh);
                return;
            }
            static auto method0 = BNM::Defaults::Get<TMP_Text>().ToClass().GetMethod(BNM_OBFUSCATE("ClearMesh"), 0).cast<void>();
            if (method0.IsValid()) method0[(void *)this]();
        }
    };

    /**
        @brief 3D World-space TextMeshPro component.
    */
    struct TextMeshPro : public TMP_Text {

        /**
            @brief Gets sorting layer ID.
            @return Sorting layer ID integer.
        */
        inline int GetSortingLayerID() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<TextMeshPro>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingLayerID"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets sorting layer ID.
            @param value Sorting layer ID.
        */
        inline void SetSortingLayerID(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TextMeshPro>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingLayerID"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Gets sorting order within layer.
            @return Sorting order integer.
        */
        inline int GetSortingOrder() const {
            if (!IsValid()) return 0;
            static auto method = BNM::Defaults::Get<TextMeshPro>().ToClass().GetMethod(BNM_OBFUSCATE("get_sortingOrder"), 0).cast<int>();
            return method[(void *)this]();
        }

        /**
            @brief Sets sorting order within layer.
            @param value Sorting order integer.
        */
        inline void SetSortingOrder(int value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TextMeshPro>().ToClass().GetMethod(BNM_OBFUSCATE("set_sortingOrder"), 1).cast<void>();
            method[(void *)this](value);
        }
    };

    /**
        @brief 2D Canvas-based TextMeshPro UGUI component.
    */
    struct TextMeshProUGUI : public TMP_Text {
    };

    /**
        @brief TextMeshPro input field component.
        @note Fully Unity Version Aware (Unity 5.6 to 2023+).
    */
    struct TMP_InputField : public BNM::UnityEngine::UI::Selectable {

        /**
            @brief Current text string in the input field.
            @return Mono String pointer.
        */
        inline Structures::Mono::String *GetText() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_text"), 0).cast<Structures::Mono::String *>();
            return method[(void *)this]();
        }

        /**
            @brief Sets text string from string_view (invokes onValueChanged).
            @param value Text string.
        */
        inline void SetText(const std::string_view &value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("set_text"), 1).cast<void>();
            method[(void *)this](Structures::Mono::String::Create(value));
        }

        /**
            @brief Sets text string without triggering onValueChanged events.
            @param value Text string.
        */
        inline void SetTextWithoutNotify(const std::string_view &value) {
            if (!IsValid()) return;
            static auto methodNew = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("SetTextWithoutNotify"), 1).cast<void>();
            if (methodNew.IsValid()) {
                methodNew[(void *)this](Structures::Mono::String::Create(value));
                return;
            }
            SetText(value);
        }

        /**
            @brief The TMP_Text component used to render the text.
            @return TMP_Text pointer.
        */
        inline TMP_Text *GetTextComponent() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_textComponent"), 0).cast<TMP_Text *>();
            return method[(void *)this]();
        }

        /**
            @brief Gets the point size of the text.
            @return Point size float.
        */
        inline float GetPointSize() const {
            if (!IsValid()) return 14.0f;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_pointSize"), 0).cast<float>();
            return method[(void *)this]();
        }

        /**
            @brief Sets point size.
            @param value Point size float.
        */
        inline void SetPointSize(float value) {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("set_pointSize"), 1).cast<void>();
            method[(void *)this](value);
        }

        /**
            @brief Is the input field currently focused by user?
            @return True if focused.
        */
        inline bool GetIsFocused() const {
            if (!IsValid()) return false;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_isFocused"), 0).cast<bool>();
            return method[(void *)this]();
        }

        /**
            @brief Activates the input field for text editing.
        */
        inline void ActivateInputField() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("ActivateInputField"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Deactivates the input field.
        */
        inline void DeactivateInputField() {
            if (!IsValid()) return;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("DeactivateInputField"), 0).cast<void>();
            method[(void *)this]();
        }

        /**
            @brief Event triggered when text value changes.
            @return UnityEvent<Structures::Mono::String *>* pointer.
        */
        inline UnityEvent<Structures::Mono::String *> *GetOnValueChanged() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_onValueChanged"), 0).cast<UnityEvent<Structures::Mono::String *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Event triggered when user finishes text editing.
            @return UnityEvent<Structures::Mono::String *>* pointer.
        */
        inline UnityEvent<Structures::Mono::String *> *GetOnEndEdit() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_onEndEdit"), 0).cast<UnityEvent<Structures::Mono::String *> *>();
            return method[(void *)this]();
        }

        /**
            @brief Event triggered when user submits text (e.g. presses enter on desktop/action key on mobile).
            @return UnityEvent<Structures::Mono::String *>* pointer.
        */
        inline UnityEvent<Structures::Mono::String *> *GetOnSubmit() const {
            if (!IsValid()) return nullptr;
            static auto method = BNM::Defaults::Get<TMP_InputField>().ToClass().GetMethod(BNM_OBFUSCATE("get_onSubmit"), 0).cast<UnityEvent<Structures::Mono::String *> *>();
            return method[(void *)this]();
        }
    };
}

#endif
