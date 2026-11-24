#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace unity_autodetect {

struct unity_version {
    std::string player_version;
    std::string engine_version;
    std::string source_path;
    std::string source_kind;
    int major = 0;
    int minor = 0;
    int patch = 0;
    int stream_build = 0;
    int bnm_unity_ver = 0;
    int bnm_patch_ver = 0;
    char stream = 0;
    bool supported_header = false;
    bool from_unityfs = false;
};

bool parse_unity_version(const std::string& value, unity_version& result);
bool read_unity_version_file(const std::string& path, unity_version& result);
bool detect_unity_version(const std::string& package_name, const std::string& custom_path, unity_version& result);
std::string header_name_for_version(int bnm_unity_ver);
std::string variant_name_for_version(const unity_version& version);

}
