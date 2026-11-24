#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace il2cpp_autodetect {

    struct name_pair {
        std::string original_name;
        std::string runtime_name;
    };

    struct runtime_fingerprint {
        bool supported_architecture = false;
        bool module_found = false;
        bool executable_segments_found = false;
        bool references_found = false;
        bool table_resolved = false;
        bool ambiguous = false;
        std::string module_path;
        std::uintptr_t module_base = 0;
        std::uintptr_t first_reference = 0;
        std::int64_t name_delta = 0;
        std::size_t stride = 0;
        std::size_t literal_count = 0;
        std::size_t reference_count = 0;
        std::size_t delta_score = 0;
        std::size_t conflict_count = 0;
        std::size_t resolved_names = 0;
        std::size_t expected_names = 0;
        std::vector<name_pair> names;
        std::vector<std::string> diagnostics;
    };

    bool is_unity_module_loaded();
    bool detect_il2cpp_runtime(runtime_fingerprint& result);
    bool find_runtime_name(const char* original_name, std::string& runtime_name);
    std::size_t runtime_name_count();

}