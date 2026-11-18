#pragma once

/*
    Don't include this file if you don't know how BNM and il2cpp API work.
    This file can be included only by advanced users that know how BNM and il2cpp API work.
*/

#include <vector>
#include <map>

#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Utils.hpp>
#include <BNM/BasicMonoStructures.hpp>
#include <BNM/Method.hpp>
#include <BNM/Class.hpp>
#include <BNM/ClassesManagement.hpp>
#include <BNM/Loading.hpp>

#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
#include <shared_mutex>
#endif

#ifdef BNM_CLASSES_MANAGEMENT
#include <unordered_set>
#endif

/// @cond
namespace BNM::Internal {

#pragma pack(push, 1)

    struct States {
        uint8_t state : 1{};
        uint8_t lateInitAllowed : 1{};
    } extern states;
    extern void *il2cppLibraryHandle;
    extern Loading::MethodFinder currentFinderMethod;
    extern void *currentFinderData;

    //! \internal
    // A list with variables from the il2cpp VM
    extern struct VMData {
        BNM::Class Object{}, UnityEngine$$Object{}, System$$List{};
        BNM::Method<IL2CPP::Il2CppReflectionType *> Type$$GetType{};
        BNM::Method<void *> Interlocked$$CompareExchange{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakeGenericType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$MakePointerType{};
        BNM::Method<BNM::MonoType *> RuntimeType$$make_byref_type{};
        BNM::Method<BNM::IL2CPP::Il2CppReflectionMethod *> RuntimeMethodInfo$$MakeGenericMethod_impl{};
        BNM::Structures::Mono::String **String$$Empty{};
    } vmData;

    //! \internal
    // All il2cpp methods that can be used by BNM stored here to avoid searching them every BNM call
    extern struct Il2CppMethods {
        // Exported il2cpp API methods
        BNM::IL2CPP::Il2CppImage *(*il2cpp_get_corlib)(){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_class_from_name)(const BNM::IL2CPP::Il2CppImage *, const char *, const char *){};
        BNM::IL2CPP::Il2CppImage *(*il2cpp_assembly_get_image)(const BNM::IL2CPP::Il2CppAssembly *){};
#if UNITY_VER >= 183
        BNM::IL2CPP::Il2CppClass *(*il2cpp_image_get_class)(const BNM::IL2CPP::Il2CppImage *, unsigned int){};
#endif
        const char *(*il2cpp_method_get_param_name)(const BNM::IL2CPP::MethodInfo *, uint32_t){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_class_from_il2cpp_type)(const BNM::IL2CPP::Il2CppType *){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_array_class_get)(const BNM::IL2CPP::Il2CppClass *, uint32_t){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_type_get_object)(const BNM::IL2CPP::Il2CppType *){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_object_new)(const BNM::IL2CPP::Il2CppClass *){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_value_box)(const BNM::IL2CPP::Il2CppClass *, void *){};
        BNM::IL2CPP::Il2CppArray *(*il2cpp_array_new)(const BNM::IL2CPP::Il2CppClass *, BNM::IL2CPP::il2cpp_array_size_t){};
        void (*il2cpp_field_static_get_value)(const BNM::IL2CPP::FieldInfo *, void *){};
        void (*il2cpp_field_static_set_value)(const BNM::IL2CPP::FieldInfo *, void *){};
        BNM::Structures::Mono::String *(*il2cpp_string_new)(const char *){};
        void *(*il2cpp_resolve_icall)(const char *){};
        void *(*il2cpp_runtime_invoke)(BNM::IL2CPP::MethodInfo *, void *, void **, BNM::IL2CPP::Il2CppException **){};
        IL2CPP::Il2CppDomain *(*il2cpp_domain_get)(){};
        const BNM::IL2CPP::Il2CppAssembly **(*il2cpp_domain_get_assemblies)(const BNM::IL2CPP::Il2CppDomain *, size_t *){};
        IL2CPP::Il2CppThread *(*il2cpp_thread_current)(IL2CPP::Il2CppDomain *){};
        IL2CPP::Il2CppThread *(*il2cpp_thread_attach)(IL2CPP::Il2CppDomain *){};
        void (*il2cpp_thread_detach)(IL2CPP::Il2CppThread *){};
#if UNITY_VER >= 212
        void *(*il2cpp_gc_alloc_fixed)(size_t){};
        void (*il2cpp_gc_free_fixed)(void*){};
#endif

        // GC handles (canonical Unity names)
        BNM::IL2CPP::Il2CppGCHandle (*il2cpp_gchandle_new)(BNM::IL2CPP::Il2CppObject *, bool){};
        void (*il2cpp_gchandle_free)(BNM::IL2CPP::Il2CppGCHandle){};
        BNM::IL2CPP::Il2CppObject *(*il2cpp_gchandle_get_target)(BNM::IL2CPP::Il2CppGCHandle){};
        BNM::IL2CPP::Il2CppGCHandle (*il2cpp_gchandle_new_weakref)(BNM::IL2CPP::Il2CppObject *, bool){};
#if UNITY_VER >= 193
        void (*il2cpp_gchandle_foreach_get_target)(void (*func)(void *data, void *userData), void *userData){};
#endif

        // Class runtime helpers
        void (*il2cpp_runtime_class_init)(BNM::IL2CPP::Il2CppClass *){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_object_get_class)(BNM::IL2CPP::Il2CppObject *){};
        const BNM::IL2CPP::Il2CppType *(*il2cpp_class_get_type)(BNM::IL2CPP::Il2CppClass *){};
        const char *(*il2cpp_class_get_name)(BNM::IL2CPP::Il2CppClass *){};
        const char *(*il2cpp_class_get_namespace)(BNM::IL2CPP::Il2CppClass *){};
        const BNM::IL2CPP::Il2CppImage *(*il2cpp_class_get_image)(BNM::IL2CPP::Il2CppClass *){};
        BNM::IL2CPP::Il2CppClass *(*il2cpp_class_get_parent)(BNM::IL2CPP::Il2CppClass *){};
        bool (*il2cpp_class_is_valuetype)(const BNM::IL2CPP::Il2CppClass *){};
        bool (*il2cpp_class_is_enum)(const BNM::IL2CPP::Il2CppClass *){};
        const BNM::IL2CPP::MethodInfo *(*il2cpp_class_get_method_from_name)(BNM::IL2CPP::Il2CppClass *, const char *, int){};
        BNM::IL2CPP::FieldInfo *(*il2cpp_class_get_field_from_name)(BNM::IL2CPP::Il2CppClass *, const char *){};
#if UNITY_VER >= 191
        int (*il2cpp_class_get_userdata_offset)(){};
        void (*il2cpp_class_set_userdata)(BNM::IL2CPP::Il2CppClass *, void *){};
#endif

        // String helpers
        BNM::Structures::Mono::String *(*il2cpp_string_new_len)(const char *, uint32_t){};
        BNM::Structures::Mono::String *(*il2cpp_string_new_utf16)(const BNM::IL2CPP::Il2CppChar *, int32_t){};

        // Direct il2cpp API methods
        std::vector<BNM::IL2CPP::Il2CppAssembly *> *(*Assembly$$GetAllAssemblies)(){};
        void (*orig_Image$$GetTypes)(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target){};
        void (*Class$$Init)(IL2CPP::Il2CppClass *klass){};
    } il2cppMethods;

#pragma pack(pop)

    extern BNM::ForwardList<void(*)()> onIl2CppLoaded;

    extern std::string_view constructorName;
    extern BNM::Class customListTemplateClass;
    extern std::map<uint32_t, BNM::Class> customListsMap;
    extern int32_t finalizerSlot;

#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    extern std::shared_mutex customListsMapMutex;
#endif

    // Allocation tracking sets (replaces BNM_CLASS_ALLOCATED_* flag bits on klass->flags).
    // Flags on klass->flags belong to the runtime; writing custom bits there risks false
    // positives when the runtime re-initializes a class. These sets are owned by BNM only.
#ifdef BNM_CLASSES_MANAGEMENT
    extern std::unordered_set<BNM::IL2CPP::Il2CppClass *> bnmAllocatedClasses;  // methods, fields, hierarchy
    extern std::unordered_set<BNM::IL2CPP::Il2CppClass **> bnmAllocatedInnerLists; // nestedTypes arrays
#endif

    std::vector<BNM::IL2CPP::Il2CppAssembly *> &GetAllAssemblies();

    void Image$$GetTypes(const IL2CPP::Il2CppImage *image, bool exportedOnly, std::vector<BNM::IL2CPP::Il2CppClass *> *target);

    void Load();
    void LateInit(void *il2cpp_class_from_il2cpp_type_addr);

    void *GetIl2CppMethod(const char *methodName);

    extern void *BNM_il2cpp_init_origin;
#if UNITY_VER >= 192
    extern int (*old_BNM_il2cpp_init)(const char *);
    int BNM_il2cpp_init(const char *domain_name);
#else
    // il2cpp_init returned void before Unity 2019.2
    extern void (*old_BNM_il2cpp_init)(const char *);
    void BNM_il2cpp_init(const char *domain_name);
#endif

    extern void *BNM_Class$$FromIl2CppType_origin;
    extern IL2CPP::Il2CppClass *(*old_BNM_Class$$FromIl2CppType)(IL2CPP::Il2CppReflectionType*);
    IL2CPP::Il2CppClass *BNM_Class$$FromIl2CppType(IL2CPP::Il2CppReflectionType *type);

    void SetupBNM();

    void LoadDefaults();

#ifdef BNM_COROUTINE
    void SetupCoroutine();
    void LoadCoroutine();
#endif

    IL2CPP::Il2CppImage *TryGetImage(const std::string_view &_name);
    IL2CPP::Il2CppClass *TryGetClassInImage(const IL2CPP::Il2CppImage *image, const std::string_view &_namespace, const std::string_view &_name);

    Class TryMakeGenericClass(Class genericType, const std::vector<CompileTimeClass> &templateTypes);
    MethodBase TryMakeGenericMethod(const MethodBase &genericMethod, const std::vector<CompileTimeClass> &templateTypes);
    Class GetPointer(Class target);
    Class GetReference(Class target);
    // A class that failed to init keeps sentinel counts ((uint16_t)-1 = 65535) and a null
    // array; looping with the raw count would cause a massive over-read. Returns the safe count.
    inline uint16_t SafeCount(uint16_t count, const void *arr) {
        return (count == (uint16_t)-1 || !arr) ? 0 : count;
    }

    template <class CompareMethod>
    IL2CPP::MethodInfo *IterateMethods(Class target, CompareMethod compare) {
        auto curClass = target._data;
        do {
            auto count = SafeCount(curClass->method_count, curClass->methods);
            for (uint16_t i = 0; i < count; ++i) {
                auto method = curClass->methods[i];
                if (compare((IL2CPP::MethodInfo *)method)) return (IL2CPP::MethodInfo *) method;
            }
            curClass = curClass->parent;
        } while (curClass);
        return nullptr;
    }

#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    extern std::shared_mutex loadingMutex;
#endif

#ifdef BNM_CLASSES_MANAGEMENT
    //! \internal
    namespace ClassesManagement {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
        extern std::shared_mutex classesFindAccessMutex;
#endif
        // A list with all the classes that BNM should create/modify
        extern std::vector<MANAGEMENT_STRUCTURES::CustomClass *> *classesManagementVector;

        extern IL2CPP::Il2CppClass *(*old_Class$$FromIl2CppType)(IL2CPP::Il2CppType *type);
        IL2CPP::Il2CppClass *Class$$FromIl2CppType(IL2CPP::Il2CppType *type);

        extern IL2CPP::Il2CppClass *(*old_Type$$GetClassOrElementClass)(IL2CPP::Il2CppType *type);
        IL2CPP::Il2CppClass *Type$$GetClassOrElementClass(IL2CPP::Il2CppType *type);

        extern IL2CPP::Il2CppClass *(*old_Class$$FromName)(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name);
        IL2CPP::Il2CppClass *Class$$FromName(IL2CPP::Il2CppImage *image, const char *namespaze, const char *name);

#if UNITY_VER <= 174
        // They are required because in Unity 2017 and below, in images and assemblies, they are stored by numbers

        extern IL2CPP::Il2CppImage *(*old_GetImageFromIndex)(IL2CPP::ImageIndex index);
        IL2CPP::Il2CppImage *new_GetImageFromIndex(IL2CPP::ImageIndex index);

        IL2CPP::Il2CppAssembly *Assembly$$Load(const char *name);
#endif

        void ProcessCustomClasses();

        //! \internal
        // Structure for quick search classes by their images
        extern struct BNMClassesMap {
            inline BNMClassesMap() {
                _map = new (BNM_malloc(sizeof(std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>>))) std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>>();
            }
            inline void AddClass(const IL2CPP::Il2CppImage *image, IL2CPP::Il2CppClass *cls) {
                return AddClass((BNM_PTR)image, cls);
            }

            inline void AddClass(BNM_PTR image, IL2CPP::Il2CppClass *cls) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::unique_lock lock(classesFindAccessMutex);
#endif
                (*_map)[image].emplace_back(cls);
            }

            template <class IterateMethod>
            inline void ForEachByImage(const IL2CPP::Il2CppImage *image, IterateMethod func) {
                return ForEachByImage((BNM_PTR)image, func);
            }

            template <class IterateMethod>
            inline void ForEachByImage(BNM_PTR image, IterateMethod func) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                for (auto item : (*_map)[image]) if (func(item)) break;
            }

            template <class IterateMethod>
            inline void ForEach(IterateMethod func) {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
                std::shared_lock lock(classesFindAccessMutex);
#endif
                for (auto &[img, classes] : (*_map)) if (func((IL2CPP::Il2CppImage *)img, classes)) break;
            }
        private:
            std::map<BNM_PTR, std::vector<IL2CPP::Il2CppClass *>> *_map{};
        } bnmClassesMap;
    }

#endif

    inline bool CompareImageName(IL2CPP::Il2CppImage *image, const std::string_view &name) {
        if (!image || !image->name) return false;
        bool value = image->name == name;
#if UNITY_VER >= 171
        value = value || image->nameNoExt == name;
#else
        if (!value) {
            auto nameWithoutDll = std::string_view(image->name);
            nameWithoutDll.remove_suffix(4);
            value = nameWithoutDll == name;
        }
#endif
        return value;
    }
}
/// @endcond