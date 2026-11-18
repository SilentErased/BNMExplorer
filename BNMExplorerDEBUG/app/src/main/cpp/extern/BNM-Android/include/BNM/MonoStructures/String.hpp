#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include <cctype>
#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "../DebugMessages.hpp"
#include "../Utils.hpp"

namespace BNM::Structures::Mono {

    struct String;

    /// @cond
    namespace Detail {
        inline std::string FormatArgToString(const char *s) { return s ? std::string(s) : std::string("null"); }
        inline std::string FormatArgToString(const std::string &s) { return s; }
        inline std::string FormatArgToString(const std::string_view &s) { return std::string(s); }
        inline std::string FormatArgToString(bool b) { return b ? "True" : "False"; }
        inline std::string FormatArgToString(char c) { return std::string(1, c); }
        inline std::string FormatArgToString(const void *ptr) {
            char buf[32];
            snprintf(buf, sizeof(buf), "%p", ptr);
            return std::string(buf);
        }
        template<typename T>
        inline auto FormatArgToString(const T &val) -> decltype(std::to_string(val)) {
            return std::to_string(val);
        }

        inline std::string FormatHelper(const std::string_view &fmt, const std::vector<std::string> &args) {
            std::string result{};
            result.reserve(fmt.size() + 64);
            for (size_t i = 0; i < fmt.size(); ++i) {
                if (fmt[i] == '{') {
                    if (i + 1 < fmt.size() && fmt[i + 1] == '{') {
                        result.push_back('{');
                        ++i;
                        continue;
                    }
                    size_t closePos = fmt.find('}', i);
                    if (closePos != std::string_view::npos) {
                        auto indexStr = fmt.substr(i + 1, closePos - i - 1);
                        bool isNum = !indexStr.empty() && std::all_of(indexStr.begin(), indexStr.end(), [](char c){ return std::isdigit(c); });
                        if (isNum) {
                            size_t argIdx = (size_t)std::stoul(std::string(indexStr));
                            if (argIdx < args.size()) {
                                result.append(args[argIdx]);
                            }
                            i = closePos;
                            continue;
                        }
                    }
                } else if (fmt[i] == '}' && i + 1 < fmt.size() && fmt[i + 1] == '}') {
                    result.push_back('}');
                    ++i;
                    continue;
                }
                result.push_back(fmt[i]);
            }
            return result;
        }
    }
    /// @endcond

    /**
        @brief C# System.String representation in BNM.
        @note Matches Il2CppString memory layout and provides rich C++ & C# string helper methods.
    */
    struct String : BNM::IL2CPP::Il2CppObject {
        int length{};
        IL2CPP::Il2CppChar chars[0];

        /**
            @brief Convert Mono String (UTF-16) to C++ std::string (UTF-8).
            @return Converted UTF-8 string, or error string if dead.
        */
        std::string str();

        /**
            @brief Calculate string hash code identical to IL2CPP VM.
            @return Hash integer.
        */
        [[nodiscard]] unsigned int GetHash() const;

        /**
            @brief Get C# String.Empty instance.
            @return Pointer to empty Mono String.
        */
        static String *Empty();

        /**
            @brief Create a Mono String from a C++ string_view.
            @param str UTF-8 string view.
            @return Pointer to new Mono String.
        */
        static inline String *Create(const std::string_view &str) {
            return BNM::CreateMonoString(str);
        }

        /**
            @brief Create a Mono String from a C string pointer.
            @param str C-string pointer.
            @return Pointer to new Mono String.
        */
        static inline String *Create(const char *str) {
            return BNM::CreateMonoString(str ? std::string_view(str) : std::string_view(""));
        }

        /**
            @brief Create a Mono String from std::string.
            @param str Source std::string.
            @return Pointer to new Mono String.
        */
        static inline String *Create(const std::string &str) {
            return BNM::CreateMonoString(str);
        }

        /**
            @brief Get string length in characters.
            @return Character count.
        */
        [[nodiscard]] inline int GetLength() const { return length; }

        /**
            @brief Alias for GetLength().
            @return Character count.
        */
        [[nodiscard]] inline int Length() const { return length; }

        /**
            @brief Get raw pointer to UTF-16 characters buffer.
            @return Character buffer pointer.
        */
        [[nodiscard]] inline const IL2CPP::Il2CppChar *GetChars() const { return chars; }

        /**
            @brief Check if string is null or empty.
            @return True if null or length is 0.
        */
        [[nodiscard]] inline bool IsNullOrEmpty() const { return !BNM::CheckForNull(this) || !length; }

        /**
            @brief Check if string is empty.
            @return True if null or empty.
        */
        [[nodiscard]] inline bool IsEmpty() const { return IsNullOrEmpty(); }

        /**
            @brief Access character at given index.
            @param index 0-based character index.
            @return UTF-16 character, or 0 if out of bounds.
        */
        inline IL2CPP::Il2CppChar operator[](int index) const {
            if (index < 0 || index >= length) return 0;
            return chars[index];
        }

        /**
            @brief Convert implicitly to std::string.
        */
        inline operator std::string() { return str(); }

#ifdef BNM_ALLOW_SELF_CHECKS
        /**
            @brief Self-check for null pointer in debug builds.
            @return True if valid.
        */
        [[nodiscard]] bool SelfCheck() const;
#endif

        //! =========================================================================================
        //! String Manipulation & Search Methods
        //! =========================================================================================

        /**
            @brief Check if string contains substring.
            @param substr Substring to search for.
            @return True if found.
        */
        inline bool Contains(const std::string_view &substr) {
            if (IsNullOrEmpty()) return false;
            return str().find(substr) != std::string::npos;
        }

        /**
            @brief Check if string contains another Mono String.
            @param substr Mono String to search for.
            @return True if found.
        */
        inline bool Contains(String *substr) {
            if (!substr) return false;
            return Contains(substr->str());
        }

        /**
            @brief Check if string starts with prefix.
            @param prefix Prefix to check.
            @return True if starts with prefix.
        */
        inline bool StartsWith(const std::string_view &prefix) {
            if (IsNullOrEmpty()) return false;
            auto s = str();
            return s.rfind(prefix, 0) == 0;
        }

        /**
            @brief Check if string starts with another Mono String prefix.
            @param prefix Mono String prefix.
            @return True if starts with prefix.
        */
        inline bool StartsWith(String *prefix) {
            if (!prefix) return false;
            return StartsWith(prefix->str());
        }

        /**
            @brief Check if string ends with suffix.
            @param suffix Suffix to check.
            @return True if ends with suffix.
        */
        inline bool EndsWith(const std::string_view &suffix) {
            if (IsNullOrEmpty() || suffix.length() > (size_t)length) return false;
            auto s = str();
            if (suffix.length() > s.length()) return false;
            return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
        }

        /**
            @brief Check if string ends with another Mono String suffix.
            @param suffix Mono String suffix.
            @return True if ends with suffix.
        */
        inline bool EndsWith(String *suffix) {
            if (!suffix) return false;
            return EndsWith(suffix->str());
        }

        /**
            @brief Find first index of substring.
            @param substr Substring to search.
            @param start Start search index.
            @return 0-based index or -1 if not found.
        */
        inline int IndexOf(const std::string_view &substr, size_t start = 0) {
            if (IsNullOrEmpty()) return -1;
            auto pos = str().find(substr, start);
            return pos == std::string::npos ? -1 : (int)pos;
        }

        /**
            @brief Find first index of character.
            @param c Character to search.
            @param start Start search index.
            @return 0-based index or -1 if not found.
        */
        inline int IndexOf(char c, size_t start = 0) {
            if (IsNullOrEmpty()) return -1;
            auto pos = str().find(c, start);
            return pos == std::string::npos ? -1 : (int)pos;
        }

        /**
            @brief Find last index of substring.
            @param substr Substring to search.
            @return 0-based index or -1 if not found.
        */
        inline int LastIndexOf(const std::string_view &substr) {
            if (IsNullOrEmpty()) return -1;
            auto pos = str().rfind(substr);
            return pos == std::string::npos ? -1 : (int)pos;
        }

        /**
            @brief Find last index of character.
            @param c Character to search.
            @return 0-based index or -1 if not found.
        */
        inline int LastIndexOf(char c) {
            if (IsNullOrEmpty()) return -1;
            auto pos = str().rfind(c);
            return pos == std::string::npos ? -1 : (int)pos;
        }

        /**
            @brief Extract substring from startIndex to the end.
            @param startIndex Starting index.
            @return New Mono String.
        */
        inline String *Substring(int startIndex) {
            if (IsNullOrEmpty() || startIndex < 0 || startIndex >= length) return Empty();
            auto s = str();
            return Create(s.substr(startIndex));
        }

        /**
            @brief Extract substring with given length.
            @param startIndex Starting index.
            @param subLength Substring length.
            @return New Mono String.
        */
        inline String *Substring(int startIndex, int subLength) {
            if (IsNullOrEmpty() || startIndex < 0 || subLength <= 0) return Empty();
            auto s = str();
            if (startIndex >= (int)s.length()) return Empty();
            return Create(s.substr(startIndex, subLength));
        }

        /**
            @brief Returns copy converted to lower-case.
            @return New lower-case Mono String.
        */
        inline String *ToLower() {
            if (IsNullOrEmpty()) return Empty();
            auto s = str();
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
            return Create(s);
        }

        /**
            @brief Returns copy converted to upper-case.
            @return New upper-case Mono String.
        */
        inline String *ToUpper() {
            if (IsNullOrEmpty()) return Empty();
            auto s = str();
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
            return Create(s);
        }

        /**
            @brief Trim leading and trailing whitespace.
            @return Trimmed Mono String.
        */
        inline String *Trim() {
            if (IsNullOrEmpty()) return Empty();
            auto s = str();
            size_t start = s.find_first_not_of(" \t\n\r\f\v");
            if (start == std::string::npos) return Empty();
            size_t end = s.find_last_not_of(" \t\n\r\f\v");
            return Create(s.substr(start, end - start + 1));
        }

        /**
            @brief Replace all occurrences of oldValue with newValue.
            @param oldValue Target substring to replace.
            @param newValue Replacement substring.
            @return New Mono String.
        */
        inline String *Replace(const std::string_view &oldValue, const std::string_view &newValue) {
            if (IsNullOrEmpty() || oldValue.empty()) return this;
            auto s = str();
            size_t pos = 0;
            while ((pos = s.find(oldValue, pos)) != std::string::npos) {
                s.replace(pos, oldValue.length(), newValue);
                pos += newValue.length();
            }
            return Create(s);
        }

        /**
            @brief Concatenate two Mono Strings.
            @param s1 First string.
            @param s2 Second string.
            @return Concatenated Mono String.
        */
        static inline String *Concat(String *s1, String *s2) {
            std::string res{};
            if (s1 && !s1->IsNullOrEmpty()) res += s1->str();
            if (s2 && !s2->IsNullOrEmpty()) res += s2->str();
            return Create(res);
        }

        /**
            @brief Concatenate multiple Mono Strings.
            @param strings Vector of string pointers.
            @return Concatenated Mono String.
        */
        static inline String *Concat(const std::vector<String *> &strings) {
            std::string res{};
            for (auto *s : strings) {
                if (s && !s->IsNullOrEmpty()) res += s->str();
            }
            return Create(res);
        }

        //! =========================================================================================
        //! Comparison Operators
        //! =========================================================================================

        inline bool operator==(const std::string_view &other) {
            if (IsNullOrEmpty()) return other.empty();
            return str() == other;
        }

        inline bool operator==(String *other) {
            if (this == other) return true;
            if (!other) return false;
            if (length != other->length) return false;
            return str() == other->str();
        }

        inline bool operator!=(const std::string_view &other) { return !(*this == other); }
        inline bool operator!=(String *other) { return !(*this == other); }

        //! =========================================================================================
        //! Universal String::Format Implementation (C++ & C# Support, Mixed Types)
        //! =========================================================================================

        /**
            @brief Format string using C# composite format style ({0}, {1}, {2}, etc.) with any C++ or Mono arguments.
            @tparam Args Variadic argument types (Mono::String*, const char*, std::string, int, float, bool, etc.).
            @param fmt Format string view.
            @param args Values to insert into placeholders.
            @return Formatted new Mono String.
        */
        template<typename ...Args>
        static inline String *Format(const std::string_view &fmt, Args&&... args) {
            std::vector<std::string> argStrings{ Detail::FormatArgToString(args)... };
            return Create(Detail::FormatHelper(fmt, argStrings));
        }

        /**
            @brief Format string where format template itself is a Mono String.
            @tparam Args Variadic argument types.
            @param format Mono String template containing {0}, {1}, etc.
            @param args Values to insert into placeholders.
            @return Formatted new Mono String.
        */
        template<typename ...Args>
        static inline String *Format(String *format, Args&&... args) {
            if (!format || format->IsNullOrEmpty()) return Empty();
            return Format(format->str(), std::forward<Args>(args)...);
        }

        /**
            @brief Format string using standard C printf-style (%s, %d, %f, etc.).
            @param fmt Printf format string.
            @param ... Variadic printf arguments.
            @return Formatted new Mono String.
        */
        static inline String *FormatPrintf(const char *fmt, ...) {
            if (!fmt) return Empty();
            va_list args, args_copy;
            va_start(args, fmt);
            va_copy(args_copy, args);
            int size = vsnprintf(nullptr, 0, fmt, args);
            va_end(args);
            if (size <= 0) {
                va_end(args_copy);
                return Empty();
            }
            std::string buf((size_t)size, '\0');
            vsnprintf(&buf[0], (size_t)size + 1, fmt, args_copy);
            va_end(args_copy);
            return Create(buf);
        }

    private:
        inline constexpr String() : BNM::IL2CPP::Il2CppObject() {}
    };

    /// @cond
    namespace Detail {
        inline std::string FormatArgToString(const String *s) { return s ? const_cast<String*>(s)->str() : std::string("null"); }
        inline std::string FormatArgToString(String *s) { return s ? s->str() : std::string("null"); }
    }
    /// @endcond

}
