#pragma once

#include "../UserSettings/GlobalSettings.hpp"
#include "../Il2CppHeaders.hpp"
#include "String.hpp"

namespace BNM::Structures::Mono {

    /**
        @brief System.Exception struct implementation in BNM matching Il2CppException layout.
    */
    struct Exception : BNM::IL2CPP::Il2CppObject {
        String *className{};
        String *message{};
        void *_data{};
        Exception *innerException{};
        String *helpURL{};
        void *trace_ips{};
        String *stack_trace{};
        String *remote_stack_trace{};
        int remote_stack_index{};
        void *dynamic_methods{};
        int hresult{};
        String *source{};
        void *safeSerializationManager{};
        void *captured_traces{};
        void *native_trace_ips{};

        /**
            @brief Get exception message.
            @return Mono String message.
        */
        [[nodiscard]] inline String *GetMessage() const { return message; }

        /**
            @brief Get exception stack trace string.
            @return Mono String stack trace.
        */
        [[nodiscard]] inline String *GetStackTrace() const { return stack_trace; }

        /**
            @brief Get help URL.
            @return Help URL string.
        */
        [[nodiscard]] inline String *GetHelpURL() const { return helpURL; }

        /**
            @brief Get inner exception if present.
            @return Pointer to inner exception.
        */
        [[nodiscard]] inline Exception *GetInnerException() const { return innerException; }

        /**
            @brief Get exception source string.
            @return Source string.
        */
        [[nodiscard]] inline String *GetSource() const { return source; }

        /**
            @brief Get HRESULT error code.
            @return Error code integer.
        */
        [[nodiscard]] inline int GetHResult() const { return hresult; }
    };

}
