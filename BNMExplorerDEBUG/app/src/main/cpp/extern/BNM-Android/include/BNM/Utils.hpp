#pragma once

#include <csetjmp>
#include <csignal>

#include <string_view>
#include <type_traits>

#include "UserSettings/GlobalSettings.hpp"
#include "Il2CppHeaders.hpp"

namespace BNM {
#if __DOXYGEN__
#define NO_INLINE
#else
#define NO_INLINE __attribute__((noinline))
#endif

    /**
        @brief Macro function for checking pointer for null. Due to noinline attribute allows to check even `this` of objects.
    */
    template <typename T>
    NO_INLINE bool CheckForNull(T obj) { return (void *) obj; }

    /**
        @brief Macro function for checking if pointer points to valid address.
        @return True if address is valid.
    */
    template <typename T, typename = std::enable_if_t<std::is_pointer_v<T>>>
    inline bool IsAllocated(T x) {
#if defined(BNM_ALLOW_SAFE_IS_ALLOCATED) && defined(SIGSEGV)
        if (!x) return false;

        static jmp_buf jump;
        using sig_handler_t = void (*)(int);
        static sig_handler_t handler = [](int) { longjmp(jump, 1); }; // NOLINT
        char c;
        bool ok = true;
        sig_handler_t old_handler = signal(SIGSEGV, handler);
        if (!setjmp (jump)) c = *(char *) x; else ok = false; // NOLINT
        (void)c;
        signal(SIGSEGV, old_handler);
        return ok;
#else
        return x != nullptr;
#endif
    }

    namespace Structures::Mono { struct String; }

    /**
        @brief Macro function for creating C# strings (BNM::Structures::Mono::String).
        @param str UTF-8 string_view. Prefer this over raw pointers: lengths are passed explicitly to
               il2cpp_string_new_len, so embedded nulls and non-null-terminated views are handled safely.
        @return New mono string object, or null if the il2cpp string API was not resolved.
    */
    Structures::Mono::String *CreateMonoString(const std::string_view &str);

    /**
        @brief Macro function for getting external methods (icall).
        @return Pointer to extern method if it's found, otherwise null.
    */
    void *GetExternMethod(const std::string_view &str);

    /**
        @brief Check if BNM and il2cpp are loaded.
        @return True if BNM and il2cpp are loaded.
    */
    bool IsLoaded();

    /**
        @brief Get handle of libil2cpp.so if it's used by BNM.
        @return Dlfcn handle of libil2cpp.so if it's used by BNM and BNM is loaded, otherwise null.
    */
    void *GetIl2CppLibraryHandle();

    /**
        @brief Unboxes an Il2CppObject into a value type or casts to a reference type pointer.
        @tparam T Target type (e.g. int, float, Vector3, or Transform*)
        @param obj The Il2CppObject to unbox
        @return Extracted value or casted object pointer
    */
    template<typename T>
    inline T Unbox(IL2CPP::Il2CppObject *obj) {
        if (!obj) return {};
        if constexpr (std::is_pointer_v<T>) {
            return (T) obj;
        } else {
            return *(T *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject));
        }
    }

    /**
        @brief Unbox any object.
        @return Unboxed object of passed type
    */
    template<typename T>
    inline T UnboxObject(T obj) { return (T)(void *)(((char *)obj) + sizeof(BNM::IL2CPP::Il2CppObject)); }

    template<typename T>
    inline T UnboxObject(IL2CPP::Il2CppObject *obj) { return Unbox<T>(obj); }

#ifdef BNM_DEPRECATED
    template <typename T, typename = std::enable_if<std::is_pointer<T>::value>>
    inline T CheckObj(T obj) {
        if (obj && IsAllocated(obj)) return obj;
        return nullptr;
    }

    template<typename MET_T, typename PTR_T>
    inline void InitFunc(MET_T& method, PTR_T ptr) {
        *(void **)&method = (void *)ptr;
    }
#endif

    /// @cond
    // For thread static adn const fields
    namespace PRIVATE_FieldUtils {
        void GetStaticValue(IL2CPP::FieldInfo *info, void *value);
        void SetStaticValue(IL2CPP::FieldInfo *info, void *value);
    }
    /// @endcond

    struct CompileTimeClass;

    /**
        @brief Utility functions for memory operations, debugging, and binary scanning.
    */
    namespace Utils {
#ifdef BNM_DEBUG
        void *OffsetInLib(void *);
        void LogCompileTimeClass(const BNM::CompileTimeClass &compileTimeClass);
#endif

        /**
            @brief Scans memory for a specific byte pattern with wildcard support (AOB Scanner).
            @param start Pointer to the start of memory range to scan.
            @param length Number of bytes to scan.
            @param pattern IDA-style byte pattern string (e.g. "48 8B 05 ?? ?? ?? ?? 48 85 C0" or "?? 00 00 94").
            @return Pointer to the first matching byte sequence, or nullptr if not found.
        */
        void *PatternScan(const void *start, size_t length, const std::string_view &pattern);

        /**
            @brief Scans an ELF loaded module in memory for a byte pattern.
            Automatically parses ELF headers to locate executable (.text) segments.
            @param moduleBase Base address of the loaded shared library (e.g. from dlopen / dladdr).
            @param pattern IDA-style byte pattern string.
            @return Pointer to the first matching byte sequence, or nullptr if not found.
        */
        void *PatternScanModule(const void *moduleBase, const std::string_view &pattern);
    }

    /**
        @brief Attach current thread to il2cpp VM.
        @return True if thread was attached.
    */
    bool AttachIl2Cpp();

    /**
        @brief Get current thread attached to il2cpp VM.
        @return IL2CPP::Il2CppThread if current tread is attached to VM, otherwise null.
    */
    IL2CPP::Il2CppThread *CurrentIl2CppThread();

    /**
        @brief Detach current thread from il2cpp VM.
    */
    void DetachIl2Cpp();

    /**
        @brief Allocates memory that is registered in il2cpp's GC.
        @return Allocated memory
    */
    void *Allocate(size_t size);

    /**
        @brief Frees memory that was allocated by il2cpp.
    */
    void Free(void *);

    /**
        @brief Creates a new GC handle for a managed object.
        @param obj The managed object (Il2CppObject *) to create a handle for.
        @param pinned If true, pins the object in memory (prevents GC from moving it).
        @return GC handle (void *) that keeps the object alive until freed.
    */
    void *NewGCHandle(void *obj, bool pinned = false);

    /**
        @brief Frees a GC handle previously created by NewGCHandle.
        @param handle The GC handle to free.
    */
    void FreeGCHandle(void *handle);

    /**
        @brief Gets the managed object a GC handle points to (il2cpp_gchandle_get_target).
        @param handle The GC handle created by NewGCHandle / NewWeakGCHandle.
        @return The managed object (Il2CppObject *) or null if the handle is invalid.
    */
    void *GetGCHandleTarget(void *handle);

    /**
        @brief Creates a new weak-reference GC handle for a managed object (il2cpp_gchandle_new_weakref).
        @param obj The managed object (Il2CppObject *) to create a weak handle for.
        @param trackResurrection If true, the handle tracks resurrection.
        @return Weak GC handle (void *) that does not keep the object alive.
    */
    void *NewWeakGCHandle(void *obj, bool trackResurrection = false);

    /**
        @brief Runs the static constructor (.cctor) of a class if it has not run yet (il2cpp_runtime_class_init).
        @param klass The IL2CPP class to initialize.
    */
    void RuntimeClassInit(IL2CPP::Il2CppClass *klass);

    /**
        @brief Invokes a managed method directly via il2cpp_runtime_invoke.
        @param method The MethodInfo to invoke.
        @param obj The instance (null for static methods).
        @param params Array of parameter pointers (can be null if no parameters).
        @param exc Receives the exception object on managed exception (can be null).
        @return The boxed return value or null.
    */
    void *RuntimeInvoke(IL2CPP::MethodInfo *method, void *obj, void **params, IL2CPP::Il2CppException **exc = nullptr);

    /**
        @brief Gets the IL2CPP class of a managed object (il2cpp_object_get_class).
        @return Il2CppClass of the object or null.
    */
    IL2CPP::Il2CppClass *GetObjectClass(IL2CPP::Il2CppObject *obj);

    /**
        @brief Gets the Il2CppType of a class (il2cpp_class_get_type).
        @return Il2CppType of the class or null.
    */
    const IL2CPP::Il2CppType *GetClassType(IL2CPP::Il2CppClass *klass);

    /**
        @brief Gets the name of a class (il2cpp_class_get_name).
        @return Class name (e.g. "String") or null.
    */
    const char *GetClassName(IL2CPP::Il2CppClass *klass);

    /**
        @brief Gets the namespace of a class (il2cpp_class_get_namespace).
        @return Class namespace (e.g. "System") or null.
    */
    const char *GetClassNamespace(IL2CPP::Il2CppClass *klass);

    /**
        @brief Gets the image (assembly metadata) a class belongs to (il2cpp_class_get_image).
        @return Il2CppImage of the class or null.
    */
    const IL2CPP::Il2CppImage *GetClassImage(IL2CPP::Il2CppClass *klass);

    /**
        @brief Gets the parent (base) class of a class (il2cpp_class_get_parent).
        @return Parent Il2CppClass or null.
    */
    IL2CPP::Il2CppClass *GetClassParent(IL2CPP::Il2CppClass *klass);

    /**
        @brief Checks whether a class is a value type (il2cpp_class_is_valuetype).
    */
    bool IsClassValueType(IL2CPP::Il2CppClass *klass);

    /**
        @brief Checks whether a class is an enum (il2cpp_class_is_enum).
    */
    bool IsClassEnum(IL2CPP::Il2CppClass *klass);

    /**
        @brief Finds a method by name in a class, optionally filtered by argument count (il2cpp_class_get_method_from_name).
        @param klass The class to search.
        @param name Method name (any string_view is accepted; internally copied to a null-terminated buffer).
        @param argsCount Required argument count, or -1 to match any overload.
        @return MethodInfo or null if not found.
    */
    const IL2CPP::MethodInfo *GetMethodFromName(IL2CPP::Il2CppClass *klass, const std::string_view &name, int argsCount = -1);

    /**
        @brief Finds a field by name in a class (il2cpp_class_get_field_from_name).
        @param klass The class to search.
        @param name Field name (any string_view is accepted; internally copied to a null-terminated buffer).
        @return FieldInfo or null if not found.
    */
    IL2CPP::FieldInfo *GetFieldFromName(IL2CPP::Il2CppClass *klass, const std::string_view &name);

    /**
        @brief Gets the offset of the per-class userdata pointer in Il2CppClass (il2cpp_class_get_userdata_offset).
        @return The offset, or -1 on Unity < 2019.2 where this API does not exist.
    */
    int GetClassUserDataOffset();

    /**
        @brief Sets the per-class userdata pointer (il2cpp_class_set_userdata). No-op on Unity < 2019.2.
    */
    void SetClassUserData(IL2CPP::Il2CppClass *klass, void *userData);

    /**
        @brief Creates a C# string with an explicit length (il2cpp_string_new_len).
        @param str UTF-8 characters (may contain embedded nulls).
        @return New mono string object or null.
    */
    Structures::Mono::String *CreateMonoStringLen(const std::string_view &str);

#if UNITY_VER >= 232
    /**
        @brief Unmarshals unity object.
        @return Unmarshaled unity object
    */
    inline BNM_INT_PTR UnmarshalUnityObject(BNM_INT_PTR gcHandlePtr) {
        if (!gcHandlePtr) return 0;
        return *(BNM_INT_PTR *)(uintptr_t)(gcHandlePtr & ~(BNM_INT_PTR)1);
    }
#endif

    /// @cond
    namespace PRIVATE_INTERNAL {
        template<typename T>
        inline T ReturnEmpty() { if constexpr (std::is_same_v<T, void>) return; else return {}; }

        inline IL2CPP::Il2CppClass *&GetMethodClass(IL2CPP::MethodInfo *methodInfo) {
#if UNITY_VER > 174
#define kls klass
#else
#define kls declaring_type
#endif
            return methodInfo->kls;
#undef kls
        }
    }
    /// @endcond

    /**
        @brief BNM's custom forward list implementation

        @warning Don't use it anywhere! Internal Only

        Almost same as std::forward_list, but works how BNM needs to.
        Has only pointer to last element, but elements are looped, so last element points to first one.
        It used internally, so it lacks a lot of C++ stuff like foreach.
    */
    template<typename T>
    struct ForwardList {
        struct Element {
            Element *next{};
            T value{};
        };

        Element *lastElement{};

        inline ForwardList() = default;

        inline ~ForwardList() { Clear(); }

        inline ForwardList(const ForwardList& other) : lastElement(nullptr) {
            if (other.IsEmpty()) return;

            auto currentOther = other.lastElement->next;
            do {
                Add(currentOther->value);
                currentOther = currentOther->next;
            } while (currentOther != other.lastElement->next);
        }

        inline ForwardList& operator=(const ForwardList& other) {
            if (this == &other) return *this;

            Clear();

            if (other.IsEmpty()) return *this;

            auto currentOther = other.lastElement->next;
            do {
                Add(currentOther->value);
                currentOther = currentOther->next;
            } while (currentOther != other.lastElement->next);

            return *this;
        }

        [[nodiscard]] inline bool IsEmpty() const { return !lastElement; }

        inline void Add(T value) {
            auto newElement = (Element *) BNM_malloc(sizeof(Element));
            newElement->next = nullptr;
            newElement->value = value;

            if (IsEmpty()) {
                newElement->next = newElement;
                lastElement = newElement;
            } else {
                newElement->next = lastElement->next;
                lastElement->next = newElement;
                lastElement = newElement;
            }
        }

        inline void Clear(void (*onElementFreed)(T value) = nullptr) {
            if (IsEmpty()) return;

            auto current = lastElement->next;
            lastElement->next = nullptr;

            while (current) {
                auto next = current->next;
                if (onElementFreed) onElementFreed(current->value);
                BNM_free(current);
                current = next;
            }

            lastElement = nullptr;
        }
    };
}