#pragma once

#include <cstdint>
#include <cstring>
#include <BNM/UserSettings/GlobalSettings.hpp>

namespace BNM::AssemblerUtils {

    /**
        @brief Direct binary disassembly utilities for decoding branch / call target addresses.
        @note Pure bitwise manipulation without string allocations for high performance.
    */

#if defined(__ARM_ARCH_7A__)

    // ARM32 (A32): B / BL instruction check and target decoding
    // Encoding: cond(4) 101L(4) imm24(24)
    static inline bool IsBranch(uint32_t insn) {
        return (insn & 0x0E000000) == 0x0A000000;
    }

    static inline bool DecodeBranchOrCall(uint32_t insn, BNM_PTR pc, BNM_PTR &outTarget) {
        if (!IsBranch(insn)) return false;
        // imm24 is sign-extended and shifted left by 2 bits. In ARM state, PC is address + 8.
        int32_t imm24 = (int32_t)(insn & 0x00FFFFFF);
        int32_t offset = (imm24 << 8) >> 6; // Sign-extend 24-bit to 32-bit and multiply by 4
        outTarget = (BNM_PTR)((int64_t)pc + 8 + offset);
        return true;
    }

    static inline BNM_PTR FindNextJump(BNM_PTR start, uint8_t index) {
        if (!start) return 0;
        BNM_PTR curAddr = start;
        BNM_PTR target = 0;
        size_t maxInsn = 256;
        while (index > 0 && maxInsn-- > 0) {
            uint32_t insn = *(const uint32_t *)curAddr;
            // Stop scanning if return instruction is hit (BX LR or POP {..., PC})
            if (insn == 0xE12FFF1E || (insn & 0x0FFF0000) == 0x08BD0000) break;
            if (DecodeBranchOrCall(insn, curAddr, target)) {
                index--;
                if (index == 0) return target;
            }
            curAddr += 4;
        }
        return target;
    }

#elif defined(__aarch64__)

    // ARM64 (A64): B / BL instruction check and target decoding
    // Encoding: op(1) 00101(5) imm26(26) -> op=0 for B, op=1 for BL
    static inline bool IsBranch(uint32_t insn) {
        return (insn & 0x7C000000) == 0x14000000;
    }

    static inline bool DecodeBranchOrCall(uint32_t insn, BNM_PTR pc, BNM_PTR &outTarget) {
        if (!IsBranch(insn)) return false;
        // imm26 is sign-extended and shifted left by 2 bits.
        int32_t imm26 = (int32_t)(insn & 0x03FFFFFF);
        int64_t offset = (int64_t)((imm26 << 6) >> 4); // Sign-extend 26-bit to 64-bit and multiply by 4
        outTarget = (BNM_PTR)((int64_t)pc + offset);
        return true;
    }

    static inline BNM_PTR FindNextJump(BNM_PTR start, uint8_t index) {
        if (!start) return 0;
        BNM_PTR curAddr = start;
        BNM_PTR target = 0;
        size_t maxInsn = 256;
        while (index > 0 && maxInsn-- > 0) {
            uint32_t insn = *(const uint32_t *)curAddr;
            // Stop scanning if return instruction is hit (RET)
            if ((insn & 0xFFFFFC1F) == 0xD65F0000) break;
            if (DecodeBranchOrCall(insn, curAddr, target)) {
                index--;
                if (index == 0) return target;
            }
            curAddr += 4;
        }
        return target;
    }

#elif defined(__i386__) || defined(__x86_64__)

    // x86 / x86_64: Relative near CALL (opcode 0xE8)
    static inline bool IsCall(uint8_t opcode) {
        return opcode == 0xE8;
    }

    static inline bool DecodeBranchOrCall(const uint8_t *ptr, BNM_PTR pc, BNM_PTR &outTarget) {
        if (!IsCall(*ptr)) return false;
        int32_t relOffset = *(const int32_t *)(ptr + 1);
        // Target = next instruction address (pc + 5) + relative 32-bit offset
        outTarget = (BNM_PTR)((int64_t)pc + 5 + relOffset);
        return true;
    }

    static inline BNM_PTR FindNextJump(BNM_PTR start, uint8_t index) {
        if (!start) return 0;
        BNM_PTR curAddr = start;
        BNM_PTR target = 0;
        size_t maxBytes = 1024;
        while (index > 0 && maxBytes-- > 0) {
            const uint8_t *p = (const uint8_t *)curAddr;
            // Stop scanning if return instruction is hit (RET / RET imm16)
            if (*p == 0xC3 || *p == 0xC2) break;
            if (DecodeBranchOrCall(p, curAddr, target)) {
                index--;
                if (index == 0) return target;
            }
            curAddr += 1;
        }
        return target;
    }

#else
#error "BNM only supports arm64, arm, x86 and x86_64"
#endif

}
