#include <jni.h>
#include <android/log.h>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <utility>

#include "Include/autodetect/unity_version.hpp"
#include "Include/autodetect/il2cpp_runtime.hpp"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "BNMExplorer", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "BNMExplorer", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "BNMExplorer", __VA_ARGS__)

extern "C" JNIEXPORT jint JNICALL bnm_init_v56(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v171(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v172(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v181(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v182(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v183(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v191(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v193(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v194(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v201(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v202(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v203(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v211_pre24(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v211_24(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v212(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v213(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v221(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v222(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v231(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v232(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v600(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v605(JavaVM*, void*);
extern "C" JNIEXPORT jint JNICALL bnm_init_v606(JavaVM*, void*);

using init_function = jint (*)(JavaVM*, void*);

namespace {

    std::atomic<bool> initialization_started{false};

    constexpr auto UNITY_MODULE_WAIT_LIMIT = std::chrono::seconds(30);
    constexpr auto UNITY_MODULE_POLL_INTERVAL = std::chrono::milliseconds(100);
    constexpr auto NAME_MAP_RETRY_LIMIT = std::chrono::milliseconds(3000);
    constexpr auto NAME_MAP_RETRY_INTERVAL = std::chrono::milliseconds(250);

    static init_function select_initializer(const unity_autodetect::unity_version& version) {
        using unity_autodetect::variant_name_for_version;
        const std::string variant = variant_name_for_version(version);
        if (variant == "v56") return bnm_init_v56;
        if (variant == "v171") return bnm_init_v171;
        if (variant == "v172") return bnm_init_v172;
        if (variant == "v181") return bnm_init_v181;
        if (variant == "v182") return bnm_init_v182;
        if (variant == "v183") return bnm_init_v183;
        if (variant == "v191") return bnm_init_v191;
        if (variant == "v193") return bnm_init_v193;
        if (variant == "v194") return bnm_init_v194;
        if (variant == "v201") return bnm_init_v201;
        if (variant == "v202") return bnm_init_v202;
        if (variant == "v203") return bnm_init_v203;
        if (variant == "v211_pre24") return bnm_init_v211_pre24;
        if (variant == "v211_24") return bnm_init_v211_24;
        if (variant == "v212") return bnm_init_v212;
        if (variant == "v213") return bnm_init_v213;
        if (variant == "v221") return bnm_init_v221;
        if (variant == "v222") return bnm_init_v222;
        if (variant == "v231") return bnm_init_v231;
        if (variant == "v232") return bnm_init_v232;
        if (variant == "v600") return bnm_init_v600;
        if (variant == "v605") return bnm_init_v605;
        if (variant == "v606") return bnm_init_v606;
        return nullptr;
    }

    static bool validate_runtime_name_map(int unity_header_version, std::vector<std::string>& missing_names) {
        static constexpr const char* required_names[] = {
                "il2cpp_init",
                "il2cpp_class_from_il2cpp_type",
                "il2cpp_array_new_specific",
                "il2cpp_class_from_type",
                "il2cpp_type_get_class_or_element_class",
                "il2cpp_get_corlib",
                "il2cpp_class_from_name",
                "il2cpp_assembly_get_image",
                "il2cpp_method_get_param_name",
                "il2cpp_array_class_get",
                "il2cpp_type_get_object",
                "il2cpp_object_new",
                "il2cpp_value_box",
                "il2cpp_array_new",
                "il2cpp_field_static_get_value",
                "il2cpp_field_static_set_value",
                "il2cpp_string_new",
                "il2cpp_resolve_icall",
                "il2cpp_runtime_invoke",
                "il2cpp_domain_get",
                "il2cpp_domain_get_assemblies",
                "il2cpp_thread_current",
                "il2cpp_thread_attach",
                "il2cpp_thread_detach",
                "il2cpp_gchandle_new",
                "il2cpp_gchandle_free",
                "il2cpp_gchandle_get_target",
                "il2cpp_gchandle_new_weakref",
                "il2cpp_runtime_class_init",
                "il2cpp_object_get_class",
                "il2cpp_class_get_type",
                "il2cpp_class_get_name",
                "il2cpp_class_get_namespace",
                "il2cpp_class_get_image",
                "il2cpp_class_get_parent",
                "il2cpp_class_is_valuetype",
                "il2cpp_class_is_enum",
                "il2cpp_class_get_method_from_name",
                "il2cpp_class_get_field_from_name",
                "il2cpp_string_new_len",
                "il2cpp_string_new_utf16"
        };

        missing_names.clear();
        for (const char* name : required_names) {
            std::string runtime_name;
            if (!il2cpp_autodetect::find_runtime_name(name, runtime_name)) missing_names.emplace_back(name);
        }

        if (unity_header_version <= 174) {
            std::string runtime_name;
            if (!il2cpp_autodetect::find_runtime_name("il2cpp_domain_assembly_open", runtime_name)) {
                missing_names.emplace_back("il2cpp_domain_assembly_open");
            }
        }

        return missing_names.empty();
    }

    static void log_scan_result(const il2cpp_autodetect::runtime_fingerprint& result) {
        for (const std::string& line : result.diagnostics) {
            LOGI("IL2CPP resolver diag: %s", line.c_str());
        }

        if (result.table_resolved) {
            LOGI("IL2CPP name map resolved: module=%s base=%p first_reference=%p delta=%lld stride=0x%zx names=%zu/%zu literals=%zu references=%zu conflicts=%zu",
                 result.module_path.c_str(),
                 reinterpret_cast<void*>(result.module_base),
                 reinterpret_cast<void*>(result.first_reference),
                 static_cast<long long>(result.name_delta),
                 result.stride,
                 result.resolved_names,
                 result.expected_names,
                 result.literal_count,
                 result.reference_count,
                 result.conflict_count);
            std::string mapped_domain_assemblies;
            if (il2cpp_autodetect::find_runtime_name("il2cpp_domain_get_assemblies", mapped_domain_assemblies)) {
                LOGI("IL2CPP export mapping: il2cpp_domain_get_assemblies -> %s", mapped_domain_assemblies.c_str());
            } else {
                LOGW("IL2CPP export mapping does not contain il2cpp_domain_get_assemblies");
            }
            return;
        }

        if (!result.module_found) {
            LOGW("libunity.so was not visible to the runtime module enumerator.");
        } else if (!result.executable_segments_found) {
            LOGW("libunity.so is loaded, but no readable executable mapping was available for scanning.");
        } else if (result.ambiguous) {
            LOGW("IL2CPP name offsets are ambiguous: several deltas have the same score; refusing to guess.");
        } else if (result.references_found) {
            LOGW("IL2CPP string references found, but names did not resolve: literals=%zu references=%zu delta=%lld score=%zu resolved=%zu/%zu.",
                 result.literal_count,
                 result.reference_count,
                 static_cast<long long>(result.name_delta),
                 result.delta_score,
                 result.resolved_names,
                 result.expected_names);
        } else {
            LOGW("libunity.so executable mappings are present, but no code references to 'il2cpp: function' strings were found: literals=%zu.",
                 result.literal_count);
        }
    }

    static void run_initialization(JavaVM* vm, init_function initializer, std::string variant, std::string engine_version, int unity_header_version) {
        const auto module_deadline = std::chrono::steady_clock::now() + UNITY_MODULE_WAIT_LIMIT;
        bool module_seen = false;

        while (std::chrono::steady_clock::now() < module_deadline) {
            if (il2cpp_autodetect::is_unity_module_loaded()) {
                module_seen = true;
                break;
            }
            std::this_thread::sleep_for(UNITY_MODULE_POLL_INTERVAL);
        }

        if (!module_seen) {
            LOGE("Timed out waiting for libunity.so; BNM initialization was skipped to avoid loading with unresolved IL2CPP names.");
            return;
        }

        LOGI("Detected libunity.so after deferred startup; scanning its executable mappings before BNM initialization.");
        const auto scan_deadline = std::chrono::steady_clock::now() + NAME_MAP_RETRY_LIMIT;
        il2cpp_autodetect::runtime_fingerprint last_result;
        bool map_ready = false;

        do {
            const auto scan_start = std::chrono::steady_clock::now();
            il2cpp_autodetect::runtime_fingerprint current_result;
            const bool resolved = il2cpp_autodetect::detect_il2cpp_runtime(current_result);
            const auto scan_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - scan_start).count();
            LOGI("IL2CPP scan pass completed in %lld ms: module=%d executable=%d references=%d resolved=%d literals=%zu refs=%zu names=%zu/%zu.",
                 static_cast<long long>(scan_elapsed),
                 current_result.module_found ? 1 : 0,
                 current_result.executable_segments_found ? 1 : 0,
                 current_result.references_found ? 1 : 0,
                 current_result.table_resolved ? 1 : 0,
                 current_result.literal_count,
                 current_result.reference_count,
                 current_result.resolved_names,
                 current_result.expected_names);
            last_result = std::move(current_result);
            if (resolved) {
                map_ready = true;
                break;
            }
            if (std::chrono::steady_clock::now() >= scan_deadline) break;
            std::this_thread::sleep_for(NAME_MAP_RETRY_INTERVAL);
        } while (std::chrono::steady_clock::now() < scan_deadline);

        log_scan_result(last_result);
        if (!map_ready) {
            LOGE("BNM adapter initialization was skipped: the runtime name map is unresolved. The game uses renamed IL2CPP exports, so the canonical-name fallback is unsafe.");
            return;
        }

        std::vector<std::string> missing_names;
        if (!validate_runtime_name_map(unity_header_version, missing_names)) {
            std::ostringstream missing;
            for (std::size_t index = 0; index < missing_names.size(); ++index) {
                if (index) missing << ", ";
                missing << missing_names[index];
            }
            LOGE("BNM adapter initialization was skipped: the resolved map is missing required BNM APIs: %s", missing.str().c_str());
            return;
        }

        LOGI("Runtime name map is ready and contains the required BNM APIs; installing the BNM MethodFinder.");
        JNIEnv* env = nullptr;
        const jint attach_result = vm ? vm->AttachCurrentThread(&env, nullptr) : JNI_ERR;
        if (attach_result != JNI_OK || !env) {
            LOGE("Could not attach the deferred BNM initialization thread to the JVM: %d", attach_result);
            return;
        }

        LOGI("Starting BNM runtime adapter: %s for Unity %s", variant.c_str(), engine_version.c_str());
        const jint init_result = initializer(vm, nullptr);
        if (init_result == JNI_ERR) LOGE("BNM runtime adapter initialization returned JNI_ERR.");
        else LOGI("BNM runtime adapter entry completed; this does not yet mean BNM has finished loading.");

        vm->DetachCurrentThread();
    }
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    if (!vm) return JNI_ERR;
    const char* configured_path = std::getenv("BNM_UNITY_DATA_PATH");
    std::ifstream command_line("/proc/self/cmdline", std::ios::binary);
    std::string package_name;
    if (command_line) std::getline(command_line, package_name, '\0');
    const std::size_t process_suffix = package_name.find(':');
    if (process_suffix != std::string::npos) package_name.resize(process_suffix);

    unity_autodetect::unity_version version;
    if (!unity_autodetect::detect_unity_version(package_name, configured_path ? configured_path : "", version)) {
        LOGE("Unity version was not found. BNM initialization was skipped.");
        return JNI_VERSION_1_6;
    }

    LOGI("Unity engine=%s player=%s header=%s variant=%s source=%s kind=%s",
         version.engine_version.c_str(),
         version.player_version.c_str(),
         unity_autodetect::header_name_for_version(version.bnm_unity_ver).c_str(),
         unity_autodetect::variant_name_for_version(version).c_str(),
         version.source_path.c_str(),
         version.source_kind.c_str());

    if (!version.supported_header) {
        LOGE("Unity version %s has no matching IL2CPP header in this build.", version.engine_version.c_str());
        return JNI_VERSION_1_6;
    }

    const init_function initializer = select_initializer(version);
    if (!initializer) {
        LOGE("No compiled BNM runtime adapter for Unity %s.", version.engine_version.c_str());
        return JNI_VERSION_1_6;
    }

    if (initialization_started.exchange(true)) {
        LOGW("Deferred BNM initialization was already scheduled; skipping duplicate request.");
        return JNI_VERSION_1_6;
    }

    const std::string variant = unity_autodetect::variant_name_for_version(version);
    const std::string engine_version = version.engine_version;
    LOGI("Deferring BNM initialization until libunity.so is loaded; JNI_OnLoad ran before UnityPlayer native startup.");

    try {
        std::thread(run_initialization, vm, initializer, variant, engine_version, version.bnm_unity_ver).detach();
    } catch (const std::exception& error) {
        LOGE("Failed to start deferred BNM initialization thread: %s", error.what());
        initialization_started.store(false);
        return JNI_VERSION_1_6;
    }

    static_cast<void>(reserved);
    return JNI_VERSION_1_6;
}