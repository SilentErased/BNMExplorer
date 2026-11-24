#include "il2cpp_runtime.hpp"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <limits>
#include <link.h>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#if defined(__aarch64__) || defined(il2cpp_force_arm64)
#define il2cpp_runtime_arm64 1
#else
#define il2cpp_runtime_arm64 0
#endif

namespace il2cpp_autodetect {
    namespace {

        std::mutex runtime_names_mutex;
        std::unordered_map<std::string, std::string> runtime_names;

        constexpr std::string_view original_prefix = "il2cpp: function ";
        constexpr std::size_t min_resolved_names = 8;
        constexpr std::size_t expected_name_count = 229;
        constexpr std::size_t max_name_length = 256;
        constexpr std::size_t max_literal_name_length = 160;
        constexpr std::int64_t known_name_delta = -0x1C;
        constexpr std::int64_t delta_window = 0x100;
        constexpr std::size_t add_search_window = 4;
        constexpr std::size_t max_stride = 0x200;
        constexpr std::size_t max_diagnostic_words = 24;
        constexpr std::size_t irregular_search_bytes = 0x48;

        struct memory_range {
            std::uintptr_t begin = 0;
            std::uintptr_t end = 0;
            bool readable = false;
            bool executable = false;
            std::string path;
        };

        struct code_reference {
            std::uintptr_t address = 0;
            std::uintptr_t literal = 0;
        };

        struct delta_entry {
            std::int64_t delta = 0;
            std::size_t score = 0;
            std::string sample;
        };

        using literal_map = std::unordered_map<std::uintptr_t, std::string>;

        std::string format_text(const char* format, ...) __attribute__((format(printf, 1, 2)));

        std::string format_text(const char* format, ...) {
            char buffer[768];
            va_list arguments;
            va_start(arguments, format);
            std::vsnprintf(buffer, sizeof(buffer), format, arguments);
            va_end(arguments);
            return buffer;
        }

        bool is_unity_path(const std::string& path) {
            return path.find("libunity.so") != std::string::npos;
        }

        std::vector<memory_range> read_memory_ranges() {
            std::vector<memory_range> ranges;
            std::ifstream maps("/proc/self/maps");
            std::string line;
            while (std::getline(maps, line)) {
                std::istringstream stream(line);
                std::string address;
                std::string permissions;
                std::string offset;
                std::string device;
                std::string inode;
                if (!(stream >> address >> permissions >> offset >> device >> inode)) continue;
                std::string path;
                std::getline(stream, path);
                const std::size_t path_start = path.find_first_not_of(" \t");
                if (path_start == std::string::npos) path.clear();
                else path.erase(0, path_start);
                const std::size_t separator = address.find('-');
                if (separator == std::string::npos) continue;
                try {
                    memory_range range;
                    range.begin = static_cast<std::uintptr_t>(std::stoull(address.substr(0, separator), nullptr, 16));
                    range.end = static_cast<std::uintptr_t>(std::stoull(address.substr(separator + 1), nullptr, 16));
                    range.readable = permissions.size() > 0 && permissions[0] == 'r';
                    range.executable = permissions.size() > 2 && permissions[2] == 'x';
                    range.path = std::move(path);
                    if (range.begin < range.end) ranges.push_back(std::move(range));
                } catch (...) {
                }
            }
            std::sort(ranges.begin(), ranges.end(), [](const memory_range& left, const memory_range& right) {
                return left.begin < right.begin;
            });
            return ranges;
        }

        const memory_range* find_memory_range(const std::vector<memory_range>& ranges, std::uintptr_t address, std::size_t size) {
            if (!address || size == 0 || address > std::numeric_limits<std::uintptr_t>::max() - size) return nullptr;
            const auto it = std::upper_bound(ranges.begin(), ranges.end(), address,
                                             [](std::uintptr_t value, const memory_range& range) { return value < range.begin; });
            if (it == ranges.begin()) return nullptr;
            const memory_range& range = *std::prev(it);
            if (address < range.begin || address + size > range.end) return nullptr;
            return &range;
        }

        bool add_signed_address(std::uintptr_t base, std::int64_t displacement, std::uintptr_t& result) {
            if (displacement >= 0) {
                const auto amount = static_cast<std::uintptr_t>(displacement);
                if (base > std::numeric_limits<std::uintptr_t>::max() - amount) return false;
                result = base + amount;
                return true;
            }
            const auto amount = static_cast<std::uintptr_t>(-(displacement + 1)) + 1U;
            if (base < amount) return false;
            result = base - amount;
            return true;
        }

        std::int64_t sign_extend(std::uint64_t value, unsigned bits) {
            const std::uint64_t mask = (std::uint64_t{1} << bits) - 1U;
            const std::uint64_t sign = std::uint64_t{1} << (bits - 1U);
            value &= mask;
            if ((value & sign) == 0) return static_cast<std::int64_t>(value);
            return -static_cast<std::int64_t>(((~value) & mask) + 1U);
        }

        std::int64_t adr_immediate(std::uint32_t instruction) {
            const std::uint64_t immlo = (instruction >> 29U) & 0x3U;
            const std::uint64_t immhi = (instruction >> 5U) & 0x7FFFFU;
            return sign_extend((immhi << 2U) | immlo, 21);
        }

        bool decode_target(const std::uint32_t* words, std::size_t available, std::uintptr_t address, const std::vector<memory_range>& ranges, std::uintptr_t& target) {
            const std::uint32_t instruction = words[0];

            if ((instruction & 0x9F000000U) == 0x90000000U) {
                const std::uint32_t destination = instruction & 0x1FU;
                std::uintptr_t page = 0;
                if (!add_signed_address(address & ~std::uintptr_t{0xFFF}, adr_immediate(instruction) * 4096, page)) return false;
                const std::size_t limit = std::min(available, add_search_window + 1);
                for (std::size_t index = 1; index < limit; ++index) {
                    const std::uint32_t next = words[index];
                    if ((next & 0xFF000000U) != 0x91000000U) continue;
                    if (((next >> 5U) & 0x1FU) != destination) continue;
                    std::uintptr_t addend = (next >> 10U) & 0xFFFU;
                    if ((next >> 22U) & 1U) addend <<= 12U;
                    target = page + addend;
                    return true;
                }
                target = page;
                return true;
            }

            if ((instruction & 0x9F000000U) == 0x10000000U) {
                return add_signed_address(address, adr_immediate(instruction), target);
            }

            if ((instruction & 0xFF000000U) == 0x58000000U) {
                const std::int64_t displacement = sign_extend((instruction >> 5U) & 0x7FFFFU, 19) * 4;
                std::uintptr_t pool = 0;
                if (!add_signed_address(address, displacement, pool)) return false;
                const memory_range* range = find_memory_range(ranges, pool, sizeof(std::uintptr_t));
                if (!range || !range->readable) return false;
                std::memcpy(&target, reinterpret_cast<const void*>(pool), sizeof(target));
                return target != 0;
            }

            return false;
        }

        bool decode_at(std::uintptr_t address, const std::vector<memory_range>& ranges, std::uintptr_t& target) {
            std::uint32_t words[add_search_window + 1];
            std::size_t available = 0;
            for (; available < add_search_window + 1; ++available) {
                const std::uintptr_t current = address + available * sizeof(std::uint32_t);
                const memory_range* range = find_memory_range(ranges, current, sizeof(std::uint32_t));
                if (!range || !range->readable) break;
                std::memcpy(&words[available], reinterpret_cast<const void*>(current), sizeof(std::uint32_t));
            }
            if (available == 0) return false;
            return decode_target(words, available, address, ranges, target);
        }

        bool read_c_string(std::uintptr_t address, const std::vector<memory_range>& ranges, std::size_t max_length, std::string& value) {
            value.clear();
            if (address > std::numeric_limits<std::uintptr_t>::max() - max_length - 1U) return false;
            const memory_range* range = nullptr;
            for (std::size_t offset = 0; offset <= max_length; ++offset) {
                const std::uintptr_t current = address + offset;
                if (!range || current >= range->end) {
                    range = find_memory_range(ranges, current, 1);
                    if (!range || !range->readable) return false;
                }
                const char character = *reinterpret_cast<const char*>(current);
                if (character == 0) return !value.empty();
                value.push_back(character);
            }
            return false;
        }

        bool is_name_character(char character) {
            return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') ||
                   (character >= '0' && character <= '9') || character == '_' || character == '.';
        }

        bool parse_literal_name(const char* literal, const char* end, std::string& name) {
            name.clear();
            const char* cursor = literal + original_prefix.size();
            while (cursor < end && name.size() < max_literal_name_length && is_name_character(*cursor)) {
                name.push_back(*cursor);
                ++cursor;
            }
            return !name.empty() && cursor < end;
        }

        void collect_literals(const memory_range& range, literal_map& literals) {
            const char* cursor = reinterpret_cast<const char*>(range.begin);
            const char* end = reinterpret_cast<const char*>(range.end);
            while (cursor < end) {
                const void* found = std::memchr(cursor, 'i', static_cast<std::size_t>(end - cursor));
                if (!found) break;
                cursor = static_cast<const char*>(found);
                if (static_cast<std::size_t>(end - cursor) > original_prefix.size() &&
                    std::memcmp(cursor, original_prefix.data(), original_prefix.size()) == 0) {
                    std::string name;
                    if (parse_literal_name(cursor, end, name)) literals.emplace(reinterpret_cast<std::uintptr_t>(cursor), std::move(name));
                }
                ++cursor;
            }
        }

        void collect_references(const memory_range& range, const std::vector<memory_range>& ranges, const literal_map& literals, std::vector<code_reference>& references) {
            const std::uintptr_t begin = (range.begin + 3U) & ~std::uintptr_t{3};
            if (range.end < begin + sizeof(std::uint32_t)) return;
            const std::size_t count = (range.end - begin) / sizeof(std::uint32_t);
            const auto* words = reinterpret_cast<const std::uint32_t*>(begin);
            for (std::size_t index = 0; index < count; ++index) {
                const std::uint32_t instruction = words[index];
                const bool pc_relative = (instruction & 0x1F000000U) == 0x10000000U || (instruction & 0xFF000000U) == 0x58000000U;
                if (!pc_relative) continue;
                const std::uintptr_t address = begin + index * sizeof(std::uint32_t);
                std::uintptr_t target = 0;
                if (!decode_target(words + index, std::min(count - index, add_search_window + 1), address, ranges, target)) continue;
                if (literals.find(target) != literals.end()) references.push_back({address, target});
            }
        }

        std::size_t count_pointer_hits(const std::vector<memory_range>& unity_ranges, const literal_map& literals) {
            std::size_t hits = 0;
            for (const auto& range : unity_ranges) {
                if (!range.readable) continue;
                const std::uintptr_t begin = (range.begin + 7U) & ~std::uintptr_t{7};
                if (range.end < begin + sizeof(std::uintptr_t)) continue;
                const std::size_t count = (range.end - begin) / sizeof(std::uintptr_t);
                const auto* values = reinterpret_cast<const std::uintptr_t*>(begin);
                for (std::size_t index = 0; index < count; ++index) {
                    if (literals.find(values[index]) != literals.end()) ++hits;
                }
            }
            return hits;
        }

        bool order_delta_entries(const delta_entry& left, const delta_entry& right) {
            if (left.score != right.score) return left.score > right.score;
            const bool left_known = left.delta == known_name_delta;
            const bool right_known = right.delta == known_name_delta;
            if (left_known != right_known) return left_known;
            const std::int64_t left_distance = left.delta < 0 ? -left.delta : left.delta;
            const std::int64_t right_distance = right.delta < 0 ? -right.delta : right.delta;
            return left_distance < right_distance;
        }

        std::vector<delta_entry> score_deltas(const std::vector<code_reference>& references, const std::vector<memory_range>& ranges, const literal_map& literals, std::size_t stride) {
            std::vector<delta_entry> entries;
            for (std::int64_t delta = -delta_window; delta <= delta_window; delta += 4) {
                if (delta == 0) continue;
                if (stride && (delta >= 0 || -delta >= static_cast<std::int64_t>(stride))) continue;
                std::unordered_set<std::uintptr_t> distinct;
                std::string sample;
                for (const auto& reference : references) {
                    std::uintptr_t source = 0;
                    if (!add_signed_address(reference.address, delta, source)) continue;
                    std::uintptr_t target = 0;
                    if (!decode_at(source, ranges, target)) continue;
                    if (literals.find(target) != literals.end()) continue;
                    std::string text;
                    if (!read_c_string(target, ranges, max_name_length, text)) continue;
                    if (text.compare(0, 8, "il2cpp: ") == 0) continue;
                    if (distinct.insert(target).second && sample.empty()) sample = text;
                }
                if (!distinct.empty()) entries.push_back({delta, distinct.size(), std::move(sample)});
            }
            std::sort(entries.begin(), entries.end(), order_delta_entries);
            return entries;
        }

        bool select_delta(const std::vector<delta_entry>& entries, delta_entry& chosen, bool& ambiguous) {
            ambiguous = false;
            if (entries.empty() || entries.front().score < min_resolved_names) return false;
            for (const auto& entry : entries) {
                if (entry.delta == known_name_delta && entry.score + 2 >= entries.front().score) {
                    chosen = entry;
                    return true;
                }
            }
            if (entries.size() > 1 && entries[1].score == entries[0].score) {
                ambiguous = true;
                return false;
            }
            chosen = entries.front();
            return true;
        }

        bool decode_paired_reference(std::uintptr_t address, std::uint32_t register_index, const std::vector<memory_range>& ranges, std::uintptr_t& target) {
            std::uint32_t words[add_search_window + 1];
            std::size_t available = 0;
            for (; available < add_search_window + 1; ++available) {
                const std::uintptr_t current = address + available * sizeof(std::uint32_t);
                const memory_range* range = find_memory_range(ranges, current, sizeof(std::uint32_t));
                if (!range || !range->readable) break;
                std::memcpy(&words[available], reinterpret_cast<const void*>(current), sizeof(std::uint32_t));
            }
            if (available < 2) return false;
            if ((words[0] & 0x9F000000U) != 0x90000000U || (words[0] & 0x1FU) != register_index) return false;
            std::uintptr_t page = 0;
            if (!add_signed_address(address & ~std::uintptr_t{0xFFF}, adr_immediate(words[0]) * 4096, page)) return false;
            for (std::size_t index = 1; index < available; ++index) {
                const std::uint32_t next = words[index];
                if ((next & 0xFF000000U) != 0x91000000U || ((next >> 5U) & 0x1FU) != register_index) continue;
                std::uintptr_t addend = (next >> 10U) & 0xFFFU;
                if ((next >> 22U) & 1U) addend <<= 12U;
                target = page + addend;
                return true;
            }
            return false;
        }

        bool find_irregular_name(const code_reference& reference, std::uintptr_t lower_limit, std::uint32_t register_index, const std::vector<memory_range>& ranges, const literal_map& literals, const std::unordered_set<std::uintptr_t>& regular_targets, std::uintptr_t& target, std::string& name) {
            for (std::size_t step = 1; step <= irregular_search_bytes / sizeof(std::uint32_t); ++step) {
                const std::uintptr_t distance = step * sizeof(std::uint32_t);
                if (reference.address < distance) break;
                const std::uintptr_t address = reference.address - distance;
                if (address < lower_limit) break;
                std::uintptr_t candidate = 0;
                if (!decode_paired_reference(address, register_index, ranges, candidate)) continue;
                if (literals.find(candidate) != literals.end() || regular_targets.find(candidate) != regular_targets.end()) continue;
                if (!read_c_string(candidate, ranges, max_name_length, name) || name.compare(0, 8, "il2cpp: ") == 0) continue;
                target = candidate;
                return true;
            }
            return false;
        }

        std::uint32_t dominant_register(const std::vector<code_reference>& references, std::int64_t delta, const std::vector<memory_range>& ranges) {
            std::map<std::uint32_t, std::size_t> histogram;
            for (const auto& reference : references) {
                std::uintptr_t source = 0;
                if (!add_signed_address(reference.address, delta, source)) continue;
                const memory_range* range = find_memory_range(ranges, source, sizeof(std::uint32_t));
                if (!range || !range->readable) continue;
                std::uint32_t word = 0;
                std::memcpy(&word, reinterpret_cast<const void*>(source), sizeof(word));
                if ((word & 0x9F000000U) == 0x90000000U) ++histogram[word & 0x1FU];
            }
            std::uint32_t best_register = 1;
            std::size_t best_count = 0;
            for (const auto& item : histogram) {
                if (item.second > best_count) {
                    best_count = item.second;
                    best_register = item.first;
                }
            }
            return best_register;
        }

        std::size_t find_stride(const std::vector<code_reference>& references) {
            std::map<std::size_t, std::size_t> histogram;
            for (std::size_t index = 1; index < references.size(); ++index) {
                const std::size_t difference = references[index].address - references[index - 1].address;
                if (difference >= 4 && difference <= max_stride) ++histogram[difference];
            }
            std::size_t best_stride = 0;
            std::size_t best_count = 0;
            for (const auto& item : histogram) {
                if (item.second > best_count) {
                    best_count = item.second;
                    best_stride = item.first;
                }
            }
            return best_count + 1 >= min_resolved_names ? best_stride : 0;
        }

        std::vector<code_reference> filter_by_stride(const std::vector<code_reference>& references, std::size_t stride) {
            if (!stride) return references;
            std::vector<code_reference> kept;
            for (std::size_t index = 0; index < references.size(); ++index) {
                const bool has_previous = index > 0 && references[index].address - references[index - 1].address == stride;
                const bool has_next = index + 1 < references.size() && references[index + 1].address - references[index].address == stride;
                if (has_previous || has_next) kept.push_back(references[index]);
            }
            return kept.size() >= min_resolved_names ? kept : references;
        }

        std::string dump_words(std::uintptr_t center, const std::vector<memory_range>& ranges) {
            std::string text;
            const std::int64_t first = -static_cast<std::int64_t>(max_diagnostic_words * 2);
            for (std::int64_t offset = first; offset < static_cast<std::int64_t>(max_diagnostic_words); offset += 1) {
                std::uintptr_t address = 0;
                if (!add_signed_address(center, offset * 4, address)) continue;
                const memory_range* range = find_memory_range(ranges, address, sizeof(std::uint32_t));
                if (!range || !range->readable) continue;
                std::uint32_t word = 0;
                std::memcpy(&word, reinterpret_cast<const void*>(address), sizeof(word));
                text += format_text("%s%08X", text.empty() ? "" : " ", word);
            }
            return text;
        }

        int collect_module(dl_phdr_info* info, std::size_t, void* opaque) {
            auto* module = static_cast<std::pair<std::string, std::uintptr_t>*>(opaque);
            const std::string path = info->dlpi_name ? info->dlpi_name : "";
            if (!is_unity_path(path)) return 0;
            module->first = path;
            module->second = static_cast<std::uintptr_t>(info->dlpi_addr);
            return 1;
        }

        int probe_unity_module(dl_phdr_info* info, std::size_t, void* opaque) {
            auto* found = static_cast<bool*>(opaque);
            const std::string path = info->dlpi_name ? info->dlpi_name : "";
            if (!is_unity_path(path)) return 0;
            *found = true;
            return 1;
        }

    }

    bool is_unity_module_loaded() {
#if !il2cpp_runtime_arm64
        return false;
#else
        bool found = false;
    dl_iterate_phdr(probe_unity_module, &found);
    if (found) return true;
    const std::vector<memory_range> ranges = read_memory_ranges();
    return std::any_of(ranges.begin(), ranges.end(), [](const memory_range& range) {
        return is_unity_path(range.path);
    });
#endif
    }

    bool detect_il2cpp_runtime(runtime_fingerprint& result) {
        result = {};
#if !il2cpp_runtime_arm64
        return false;
#else
        result.supported_architecture = true;
    result.expected_names = expected_name_count;

    const std::vector<memory_range> ranges = read_memory_ranges();
    if (ranges.empty()) return false;

    std::vector<memory_range> unity_ranges;
    for (const auto& range : ranges) {
        if (is_unity_path(range.path)) unity_ranges.push_back(range);
    }

    std::pair<std::string, std::uintptr_t> module;
    dl_iterate_phdr(collect_module, &module);
    result.module_found = !module.first.empty() || !unity_ranges.empty();
    result.module_path = !module.first.empty() ? module.first : (unity_ranges.empty() ? std::string() : unity_ranges.front().path);
    result.module_base = module.second ? module.second : (unity_ranges.empty() ? 0 : unity_ranges.front().begin);

    std::size_t executable_ranges = 0;
    std::size_t unreadable_executable_ranges = 0;
    std::size_t readable_ranges = 0;
    for (const auto& range : unity_ranges) {
        if (range.readable) ++readable_ranges;
        if (!range.executable) continue;
        if (range.readable) ++executable_ranges;
        else ++unreadable_executable_ranges;
    }
    result.executable_segments_found = executable_ranges > 0;
    result.diagnostics.push_back(format_text("unity ranges=%zu readable=%zu executable_readable=%zu executable_unreadable=%zu",
        unity_ranges.size(), readable_ranges, executable_ranges, unreadable_executable_ranges));
    if (!result.executable_segments_found) return false;

    literal_map literals;
    for (const auto& range : unity_ranges) {
        if (range.readable) collect_literals(range, literals);
    }
    result.literal_count = literals.size();
    if (literals.empty()) {
        result.diagnostics.push_back("no 'il2cpp: function ' literal was found in readable libunity mappings");
        return false;
    }

    std::vector<code_reference> references;
    for (const auto& range : unity_ranges) {
        if (range.readable && range.executable) collect_references(range, ranges, literals, references);
    }
    std::sort(references.begin(), references.end(), [](const code_reference& left, const code_reference& right) {
        return left.address < right.address;
    });
    references.erase(std::unique(references.begin(), references.end(), [](const code_reference& left, const code_reference& right) {
        return left.address == right.address;
    }), references.end());
    result.reference_count = references.size();
    result.references_found = !references.empty();

    if (references.empty()) {
        const auto first_literal = literals.begin();
        result.diagnostics.push_back(format_text("literal sample %p name=%s pointer_hits=%zu",
            reinterpret_cast<void*>(first_literal->first), first_literal->second.c_str(), count_pointer_hits(unity_ranges, literals)));
        return false;
    }

    result.first_reference = references.front().address;

    result.stride = find_stride(references);
    const std::vector<delta_entry> entries = score_deltas(references, ranges, literals, result.stride);
    for (std::size_t index = 0; index < entries.size() && index < 3; ++index) {
        result.diagnostics.push_back(format_text("delta candidate %lld score=%zu sample=%.60s",
            static_cast<long long>(entries[index].delta), entries[index].score, entries[index].sample.c_str()));
    }
    result.diagnostics.push_back(format_text("first reference %p words: %s",
        reinterpret_cast<void*>(references.front().address), dump_words(references.front().address, ranges).c_str()));

    delta_entry chosen;
    if (!select_delta(entries, chosen, result.ambiguous)) return false;

    const std::int64_t delta = chosen.delta;
    result.name_delta = delta;
    result.delta_score = chosen.score;
    const std::vector<code_reference> selected = filter_by_stride(references, result.stride);
    const std::uint32_t register_index = dominant_register(selected, delta, ranges);

    std::unordered_set<std::uintptr_t> regular_targets;
    for (const auto& reference : selected) {
        std::uintptr_t source = 0;
        std::uintptr_t target = 0;
        if (add_signed_address(reference.address, delta, source) && decode_at(source, ranges, target)) regular_targets.insert(target);
    }

    std::unordered_map<std::string, std::string> resolved;
    std::size_t irregular_count = 0;
    for (std::size_t index = 0; index < selected.size(); ++index) {
        const code_reference& reference = selected[index];
        std::uintptr_t source = 0;
        std::uintptr_t target = 0;
        std::string fake_name;
        bool found = add_signed_address(reference.address, delta, source) && decode_at(source, ranges, target) &&
                     literals.find(target) == literals.end() && read_c_string(target, ranges, max_name_length, fake_name) &&
                     fake_name.compare(0, 8, "il2cpp: ") != 0;
        if (!found) {
            const std::uintptr_t lower_limit = index > 0 ? selected[index - 1].address + sizeof(std::uint32_t) : 0;
            found = find_irregular_name(reference, lower_limit, register_index, ranges, literals, regular_targets, target, fake_name);
            if (found) ++irregular_count;
        }
        if (!found) continue;
        const std::string& original_name = literals.at(reference.literal);
        const auto existing = resolved.find(original_name);
        if (existing != resolved.end()) {
            if (existing->second != fake_name) ++result.conflict_count;
            continue;
        }
        resolved.emplace(original_name, fake_name);
        result.names.push_back({original_name, fake_name});
    }
    if (irregular_count) result.diagnostics.push_back(format_text("records resolved through the irregular-record fallback: %zu", irregular_count));
    result.expected_names = selected.size();

    result.resolved_names = result.names.size();
    result.table_resolved = result.resolved_names >= min_resolved_names;
    if (!result.table_resolved) return false;

    std::lock_guard<std::mutex> lock(runtime_names_mutex);
    runtime_names.clear();
    for (const auto& item : result.names) runtime_names[item.original_name] = item.runtime_name;
    return true;
#endif
    }

    std::size_t runtime_name_count() {
        std::lock_guard<std::mutex> lock(runtime_names_mutex);
        return runtime_names.size();
    }

    bool find_runtime_name(const char* original_name, std::string& runtime_name) {
        if (!original_name || !*original_name) return false;
        std::lock_guard<std::mutex> lock(runtime_names_mutex);
        const auto found = runtime_names.find(original_name);
        if (found == runtime_names.end()) return false;
        runtime_name = found->second;
        return !runtime_name.empty();
    }

}