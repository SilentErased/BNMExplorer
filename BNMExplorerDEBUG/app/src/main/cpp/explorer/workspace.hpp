#pragma once
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cctype>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <zlib.h>

#include "autodetect/variant_namespace.hpp"
#ifndef BNM_APP_NAMESPACE_OPEN
namespace BNM_APP_NAMESPACE {
#define BNM_APP_NAMESPACE_CLOSE
#endif

struct ws_entry {
    std::string path;
    bool is_dir;
    uint64_t size;
};

struct ws_file_data {
    std::string path;
    std::string data;
};

struct ws_zip_info {
    std::string name;
    uint64_t size;
    int64_t mtime;
};

static std::recursive_mutex g_ws_mtx;
static std::string g_ws_base;
static bool g_ws_ready = false;

static const size_t ws_max_file_size = 1024 * 1024;
static const size_t ws_max_total_size = 8 * 1024 * 1024;
static const size_t ws_max_files = 500;
static const size_t ws_max_depth = 8;

static bool ws_is_dir(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

static bool ws_is_file(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

static bool ws_make_dirs(const std::string& path) {
    if (path.empty()) return false;
    std::string current;
    size_t i = 0;
    while (i <= path.size()) {
        size_t next = path.find('/', i);
        if (next == std::string::npos) next = path.size();
        current = path.substr(0, next);
        if (!current.empty() && !ws_is_dir(current)) {
            if (mkdir(current.c_str(), 0775) != 0 && !ws_is_dir(current)) return false;
        }
        i = next + 1;
    }
    return true;
}

static std::string ws_read_package_name() {
    FILE* f = fopen("/proc/self/cmdline", "rb");
    if (!f) return "";
    char buf[256];
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    fclose(f);
    buf[n] = '\0';
    return std::string(buf);
}

static bool ws_dir_writable(const std::string& dir) {
    if (!ws_make_dirs(dir)) return false;
    std::string probe = dir + "/.probe";
    FILE* f = fopen(probe.c_str(), "wb");
    if (!f) return false;
    fclose(f);
    unlink(probe.c_str());
    return true;
}

static std::string ws_workspace_dir() { return g_ws_base + "/workspace"; }
static std::string ws_zips_dir() { return g_ws_base + "/zips"; }

static std::string ws_sanitize_path(const std::string& input, bool require_lua) {
    std::string normalized = input;
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    std::vector<std::string> parts;
    size_t i = 0;
    while (i <= normalized.size()) {
        size_t next = normalized.find('/', i);
        if (next == std::string::npos) next = normalized.size();
        std::string part = normalized.substr(i, next - i);
        i = next + 1;
        if (part.empty() || part == ".") continue;
        if (part == "..") return "";
        if (part.size() > 100) return "";
        for (unsigned char c : part) {
            if (c < 0x20 || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') return "";
        }
        if (part[0] == '.') return "";
        parts.push_back(part);
    }
    if (parts.empty() || parts.size() > ws_max_depth) return "";
    std::string out;
    for (size_t k = 0; k < parts.size(); k++) {
        if (k) out += "/";
        out += parts[k];
    }
    if (out.size() > 240) return "";
    if (require_lua) {
        const std::string& last = parts.back();
        if (last.size() <= 4 || last.compare(last.size() - 4, 4, ".lua") != 0) return "";
    }
    return out;
}

static std::string ws_sanitize_zip_name(const std::string& input) {
    std::string out;
    for (unsigned char c : input) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-' || c == ' ' || c == '(' || c == ')') out += (char)c;
    }
    while (!out.empty() && (out[0] == '.' || out[0] == ' ')) out.erase(0, 1);
    if (out.empty() || out.size() > 80) return "";
    if (out.size() < 4 || out.compare(out.size() - 4, 4, ".zip") != 0) out += ".zip";
    return out;
}

static void ws_walk(const std::string& root, const std::string& rel, std::vector<ws_entry>& out) {
    DIR* d = opendir((rel.empty() ? root : root + "/" + rel).c_str());
    if (!d) return;
    std::vector<std::string> names;
    while (dirent* e = readdir(d)) {
        if (e->d_name[0] == '.') continue;
        names.push_back(e->d_name);
    }
    closedir(d);
    std::sort(names.begin(), names.end());
    for (const auto& name : names) {
        std::string child_rel = rel.empty() ? name : rel + "/" + name;
        std::string full = root + "/" + child_rel;
        struct stat st;
        if (stat(full.c_str(), &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            out.push_back({child_rel, true, 0});
            ws_walk(root, child_rel, out);
        } else if (S_ISREG(st.st_mode) && name.size() > 4 && name.compare(name.size() - 4, 4, ".lua") == 0) {
            out.push_back({child_rel, false, (uint64_t)st.st_size});
        }
    }
}

static std::vector<ws_entry> ws_list() {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::vector<ws_entry> out;
    ws_walk(ws_workspace_dir(), "", out);
    return out;
}

static bool ws_read_file(const std::string& rel_path, std::string& out) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::string full = ws_workspace_dir() + "/" + rel_path;
    if (!ws_is_file(full)) return false;
    FILE* f = fopen(full.c_str(), "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0 || (size_t)size > ws_max_file_size) {
        fclose(f);
        return false;
    }
    out.assign((size_t)size, '\0');
    size_t got = size > 0 ? fread(&out[0], 1, (size_t)size, f) : 0;
    fclose(f);
    return got == (size_t)size;
}

static bool ws_write_raw(const std::string& full_path, const std::string& data) {
    std::string tmp = full_path + ".tmp";
    FILE* f = fopen(tmp.c_str(), "wb");
    if (!f) return false;
    size_t wrote = data.empty() ? 0 : fwrite(data.data(), 1, data.size(), f);
    bool ok = wrote == data.size();
    ok = (fclose(f) == 0) && ok;
    if (!ok) {
        unlink(tmp.c_str());
        return false;
    }
    if (rename(tmp.c_str(), full_path.c_str()) != 0) {
        unlink(tmp.c_str());
        return false;
    }
    return true;
}

static bool ws_write_file(const std::string& rel_path, const std::string& data) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    if (data.size() > ws_max_file_size) return false;
    std::string full = ws_workspace_dir() + "/" + rel_path;
    size_t slash = full.rfind('/');
    if (slash != std::string::npos && !ws_make_dirs(full.substr(0, slash))) return false;
    return ws_write_raw(full, data);
}

static void ws_remove_tree(const std::string& full) {
    if (ws_is_dir(full)) {
        DIR* d = opendir(full.c_str());
        if (d) {
            std::vector<std::string> names;
            while (dirent* e = readdir(d)) {
                if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
                names.push_back(e->d_name);
            }
            closedir(d);
            for (const auto& n : names) ws_remove_tree(full + "/" + n);
        }
        rmdir(full.c_str());
    } else {
        unlink(full.c_str());
    }
}

static bool ws_delete(const std::string& rel_path) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::string full = ws_workspace_dir() + "/" + rel_path;
    struct stat st;
    if (stat(full.c_str(), &st) != 0) return false;
    ws_remove_tree(full);
    return true;
}

static bool ws_make_folder(const std::string& rel_path) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    return ws_make_dirs(ws_workspace_dir() + "/" + rel_path);
}

static bool ws_rename(const std::string& from, const std::string& to) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::string full_from = ws_workspace_dir() + "/" + from;
    std::string full_to = ws_workspace_dir() + "/" + to;
    struct stat st;
    if (stat(full_from.c_str(), &st) != 0) return false;
    if (stat(full_to.c_str(), &st) == 0) return false;
    size_t slash = full_to.rfind('/');
    if (slash != std::string::npos && !ws_make_dirs(full_to.substr(0, slash))) return false;
    return rename(full_from.c_str(), full_to.c_str()) == 0;
}

static void ws_wipe() {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::string root = ws_workspace_dir();
    DIR* d = opendir(root.c_str());
    if (!d) return;
    std::vector<std::string> names;
    while (dirent* e = readdir(d)) {
        if (strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0) continue;
        names.push_back(e->d_name);
    }
    closedir(d);
    for (const auto& n : names) ws_remove_tree(root + "/" + n);
}

static void put16(std::string& s, uint16_t v) {
    s += (char)(v & 0xFF);
    s += (char)((v >> 8) & 0xFF);
}

static void put32(std::string& s, uint32_t v) {
    for (int i = 0; i < 4; i++) s += (char)((v >> (8 * i)) & 0xFF);
}

static uint16_t get16(const std::string& s, size_t pos) {
    return (uint16_t)((uint8_t)s[pos] | ((uint8_t)s[pos + 1] << 8));
}

static uint32_t get32(const std::string& s, size_t pos) {
    return (uint32_t)((uint8_t)s[pos] | ((uint8_t)s[pos + 1] << 8) | ((uint8_t)s[pos + 2] << 16) | ((uint32_t)(uint8_t)s[pos + 3] << 24));
}

static void ws_dos_time(uint16_t& dos_time, uint16_t& dos_date) {
    time_t now = time(nullptr);
    struct tm t;
    localtime_r(&now, &t);
    int year = t.tm_year + 1900;
    if (year < 1980) year = 1980;
    dos_time = (uint16_t)((t.tm_hour << 11) | (t.tm_min << 5) | (t.tm_sec / 2));
    dos_date = (uint16_t)(((year - 1980) << 9) | ((t.tm_mon + 1) << 5) | t.tm_mday);
}

static bool ws_build_zip(std::string& out) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::vector<ws_entry> entries = ws_list();
    std::string central;
    uint16_t dos_time = 0;
    uint16_t dos_date = 0;
    ws_dos_time(dos_time, dos_date);
    uint32_t count = 0;
    out.clear();
    for (const auto& e : entries) {
        if (e.is_dir) continue;
        std::string data;
        if (!ws_read_file(e.path, data)) continue;
        uint32_t crc = (uint32_t)crc32(0L, (const Bytef*)data.data(), (uInt)data.size());
        uint32_t offset = (uint32_t)out.size();
        put32(out, 0x04034b50);
        put16(out, 20);
        put16(out, 0x0800);
        put16(out, 0);
        put16(out, dos_time);
        put16(out, dos_date);
        put32(out, crc);
        put32(out, (uint32_t)data.size());
        put32(out, (uint32_t)data.size());
        put16(out, (uint16_t)e.path.size());
        put16(out, 0);
        out += e.path;
        out += data;
        put32(central, 0x02014b50);
        put16(central, 20);
        put16(central, 20);
        put16(central, 0x0800);
        put16(central, 0);
        put16(central, dos_time);
        put16(central, dos_date);
        put32(central, crc);
        put32(central, (uint32_t)data.size());
        put32(central, (uint32_t)data.size());
        put16(central, (uint16_t)e.path.size());
        put16(central, 0);
        put16(central, 0);
        put16(central, 0);
        put16(central, 0);
        put32(central, 0);
        put32(central, offset);
        central += e.path;
        count++;
    }
    uint32_t central_offset = (uint32_t)out.size();
    out += central;
    put32(out, 0x06054b50);
    put16(out, 0);
    put16(out, 0);
    put16(out, (uint16_t)count);
    put16(out, (uint16_t)count);
    put32(out, (uint32_t)central.size());
    put32(out, central_offset);
    put16(out, 0);
    return true;
}

static bool ws_inflate_raw(const char* src, size_t src_size, std::string& dst, size_t expected) {
    dst.assign(expected, '\0');
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, -MAX_WBITS) != Z_OK) return false;
    zs.next_in = (Bytef*)src;
    zs.avail_in = (uInt)src_size;
    zs.next_out = expected ? (Bytef*)&dst[0] : nullptr;
    zs.avail_out = (uInt)expected;
    int rc = inflate(&zs, Z_FINISH);
    bool ok = (rc == Z_STREAM_END) && zs.total_out == expected;
    inflateEnd(&zs);
    return ok;
}

static bool ws_parse_zip(const std::string& data, std::vector<ws_file_data>& out, int& skipped, std::string& error) {
    out.clear();
    skipped = 0;
    if (data.size() < 22) {
        error = "file is too small to be a zip archive";
        return false;
    }
    size_t eocd = std::string::npos;
    size_t scan_start = data.size() >= 22 + 65535 ? data.size() - 22 - 65535 : 0;
    for (size_t pos = data.size() - 22 + 1; pos-- > scan_start;) {
        if (get32(data, pos) == 0x06054b50) {
            eocd = pos;
            break;
        }
    }
    if (eocd == std::string::npos) {
        error = "zip directory not found";
        return false;
    }
    uint32_t total = get16(data, eocd + 10);
    uint32_t cd_size = get32(data, eocd + 12);
    uint32_t cd_offset = get32(data, eocd + 16);
    if (total == 0xFFFF || cd_size == 0xFFFFFFFFu || cd_offset == 0xFFFFFFFFu) {
        error = "zip64 archives are not supported";
        return false;
    }
    if ((uint64_t)cd_offset + cd_size > data.size()) {
        error = "zip directory is corrupted";
        return false;
    }
    size_t pos = cd_offset;
    size_t total_bytes = 0;
    for (uint32_t i = 0; i < total; i++) {
        if (pos + 46 > data.size() || get32(data, pos) != 0x02014b50) {
            error = "zip directory entry is corrupted";
            return false;
        }
        uint16_t flags = get16(data, pos + 8);
        uint16_t method = get16(data, pos + 10);
        uint32_t crc = get32(data, pos + 16);
        uint32_t csize = get32(data, pos + 20);
        uint32_t usize = get32(data, pos + 24);
        uint16_t nlen = get16(data, pos + 28);
        uint16_t elen = get16(data, pos + 30);
        uint16_t clen = get16(data, pos + 32);
        uint32_t local = get32(data, pos + 42);
        if (pos + 46 + nlen > data.size()) {
            error = "zip entry name is corrupted";
            return false;
        }
        std::string raw_name = data.substr(pos + 46, nlen);
        pos += 46 + nlen + elen + clen;
        if (raw_name.empty() || raw_name.back() == '/') continue;
        if (raw_name.compare(0, 9, "__MACOSX/") == 0) continue;
        std::string name = ws_sanitize_path(raw_name, true);
        if (name.empty() || (flags & 1)) {
            skipped++;
            continue;
        }
        if (usize > ws_max_file_size || out.size() >= ws_max_files || total_bytes + usize > ws_max_total_size) {
            error = "archive is too large (limits: 500 files, 1 MB per file, 8 MB total)";
            return false;
        }
        if ((uint64_t)local + 30 > data.size() || get32(data, local) != 0x04034b50) {
            error = "zip local header is corrupted";
            return false;
        }
        uint16_t lnlen = get16(data, local + 26);
        uint16_t lelen = get16(data, local + 28);
        uint64_t start = (uint64_t)local + 30 + lnlen + lelen;
        if (start + csize > data.size()) {
            error = "zip entry data is corrupted";
            return false;
        }
        std::string content;
        if (method == 0) {
            if (csize != usize) {
                error = "zip entry size mismatch";
                return false;
            }
            content.assign(data, (size_t)start, csize);
        } else if (method == 8) {
            if (!ws_inflate_raw(data.data() + start, csize, content, usize)) {
                error = "failed to decompress " + name;
                return false;
            }
        } else {
            skipped++;
            continue;
        }
        uint32_t actual = (uint32_t)crc32(0L, (const Bytef*)content.data(), (uInt)content.size());
        if (actual != crc) {
            error = "checksum mismatch in " + name;
            return false;
        }
        total_bytes += content.size();
        bool replaced = false;
        for (auto& existing : out) {
            if (existing.path == name) {
                existing.data = content;
                replaced = true;
                break;
            }
        }
        if (!replaced) out.push_back({name, content});
    }
    if (!out.empty()) {
        std::string root;
        bool common = true;
        for (const auto& f : out) {
            size_t slash = f.path.find('/');
            if (slash == std::string::npos) {
                common = false;
                break;
            }
            std::string first = f.path.substr(0, slash);
            if (root.empty()) root = first;
            else if (root != first) {
                common = false;
                break;
            }
        }
        if (common) {
            for (auto& f : out) f.path = f.path.substr(root.size() + 1);
        }
    }
    return true;
}

static bool ws_replace_with(const std::vector<ws_file_data>& files, std::string& error) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    if (files.empty()) {
        error = "no .lua files found in archive";
        return false;
    }
    ws_wipe();
    for (const auto& f : files) {
        if (!ws_write_file(f.path, f.data)) {
            error = "failed to write " + f.path;
            return false;
        }
    }
    return true;
}

static std::vector<ws_zip_info> ws_list_zips() {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::vector<ws_zip_info> out;
    DIR* d = opendir(ws_zips_dir().c_str());
    if (!d) return out;
    while (dirent* e = readdir(d)) {
        std::string name = e->d_name;
        if (name.size() < 5 || name.compare(name.size() - 4, 4, ".zip") != 0) continue;
        struct stat st;
        if (stat((ws_zips_dir() + "/" + name).c_str(), &st) != 0 || !S_ISREG(st.st_mode)) continue;
        out.push_back({name, (uint64_t)st.st_size, (int64_t)st.st_mtime});
    }
    closedir(d);
    std::sort(out.begin(), out.end(), [](const ws_zip_info& a, const ws_zip_info& b) { return a.mtime > b.mtime; });
    return out;
}

static bool ws_save_zip(const std::string& name, std::string& error) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    std::string data;
    ws_build_zip(data);
    if (!ws_write_raw(ws_zips_dir() + "/" + name, data)) {
        error = "failed to write zip to device storage";
        return false;
    }
    return true;
}

static bool ws_read_zip(const std::string& name, std::string& out) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    FILE* f = fopen((ws_zips_dir() + "/" + name).c_str(), "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0 || (size_t)size > 64 * 1024 * 1024) {
        fclose(f);
        return false;
    }
    out.assign((size_t)size, '\0');
    size_t got = size > 0 ? fread(&out[0], 1, (size_t)size, f) : 0;
    fclose(f);
    return got == (size_t)size;
}

static bool ws_delete_zip(const std::string& name) {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    return unlink((ws_zips_dir() + "/" + name).c_str()) == 0;
}

static void ws_seed_defaults() {
    ws_write_file("main.lua", "local movements = require(\"movements.lua\")\n\nprint(\"workspace ready\")\nmovements.hello()\n");
    ws_write_file("movements.lua", "local movements = {}\n\nfunction movements.hello()\n  print(\"movements module loaded\")\nend\n\nreturn movements\n");
}

static void ws_init() {
    std::lock_guard<std::recursive_mutex> lk(g_ws_mtx);
    if (g_ws_ready) return;
    std::string pkg = ws_read_package_name();
    std::vector<std::string> candidates;
    if (!pkg.empty()) {
        candidates.push_back("/storage/emulated/0/Android/data/" + pkg + "/files");
        candidates.push_back("/sdcard/Android/data/" + pkg + "/files");
        candidates.push_back("/data/user/0/" + pkg + "/files");
        candidates.push_back("/data/data/" + pkg + "/files");
    }
    candidates.push_back("/data/local/tmp");
    for (const auto& c : candidates) {
        std::string base = c + "/BNMExplorer";
        if (ws_dir_writable(base)) {
            g_ws_base = base;
            break;
        }
    }
    if (g_ws_base.empty()) g_ws_base = "/data/local/tmp/BNMExplorer";
    bool fresh = !ws_is_dir(ws_workspace_dir());
    ws_make_dirs(ws_workspace_dir());
    ws_make_dirs(ws_zips_dir());
    if (fresh) ws_seed_defaults();
    g_ws_ready = true;
}

#ifdef BNM_APP_NAMESPACE_CLOSE
}
#undef BNM_APP_NAMESPACE_CLOSE
#endif
