#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/UserSettings/Il2CppMethodNames.hpp>
#include <BNM/Loading.hpp>
#include <BNM/Field.hpp>

#include <Internals.hpp>
#include <AssemblerUtils.hpp>
#include <cstdint>
#include <cstring>

using namespace BNM;

void Internal::Load() {
#ifdef BNM_ALLOW_MULTI_THREADING_SYNC
    std::unique_lock lock(loadingMutex);
#endif

    // Load BNM
    SetupBNM();

    BNM::Internal::LoadDefaults();

#ifdef BNM_CLASSES_MANAGEMENT

#ifdef BNM_COROUTINE
    BNM::Internal::SetupCoroutine();
#endif

    BNM::Internal::ClassesManagement::ProcessCustomClasses();

#ifdef BNM_COROUTINE
    BNM::Internal::LoadCoroutine();
#endif

#endif
    states.state = true;

    // Call all events after loading il2cpp
    if (!onIl2CppLoaded.IsEmpty()) {
        auto current = onIl2CppLoaded.lastElement->next;
        do {
            if (current->value) current->value();

            current = current->next;
        } while (current != onIl2CppLoaded.lastElement->next);
    }
}

void *Internal::GetIl2CppMethod(const char *methodName) {
    return currentFinderMethod(methodName, currentFinderData);
}

void Loading::AllowLateInitHook() {
    Internal::states.lateInitAllowed = true;
}

static bool CheckHandle(void *handle) {
    if (!handle) return false;
    void *init = BNM_dlsym(handle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_init));
    if (!init) return false;

    Internal::BNM_il2cpp_init_origin = ::BasicHook(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::states.lateInitAllowed) Internal::LateInit(BNM_dlsym(handle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type)));

    Internal::il2cppLibraryHandle = handle;
    return true;
}

#if defined(__ARM_ARCH_7A__)
#    define CURRENT_ARCH "armeabi-v7a"
#    define CURRENT_ARCH_SPLIT "armeabi_v7a"
#elif defined(__aarch64__)
#    define CURRENT_ARCH "arm64-v8a"
#    define CURRENT_ARCH_SPLIT "arm64_v8a"
#elif defined(__i386__)
#    define CURRENT_ARCH "x86"
#    define CURRENT_ARCH_SPLIT "x86"
#elif defined(__x86_64__)
#    define CURRENT_ARCH "x86_64"
#    define CURRENT_ARCH_SPLIT "x86_64"
#elif defined(__riscv)
#    define CURRENT_ARCH "riscv64"
#    define CURRENT_ARCH_SPLIT "riscv64"
#endif

bool Loading::TryLoadByJNI(JNIEnv *env, jobject context) {
    bool result = false;
#ifdef __ANDROID__
    if (!env || Internal::il2cppLibraryHandle || Internal::states.state) return result;

    if (context == nullptr) {
        jclass activityThread = env->FindClass(BNM_OBFUSCATE_TMP("android/app/ActivityThread"));
        if (!activityThread) return false;
        auto currentActivityThread = env->CallStaticObjectMethod(activityThread, env->GetStaticMethodID(activityThread, BNM_OBFUSCATE_TMP("currentActivityThread"), BNM_OBFUSCATE_TMP("()Landroid/app/ActivityThread;")));
        if (!currentActivityThread) {
            env->DeleteLocalRef(activityThread);
            return false;
        }
        context = env->CallObjectMethod(currentActivityThread, env->GetMethodID(activityThread, BNM_OBFUSCATE_TMP("getApplication"), BNM_OBFUSCATE_TMP("()Landroid/app/Application;")));
        env->DeleteLocalRef(currentActivityThread);
        env->DeleteLocalRef(activityThread);
        if (!context) return false;
    }

    auto applicationInfo = env->CallObjectMethod(context, env->GetMethodID(env->GetObjectClass(context), BNM_OBFUSCATE_TMP("getApplicationInfo"), BNM_OBFUSCATE_TMP("()Landroid/content/pm/ApplicationInfo;")));
    if (!applicationInfo) return false;
    auto applicationInfoClass = env->GetObjectClass(applicationInfo);

    auto flags = env->GetIntField(applicationInfo, env->GetFieldID(applicationInfoClass, BNM_OBFUSCATE_TMP("flags"), BNM_OBFUSCATE_TMP("I")));
    bool isLibrariesExtracted = (flags & 0x10000000) == 0x10000000; // ApplicationInfo.FLAG_EXTRACT_NATIVE_LIBS

    auto jDir = (jstring) env->GetObjectField(applicationInfo, env->GetFieldID(applicationInfoClass, isLibrariesExtracted ? BNM_OBFUSCATE_TMP("nativeLibraryDir") : BNM_OBFUSCATE_TMP("sourceDir"), BNM_OBFUSCATE_TMP("Ljava/lang/String;")));
    if (!jDir) {
        env->DeleteLocalRef(applicationInfo);
        env->DeleteLocalRef(applicationInfoClass);
        return false;
    }

    auto cDir = std::string_view(env->GetStringUTFChars(jDir, nullptr));
    env->DeleteLocalRef(applicationInfo); env->DeleteLocalRef(applicationInfoClass);

    // isLibrariesExtracted = true:   Full path to library /data/app/.../package name-.../lib/architecture/libil2cpp.so
    // isLibrariesExtracted = false:  Full path to library in base.apk /data/app/.../package name-.../base.apk!/lib/architecture/libil2cpp.so
    std::string file = std::string(cDir) + (isLibrariesExtracted ? BNM_OBFUSCATE_TMP("/libil2cpp.so") : BNM_OBFUSCATE_TMP("!/lib/" CURRENT_ARCH "/libil2cpp.so"));

    // Try to load il2cpp using this path
    auto handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) {
        BNM_LOG_ERR_IF(isLibrariesExtracted, DBG_BNM_MSG_TryLoadByJNI_Fail);
    } else goto FINISH;
    if (isLibrariesExtracted) goto FINISH;
    file.clear();

    // Full path to library in split_config.architecture.apk /data/app/.../package name-.../split_config.architecture.apk!/lib/architecture/libil2cpp.so
    file = std::string(cDir).substr(0, cDir.length() - 8) + BNM_OBFUSCATE_TMP("split_config." CURRENT_ARCH_SPLIT ".apk!/lib/" CURRENT_ARCH "/libil2cpp.so");
    handle = BNM_dlopen(file.c_str(), RTLD_LAZY);
    if (!(result = CheckHandle(handle))) BNM_LOG_ERR(DBG_BNM_MSG_TryLoadByJNI_Fail);

    FINISH:
    env->ReleaseStringUTFChars(jDir, cDir.data()); env->DeleteLocalRef(jDir);
#endif
    return result;
}

bool Loading::TryLoadByDlfcnHandle(void *handle) {
    return CheckHandle(handle);
}

void Loading::SetMethodFinder(BNM::Loading::MethodFinder finderMethod, void *userData) {
    Internal::currentFinderMethod = finderMethod;
    Internal::currentFinderData = userData;
}

bool Loading::TryLoadByUsersFinder() {
    auto init = Internal::currentFinderMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_init), Internal::currentFinderData);
    if (!init) return false;

    Internal::BNM_il2cpp_init_origin = ::BasicHook(init, Internal::BNM_il2cpp_init, Internal::old_BNM_il2cpp_init);

    if (Internal::states.lateInitAllowed) Internal::LateInit(Internal::currentFinderMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type), Internal::currentFinderData));

    return true;
}

void Loading::TrySetupByUsersFinder() {
    return Internal::Load();
}

void Internal::LateInit(void *il2cpp_class_from_il2cpp_type_addr) {
    if (!il2cpp_class_from_il2cpp_type_addr) return;

#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    // x86 has one add-on call at start
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::FromIl2CppType
    // Path:
    // il2cpp_class_from_il2cpp_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_il2cpp_type = AssemblerUtils::FindNextJump((BNM_PTR) il2cpp_class_from_il2cpp_type_addr, count);

    Internal::BNM_Class$$FromIl2CppType_origin = ::BasicHook(from_il2cpp_type, Internal::BNM_Class$$FromIl2CppType, Internal::old_BNM_Class$$FromIl2CppType);
}

static void EmptyMethod() {}

#ifdef BNM_DEBUG
static void *OffsetInLib(void *offsetInMemory) {
    if (offsetInMemory == nullptr) return nullptr;
    BNM_Dl_info info; BNM_dladdr(offsetInMemory, &info);
    return (void *) ((BNM_PTR) offsetInMemory - (BNM_PTR) info.dli_fbase);
}

void *Utils::OffsetInLib(void *offsetInMemory) {
    return ::OffsetInLib(offsetInMemory);
}
#endif

void Internal::SetupBNM() {
#if defined(__ARM_ARCH_7A__) || defined(__aarch64__)
    const uint8_t count = 1;
#elif defined(__i386__) || defined(__x86_64__)
    // x86 has one add-on call at start
    const uint8_t count = 2;
#endif

    //! il2cpp::vm::Class::Init
    // Path:
    // il2cpp_array_new_specific ->
    // il2cpp::vm::Array::NewSpecific ->
    // il2cpp::vm::Class::Init
    il2cppMethods.Class$$Init = (decltype(il2cppMethods.Class$$Init)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_array_new_specific)), count), count);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Class_Init, OffsetInLib((void *)il2cppMethods.Class$$Init));
    // Class$$Init is required by TryInit() across the whole public API; without it every call jumps to address 0
    if (!il2cppMethods.Class$$Init) {
        BNM_LOG_ERR(DBG_BNM_MSG_SetupBNM_Class_Init_Failed);
        return;
    }


#define INIT_IL2CPP_API(name) il2cppMethods.name = (decltype(il2cppMethods.name)) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_##name)); BNM_LOG_DEBUG("[INIT_IL2CPP_API]: " #name " (" BNM_IL2CPP_API_##name ") in lib %p", OffsetInLib((void *)il2cppMethods.name))

#if UNITY_VER >= 183
    INIT_IL2CPP_API(il2cpp_image_get_class);
#endif
    INIT_IL2CPP_API(il2cpp_get_corlib);
    INIT_IL2CPP_API(il2cpp_class_from_name);
    INIT_IL2CPP_API(il2cpp_assembly_get_image);
    INIT_IL2CPP_API(il2cpp_method_get_param_name);
    INIT_IL2CPP_API(il2cpp_class_from_il2cpp_type);
    INIT_IL2CPP_API(il2cpp_array_class_get);
    INIT_IL2CPP_API(il2cpp_type_get_object);
    INIT_IL2CPP_API(il2cpp_object_new);
    INIT_IL2CPP_API(il2cpp_value_box);
    INIT_IL2CPP_API(il2cpp_array_new);
    INIT_IL2CPP_API(il2cpp_field_static_get_value);
    INIT_IL2CPP_API(il2cpp_field_static_set_value);
    INIT_IL2CPP_API(il2cpp_string_new);
    INIT_IL2CPP_API(il2cpp_resolve_icall);
    INIT_IL2CPP_API(il2cpp_runtime_invoke);
    INIT_IL2CPP_API(il2cpp_domain_get);
    INIT_IL2CPP_API(il2cpp_domain_get_assemblies);
    INIT_IL2CPP_API(il2cpp_thread_current);
    INIT_IL2CPP_API(il2cpp_thread_attach);
    INIT_IL2CPP_API(il2cpp_thread_detach);
#if UNITY_VER >= 212
    INIT_IL2CPP_API(il2cpp_gc_alloc_fixed);
    INIT_IL2CPP_API(il2cpp_gc_free_fixed);
#endif
    INIT_IL2CPP_API(il2cpp_gchandle_new);
    INIT_IL2CPP_API(il2cpp_gchandle_free);
    INIT_IL2CPP_API(il2cpp_gchandle_get_target);
    INIT_IL2CPP_API(il2cpp_gchandle_new_weakref);
#if UNITY_VER >= 193
    INIT_IL2CPP_API(il2cpp_gchandle_foreach_get_target);
#endif
    INIT_IL2CPP_API(il2cpp_runtime_class_init);
    INIT_IL2CPP_API(il2cpp_object_get_class);
    INIT_IL2CPP_API(il2cpp_class_get_type);
    INIT_IL2CPP_API(il2cpp_class_get_name);
    INIT_IL2CPP_API(il2cpp_class_get_namespace);
    INIT_IL2CPP_API(il2cpp_class_get_image);
    INIT_IL2CPP_API(il2cpp_class_get_parent);
    INIT_IL2CPP_API(il2cpp_class_is_valuetype);
    INIT_IL2CPP_API(il2cpp_class_is_enum);
    INIT_IL2CPP_API(il2cpp_class_get_method_from_name);
    INIT_IL2CPP_API(il2cpp_class_get_field_from_name);
#if UNITY_VER >= 191
    INIT_IL2CPP_API(il2cpp_class_get_userdata_offset);
    INIT_IL2CPP_API(il2cpp_class_set_userdata);
#endif
    INIT_IL2CPP_API(il2cpp_string_new_len);
    INIT_IL2CPP_API(il2cpp_string_new_utf16);

#undef INIT_IL2CPP_API

    //! il2cpp::vm::Image::GetTypes
#if UNITY_VER >= 183
    if (il2cppMethods.il2cpp_image_get_class == nullptr)
#endif
    {
        if (!il2cppMethods.il2cpp_get_corlib || !il2cppMethods.il2cpp_class_from_name) return;
        auto assemblyClass = il2cppMethods.il2cpp_class_from_name(il2cppMethods.il2cpp_get_corlib(), BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("Assembly"));
        BNM_PTR GetTypesAdr = Class(assemblyClass).GetMethod(BNM_OBFUSCATE_TMP("GetTypes"), 1).GetOffset();

#if UNITY_VER >= 211
        const int sCount = count;
#elif UNITY_VER > 174
        const int sCount = count + 1;
#else
        const int sCount = count + 2;
#endif
        // Path:
        // System.Reflection.Assembly.GetTypes(bool) ->
        // il2cpp::icalls::mscorlib::System::Reflection::Assembly::GetTypes ->
        // il2cpp::icalls::mscorlib::System::Module::InternalGetTypes ->
        // il2cpp::vm::Image::GetTypes
        il2cppMethods.orig_Image$$GetTypes = (decltype(il2cppMethods.orig_Image$$GetTypes)) AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump(GetTypesAdr, count), sCount), count);

        BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Image_GetTypes, OffsetInLib((void *)il2cppMethods.orig_Image$$GetTypes));
    }
#if UNITY_VER >= 183
    else BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_image_get_class_exists);
#endif

#ifdef BNM_CLASSES_MANAGEMENT

    //! il2cpp::vm::Class::FromIl2CppType
    // Path:
    // il2cpp_class_from_type ->
    // il2cpp::vm::Class::FromIl2CppType
    auto from_type_adr = AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_class_from_type)), count);
    ::BasicHook(from_type_adr, ClassesManagement::Class$$FromIl2CppType, ClassesManagement::old_Class$$FromIl2CppType);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Class_FromIl2CppType, OffsetInLib((void *)from_type_adr));


    //! il2cpp::vm::Type::GetClassOrElementClass
    // Path:
    // il2cpp_type_get_class_or_element_class ->
    // il2cpp::vm::Type::GetClassOrElementClass
    auto type_get_class_adr = AssemblerUtils::FindNextJump((BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_type_get_class_or_element_class)), count);
    ::BasicHook(type_get_class_adr, ClassesManagement::Type$$GetClassOrElementClass, ClassesManagement::old_Type$$GetClassOrElementClass);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Type_GetClassOrElementClass, OffsetInLib((void *)type_get_class_adr));

    //! il2cpp::vm::Image::ClassFromName
    // Path:
    // il2cpp_class_from_name ->
    // il2cpp::vm::Class::FromName ->
    // il2cpp::vm::Image::ClassFromName
    auto from_name_adr = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_class_from_name, count), count);
    ::BasicHook(from_name_adr, ClassesManagement::Class$$FromName, ClassesManagement::old_Class$$FromName);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Image_FromName, OffsetInLib((void *)from_name_adr));
#if UNITY_VER <= 174

    //! il2cpp::vm::MetadataCache::GetImageFromIndex
    // Path:
    // il2cpp_assembly_get_image ->
    // il2cpp::vm::Assembly::GetImage ->
    // il2cpp::vm::MetadataCache::GetImageFromIndex
    auto GetImageFromIndexOffset = AssemblerUtils::FindNextJump(AssemblerUtils::FindNextJump((BNM_PTR) il2cppMethods.il2cpp_assembly_get_image, count), count);
    ::BasicHook(GetImageFromIndexOffset, ClassesManagement::new_GetImageFromIndex, ClassesManagement::old_GetImageFromIndex);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_MetadataCache_GetImageFromIndex, OffsetInLib((void *)GetImageFromIndexOffset));

    //! il2cpp::vm::Assembly::Load
    // Path:
    // il2cpp_domain_assembly_open ->
    // il2cpp::vm::Assembly::Load
    BNM_PTR AssemblyLoadOffset = AssemblerUtils::FindNextJump((BNM_PTR) BNM_dlsym(il2cppLibraryHandle, BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_domain_assembly_open)), count);
    ::BasicHook(AssemblyLoadOffset, ClassesManagement::Assembly$$Load, nullptr);
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Assembly_Load, OffsetInLib((void *)AssemblyLoadOffset));

#endif
#endif

    //! il2cpp::vm::Assembly::GetAllAssemblies
    // Path:
    // il2cpp_domain_get_assemblies ->
    // il2cpp::vm::Assembly::GetAllAssemblies
    auto adr = (BNM_PTR) GetIl2CppMethod(BNM_OBFUSCATE_TMP(BNM_IL2CPP_API_il2cpp_domain_get_assemblies));
    if (adr) {
        auto jumpTarget = AssemblerUtils::FindNextJump(adr, count);
        if (jumpTarget) {
            il2cppMethods.Assembly$$GetAllAssemblies = (std::vector<IL2CPP::Il2CppAssembly *> *(*)()) jumpTarget;
        }
    }
    BNM_LOG_DEBUG(DBG_BNM_MSG_SetupBNM_Assembly_GetAllAssemblies, OffsetInLib((void *)il2cppMethods.Assembly$$GetAllAssemblies));

    if (!il2cppMethods.il2cpp_get_corlib) return;
    auto mscorlib = il2cppMethods.il2cpp_get_corlib();

    // Get MakeGenericMethod_impl. Depending on the version of Unity, it may be in different classes.
    auto runtimeMethodInfoClassPtr = TryGetClassInImage(mscorlib, BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("RuntimeMethodInfo"));
    if (runtimeMethodInfoClassPtr) {
        Internal::il2cppMethods.Class$$Init(runtimeMethodInfoClassPtr);
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = BNM::MethodBase(IterateMethods(runtimeMethodInfoClassPtr, [methodName = BNM_OBFUSCATE_TMP("MakeGenericMethod_impl")](const MethodBase &methodBase) {
            return !strcmp(methodBase._data->name, methodName);
        }));
    }
    if (!vmData.RuntimeMethodInfo$$MakeGenericMethod_impl.IsValid())
        vmData.RuntimeMethodInfo$$MakeGenericMethod_impl = Class(BNM_OBFUSCATE_TMP("System.Reflection"), BNM_OBFUSCATE_TMP("MonoMethod"), mscorlib).GetMethod(BNM_OBFUSCATE_TMP("MakeGenericMethod_impl"));

    auto runtimeTypeClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("RuntimeType"), mscorlib);
    auto stringClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("String"), mscorlib);
    auto interlockedClass = Class(BNM_OBFUSCATE_TMP("System.Threading"), BNM_OBFUSCATE_TMP("Interlocked"), mscorlib);
    auto objectClass = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("Object"), mscorlib);
    if (objectClass._data) {
        for (uint16_t slot = 0; slot < objectClass._data->vtable_count; slot++) {
            const BNM::IL2CPP::MethodInfo* vMethod = objectClass._data->vtable[slot].method;
            if (!vMethod || !vMethod->name) continue;
            if (strcmp(vMethod->name, BNM_OBFUSCATE_TMP("Finalize")) != 0) continue;
            finalizerSlot = slot;
            break;
        }
    }

    auto UnityEngineCoreModule = Image(BNM_OBFUSCATE_TMP("UnityEngine.CoreModule.dll"));

    vmData.Object = objectClass;
    vmData.UnityEngine$$Object = Class(BNM_OBFUSCATE_TMP("UnityEngine"), BNM_OBFUSCATE_TMP("Object"), UnityEngineCoreModule);
    vmData.Type$$GetType = Class(BNM_OBFUSCATE_TMP("System"), BNM_OBFUSCATE_TMP("Type"), mscorlib).GetMethod(BNM_OBFUSCATE_TMP("GetType"), 1);
    vmData.Interlocked$$CompareExchange = interlockedClass.GetMethod(BNM_OBFUSCATE_TMP("CompareExchange"), {objectClass, objectClass, objectClass});
    vmData.RuntimeType$$MakeGenericType = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("MakeGenericType"), 2);
    vmData.RuntimeType$$MakePointerType = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("MakePointerType"), 1);
    vmData.RuntimeType$$make_byref_type = runtimeTypeClass.GetMethod(BNM_OBFUSCATE_TMP("make_byref_type"), 0);
    vmData.String$$Empty = stringClass.GetField(BNM_OBFUSCATE_TMP("Empty")).cast<Structures::Mono::String *>().GetPointer();

    auto listClass = vmData.System$$List = Class(BNM_OBFUSCATE_TMP("System.Collections.Generic"), BNM_OBFUSCATE_TMP("List`1"));
    auto cls = listClass._data;
    auto size = sizeof(IL2CPP::Il2CppClass) + cls->vtable_count * sizeof(IL2CPP::VirtualInvokeData);
    listClass._data = (IL2CPP::Il2CppClass *) BNM_malloc(size);
    memcpy(listClass._data, cls, size);
    listClass._data->has_finalize = 0;
    listClass._data->instance_size = sizeof(Structures::Mono::List<void*>);

    // Bypassing the creation of a static _emptyArray field because it cannot exist
    listClass._data->has_cctor = 0;
    listClass._data->cctor_started = 0;
#if UNITY_VER >= 212
    listClass._data->cctor_finished_or_no_cctor = 1;
#else
    listClass._data->cctor_finished = 1;
#endif

    auto constructor = listClass.GetMethod(Internal::constructorName, 0)._data;
    if (!constructor) return;

    auto methodCount = Internal::SafeCount(listClass._data->method_count, listClass._data->methods);
    auto newMethods = (IL2CPP::MethodInfo **) BNM_malloc(sizeof(IL2CPP::MethodInfo *) * methodCount);
    memcpy(newMethods, listClass._data->methods, sizeof(IL2CPP::MethodInfo *) * methodCount);

    auto newConstructor = (IL2CPP::MethodInfo *) BNM_malloc(sizeof(IL2CPP::MethodInfo));
    *newConstructor = *constructor;
    newConstructor->methodPointer = (decltype(newConstructor->methodPointer)) EmptyMethod;
    newConstructor->invoker_method = (decltype(newConstructor->invoker_method)) EmptyMethod;

    for (uint16_t i = 0; i < methodCount; ++i) {
        if (listClass._data->methods[i] == constructor) {
            newMethods[i] = newConstructor;
            continue;
        }
        newMethods[i] = (IL2CPP::MethodInfo *) listClass._data->methods[i];
    }
    listClass._data->methods = (const IL2CPP::MethodInfo **) newMethods;
    customListTemplateClass = listClass;
}

bool Loading::IsLoaded() noexcept {
    return Internal::states.state;
}

void *Loading::GetIl2CppLibraryHandle() noexcept {
    return Internal::il2cppLibraryHandle;
}

void Loading::AddOnLoadedEvent(void (*event)()) {
    if (!event) return;
    if (Internal::states.state) {
        event();
    } else {
        Internal::onIl2CppLoaded.Add(event);
    }
}

void Loading::ClearOnLoadedEvents() {
    Internal::onIl2CppLoaded.Clear();
}

