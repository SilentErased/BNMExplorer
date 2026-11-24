import idc
import idaapi
import ida_funcs
import ida_bytes

SIGNATURE = "? ? ? F8 ? ? ? A9 E1 03 1F 2A ? ? ? 97"
START_DELTA = 0x60
FAKE_NAME_DELTA = -0xC
ORIG_STR_DELTA = 0x10
STEP_DELTA = 0x30
LAST_OFFSET = 0x2AC0
TABLE_BASE_DELTA = 0
MAX_STRING_LENGTH = 512


def get_min_ea():
    if hasattr(idaapi, "inf_get_min_ea"):
        return idaapi.inf_get_min_ea()
    return idc.get_inf_attr(idc.INF_MIN_EA)


def get_max_ea():
    if hasattr(idaapi, "inf_get_max_ea"):
        return idaapi.inf_get_max_ea()
    return idc.get_inf_attr(idc.INF_MAX_EA)


def find_next_signature(start_ea, end_ea):
    flags = ida_bytes.BIN_SEARCH_FORWARD | ida_bytes.BIN_SEARCH_NOSHOW
    if hasattr(ida_bytes, "find_bytes"):
        return ida_bytes.find_bytes(SIGNATURE, start_ea, range_end=end_ea, flags=flags, radix=16)
    if hasattr(ida_bytes, "compiled_binpat_vec_t") and hasattr(ida_bytes, "parse_binpat_str"):
        patterns = ida_bytes.compiled_binpat_vec_t()
        parsed = ida_bytes.parse_binpat_str(patterns, start_ea, SIGNATURE, 16)
        if parsed:
            result = ida_bytes.bin_search(start_ea, end_ea, patterns, flags)
            if isinstance(result, tuple):
                return result[0]
            return result
    if hasattr(idc, "find_binary"):
        result = idc.find_binary(start_ea, idc.SEARCH_DOWN, SIGNATURE, 16)
        if result != idc.BADADDR and result < end_ea:
            return result
    return idc.BADADDR


def find_signatures():
    matches = []
    search_ea = get_min_ea()
    max_ea = get_max_ea()

    while search_ea < max_ea:
        match_ea = find_next_signature(search_ea, max_ea)
        if match_ea == idc.BADADDR or match_ea >= max_ea:
            break
        matches.append(match_ea)
        search_ea = match_ea + 1

    return matches


def resolve_ptr(ea):
    mnemonic = idc.print_insn_mnem(ea)
    if mnemonic in ("ADRL", "ADRP", "ADR"):
        value = idc.get_operand_value(ea, 1)
        if value and value != idc.BADADDR:
            if mnemonic == "ADRP":
                next_ea = ea + idc.get_item_size(ea)
                next_mnemonic = idc.print_insn_mnem(next_ea)
                if next_mnemonic == "ADD":
                    add_value = idc.get_operand_value(next_ea, 2)
                    if add_value:
                        value += add_value
            return value
    ptr = idc.get_qword(ea)
    if ptr and ptr != idc.BADADDR:
        return ptr
    return None


def read_str(ea):
    if not ea or ea == idc.BADADDR:
        return None
    value = idc.get_strlit_contents(ea, -1, idc.STRTYPE_C)
    if value:
        return value.decode("utf-8", errors="ignore")
    raw = bytearray()
    for offset in range(MAX_STRING_LENGTH):
        current_ea = ea + offset
        if hasattr(ida_bytes, "is_loaded") and not ida_bytes.is_loaded(current_ea):
            return None
        byte_value = ida_bytes.get_byte(current_ea)
        if byte_value == 0:
            return raw.decode("utf-8", errors="ignore") if raw else None
        if byte_value == 0xFF and not ida_bytes.is_loaded(current_ea):
            return None
        raw.append(byte_value)
    return None


def parse_original_name(error_str):
    parts = error_str.strip().split()
    if len(parts) >= 3 and parts[0] == "il2cpp:" and parts[1] == "function":
        return parts[2]
    return None


def dump_il2cpp_name_map(base_addr, print_results=True):
    results = []
    offset = START_DELTA
    total = (LAST_OFFSET // STEP_DELTA) + 1

    while offset <= START_DELTA + LAST_OFFSET:
        current_base = base_addr + offset
        fake_ptr = resolve_ptr(current_base + FAKE_NAME_DELTA)
        original_ptr = resolve_ptr(current_base + ORIG_STR_DELTA)
        fake_name = read_str(fake_ptr)
        original_str = read_str(original_ptr)

        if fake_name and original_str:
            original_name = parse_original_name(original_str)
            if original_name:
                results.append((original_name, fake_name))
                if print_results:
                    print("{:<40} {}".format(original_name, fake_name))

        offset += STEP_DELTA

    return results, total


def main():
    matches = find_signatures()
    if not matches:
        print("[-] signature not found")
        return

    print("[+] {} signature match(es)".format(len(matches)))
    candidates = {}

    for index, match_ea in enumerate(matches):
        function = ida_funcs.get_func(match_ea)
        function_start = function.start_ea if function else match_ea
        base_addr = function_start + TABLE_BASE_DELTA
        candidates[base_addr] = (match_ea, function_start)
        print("[{}] match EA=0x{:X} RVA=0x{:X}, function EA=0x{:X}".format(
            index, match_ea, match_ea - idaapi.get_imagebase(), function_start))

    scored = []
    for base_addr, addresses in candidates.items():
        results, total = dump_il2cpp_name_map(base_addr, False)
        scored.append((len(results), base_addr, addresses, results, total))

    scored.sort(key=lambda item: item[0], reverse=True)
    best_score, base_addr, addresses, results, total = scored[0]

    if best_score == 0:
        print("[-] signature matches found, but no name table resolved")
        print("[-] the signature identifies matching code, not necessarily the table base")
        print("[-] inspect the function and adjust TABLE_BASE_DELTA / START_DELTA")
        return

    if len(scored) > 1 and scored[1][0] == best_score:
        print("[-] multiple candidates produced the same result; refusing to guess")
        for score, candidate_base, candidate_addresses, candidate_results, candidate_total in scored[:10]:
            print("    base=0x{:X}, match=0x{:X}, resolved={}/{}".format(
                candidate_base, candidate_addresses[0], score, candidate_total))
        return

    match_ea, function_start = addresses
    print("[+] selected match EA=0x{:X}, function EA=0x{:X}, table base=0x{:X}".format(
        match_ea, function_start, base_addr))
    print("\n{:<40} {}".format("ORIGINAL", "IN-GAME (FAKE)"))
    print("-" * 70)
    for original_name, fake_name in results:
        print("{:<40} {}".format(original_name, fake_name))
    print("\n[*] {}/{} functions resolved".format(best_score, total))

    output_path = idaapi.get_user_idadir() + "/il2cpp_name_map.txt"
    try:
        with open(output_path, "w", encoding="utf-8") as output_file:
            output_file.write("{:<40} IN-GAME\n".format("ORIGINAL"))
            output_file.write("-" * 70 + "\n")
            for original_name, fake_name in results:
                output_file.write("{:<40} {}\n".format(original_name, fake_name))
        print("[+] saved to {}".format(output_path))
    except Exception as error:
        print("[-] file write failed: {}".format(error))


main()
