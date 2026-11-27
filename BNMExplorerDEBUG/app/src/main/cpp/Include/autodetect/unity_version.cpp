#include "unity_version.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <limits>
#include <set>
#include <string_view>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <zlib.h>

namespace unity_autodetect {

static constexpr std::size_t max_scan_entries = 4096;
static constexpr std::size_t max_scan_depth = 5;

static uint16_t read_u16_le(const uint8_t* data) {
    return static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8U);
}

static uint32_t read_u32_le(const uint8_t* data) {
    return static_cast<uint32_t>(data[0]) |
           (static_cast<uint32_t>(data[1]) << 8U) |
           (static_cast<uint32_t>(data[2]) << 16U) |
           (static_cast<uint32_t>(data[3]) << 24U);
}

static uint32_t read_u32_be(const uint8_t* data) {
    return (static_cast<uint32_t>(data[0]) << 24U) |
           (static_cast<uint32_t>(data[1]) << 16U) |
           (static_cast<uint32_t>(data[2]) << 8U) |
           static_cast<uint32_t>(data[3]);
}

static bool read_file_prefix(const std::string& path, std::vector<uint8_t>& data, std::size_t limit = 8192) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    const std::streamoff size = file.tellg();
    if (size <= 0) return false;
    const std::size_t read_size = static_cast<std::size_t>(std::min<std::streamoff>(size, static_cast<std::streamoff>(limit)));
    data.resize(read_size);
    file.seekg(0, std::ios::beg);
    return static_cast<bool>(file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size())));
}

static bool parse_number(std::string_view value, std::size_t& position, int& output) {
    if (position >= value.size() || !std::isdigit(static_cast<unsigned char>(value[position]))) return false;
    int parsed = 0;
    while (position < value.size() && std::isdigit(static_cast<unsigned char>(value[position]))) {
        const int digit = value[position] - '0';
        if (parsed > 100000000) return false;
        parsed = parsed * 10 + digit;
        ++position;
    }
    output = parsed;
    return true;
}

static int map_bnm_version(const unity_version& version) {
    const int major = version.major;
    const int minor = version.minor;
    const int patch = version.patch;

    if (major == 2017 && minor == 1) return 171;
    if (major == 2017 && minor >= 2 && minor <= 4) return 172;
    if (major == 2018 && minor == 1) return 181;
    if (major == 2018 && minor == 2) return 182;
    if (major == 2018 && minor >= 3 && minor <= 4) return 183;
    if (major == 2019 && minor >= 1 && minor <= 2) return 191;
    if (major == 2019 && minor == 3) return 193;
    if (major == 2019 && minor == 4) return 194;
    if (major == 2020 && minor == 1) return 201;
    if (major == 2020 && minor == 2) return 202;
    if (major == 2020 && minor == 3) return patch < 20 ? 202 : 203;
    if (major == 2021 && minor == 1) return 211;
    if (major == 2021 && minor == 2) return 212;
    if (major == 2021 && minor == 3) return 213;
    if (major == 2022 && minor == 1) return 221;
    if (major == 2022 && minor >= 2 && minor <= 3) return 222;
    if (major == 2023 && minor == 1) return 231;
    if (major == 2023 && minor >= 2 && minor <= 3) return 232;
    if (major == 6000 && minor <= 4) return 600;
    if (major == 6000 && minor == 5) return 605;
    if (major == 6000 && minor >= 6) return 606;
    return 0;
}

bool parse_unity_version(const std::string& value, unity_version& result) {
    std::size_t position = 0;
    unity_version parsed;
    if (!parse_number(value, position, parsed.major)) return false;
    if (position >= value.size() || value[position++] != '.') return false;
    if (!parse_number(value, position, parsed.minor)) return false;
    if (position >= value.size() || value[position++] != '.') return false;
    if (!parse_number(value, position, parsed.patch)) return false;
    if (position >= value.size()) return false;
    parsed.stream = value[position++];
    if (parsed.stream != 'a' && parsed.stream != 'b' && parsed.stream != 'f' && parsed.stream != 'p') return false;
    if (!parse_number(value, position, parsed.stream_build) || position != value.size()) return false;
    parsed.engine_version = value;
    parsed.player_version = value;
    parsed.bnm_patch_ver = parsed.patch;
    parsed.bnm_unity_ver = map_bnm_version(parsed);
    parsed.supported_header = parsed.bnm_unity_ver != 0;
    result = std::move(parsed);
    return true;
}

static bool parse_unityfs(const std::vector<uint8_t>& data, unity_version& result) {
    if (data.size() < 16 || std::memcmp(data.data(), "UnityFS\0", 8) != 0) return false;
    const uint32_t format_version = read_u32_be(data.data() + 8);
    if (format_version == 0 || format_version > 100) return false;
    std::size_t position = 12;
    auto read_string = [&](std::string& output) {
        output.clear();
        while (position < data.size() && output.size() < 128) {
            const uint8_t ch = data[position++];
            if (ch == 0) return !output.empty();
            if (ch < 0x20 || ch > 0x7e) return false;
            output.push_back(static_cast<char>(ch));
        }
        return false;
    };

    std::string player;
    std::string engine;
    if (!read_string(player) || !read_string(engine)) return false;

    unity_version parsed;
    if (!parse_unity_version(engine, parsed)) return false;
    parsed.player_version = std::move(player);
    parsed.engine_version = std::move(engine);
    parsed.from_unityfs = true;
    result = std::move(parsed);
    return true;
}

static bool find_version_string(const std::vector<uint8_t>& data, unity_version& result) {
    const std::size_t limit = std::min<std::size_t>(data.size(), 8192);
    for (std::size_t start = 0; start < limit; ++start) {
        if (!std::isdigit(data[start])) continue;
        std::size_t end = start;
        while (end < limit && data[end] != 0 && end - start <= 48) ++end;
        if (end == limit || data[end] != 0) continue;
        const std::string candidate(reinterpret_cast<const char*>(data.data() + start), end - start);
        unity_version parsed;
        if (!parse_unity_version(candidate, parsed)) continue;
        parsed.from_unityfs = false;
        result = std::move(parsed);
        return true;
    }
    return false;
}

static bool parse_data_blob(const std::vector<uint8_t>& data, unity_version& result) {
    return parse_unityfs(data, result) || find_version_string(data, result);
}

static bool inflate_zip_entry_prefix(std::ifstream& file, uint64_t payload_offset, uint32_t compressed_size, uint32_t uncompressed_size, int method, std::vector<uint8_t>& output) {
    if (uncompressed_size == 0 || compressed_size == 0) return false;
    const std::size_t expected_prefix = std::min<std::size_t>(uncompressed_size, 8192);
    output.resize(expected_prefix);
    file.clear();
    file.seekg(static_cast<std::streamoff>(payload_offset), std::ios::beg);
    if (method == 0) {
        return static_cast<bool>(file.read(reinterpret_cast<char*>(output.data()), static_cast<std::streamsize>(output.size())));
    }
    if (method != 8) return false;

    z_stream stream{};
    if (inflateInit2(&stream, -MAX_WBITS) != Z_OK) return false;
    std::array<uint8_t, 65536> input{};
    uint64_t remaining = compressed_size;
    std::size_t produced = 0;
    int status = Z_OK;
    while (remaining > 0 && produced < output.size() && status == Z_OK) {
        const std::size_t chunk_size = static_cast<std::size_t>(std::min<uint64_t>(remaining, input.size()));
        file.clear();
        if (!file.read(reinterpret_cast<char*>(input.data()), static_cast<std::streamsize>(chunk_size))) {
            status = Z_DATA_ERROR;
            break;
        }
        remaining -= chunk_size;
        stream.next_in = reinterpret_cast<Bytef*>(input.data());
        stream.avail_in = static_cast<uInt>(chunk_size);
        while (stream.avail_in > 0 && produced < output.size() && status == Z_OK) {
            stream.next_out = reinterpret_cast<Bytef*>(output.data() + produced);
            stream.avail_out = static_cast<uInt>(output.size() - produced);
            status = inflate(&stream, Z_NO_FLUSH);
            produced = output.size() - stream.avail_out;
        }
    }
    inflateEnd(&stream);
    output.resize(produced);
    return produced > 0 && (status == Z_OK || status == Z_STREAM_END || produced == expected_prefix);
}

static bool is_version_entry_name(std::string_view name) {
    const auto slash = name.find_last_of('/');
    const std::string_view base = slash == std::string_view::npos ? name : name.substr(slash + 1);
    return base == "data.unity3d" || base == "globalgamemanagers" || base == "globalgamemanagers.assets";
}

static bool parse_zip_archive(const std::string& archive_path, unity_version& result) {
    std::ifstream file(archive_path, std::ios::binary | std::ios::ate);
    if (!file) return false;
    const std::streamoff file_size = file.tellg();
    if (file_size < 22) return false;
    const std::size_t tail_size = static_cast<std::size_t>(std::min<std::streamoff>(file_size, 65557));
    std::vector<uint8_t> tail(tail_size);
    file.seekg(file_size - static_cast<std::streamoff>(tail_size), std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(tail.data()), static_cast<std::streamsize>(tail.size()))) return false;

    std::size_t eocd = std::string::npos;
    if (tail.size() < 22) return false;
    for (std::size_t i = tail.size() - 22;; --i) {
        if (read_u32_le(tail.data() + i) == 0x06054b50U) {
            eocd = i;
            break;
        }
        if (i == 0) break;
    }
    if (eocd == std::string::npos || eocd + 22 > tail.size()) return false;
    const uint16_t entry_count = read_u16_le(tail.data() + eocd + 10);
    const uint32_t central_size = read_u32_le(tail.data() + eocd + 12);
    const uint32_t central_offset = read_u32_le(tail.data() + eocd + 16);
    if (central_size == 0 || central_size > 64U * 1024U * 1024U) return false;
    if (static_cast<uint64_t>(central_offset) + central_size > static_cast<uint64_t>(file_size)) return false;

    std::vector<uint8_t> central(central_size);
    file.clear();
    file.seekg(central_offset, std::ios::beg);
    if (!file.read(reinterpret_cast<char*>(central.data()), static_cast<std::streamsize>(central.size()))) return false;

    std::size_t position = 0;
    for (uint16_t index = 0; index < entry_count && position + 46 <= central.size(); ++index) {
        const uint8_t* header = central.data() + position;
        if (read_u32_le(header) != 0x02014b50U) return false;
        const uint16_t flags = read_u16_le(header + 8);
        const uint16_t method = read_u16_le(header + 10);
        const uint32_t compressed_size = read_u32_le(header + 20);
        const uint32_t uncompressed_size = read_u32_le(header + 24);
        const uint16_t name_size = read_u16_le(header + 28);
        const uint16_t extra_size = read_u16_le(header + 30);
        const uint16_t comment_size = read_u16_le(header + 32);
        const uint32_t local_offset = read_u32_le(header + 42);
        const std::size_t record_size = 46U + name_size + extra_size + comment_size;
        if (position + record_size > central.size()) return false;
        const std::string_view name(reinterpret_cast<const char*>(header + 46), name_size);
        position += record_size;
        if ((flags & 1U) != 0 || !is_version_entry_name(name)) continue;
        if (compressed_size == 0 || uncompressed_size == 0) continue;
        if (static_cast<uint64_t>(local_offset) + 30U > static_cast<uint64_t>(file_size)) continue;

        std::array<uint8_t, 30> local{};
        file.clear();
        file.seekg(local_offset, std::ios::beg);
        if (!file.read(reinterpret_cast<char*>(local.data()), static_cast<std::streamsize>(local.size()))) continue;
        if (read_u32_le(local.data()) != 0x04034b50U) continue;
        const uint16_t local_name_size = read_u16_le(local.data() + 26);
        const uint16_t local_extra_size = read_u16_le(local.data() + 28);
        const uint64_t payload_offset = static_cast<uint64_t>(local_offset) + 30U + local_name_size + local_extra_size;
        if (payload_offset + compressed_size > static_cast<uint64_t>(file_size)) continue;

        std::vector<uint8_t> blob;
        if (!inflate_zip_entry_prefix(file, payload_offset, compressed_size, uncompressed_size, method, blob)) continue;
        if (!parse_data_blob(blob, result)) continue;
        result.source_path = archive_path + "!" + std::string(name);
        result.source_kind = method == 0 ? "zip-store" : "zip-deflate";
        return true;
    }
    return false;
}

static bool has_suffix(const std::string& value, const std::string& suffix) {
    return value.size() >= suffix.size() && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static void scan_directory(const std::string& root, std::size_t depth, std::set<std::string>& files) {
    if (depth == 0 || files.size() >= max_scan_entries) return;
    DIR* directory = opendir(root.c_str());
    if (!directory) return;
    dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr && files.size() < max_scan_entries) {
        if (entry->d_name[0] == '.') continue;
        const std::string path = root + "/" + entry->d_name;
        struct stat info{};
        if (lstat(path.c_str(), &info) != 0) continue;
        if (S_ISDIR(info.st_mode)) {
            scan_directory(path, depth - 1, files);
            continue;
        }
        if (!S_ISREG(info.st_mode)) continue;
        const std::string name = entry->d_name;
        if (name == "data.unity3d" || name == "globalgamemanagers" || name == "globalgamemanagers.assets" || has_suffix(name, ".obb") || has_suffix(name, ".apk")) {
            files.insert(path);
        }
    }
    closedir(directory);
}

static void collect_mapped_archives(std::set<std::string>& files) {
    std::ifstream maps("/proc/self/maps");
    std::string line;
    while (std::getline(maps, line)) {
        const std::size_t path_pos = line.find('/');
        if (path_pos == std::string::npos) continue;
        std::string path = line.substr(path_pos);
        const std::size_t deleted = path.find(" (deleted)");
        if (deleted != std::string::npos) path.resize(deleted);
        if (has_suffix(path, ".apk") || has_suffix(path, ".obb")) files.insert(path);
    }
}

static void collect_app_archives(const std::string& package_name, std::set<std::string>& files) {
    if (package_name.empty()) return;
    std::set<std::string> app_files;
    scan_directory("/data/app", 4, app_files);
    for (const auto& path : app_files) {
        if (path.find(package_name) != std::string::npos && has_suffix(path, ".apk")) files.insert(path);
    }
}

bool read_unity_version_file(const std::string& path, unity_version& result) {
    std::vector<uint8_t> data;
    if (read_file_prefix(path, data) && parse_data_blob(data, result)) {
        result.source_path = path;
        result.source_kind = result.from_unityfs ? "unityfs" : "serialized-file-or-binary";
        return true;
    }
    return parse_zip_archive(path, result);
}

bool detect_unity_version(const std::string& package_name, const std::string& custom_path, unity_version& result) {
    if (!custom_path.empty() && read_unity_version_file(custom_path, result)) return true;
    std::set<std::string> candidates;
    collect_mapped_archives(candidates);
    collect_app_archives(package_name, candidates);

    if (!package_name.empty()) {
        const std::array<std::string, 6> roots = {
            "/data/user/0/" + package_name,
            "/data/data/" + package_name,
            "/sdcard/Android/data/" + package_name,
            "/storage/emulated/0/Android/data/" + package_name,
            "/sdcard/Android/obb/" + package_name,
            "/storage/emulated/0/Android/obb/" + package_name
        };
        for (const auto& root : roots) scan_directory(root, max_scan_depth, candidates);
    }

    for (const auto& path : candidates) {
        unity_version parsed;
        if (!read_unity_version_file(path, parsed)) continue;
        result = std::move(parsed);
        return true;
    }
    return false;
}

std::string header_name_for_version(int value) {
    switch (value) {
        case 171: return "2017.1.h";
        case 172: return "2017.4.h";
        case 181: return "2018.1.h";
        case 182: return "2018.2.h";
        case 183: return "2018.4.h";
        case 191: return "2019.2.h";
        case 193: return "2019.3.h";
        case 194: return "2019.4.h";
        case 201: return "2020.1.h";
        case 202: return "2020.2.h";
        case 203: return "2020.3.h";
        case 211: return "2021.1.h";
        case 212: return "2021.2.h";
        case 213: return "2021.3.h";
        case 221: return "2022.1.h";
        case 222: return "2022.2.h";
        case 231: return "2023.1.h";
        case 232: return "2023.2.h";
        case 600: return "6000.0.h";
        case 605: return "6000.5.h";
        case 606: return "6000.6.h";
        default: return {};
    }
}

std::string variant_name_for_version(const unity_version& version) {
    if (version.bnm_unity_ver == 211) return version.patch >= 24 ? "v211_24" : "v211_pre24";
    switch (version.bnm_unity_ver) {
        case 171: return "v171";
        case 172: return "v172";
        case 181: return "v181";
        case 182: return "v182";
        case 183: return "v183";
        case 191: return "v191";
        case 193: return "v193";
        case 194: return "v194";
        case 201: return "v201";
        case 202: return "v202";
        case 203: return "v203";
        case 212: return "v212";
        case 213: return "v213";
        case 221: return "v221";
        case 222: return "v222";
        case 231: return "v231";
        case 232: return "v232";
        case 600: return "v600";
        case 605: return "v605";
        case 606: return "v606";
        default: return {};
    }
}

}
