"""
BNM Game Binding Generator
==========================

Generates strongly-typed C++20 binding headers (`GB::...`) for a Unity IL2CPP
game from a Cpp2IL/Il2CppDumper-style `dump.cs` file. The output plugs into the
macro layer of `include/BNM/GameBinding.hpp` (see docs/game-binding.md).

Usage:
    python BNM_GameBinding_Generator.py <path_to_dump.cs> [output_directory]

    `output_directory` defaults to `GameBinding`; generated headers are written
    to `<output_directory>/GameBinding/` (plus an aggregate `GameBinding.hpp`
    that includes every generated header).

Typical flow:
    Il2CppDumper/Cpp2IL -> dump.cs -> this script -> include the GameBinding
    output directory in your mod project and `#include <GameBinding.hpp>`
    (or individual headers).

Key behaviors:
    * Compiler-generated C# types (state machines, closures, anonymous types)
      are skipped entirely.
    * BCL delegate families (System.Action/Func/...) map to native BNM types
      from BNM/Delegates.hpp instead of generated structs.
    * System.Collections[.Generic].IEnumerator maps to BNM's coroutine
      wrappers from BNM/Coroutine.hpp.
    * BNM-native Unity types (GameObject, MonoBehaviour, ...) are referenced
      from their BNM headers instead of being generated.
    * Generic types use the BNM_GAME_GENERIC_CLASS_1/2/3 wrappers when safe,
      falling back to the _EX escape hatch otherwise.
"""

import os
import re
import sys
import time
import bisect
from pathlib import Path

# ============================================================================
# C# primitive -> C++ type mapping
# ============================================================================
TYPE_MAP = {
    "void": "void",
    "bool": "bool",
    "byte": "uint8_t",
    "sbyte": "int8_t",
    "char": "char16_t",
    "short": "int16_t",
    "ushort": "uint16_t",
    "int": "int",
    "uint": "uint32_t",
    "long": "int64_t",
    "ulong": "uint64_t",
    "float": "float",
    "double": "double",
    "IntPtr": "intptr_t",
    "UIntPtr": "uintptr_t",
}

# C# value structs provided by BNM under BNM::Structures::Unity.
# Maps the C# type name to the BNM header that defines it (a few share one
# header: Color32->Color.hpp, Touch->Input.hpp, FrustumPlanes->Matrix4x4.hpp,
# Scene->SceneManager.hpp).
BNM_UNITY_VALUE_STRUCTS = {
    "Vector2": "Vector2.hpp", "Vector3": "Vector3.hpp", "Vector4": "Vector4.hpp",
    "Quaternion": "Quaternion.hpp", "Color": "Color.hpp", "Color32": "Color.hpp",
    "Matrix3x3": "Matrix3x3.hpp", "Matrix4x4": "Matrix4x4.hpp",
    "Ray": "Ray.hpp", "Bounds": "Bounds.hpp", "Rect": "Rect.hpp",
    "RaycastHit": "RaycastHit.hpp", "RaycastHit2D": "RaycastHit2D.hpp",
    "Touch": "Input.hpp", "FrustumPlanes": "Matrix4x4.hpp", "Scene": "SceneManager.hpp",
}

# Value structs that do NOT live in BNM::Structures::Unity (Touch is declared in
# BNM::UnityEngine, Scene in BNM::UnityEngine::SceneManagement with a
# BNM::UnityEngine::Scene alias).
BNM_VALUE_STRUCT_QUALNAMES = {
    "Touch": "BNM::UnityEngine::Touch",
    "Scene": "BNM::UnityEngine::SceneManagement::Scene",
}

# C# object types provided natively by BNM under BNM::UnityEngine.
# These are NOT generated; references map to BNM's own headers.
BNM_UNITY_OBJECT_TYPES = {
    "GameObject", "Transform", "RectTransform", "Component", "Behaviour",
    "MonoBehaviour", "ScriptableObject", "Object", "Camera", "Material", "Mesh",
    "Renderer", "MeshRenderer", "SkinnedMeshRenderer", "SpriteRenderer",
    "Collider", "Collider2D", "BoxCollider", "BoxCollider2D", "SphereCollider",
    "CapsuleCollider", "CircleCollider2D", "MeshCollider", "Rigidbody",
    "Rigidbody2D", "AudioSource", "AudioListener", "AudioClip", "Light",
    "Animator", "Animation", "AnimationClip", "AsyncOperation", "Canvas",
    "CanvasScaler", "CanvasHelper", "Graphic", "MaskableGraphic", "Image",
    "Text", "TextMeshPro", "TextMeshProUGUI", "TMP_Text", "TMP_InputField",
    "InputField", "Button", "Slider", "Toggle", "Selectable", "UIBehaviour",
    "Shader", "Texture", "Texture2D", "RenderTexture", "Screen", "Time",
    "Input", "Physics", "Physics2D", "PlayerPrefs", "Resources", "SceneManager",
    "Scene", "EventSystem", "GraphicRaycaster", "YieldInstruction", "UnityEvent",
    "UnityEventBase", "UnityAction", "Application", "SystemInfo",
}

# Maps each BNM-provided Unity object type to the header that defines it.
# Several families share one header (Physics*, Audio, UI, TextMeshPro, Events,
# Animation), so the include path cannot be derived from the type name alone.
BNM_UNITY_TYPE_HEADERS = {
    "GameObject": "GameObject.hpp", "Transform": "Transform.hpp",
    "RectTransform": "UI.hpp", "Component": "Component.hpp",
    "Behaviour": "Behaviour.hpp", "MonoBehaviour": "MonoBehaviour.hpp",
    "ScriptableObject": "ScriptableObject.hpp", "Object": "Object.hpp",
    "Camera": "Camera.hpp", "Material": "Material.hpp", "Mesh": "Mesh.hpp",
    "Renderer": "Renderer.hpp", "MeshRenderer": "MeshRenderer.hpp",
    "SkinnedMeshRenderer": "SkinnedMeshRenderer.hpp",
    "SpriteRenderer": "SpriteRenderer.hpp",
    "Collider": "Physics.hpp", "Collider2D": "Physics2D.hpp",
    "BoxCollider": "Physics.hpp", "BoxCollider2D": "Physics2D.hpp",
    "SphereCollider": "Physics.hpp", "CapsuleCollider": "Physics.hpp",
    "CircleCollider2D": "Physics2D.hpp", "MeshCollider": "Physics.hpp",
    "Rigidbody": "Physics.hpp", "Rigidbody2D": "Physics2D.hpp",
    "AudioSource": "Audio.hpp", "AudioListener": "Audio.hpp",
    "AudioClip": "Audio.hpp", "Light": "Light.hpp",
    "Animator": "Animation.hpp", "Animation": "Animation.hpp",
    "AnimationClip": "Animation.hpp", "AsyncOperation": "AsyncOperation.hpp",
    "Canvas": "UI.hpp", "CanvasScaler": "UI.hpp", "CanvasHelper": "UI.hpp",
    "Graphic": "UI.hpp", "MaskableGraphic": "UI.hpp", "Image": "UI.hpp",
    "Text": "UI.hpp", "TextMeshPro": "TextMeshPro.hpp",
    "TextMeshProUGUI": "TextMeshPro.hpp", "TMP_Text": "TextMeshPro.hpp",
    "TMP_InputField": "TextMeshPro.hpp", "InputField": "UI.hpp",
    "Button": "UI.hpp", "Slider": "UI.hpp", "Toggle": "UI.hpp",
    "Selectable": "UI.hpp", "UIBehaviour": "UI.hpp", "Shader": "Shader.hpp",
    "Texture": "Texture.hpp", "Texture2D": "Texture2D.hpp",
    "RenderTexture": "RenderTexture.hpp", "Screen": "Screen.hpp",
    "Time": "Time.hpp", "Input": "Input.hpp", "Physics": "Physics.hpp",
    "Physics2D": "Physics2D.hpp", "PlayerPrefs": "PlayerPrefs.hpp",
    "Resources": "Resources.hpp", "SceneManager": "SceneManager.hpp",
    "Scene": "SceneManager.hpp", "EventSystem": "UI.hpp",
    "GraphicRaycaster": "UI.hpp", "YieldInstruction": "AsyncOperation.hpp",
    "UnityEvent": "Events.hpp", "UnityEventBase": "Events.hpp",
    "UnityAction": "Events.hpp", "Application": "Application.hpp",
    "SystemInfo": "SystemInfo.hpp",
}

# BNM-provided types whose C++ qualname lives in a nested namespace that cannot
# be derived from the C# name (UnityEngine.UI.RectTransform lives in
# BNM::UnityEngine::UI, Scene lives in BNM::UnityEngine::SceneManagement, ...).
# convert_type/_resolve_base use this instead of blindly emitting
# BNM::UnityEngine::<Name>.
BNM_UNITY_QUALNAMES = {
    "RectTransform": "BNM::UnityEngine::UI::RectTransform",
    "Canvas": "BNM::UnityEngine::UI::Canvas",
    "CanvasScaler": "BNM::UnityEngine::UI::CanvasScaler",
    "CanvasHelper": "BNM::UnityEngine::UI::CanvasHelper",
    "Graphic": "BNM::UnityEngine::UI::Graphic",
    "MaskableGraphic": "BNM::UnityEngine::UI::MaskableGraphic",
    "Image": "BNM::UnityEngine::UI::Image",
    "Text": "BNM::UnityEngine::UI::Text",
    "InputField": "BNM::UnityEngine::UI::InputField",
    "Button": "BNM::UnityEngine::UI::Button",
    "Slider": "BNM::UnityEngine::UI::Slider",
    "Toggle": "BNM::UnityEngine::UI::Toggle",
    "Selectable": "BNM::UnityEngine::UI::Selectable",
    "UIBehaviour": "BNM::UnityEngine::UI::UIBehaviour",
    "GraphicRaycaster": "BNM::UnityEngine::UI::GraphicRaycaster",
    "EventSystem": "BNM::UnityEngine::UI::EventSystem",
    "TextMeshPro": "BNM::UnityEngine::TMPro::TextMeshPro",
    "TextMeshProUGUI": "BNM::UnityEngine::TMPro::TextMeshProUGUI",
    "TMP_Text": "BNM::UnityEngine::TMPro::TMP_Text",
    "TMP_InputField": "BNM::UnityEngine::TMPro::TMP_InputField",
    "Scene": "BNM::UnityEngine::SceneManagement::Scene",
    "SceneManager": "BNM::UnityEngine::SceneManagement::SceneManager",
}

# BNM Unity types that are C++ templates in BNM and therefore cannot be written
# bare: a field/param of C# type "UnityAction" maps to UnityAction<> *.
BNM_UNITY_VARIADIC_TEMPLATES = frozenset({"UnityAction", "UnityEvent", "InvokableCall"})


# BNM Mono types -> header (for include tracking)
BNM_MONO_HEADERS = {
    "String": "BNM/MonoStructures/String.hpp",
    "List": "BNM/MonoStructures/List.hpp",
    "Array": "BNM/MonoStructures/Array.hpp",
    "Dictionary": "BNM/MonoStructures/Dictionary.hpp",
    "Nullable": "BNM/MonoStructures/Nullable.hpp",
    "Delegates": "BNM/Delegates.hpp",
}

# BCL delegate families served natively by BNM (include/BNM/Delegates.hpp).
# Their generated binding files are skipped (System.Action_0/Func_1/...) and
# every reference maps to the BNM wrapper instead:
#   Action / Action<T...>          -> BNM::Structures::Mono::Action<T...> *
#   Func<Args..., R>               -> BNM::Delegate<R> *
#   Predicate<T>                   -> BNM::Delegate<bool> *
#   Comparison<T>                  -> BNM::Delegate<int> *
#   Converter<TIn, TOut>           -> BNM::Delegate<TOut> *
#   EventHandler[<T>]/AsyncCallback-> BNM::Delegate<void> *
#   Delegate                       -> BNM::Delegate<void> *
#   MulticastDelegate              -> BNM::MulticastDelegate<void> *
BNM_DELEGATE_FAMILIES = frozenset({
    "Action", "Func", "Predicate", "Comparison", "EventHandler",
    "Converter", "AsyncCallback", "Delegate", "MulticastDelegate",
})

# BCL enumerator interfaces served natively by BNM's coroutine wrappers
# (include/BNM/Coroutine.hpp). Their generated binding files are skipped and
# every reference maps to the native type instead:
#   System.Collections.IEnumerator         -> BNM::Coroutine::IEnumerator *
#   System.Collections.Generic.IEnumerator<T> -> BNM::Coroutine::Generic::IEnumerator<T> *
BNM_COROUTINE_HEADER = "BNM/Coroutine.hpp"
BNM_COROUTINE_GENERIC = "BNM::Coroutine::Generic::IEnumerator"

# BCL collection types served natively by BNM's Mono structures
# (include/BNM/MonoStructures/). Their generated binding files are skipped and
# every reference maps to the native type instead:
#   HashSet<T>              -> BNM::Structures::Mono::HashSet<T> *
#   Queue<T>                -> BNM::Structures::Mono::Queue<T> *
#   Stack<T>                -> BNM::Structures::Mono::Stack<T> *
#   IReadOnlyList<T>        -> BNM::Structures::Mono::IReadOnlyList<T> *
#   IReadOnlyCollection<T>  -> BNM::Structures::Mono::IReadOnlyCollection<T> *
BNM_COLLECTION_HEADERS = {
    "HashSet": "BNM/MonoStructures/HashSet.hpp",
    "Queue": "BNM/MonoStructures/Queue.hpp",
    "Stack": "BNM/MonoStructures/Stack.hpp",
    "IReadOnlyList": "BNM/MonoStructures/ReadOnlyList.hpp",
    "IReadOnlyCollection": "BNM/MonoStructures/ReadOnlyList.hpp",
}
BNM_COLLECTION_QUALNAMES = {
    "HashSet": "BNM::Structures::Mono::HashSet",
    "Queue": "BNM::Structures::Mono::Queue",
    "Stack": "BNM::Structures::Mono::Stack",
    "IReadOnlyList": "BNM::Structures::Mono::IReadOnlyList",
    "IReadOnlyCollection": "BNM::Structures::Mono::IReadOnlyCollection",
}

CPP_KEYWORDS = {
    "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit",
    "atomic_noexcept", "auto", "bitand", "bitor", "bool", "break", "case", "catch",
    "char", "char8_t", "char16_t", "char32_t", "class", "compl", "concept",
    "const", "consteval", "constexpr", "constinit", "const_cast", "continue",
    "co_await", "co_return", "co_yield", "decltype", "default", "delete", "do",
    "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern",
    "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
    "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr",
    "operator", "or", "or_eq", "private", "protected", "public", "reflexpr",
    "register", "reinterpret_cast", "requires", "return", "short", "signed",
    "sizeof", "static", "static_assert", "static_cast", "struct", "switch",
    "synchronized", "template", "this", "thread_local", "throw", "true",
    "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
    "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
}

# Root C++ namespace wrapping every generated binding.
ROOT_NS = "GB"


def sanitize_ident(name: str) -> str:
    """Sanitizes any identifier to valid C++ syntax."""
    if not name:
        return "_unnamed"
    sanitized = re.sub(r"[^\w]", "_", name)
    sanitized = re.sub(r"_+", "_", sanitized)
    if sanitized and sanitized[0].isdigit():
        sanitized = f"_{sanitized}"
    if sanitized in CPP_KEYWORDS:
        sanitized = f"_{sanitized}"
    # MSVC/clang-cl treat _int32/_uint64/... as integer type names on Windows
    # targets (altintrin aliases), so "int _int32" is a syntax error there.
    if re.match(r"^_(u?int(8|16|32|64)|intptr|uintptr)$", sanitized):
        sanitized = f"{sanitized}_"
    return sanitized or "_unnamed"


def extract_backing_field_name(field_name: str):
    """Extracts property name from compiler generated backing field, e.g. <NpcId>k__BackingField -> NpcId."""
    m = re.match(r"^<([^>]+)>k__BackingField$", field_name)
    if m:
        return m.group(1)
    return None


# Environment macros (from libc/CRT headers) that collide with C# identifiers of
# the same spelling. BNM headers pull in <cstdio>/<cerrno> transitively, so any
# enum member, field or method named like one of these expands into garbage at
# compile time. The emitter wraps such declarations in
# #pragma push_macro/#undef/#pragma pop_macro.
ENV_MACROS = frozenset({
    # errno.h (glibc/ucrt)
    "E2BIG", "EACCES", "EADDRINUSE", "EADDRNOTAVAIL", "EAFNOSUPPORT", "EAGAIN",
    "EALREADY", "EBADF", "EBADMSG", "EBUSY", "ECANCELED", "ECHILD",
    "ECONNABORTED", "ECONNREFUSED", "ECONNRESET", "EDEADLK", "EDESTADDRREQ",
    "EDOM", "EDQUOT", "EEXIST", "EFAULT", "EFBIG", "EHOSTDOWN", "EHOSTUNREACH",
    "EIDRM", "EILSEQ", "EINPROGRESS", "EINTR", "EINVAL", "EIO", "EISCONN",
    "EISDIR", "ELOOP", "EMFILE", "EMLINK", "EMSGSIZE", "EMULTIHOP",
    "ENAMETOOLONG", "ENETDOWN", "ENETRESET", "ENETUNREACH", "ENFILE",
    "ENOBUFS", "ENODATA", "ENODEV", "ENOENT", "ENOEXEC", "ENOLCK", "ENOLINK",
    "ENOMEM", "ENOMSG", "ENOPROTOOPT", "ENOSPC", "ENOSYS", "ENOTCONN",
    "ENOTDIR", "ENOTEMPTY", "ENOTSOCK", "ENOTSUP", "ENOTTY", "ENXIO",
    "EOPNOTSUPP", "EOVERFLOW", "EPERM", "EPFNOSUPPORT", "EPIPE", "EPROTO",
    "EPROTONOSUPPORT", "EPROTOTYPE", "ERANGE", "EROFS", "ESHUTDOWN", "ESPIPE",
    "ESRCH", "ESTALE", "ESOCKTNOSUPPORT", "ETIMEDOUT", "ETXTBSY", "EUNATCH",
    "EUSERS", "EWOULDBLOCK", "EXDEV",
    # stdio.h / wchar.h
    "stdin", "stdout", "stderr", "NULL", "BUFSIZ", "EOF", "FOPEN_MAX",
    "FILENAME_MAX", "L_tmpnam", "SEEK_CUR", "SEEK_END", "SEEK_SET", "TMP_MAX",
    "stdin", "getc", "putc", "getchar", "putchar",
    # ctype.h / limits.h / signal.h
    "IN", "OUT", "min", "max", "PIPE_BUF", "SIG_DFL", "SIG_ERR", "SIG_IGN",
    "SIGABRT", "SIGFPE", "SIGILL", "SIGINT", "SIGSEGV", "SIGTERM",
    # math.h
    "M_PI", "M_E", "NAN", "INFINITY", "isnan", "isinf", "isfinite",
    # ms windows / minwindef
    "ERROR", "DELETE", "OPTIONAL", "INVALID_SOCKET", "SOCKET_ERROR",
    "interface", "small", "near", "far", "hyper", "pascal", "cdecl",
    "WINAPI", "CALLBACK", "VOID",
})


def env_macro_guard(name: str):
    """Returns (prologue, epilogue) #pragma push_macro / #undef guard lines for
    an identifier that collides with an environment macro, or (None, None)."""
    if name in ENV_MACROS:
        return (f'#pragma push_macro("{name}")\n#undef {name}\n',
                f'#pragma pop_macro("{name}")\n')
    return None, None


def ns_to_parts(namespace: str):
    """Splits a C# namespace into sanitized parts ([] for global)."""
    if not namespace:
        return []
    return [sanitize_ident(p) for p in namespace.split(".") if p]


class DumpParser:
    def __init__(self, dump_file: str, output_dir: str):
        self.dump_file = Path(dump_file)
        self.output_dir = Path(output_dir)
        self.gb_root = self.output_dir / "GameBinding"
        self.images = []            # List of (start_type_def_idx, image_name)
        self.image_indices = []
        self.known_enums = set()    # Simple names of all enums (value types)
        self.classes = []
        self.enums = []
        self.type_count = 0
        # Registry indexes (built in build_registry).
        self.by_qualified = {}
        self.by_simple = {}
        self.name_collisions = {}
        # Per-type dep scratch (reset before converting one type's members)
        self.cur_include_deps = set()   # rel_path that must be #include-d (by-value/base)
        self.cur_fwd_deps = set()       # 'cpp_ns::Name' that only need forward declaration
        self.cur_bnm_includes = set()
        # Namespace of the type currently being generated (context for name resolution)
        self._cur_ns = ROOT_NS
        # Alias map: raw C# nested name ('Outer.Inner' or its simple inner name)
        # -> flattened generated struct name ('Outer_Inner').
        self.alias = {}
        self._pending_alias = {}

    # ------------------------------------------------------------------
    def get_image_for_typedef(self, type_def_idx: int) -> str:
        if not self.images:
            return "Assembly-CSharp.dll"
        idx = bisect.bisect_right(self.image_indices, type_def_idx) - 1
        if 0 <= idx < len(self.images):
            return self.images[idx][1]
        return self.images[0][1]

    def is_bnm_native(self, simple_name: str) -> bool:
        return simple_name in BNM_UNITY_OBJECT_TYPES

    def _dep(self, info, by_value: bool):
        """Records a dependency on a generated type.

        by_value=True  -> the type's full definition is required -> #include.
        by_value=False -> only a pointer/reference is used        -> forward declare.
        Forward-declaring pointer-only deps breaks include cycles (e.g. two
        classes referencing each other, or outer<->inner nesting).
        """
        if by_value:
            self.cur_include_deps.add(info["rel_path"])
            self.cur_fwd_deps.discard(info["cpp_ns"] + "::" + info["name"])
        else:
            key = info["cpp_ns"] + "::" + info["name"]
            if info["rel_path"] not in self.cur_include_deps:
                self.cur_fwd_deps.add(key)

    @staticmethod
    def _bnm_unity_include(simple: str) -> str:
        """The BNM header that defines a Unity object type. Several families share
        one header (Physics*, Audio, UI, TextMeshPro, Events, Animation), so this
        looks the type up in BNM_UNITY_TYPE_HEADERS instead of assuming a
        same-named file."""
        hdr = BNM_UNITY_TYPE_HEADERS.get(simple, f"{simple}.hpp")
        return f"BNM/UnityStructures/{hdr}"

    # ------------------------------------------------------------------
    def convert_type(self, csharp_type: str, by_value: bool = False,
                     type_params_in_scope: frozenset = frozenset()) -> str:
        """Converts a C# type to a BNM C++ representation.

        Side effects: records binding include deps (self.cur_include_deps),
        forward-declare deps (self.cur_fwd_deps) and BNM includes
        (self.cur_bnm_includes) for every referenced type.
        `by_value` should be True when the caller needs the complete type.
        `type_params_in_scope` holds the open generic type parameters (e.g. {"T"})
        of the class/method currently being generated; a bare "T" maps to "T *".
        """
        if not csharp_type:
            return "BNM::IL2CPP::Il2CppObject *"
        t = csharp_type.strip()
        t = re.sub(r"^(ref|out|in|params)\s+", "", t).strip()
        # Strip default-value artifacts that may cling to the type token.
        t = t.split("=")[0].strip()

        # Open generic type parameter (T, T1, ...) -> pointer to the C++ template param.
        if t in type_params_in_scope:
            return f"{t} *"

        # Primitive types
        if t in TYPE_MAP:
            return TYPE_MAP[t]

        # string / String -> BNM Mono String
        if t in ("string", "String", "System.String"):
            self.cur_bnm_includes.add(BNM_MONO_HEADERS["String"])
            return "BNM::Structures::Mono::String *"

        # object / Object -> Il2CppObject
        if t in ("object", "Object", "System.Object"):
            return "BNM::IL2CPP::Il2CppObject *"

        # C# 1D array T[]
        if t.endswith("[]"):
            # BNM's Array<T> stores elements inline (T m_Items[0]) but a
            # pointer declaration (Array<T> *) never instantiates the template,
            # so the element dep follows the CURRENT by_value context. Forcing
            # by_value=True here closed an include cycle between Expression_0
            # and ParameterExpression (Array<ParameterExpression *> is used in
            # Expression.Lambda overloads while ParameterExpression derives
            # from Expression_0).
            elem = self.convert_type(t[:-2], by_value=by_value, type_params_in_scope=type_params_in_scope)
            self.cur_bnm_includes.add(BNM_MONO_HEADERS["Array"])
            return f"BNM::Structures::Mono::Array<{elem}> *"

        # Generic instantiation Name<args>
        gm = re.match(r"^([\w\.]+)<(.+)>$", t)
        if gm:
            gen_base, gen_args = gm.groups()
            base_name = gen_base.split(".")[-1]
            if base_name in ("List", "IList", "IEnumerable", "ICollection"):
                # List<T> stores elements behind Array<T> *items, so a pointer
                # declaration (List<T> *) never needs the element's definition;
                # the element dep follows the current by_value context (see the
                # T[] branch above for the Expression_0 cycle this avoids).
                inner = self.convert_type(self._split_top(gen_args)[0], by_value=by_value, type_params_in_scope=type_params_in_scope)
                self.cur_bnm_includes.add(BNM_MONO_HEADERS["List"])
                return f"BNM::Structures::Mono::List<{inner}> *"
            if base_name in ("Dictionary", "IDictionary"):
                parts = self._split_top(gen_args)
                if len(parts) >= 2:
                    k = self.convert_type(parts[0], by_value=by_value, type_params_in_scope=type_params_in_scope)
                    v = self.convert_type(parts[1], by_value=by_value, type_params_in_scope=type_params_in_scope)
                    self.cur_bnm_includes.add(BNM_MONO_HEADERS["Dictionary"])
                    return f"BNM::Structures::Mono::Dictionary<{k}, {v}> *"
            if base_name == "Nullable":
                inner = self.convert_type(self._split_top(gen_args)[0], by_value=True, type_params_in_scope=type_params_in_scope)
                self.cur_bnm_includes.add(BNM_MONO_HEADERS["Nullable"])
                return f"BNM::Structures::Mono::Nullable<{inner}>"
            # BCL delegate families -> BNM native wrappers. A reference like
            # Action<T1, T2> or Func<A1, A2, R> maps to the typed BNM delegate;
            # see BNM_DELEGATE_FAMILIES for the exact shape per family.
            if base_name in BNM_DELEGATE_FAMILIES:
                args = [self._cpp_template_arg(a, by_value, type_params_in_scope)
                        for a in self._split_top(gen_args)]
                self.cur_bnm_includes.add(BNM_MONO_HEADERS["Delegates"])
                if base_name == "Action":
                    return f"BNM::Structures::Mono::Action<{', '.join(args)}> *"
                if base_name == "Func":
                    return f"BNM::Delegate<{args[-1] if args else 'void'}> *"
                if base_name == "Predicate":
                    return "BNM::Delegate<bool> *"
                if base_name == "Comparison":
                    return "BNM::Delegate<int> *"
                if base_name == "Converter":
                    return f"BNM::Delegate<{args[-1] if args else 'void'}> *"
                # EventHandler<T> / EventHandler / AsyncCallback
                return "BNM::Delegate<void> *"
            # System.Collections.Generic.IEnumerator<T> -> BNM's typed coroutine
            # wrapper (Coroutine.hpp); the type argument is kept as a tag.
            if base_name == "IEnumerator":
                inner = self._cpp_template_arg(self._split_top(gen_args)[0], by_value, type_params_in_scope)
                self.cur_bnm_includes.add(BNM_COROUTINE_HEADER)
                return f"{BNM_COROUTINE_GENERIC}<{inner}> *"
            # BCL collection types -> BNM's native Mono structures
            # (HashSet/Queue/Stack/IReadOnlyList/IReadOnlyCollection).
            if base_name in BNM_COLLECTION_HEADERS:
                inner = self._cpp_template_arg(self._split_top(gen_args)[0], by_value, type_params_in_scope)
                self.cur_bnm_includes.add(BNM_COLLECTION_HEADERS[base_name])
                return f"{BNM_COLLECTION_QUALNAMES[base_name]}<{inner}> *"
            # Generic instantiation of a generated binding type or BNM object type
            # (e.g. MySingleton<PlayerController>, Receiver<T>, Duo<F, S>, Holder.
            # Operator<TVal>). Pass the FULL base path (with any "Outer." prefix)
            # so _convert_generic_instantiation can resolve nested-type aliases.
            inst = self._convert_generic_instantiation(gen_base, gen_args, by_value, type_params_in_scope)
            if inst:
                return inst
            return "BNM::IL2CPP::Il2CppObject *"

        # Nullable<T> shorthand T?
        if t.endswith("?"):
            inner = self.convert_type(t[:-1], by_value=True, type_params_in_scope=type_params_in_scope)
            self.cur_bnm_includes.add(BNM_MONO_HEADERS["Nullable"])
            return f"BNM::Structures::Mono::Nullable<{inner}>"

        # Resolve nested names (Outer.Inner) to their flattened generated
        # struct name (Outer_Inner) before any further lookup.
        t = self.alias.get(t, t)
        # Strip namespace to a simple name for the remaining checks. A bare
        # name must NOT go through the alias registry: it maps the inner name
        # of some unrelated nested type (Action -> SomeWrapper_Action,
        # Type -> GamePanel_Type). Bare names resolve via _resolve_generated
        # under the current namespace instead.
        simple = t.split(".")[-1].split("+")[-1]

        # BCL delegate families (bare, non-generic) -> BNM native wrappers.
        if simple in BNM_DELEGATE_FAMILIES:
            self.cur_bnm_includes.add(BNM_MONO_HEADERS["Delegates"])
            if simple == "Action":
                return "BNM::Structures::Mono::Action<> *"
            if simple == "MulticastDelegate":
                return "BNM::MulticastDelegate<void> *"
            return "BNM::Delegate<void> *"

        # System.Collections.IEnumerator (bare or fully qualified) -> BNM's
        # coroutine wrapper (Coroutine.hpp); the binding struct for it is not
        # generated (see the parser skip).
        if simple == "IEnumerator":
            self.cur_bnm_includes.add(BNM_COROUTINE_HEADER)
            return "BNM::Coroutine::IEnumerator *"

        # BNM Unity value structs (by value)
        if simple in BNM_UNITY_VALUE_STRUCTS:
            self.cur_bnm_includes.add(f"BNM/UnityStructures/{BNM_UNITY_VALUE_STRUCTS[simple]}")
            return f"{BNM_VALUE_STRUCT_QUALNAMES.get(simple, f'BNM::Structures::Unity::{simple}')}"

        # BNM Unity object types (pointer, NOT generated)
        if simple in BNM_UNITY_OBJECT_TYPES:
            self.cur_bnm_includes.add(self._bnm_unity_include(simple))
            qual = BNM_UNITY_QUALNAMES.get(simple, f"BNM::UnityEngine::{simple}")
            if simple in BNM_UNITY_VARIADIC_TEMPLATES:
                # UnityAction / UnityEvent are variadic C++ templates in BNM:
                # a bare name lacks template arguments.
                return f"{qual}<> *"
            return f"{qual} *"

        # Generated enum or binding type. Resolve through the registry and branch
        # on the kind: enums are VALUE types (full include, no pointer), classes/
        # structs/interfaces are pointer types. Resolving by kind (not by the
        # simple-name enum set) also handles nested enums, whose flattened name
        # (Outer_Inner) is not present in known_enums.
        info = self._resolve_generated(simple, self._cur_ns)
        if info and info["kind"] == "enum":
            self._dep(info, by_value=True)
            return f"{info['cpp_ns']}::{info['name']}"
        if info:
            self._dep(info, by_value=by_value)
            return f"{info['cpp_ns']}::{info['name']} *"

        # Unknown custom type -> opaque IL2CPP object pointer.
        return "BNM::IL2CPP::Il2CppObject *"

    @staticmethod
    def _split_top(args: str):
        """Splits generic args on top-level commas (ignores nested <>)"""
        parts, depth, cur = [], 0, ""
        for ch in args:
            if ch == "<":
                depth += 1
            elif ch == ">":
                depth -= 1
            if ch == "," and depth == 0:
                parts.append(cur.strip())
                cur = ""
            else:
                cur += ch
        if cur.strip():
            parts.append(cur.strip())
        return parts

    # ------------------------------------------------------------------
    def _cpp_template_arg(self, arg: str, by_value: bool, type_params_in_scope: frozenset) -> str:
        """Resolves a C# generic type argument to a NON-POINTER C++ type usable
        as a template argument, e.g. MySingleton<GB::PlayerController>, List<int>, Duo<T, S>.

        Template arguments are converted in POINTER context (by_value=False):
        a self-referential generic like `ReadOnlyCollection<ParameterExpression>`
        used to force a by-value include of the argument's header, which closed
        an include cycle when that header referenced the current class back
        (Expression_0 <-> ParameterExpression). Only a direct base class needs
        the argument's complete type, and that path converts args itself.
        """
        cpp = self.convert_type(arg, by_value=False, type_params_in_scope=type_params_in_scope)
        # convert_type yields a pointer for object/binding/open types; template
        # arguments need the bare type.
        return cpp[:-2] if cpp.endswith(" *") else cpp

    def _convert_generic_instantiation(self, base_name: str, gen_args: str,
                                       by_value: bool, type_params_in_scope: frozenset):
        """Handles a generic instantiation of a generated binding type or a BNM
        object type (MySingleton<PlayerController>, Receiver<T>, Duo<F, S>). Returns the
        C++ pointer type string, or None if the base cannot be resolved."""
        args = self._split_top(gen_args)
        cpp_args = ", ".join(self._cpp_template_arg(a, by_value, type_params_in_scope) for a in args)
        # A full "Outer.Inner" path is a nested type: prefer its flattened alias
        # (more specific than the bare inner name, which may collide with an
        # unrelated type elsewhere). A bare simple name resolves via the registry
        # with arity so System.Action<T> picks Action_1, not Action_16 — but the
        # bare inner-name alias must NOT be applied here or it would hijack BCL
        # generic bases (Action -> SomeWrapper_Action).
        info = None
        if "." in base_name:
            aliased = self.alias.get(base_name)
            if aliased:
                info = self._resolve_generated(aliased, self._cur_ns, arity=len(args))
        if not info:
            base_simple = base_name.split(".")[-1].split("+")[-1]
            info = self._resolve_generated(base_simple, self._cur_ns, arity=len(args))
            if not info and "." in base_name:
                aliased = self.alias.get(base_name)
                if aliased:
                    info = self._resolve_generated(aliased, self._cur_ns, arity=len(args))
        base_simple = info["name"] if info else base_name.split(".")[-1].split("+")[-1]
        # An enum is a value type even when written as a generic instantiation
        # (Holder.Style<TVal> is a dumper artefact of an enum nested in a
        # generic class; IL2CPP enums are not generic) -> plain enum, no <>.
        if info and info["kind"] == "enum":
            self._dep(info, by_value=True)
            return f"{info['cpp_ns']}::{info['name']}"
        # BNM object type (e.g. UnityEvent<int>) -> BNM::UnityEngine::X<...>
        if not info and base_simple in BNM_UNITY_OBJECT_TYPES:
            self.cur_bnm_includes.add(self._bnm_unity_include(base_simple))
            qual = BNM_UNITY_QUALNAMES.get(base_simple, f"BNM::UnityEngine::{base_simple}")
            return f"{qual}<{cpp_args}> *"
        # Generated binding type (e.g. MySingleton<PlayerController>).
        if info and info["kind"] != "enum":
            self._dep(info, by_value=by_value)
            return f"{info['cpp_ns']}::{info['name']}<{cpp_args}> *"
        return None

    # ------------------------------------------------------------------
    def scan_enums(self):
        print(f"[*] Scanning enums in dump file...")
        with open(self.dump_file, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                if "enum " in line:
                    m = re.match(r"^(?:public|protected|private|internal)?\s*(?:sealed)?\s*enum\s+([^\s:<{/]+)", line.strip())
                    if m:
                        raw = m.group(1).strip()
                        self.known_enums.add(raw.split(".")[-1].split("+")[-1])
        print(f"[+] Discovered {len(self.known_enums)} enum definitions.")

    # ------------------------------------------------------------------
    def parse(self):
        self.scan_enums()
        print(f"[*] Parsing dump file: {self.dump_file}")
        start_time = time.time()

        with open(self.dump_file, "r", encoding="utf-8", errors="ignore") as f:
            current_type = None
            current_namespace = ""
            brace_depth = 0
            is_enum = False

            for line in f:
                s = line.strip()
                if not s:
                    continue

                img = re.match(r"^//\s*Image\s*\d+:\s*(.*?)\s*-\s*(\d+)", s)
                if img:
                    self.images.append((int(img.group(2)), img.group(1).strip()))
                    continue
                if self.images and not self.image_indices and s.startswith("// Namespace:"):
                    self.images.sort(key=lambda x: x[0])
                    self.image_indices = [x[0] for x in self.images]

                nsm = re.match(r"^//\s*Namespace:\s*(.*)", s)
                if nsm:
                    current_namespace = nsm.group(1).strip()
                    continue

                if s.startswith("//") or s.startswith("/*") or s.startswith("|") or s.startswith("*-"):
                    continue

                # --- Type definition start ---
                if not current_type:
                    em = re.match(
                        r"^(?:public|protected|private|internal)?\s*(?:sealed)?\s*enum\s+([^\s:<{/]+)(?:\s*:\s*([^{/]+))?.*?(?://\s*TypeDefIndex:\s*(\d+))?",
                        s)
                    if em and "enum " in s:
                        raw_name, under, td = em.groups()
                        td_idx = int(td) if td else 0
                        # Flatten nested enums like nested classes (see below).
                        if "." in raw_name or "+" in raw_name:
                            delim = "+" if "+" in raw_name else "."
                            parts_raw = raw_name.split(delim)
                            flat_name = sanitize_ident("_".join(parts_raw))
                            self._pending_alias = {raw_name: flat_name, parts_raw[-1]: flat_name}
                        else:
                            flat_name = sanitize_ident(raw_name.split(".")[-1].split("+")[-1])
                            self._pending_alias = {raw_name: flat_name}
                        current_type = {
                            "kind": "enum", "image": self.get_image_for_typedef(td_idx),
                            "namespace": current_namespace, "raw_name": raw_name,
                            "name": flat_name, "outer_class": None,
                            "underlying": (under.strip() if under else None),
                            "type_params": [], "arity": 0,
                            "members": [],
                        }
                        is_enum = True
                        brace_depth = 1 if "{" in s else 0
                        continue

                    tm = re.match(
                        r"^(?:public|protected|private|internal)?\s*(?:abstract|sealed|static|partial|new)*\s*(class|struct|interface)\s+([^\s:<{/]+)\s*(?:<([^{}:]+)>)?(?:\s*:\s*([^{/]+))?.*?(?://\s*TypeDefIndex:\s*(\d+))?",
                        s)
                    if tm:
                        kind, raw_name, tparams_raw, inheritance, td = tm.groups()
                        td_idx = int(td) if td else 0
                        simple = raw_name.split(".")[-1].split("+")[-1]
                        # Generic class type parameters, e.g. "T" or "T1, T2" -> ["T"] / ["T1","T2"].
                        type_params = ([sanitize_ident(p.strip()) for p in tparams_raw.split(",") if p.strip()]
                                       if tparams_raw else [])

                        # Compiler-generated C# types (state machines <x>d__N,
                        # lambdas <>c / <>c__N<T>, closures < >c__DisplayClass,
                        # f__AnonymousType, <Module>, <PrivateImplementationDetails>,
                        # __StaticArrayInitTypeSize=...) are never referenced from
                        # real types and carry broken metadata names (the regex
                        # above truncates at '<', leaving a bare prefix that ends
                        # with '.'), so skip them entirely.
                        if (not raw_name or raw_name.endswith(".") or "<" in raw_name
                                or ">" in raw_name or "=" in raw_name):
                            current_type = {"_skip": True}
                            is_enum = False
                            brace_depth = 1 if "{" in s else 0
                            continue

                        # BCL delegate families are served natively by BNM
                        # (BNM/Delegates.hpp): references map to
                        # BNM::Delegate<Ret>/Structures::Mono::Action<...>, so the
                        # generated System.Action_N/Func_M/... files are not
                        # emitted at all.
                        if current_namespace == "System" and simple in BNM_DELEGATE_FAMILIES:
                            current_type = {"_skip": True}
                            is_enum = False
                            brace_depth = 1 if "{" in s else 0
                            continue

                        # BCL enumerator interfaces are served natively by BNM
                        # (BNM/Coroutine.hpp): references map to
                        # BNM::Coroutine::IEnumerator / Generic::IEnumerator<T>,
                        # so System.Collections[.Generic]/IEnumerator.hpp is not
                        # emitted at all.
                        if (current_namespace in ("System.Collections", "System.Collections.Generic")
                                and simple == "IEnumerator"):
                            current_type = {"_skip": True}
                            is_enum = False
                            brace_depth = 1 if "{" in s else 0
                            continue

                        # BCL collection types served natively by BNM's Mono
                        # structures (BNM/MonoStructures/): references map to
                        # BNM::Structures::Mono::HashSet/Queue/Stack/IReadOnlyList/
                        # IReadOnlyCollection, so their generated files are skipped.
                        if (current_namespace == "System.Collections.Generic"
                                and simple in BNM_COLLECTION_HEADERS):
                            current_type = {"_skip": True}
                            is_enum = False
                            brace_depth = 1 if "{" in s else 0
                            continue

                        # Skip BNM-native Unity types entirely (do not generate).
                        if simple in BNM_UNITY_OBJECT_TYPES:
                            current_type = {"_skip": True}
                            is_enum = False
                            brace_depth = 1 if "{" in s else 0
                            continue

                        first_base = ""
                        if inheritance:
                            # Split on TOP-LEVEL commas only: a generic base like
                            # SetPropertyBagBase<HashSet<TElement>, TElement> holds
                            # commas inside <> that must not split the base list.
                            bases = []
                            depth = 0
                            cur = ""
                            for ch in inheritance:
                                if ch == "<":
                                    depth += 1
                                elif ch == ">":
                                    depth -= 1
                                if ch == "," and depth == 0:
                                    bases.append(cur.strip())
                                    cur = ""
                                else:
                                    cur += ch
                            if cur.strip():
                                bases.append(cur.strip())
                            first_base = bases[0] if bases else ""

                        # Nested types (Outer.Inner) are flattened to a single
                        # top-level struct named 'Outer_Inner'. This avoids the
                        # BNM_GAME_INNER_CLASS include cycle (inner needs the
                        # outer's complete type while the outer often references
                        # the inner via fields). References like 'Outer.Inner'
                        # resolve to 'Outer_Inner' through the alias registry.
                        if "." in raw_name or "+" in raw_name:
                            delim = "+" if "+" in raw_name else "."
                            parts_raw = raw_name.split(delim)
                            flat_name = sanitize_ident("_".join(parts_raw))
                            # Keep ONLY the full dotted key (Outer.Inner ->
                            # Outer_Inner). A bare inner-name key would be
                            # overwritten by every subsequent nested type of the
                            # same name (Action -> SomeWrapper_Action, Type ->
                            # GamePanel_Type, GunData -> ...) and hijack
                            # bare references that belong to top-level/BCL types.
                            self._pending_alias = {raw_name: flat_name}
                        else:
                            flat_name = sanitize_ident(simple)
                            self._pending_alias = {raw_name: flat_name}

                        current_type = {
                            "kind": kind, "image": self.get_image_for_typedef(td_idx),
                            "namespace": current_namespace, "raw_name": raw_name,
                            "name": flat_name, "outer_class": None,
                            "raw_base": first_base, "type_params": type_params,
                            "arity": len(type_params),
                            "fields": [], "properties": [], "methods": [],
                        }
                        is_enum = False
                        brace_depth = 1 if "{" in s else 0
                        continue

                # --- Inside a type body ---
                if current_type:
                    brace_depth += s.count("{") - s.count("}")
                    if brace_depth <= 0 and "}" in s:
                        if current_type.get("_skip"):
                            pass  # discarded BNM-native type
                        else:
                            self.alias.update(self._pending_alias)
                            if is_enum:
                                self.enums.append(current_type)
                            else:
                                self.classes.append(current_type)
                            self.type_count += 1
                        self._pending_alias = {}
                        current_type = None
                        is_enum = False
                        brace_depth = 0
                        continue
                    if current_type.get("_skip"):
                        continue

                    # Enum members / underlying
                    if is_enum:
                        # underlying from "public int value__; // 0x0"
                        um = re.match(r"^public\s+([^\s;]+)\s+value__\s*;", s)
                        if um and not current_type["underlying"]:
                            current_type["underlying"] = um.group(1)
                            continue
                        mm = re.match(r"^(?:public\s+)?const\s+[^\s;=]+\s+([^\s;=]+)\s*=\s*([^;]+);", s)
                        if mm:
                            key, val = mm.groups()
                            if key != "value__":
                                current_type["members"].append({
                                    "name": sanitize_ident(key), "val": val.strip()
                                })
                        continue

                    # Field. The type group is (.+?) so it can contain spaces
                    # inside generic arguments (Duo<int, PlayerController> data;).
                    fm = re.match(
                        r"^(?:public|protected|private|internal)\s+(static\s+|const\s+)?(?:readonly\s+)?(.+?)\s+([^\s;=]+)\s*(?:=\s*[^;]+)?\s*;\s*(?://\s*0x([0-9a-fA-F]+))?",
                        s)
                    if fm and "(" not in s:
                        is_static, ftype, fname, off = fm.groups()
                        backing = extract_backing_field_name(fname)
                        if backing:
                            current_type["properties"].append({
                                "name": sanitize_ident(backing), "raw_name": backing,
                                "raw_type": ftype, "is_static": bool(is_static),
                            })
                        else:
                            current_type["fields"].append({
                                "name": sanitize_ident(fname), "raw_name": fname,
                                "raw_type": ftype, "is_static": bool(is_static),
                                "offset": f"0x{off}" if off else None,
                            })
                        continue

                    # Property. The type group is (.+?) for generic args with
                    # spaces (Duo<int, PlayerController> Data { get; set; }).
                    pm = re.match(
                        r"^(?:public|protected|private|internal)\s+(static\s+)?(?:virtual\s+|override\s+)?(.+?)\s+([^\s;{]+)\s*\{\s*(?:get|set)",
                        s)
                    if pm:
                        is_static, ptype, pname = pm.groups()
                        current_type["properties"].append({
                            "name": sanitize_ident(pname), "raw_name": pname,
                            "raw_type": ptype, "is_static": bool(is_static),
                        })
                        continue

                    # Method. The return type group is (.+?) so it can contain
                    # spaces inside generic arguments (Duo<F, S> Combine<T>()).
                    mm = re.match(
                        r"^(?:public|protected|private|internal)?\s*(static\s+)?(?:virtual\s+|override\s+|abstract\s+|sealed\s+|new\s+)?(.+?)\s+([^\s<(]+)\s*(?:<([^(]+)>)?\s*\((.*?)\)\s*(?:\{|;)",
                        s)
                    if mm:
                        is_static, ret, mname, mtparams_raw, params_str = mm.groups()
                        if mname in (".ctor", ".cctor"):
                            continue
                        # Generic method type parameters, e.g. "T" / "T1, T2".
                        method_type_params = ([sanitize_ident(p.strip()) for p in mtparams_raw.split(",") if p.strip()]
                                              if mtparams_raw else [])
                        params = []
                        if params_str.strip():
                            for idx, p in enumerate(self._split_top(params_str)):
                                # Strip any default value ("= ...") BEFORE tokenizing,
                                # so "bool flag = False" doesn't leak "False" as the name.
                                p = p.split("=")[0].strip()
                                if not p:
                                    continue
                                toks = p.split()
                                if len(toks) >= 2:
                                    pt = " ".join(toks[:-1])
                                    pn = toks[-1].strip()
                                    params.append({"raw_type": pt, "name": sanitize_ident(pn) or f"arg{idx}"})
                                else:
                                    params.append({"raw_type": toks[0], "name": f"arg{idx}"})
                        # De-duplicate parameter names within one method (C++
                        # requires unique parameter names in a signature).
                        seen_pn = {}
                        for p in params:
                            n = p["name"]
                            if n in seen_pn:
                                seen_pn[n] += 1
                                p["name"] = f"{n}_{seen_pn[n]}"
                            else:
                                seen_pn[n] = 0
                        current_type["methods"].append({
                            "name": sanitize_ident(mname), "raw_name": mname,
                            "raw_return": ret, "is_static": bool(is_static),
                            "params": params, "method_type_params": method_type_params,
                        })
                        continue

        elapsed = time.time() - start_time
        print(f"[+] Parsed {self.type_count} total types across {len(self.images)} images in {elapsed:.2f}s.")

    # ------------------------------------------------------------------
    def build_registry(self):
        """Builds two indexes:
        - by_qualified: 'GB::Ns::Name' -> info (source of truth)
        - by_simple:    'BaseName' -> [info, ...] (all candidates, for resolution)

        Generic arity collision handling: several distinct C# types can share the
        same simple name but differ in generic arity (System.Action<T..T16>,
        System.Func<T..T17>, ...). Flattening them all to one C++ identifier
        would clobber all but one. When a (namespace, simple-name) group holds
        more than one distinct arity, every member's C++ name gets an
        `_<arity>` suffix (Action_0, Action_1, ...). Groups with a single arity
        keep the plain name (MySingleton stays MySingleton).
        """
        print(f"[*] Building type registry...")
        self.by_qualified = {}
        self.by_simple = {}

        # Pass 1: detect (namespace, simple-name) groups with multiple arities.
        arity_sets = {}
        for coll in (self.classes, self.enums):
            for t in coll:
                parts = ns_to_parts(t["namespace"])
                key = (tuple(parts), t["name"])
                arity_sets.setdefault(key, set()).add(t.get("arity", 0))
        colliding = {key for key, arities in arity_sets.items() if len(arities) > 1}

        # Pass 2: build the indexes, applying the arity suffix where needed.
        for coll in (self.classes, self.enums):
            for t in coll:
                parts = ns_to_parts(t["namespace"])
                arity = t.get("arity", 0)
                base_name = t["name"]
                name = base_name
                if (tuple(parts), base_name) in colliding:
                    name = f"{base_name}_{arity}"
                sub = "/".join(parts)
                rel = f"{sub}/{name}.hpp" if sub else f"{name}.hpp"
                cpp_ns = ROOT_NS + ("::" + "::".join(parts) if parts else "")
                info = {
                    "name": name, "base_name": base_name, "ns_parts": parts,
                    "cpp_ns": cpp_ns, "rel_path": rel, "kind": t["kind"],
                    "outer": t["outer_class"], "type_params": t.get("type_params", []),
                    "arity": arity,
                }
                # Persist the resolved C++ name back onto the type dict so the
                # generator writes a file/struct matching the registry.
                t["cpp_name"] = name
                t["rel_path"] = rel
                self.by_qualified[f"{cpp_ns}::{name}"] = info
                self.by_simple.setdefault(base_name, []).append(info)
        self.name_collisions = {n: v for n, v in self.by_simple.items() if len(v) > 1}
        print(f"[+] Registry holds {len(self.classes) + len(self.enums)} types; "
              f"{len(self.name_collisions)} colliding simple names; "
              f"{len(colliding)} arity-suffixed names.")

    def _resolve_generated(self, simple: str, context_ns: str, arity=None):
        """Picks the best registry info for a simple (base) name given the
        namespace of the type currently being generated. When `arity` is given
        (a generic instantiation Name<...>), only candidates with that arity are
        considered. Prefers the matching namespace, then the global (GB)
        namespace, then the first candidate."""
        cands = self.by_simple.get(simple)
        if not cands:
            return None
        if arity is not None:
            ar = [c for c in cands if c.get("arity") == arity]
            if ar:
                cands = ar
            # If no candidate matches the arity, fall through to all candidates.
        else:
            # A non-generic reference (Task, not Task<TResult>) should prefer the
            # non-generic (arity-0) member of an arity-colliding overload set.
            ar0 = [c for c in cands if c.get("arity") == 0]
            if ar0:
                cands = ar0
        if len(cands) == 1:
            return cands[0]
        # Prefer exact namespace match.
        for c in cands:
            if c["cpp_ns"] == context_ns:
                return c
        # Prefer the global GB namespace (shortest path).
        for c in cands:
            if c["cpp_ns"] == ROOT_NS:
                return c
        return cands[0]

    # ------------------------------------------------------------------
    def _resolve_base(self, cls):
        """Returns (base_cpp, extra_binding_dep_or_None, extra_bnm_include_or_None).

        Handles generic base classes (PlayerController : MySingleton<PlayerController>): the base
        resolves to the generated template struct instantiated with the resolved
        type arguments, e.g. GB::MySingleton<GB::PlayerController>. Only binding-type
        generic bases are instantiated; BNM/interface generic bases fall back to
        their non-generic form (we cannot guarantee the BNM-side template shape).
        """
        fb = cls.get("raw_base", "").strip()
        if not fb:
            return "BNM::IL2CPP::Il2CppObject", None, None

        type_params_in_scope = frozenset(cls.get("type_params", []))

        # Generic base? Name<args> (e.g. MySingleton<PlayerController>, IWrapper<Foo>).
        gm = re.match(r"^([\w\.]+)<(.+)>$", fb)
        if gm:
            gen_base, gen_args = gm.groups()
            args = self._split_top(gen_args)
            # Resolve registry first (System.X<T> style), then the nested alias
            # for a full "Outer.Inner" path. Avoid the bare inner-name alias so
            # BCL generic bases resolve to their true (arity-suffixed) type.
            base_simple = gen_base.split(".")[-1].split("+")[-1]
            info = self._resolve_generated(base_simple, self._cur_ns, arity=len(args))
            if not info and "." in gen_base:
                aliased = self.alias.get(gen_base)
                if aliased:
                    info = self._resolve_generated(aliased, self._cur_ns, arity=len(args))
                    if info:
                        base_simple = aliased
            # Only instantiate generated binding bases (not interfaces/enums).
            if info and info["kind"] not in ("enum", "interface"):
                cpp_args = ", ".join(
                    self._cpp_template_arg(a, True, type_params_in_scope) for a in args)
                return (f"{info['cpp_ns']}::{info['name']}<{cpp_args}>",
                        info["rel_path"], None)
            # BNM/interface/unknown generic base -> strip args and resolve normally.
            fb = gen_base

        simple = fb.split(".")[-1].split("+")[-1]
        simple = re.sub(r"<.*>$", "", simple)  # strip generic args
        # Resolve the base via the registry under its simple name first (a bare
        # "Encoder" is System.Text.Encoder, not the nested UTF7Encoding.Encoder);
        # only a full "Outer.Inner" path uses the nested alias, which is more
        # specific than the bare inner name.
        if "." in fb or "+" in fb:
            aliased = self.alias.get(fb)
            if aliased:
                info_alias = self._resolve_generated(aliased, self._cur_ns)
                if info_alias and info_alias["kind"] not in ("enum", "interface"):
                    return (f"{info_alias['cpp_ns']}::{info_alias['name']}",
                            info_alias["rel_path"], None)
        # BCL delegate base (bare name only; System.Action_N/Func_M are skipped
        # and a delegate game class should derive from the BNM wrapper).
        if simple in BNM_DELEGATE_FAMILIES:
            if simple == "Action":
                return "BNM::Structures::Mono::Action<>", None, BNM_MONO_HEADERS["Delegates"]
            if simple == "MulticastDelegate":
                return "BNM::MulticastDelegate<void>", None, BNM_MONO_HEADERS["Delegates"]
            return "BNM::Delegate<void>", None, BNM_MONO_HEADERS["Delegates"]
        if simple in BNM_UNITY_OBJECT_TYPES:
            qual = BNM_UNITY_QUALNAMES.get(simple, f"BNM::UnityEngine::{simple}")
            if simple in BNM_UNITY_VARIADIC_TEMPLATES:
                # UnityEvent/UnityAction are variadic templates: a class deriving
                # from the bare (nullary) form uses UnityEvent<>.
                return f"{qual}<>", None, self._bnm_unity_include(simple)
            return qual, None, self._bnm_unity_include(simple)
        if simple in BNM_UNITY_VALUE_STRUCTS:
            # value struct can't be a base; fall back to object
            return "BNM::IL2CPP::Il2CppObject", None, None
        info = self._resolve_generated(simple, self._cur_ns)
        # Only classes/structs serve as a C++ base; interfaces and enums cannot.
        if info and info["kind"] not in ("enum", "interface"):
            return f"{info['cpp_ns']}::{info['name']}", info["rel_path"], None
        # Interface or unknown base -> plain object
        return "BNM::IL2CPP::Il2CppObject", None, None

    @staticmethod
    def _dedup_type(t: str) -> str:
        """Normalises a C++ type for overload-collision keys.

        On the 64-bit targets this output is built for (Android LP64 / Windows
        LLP64) intptr_t and int64_t are the SAME type, and so are uintptr_t and
        uint64_t. C# assemblies declare both spellings
        (System.IntPtr.op_Explicit(long) vs op_Explicit(IntPtr), and
        Interlocked.CompareExchange(IntPtr,...) vs (long,...)), which C++
        rejects as redeclarations or return-type-only overloads. Mapping the
        alias to its concrete counterpart lets the duplicate collapse.
        """
        return {"intptr_t": "int64_t", "uintptr_t": "uint64_t"}.get(t, t)

    def _convert_members(self, cls):
        """Converts raw member types; fills cls['include_deps']/['fwd_deps']/['bnm_includes']."""
        self.cur_include_deps = set()
        self.cur_fwd_deps = set()
        self.cur_bnm_includes = set()
        # Set resolution context to this type's namespace.
        parts = ns_to_parts(cls["namespace"])
        self._cur_ns = ROOT_NS + ("::" + "::".join(parts) if parts else "")

        # Base class -> full include (complete base type required).
        base_cpp, base_dep, base_bnm = self._resolve_base(cls)
        cls["base_cpp"] = base_cpp
        if base_dep:
            self.cur_include_deps.add(base_dep)
        if base_bnm:
            self.cur_bnm_includes.add(base_bnm)

        # Nested class: BNM_GAME_INNER_CLASS calls OuterClass::StaticClass()
        # inline (non-template), so the OUTER class needs a complete definition.
        # Include it (by value). The outer class, in turn, references the inner
        # type only via pointers in its fields, so it forward-declares the inner
        # type instead of including it — this breaks the would-be include cycle.
        cls["fwd_outer"] = None
        if cls.get("outer_class"):
            outer_info = self._resolve_generated(cls["outer_class"], self._cur_ns)
            if outer_info:
                self._dep(outer_info, by_value=True)

        # Members: fields/params/returns are pointers by default (so binding
        # types are forward-declared); enum & value-struct members force a full
        # include inside convert_type. The class's own type parameters (T, ...)
        # are in scope so a bare "T" maps to "T *".
        class_tps = frozenset(cls.get("type_params", []))
        for fld in cls["fields"]:
            fld["type"] = self.convert_type(fld["raw_type"], by_value=False, type_params_in_scope=class_tps)
        for prop in cls["properties"]:
            prop["type"] = self.convert_type(prop["raw_type"], by_value=False, type_params_in_scope=class_tps)
        for m in cls["methods"]:
            # Method type parameters extend the class's type params in scope.
            m_tps = frozenset(set(class_tps) | set(m.get("method_type_params", [])))
            m["return"] = self.convert_type(m["raw_return"], by_value=False, type_params_in_scope=m_tps)
            for p in m["params"]:
                p["type"] = self.convert_type(p["raw_type"], by_value=False, type_params_in_scope=m_tps)

        # Never include/forward-declare self. Use the resolved (possibly
        # arity-suffixed) C++ name/path from the registry.
        cpp_name = cls.get("cpp_name", cls["name"])
        self_rel = cls.get("rel_path")
        if not self_rel:
            parts = ns_to_parts(cls["namespace"])
            sub = "/".join(parts)
            self_rel = f"{sub}/{cpp_name}.hpp" if sub else f"{cpp_name}.hpp"
        self.cur_include_deps.discard(self_rel)
        self.cur_fwd_deps.discard(self._cur_ns + "::" + cpp_name)
        cls["include_deps"] = sorted(self.cur_include_deps)
        cls["fwd_deps"] = sorted(self.cur_fwd_deps)
        cls["bnm_includes"] = sorted(self.cur_bnm_includes)

    # ------------------------------------------------------------------
    def _generic_class_backtick_name(self, cls) -> str:
        """The IL2CPP generic type-definition name, e.g. MySingleton`1 / Duo`2.
        Nested types are flattened (Outer.Inner -> Outer_Inner), but the IL2CPP
        name keeps the dot and the arity is attached to the innermost type."""
        n = len(cls.get("type_params", []))
        raw = cls["raw_name"]
        # raw_name may carry a generic marker? The dump writes the C# name
        # without arity (MySingleton, Duo). Build `Name`N` from the raw name.
        return f"{raw}`{n}"

    def _generic_resolvers(self, cls) -> str:
        """The { ... } argument list for GetGeneric: one resolver per type param.
        TypeClassResolver resolves binding types via their StaticClass() and
        falls back to BNM::Defaults::Get<T>() for primitives/BNM types."""
        tps = cls.get("type_params", [])
        return ", ".join(f"BNM::Detail::TypeClassResolver<{p}>::Get()" for p in tps)

    @staticmethod
    def _wrapper_arity(type_params) -> int:
        """1/2/3 if the BNM_*_1/2/3 wrapper macros can express this type-param
        list, 0 otherwise. The wrappers rebuild the metadata name from
        #ClassName and the template decl/resolvers from the bare parameter
        names, so they only apply when 1 <= arity <= 3 and every parameter is
        a plain identifier that is not an environment macro."""
        if not 1 <= len(type_params) <= 3:
            return 0
        if not all(re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", p) for p in type_params):
            return 0
        if any(p in ENV_MACROS for p in type_params):
            return 0
        return len(type_params)

    def _emit_class_helpers(self, f):
        """The As<T>/IsA<T>/InnerClass boilerplate shared by every manually
        written class struct (generic and arity-renamed classes)."""
        f.write("    static inline BNM::Class InnerClass(const std::string_view &innerName) {\n")
        f.write("        return StaticClass().GetInnerClass(innerName);\n")
        f.write("    }\n\n")
        f.write("    template<typename Target>\n")
        f.write("    inline Target *As() const {\n")
        f.write("        if (!this || !BNM::CheckForNull((void *)this)) return nullptr;\n")
        f.write("        if constexpr (requires { Target::StaticClass(); }) {\n")
        f.write("            if (BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass())) return (Target *)this;\n")
        f.write("        } else { return (Target *)this; }\n")
        f.write("        return nullptr;\n")
        f.write("    }\n\n")
        f.write("    template<typename Target>\n")
        f.write("    inline bool IsA() const {\n")
        f.write("        if (!this || !BNM::CheckForNull((void *)this)) return false;\n")
        f.write("        if constexpr (requires { Target::StaticClass(); }) {\n")
        f.write("            return BNM::IsA((BNM::IL2CPP::Il2CppObject *)this, Target::StaticClass());\n")
        f.write("        }\n")
        f.write("        return false;\n")
        f.write("    }\n\n")

    def _emit_generic_class_open(self, f, cls, base):
        """Writes the opening of a generic class binding, preferring the
        BNM_GAME_GENERIC_CLASS_1/2/3 wrappers and falling back to
        BNM_GAME_GENERIC_CLASS_EX.

        The wrappers expand to the identical _EX call, so this only
        simplifies the generated source. They are used when the C++ identifier
        equals the C# name, the raw name carries no dot, arity is 1-3, and
        every type parameter is a plain identifier (no ENV macro) — see
        _wrapper_arity. Everything else (dotted nested names, arity-suffixed
        C++ identifiers, arity >= 4) needs _EX's explicit metadata name, since
        #ClassName stringification would produce the wrong IL2CPP name and a
        mismatch only fails at runtime inside GetGeneric."""
        tps = cls["type_params"]
        cpp_name = cls.get("cpp_name", cls["name"])
        n = self._wrapper_arity(tps)
        if (n and "." not in cls["raw_name"] and cpp_name == cls["raw_name"]
                and cpp_name not in ENV_MACROS):
            tpl = ", ".join(tps)
            f.write(f'BNM_GAME_GENERIC_CLASS_{n}("{cls["namespace"]}", {cpp_name}, '
                    f'({base}), {tpl}, "{cls["image"]}")\n\n')
            return
        tparams = ", ".join(f"typename {p}" for p in tps)
        backtick = self._generic_class_backtick_name(cls)
        resolvers = self._generic_resolvers(cls)
        f.write(f'BNM_GAME_GENERIC_CLASS_EX("{cls["namespace"]}", {cpp_name}, "{backtick}", '
                f'({base}), ({tparams}), ({resolvers}), "{cls["image"]}")\n\n')

    def _emit_renamed_class_open(self, f, cls, base):
        """Writes a NON-generic class whose C++ identifier was arity-suffixed to
        disambiguate an overload set (e.g. System.Action non-generic alongside
        Action<T>...Action<T16>). BNM_GAME_CLASS would stringify the C++
        identifier (Action_0) as the IL2CPP name, which is wrong, so the struct
        is written manually with the true C# name (raw_name)."""
        cpp_name = cls.get("cpp_name", cls["name"])
        raw = cls["raw_name"]
        f.write(f"struct {cpp_name} : public {base} {{\n")
        f.write(f"    {cpp_name}() : {base}() {{}}\n\n")
        f.write("    static inline BNM::Class StaticClass() {\n")
        f.write("        static auto cls = []() -> BNM::Class {\n")
        f.write(f'            return BNM::Class(BNM_OBFUSCATE("{cls["namespace"]}"), BNM_OBFUSCATE("{raw}"), BNM::Image(BNM_OBFUSCATE("{cls["image"]}")));\n')
        f.write("        }();\n")
        f.write("        return cls;\n")
        f.write("    }\n\n")
        self._emit_class_helpers(f)

    def _emit_generic_method(self, f, m):
        """Writes a generic method binding as a C++ template method that resolves
        via GetMethod(...).GetGeneric({...}). Prefers the
        BNM_BINDING_[STATIC_]GENERIC_METHOD_1/2/3 arity wrappers and falls back
        to the _EX form, keeping the IL2CPP name (raw_name) independent from
        the C++ identifier for renamed/env-macro cases.

        The _1/_2/_3 wrappers expand to the identical _EX call, so this only
        simplifies the generated source. They are used when the C++ identifier
        equals the C# name, the name is not an environment macro, and the
        type-parameter list is wrapper-safe (see _wrapper_arity). The body is
        written out manually whenever raw_name != name or the name is shadowed
        by an environment macro."""
        mtps = m["method_type_params"]
        ret = m["return"]
        np = len(m["params"])
        resolvers = ", ".join(f"BNM::Detail::TypeClassResolver<{p}>::Get()" for p in mtps)
        tparams = ", ".join(f"typename {p}" for p in mtps)
        pro, epi = env_macro_guard(m["name"])
        if m["name"] == m["raw_name"] and m["name"] not in ENV_MACROS:
            decl = "(" + ", ".join(f"{p['type']} {p['name']}" for p in m["params"]) + ")"
            call = "(" + ", ".join(p["name"] for p in m["params"]) + ")"
            n = self._wrapper_arity(mtps)
            if n:
                macro = f"BNM_BINDING_{'STATIC_' if m['is_static'] else ''}GENERIC_METHOD_{n}"
                tpl = ", ".join(mtps)
                f.write(f'    {macro}({m["raw_name"]}, ({ret}), {tpl}, '
                        f'{np}, {decl}, {call});\n')
                return
            macro = ("BNM_BINDING_STATIC_GENERIC_METHOD_EX" if m["is_static"]
                     else "BNM_BINDING_GENERIC_METHOD_EX")
            f.write(f'    {macro}({m["raw_name"]}, ({ret}), ({tparams}), ({resolvers}), '
                    f'{np}, {decl}, {call});\n')
            return
        static_kw = "static " if m["is_static"] else ""
        decl = ", ".join(f"{p['type']} {p['name']}" for p in m["params"])
        call = ", ".join(p["name"] for p in m["params"])
        if pro:
            f.write(pro)
        f.write(f"    template<{tparams}>\n")
        f.write(f"    {static_kw}inline {ret} {m['name']}({decl}) {{\n")
        if not m["is_static"]:
            f.write(f"        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<{ret}>();\n")
        f.write(f'        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE("{m["raw_name"]}"), {np})\n')
        f.write(f"            .GetGeneric({{{resolvers}}}).template cast<{ret}>();\n")
        if m["is_static"]:
            f.write(f"        return _method({call});\n")
        else:
            f.write(f"        return _method[(void *)this]({call});\n")
        f.write("    }\n")
        if epi:
            f.write(epi)

    def _emit_plain_method(self, f, m):
        """Writes a NON-generic method whose C# name is not a valid C++ identifier
        (e.g. an explicit interface implementation 'IFoo.Bar' -> identifier
        'IFoo_Bar') or collides with an environment macro (getchar/putchar/IN/...).
        BNM_BINDING_METHOD cannot be used because it stringifies its Name token for
        the IL2CPP lookup; here the method is written out with the C++ identifier
        (m['name']) and the IL2CPP name (m['raw_name']) kept apart."""
        ret = m["return"]
        static_kw = "static " if m["is_static"] else ""
        decl = ", ".join(f"{p['type']} {p['name']}" for p in m["params"])
        call = ", ".join(p["name"] for p in m["params"])
        np = len(m["params"])
        pro, epi = env_macro_guard(m["name"])
        if pro:
            f.write(pro)
        f.write(f"    {static_kw}inline {ret} {m['name']}({decl}) {{\n")
        if not m["is_static"]:
            f.write(f"        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<{ret}>();\n")
        f.write(f'        static auto _method = StaticClass().GetMethod(BNM_OBFUSCATE("{m["raw_name"]}"), {np}).template cast<{ret}>();\n')
        if m["is_static"]:
            f.write(f"        return _method({call});\n")
        else:
            f.write(f"        return _method[(void *)this]({call});\n")
        f.write("    }\n")
        if epi:
            f.write(epi)

    def _emit_field(self, f, fld):
        """Writes one field binding. Uses the BNM_BINDING_*_FIELD macro only when
        the C# name is already a valid C++ identifier (name == raw_name), is not
        an environment macro (stdin/stdout/...), and the type carries no comma (a
        comma inside a multi-arg generic type — e.g. Duo<int, PlayerController> or
        Dictionary<K, V> — would split the macro's arguments). Otherwise the
        accessor is written manually."""
        name, raw, typ = fld["name"], fld["raw_name"], fld["type"]
        off = f" // Offset: {fld['offset']}" if fld["offset"] else ""
        guarded = name in ENV_MACROS
        if name == raw and not guarded and "," not in typ:
            macro = "BNM_BINDING_STATIC_FIELD" if fld["is_static"] else "BNM_BINDING_FIELD"
            f.write(f"    {macro}({raw}, {typ});{off}\n")
            return
        if name == raw and not guarded and "," in typ:
            # Comma-bearing type (e.g. Dictionary<K, V> *): the generic macro
            # takes the whole type parenthesized so commas stay macro-safe.
            macro = "BNM_BINDING_GENERIC_STATIC_FIELD" if fld["is_static"] else "BNM_BINDING_GENERIC_FIELD"
            f.write(f"    {macro}({raw}, ({typ}));{off}\n")
            return
        pro, epi = env_macro_guard(name)
        if pro:
            f.write(pro)
        if fld["is_static"]:
            f.write(f"    static inline {typ} &{name}() {{\n")
            f.write(f'        static auto _field = StaticClass().GetField(BNM_OBFUSCATE("{raw}")).template cast<{typ}>();\n')
            f.write("        return *_field.GetPointer();\n")
            f.write("    }\n")
        else:
            f.write(f"    inline {typ} &{name}() {{\n")
            f.write(f'        static auto _field = StaticClass().GetField(BNM_OBFUSCATE("{raw}")).template cast<{typ}>();\n')
            f.write("        return *_field[(void *)this].GetPointer();\n")
            f.write("    }\n")
        if epi:
            f.write(epi)

    def _emit_property(self, f, prop):
        """Writes one property binding (Get<Name>/Set<Name>). Uses the plain
        macro for clean identifiers with comma-free types, the GENERIC macro for
        comma-bearing types (the type is parenthesized so commas stay
        macro-safe), and manual accessors otherwise (name differs from the C#
        name or collides with an environment macro)."""
        name, raw, typ = prop["name"], prop["raw_name"], prop["type"]
        if name == raw and name not in ENV_MACROS and "," not in typ:
            macro = "BNM_BINDING_STATIC_PROPERTY" if prop["is_static"] else "BNM_BINDING_PROPERTY"
            f.write(f"    {macro}({raw}, {typ});\n")
            return
        if name == raw and name not in ENV_MACROS and "," in typ:
            macro = "BNM_BINDING_GENERIC_STATIC_PROPERTY" if prop["is_static"] else "BNM_BINDING_GENERIC_PROPERTY"
            f.write(f"    {macro}({raw}, ({typ}));\n")
            return
        pro, epi = env_macro_guard(name)
        if pro:
            f.write(pro)
        # Manual accessors mirroring BNM_BINDING_PROPERTY / _STATIC_PROPERTY.
        inst_guard = "" if prop["is_static"] else \
            f"        if (!this || !BNM::CheckForNull((void *)this)) return BNM::Detail::DefaultReturnValue<{typ}>();\n"
        inst_guard_set = "" if prop["is_static"] else \
            "        if (!this || !BNM::CheckForNull((void *)this)) return;\n"
        this_arg = "" if prop["is_static"] else "[(void *)this]"
        static_kw = "static " if prop["is_static"] else ""
        # A static member function cannot be const-qualified.
        const_kw = "" if prop["is_static"] else " const"
        f.write(f"    {static_kw}inline {typ} Get{name}(){const_kw} {{\n")
        f.write(inst_guard)
        f.write(f'        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE("{raw}")).template cast<{typ}>();\n')
        f.write(f"        return _prop{this_arg}.Get();\n")
        f.write("    }\n")
        f.write(f"    {static_kw}inline void Set{name}({typ} value) {{\n")
        f.write(inst_guard_set)
        f.write(f'        static auto _prop = StaticClass().GetProperty(BNM_OBFUSCATE("{raw}")).template cast<{typ}>();\n')
        f.write(f"        _prop{this_arg}.Set(value);\n")
        f.write("    }\n")
        if epi:
            f.write(epi)

    # ------------------------------------------------------------------
    def generate(self):
        self.build_registry()
        print(f"[*] Generating C++ Game Binding headers to: {self.gb_root}")
        start_time = time.time()
        file_count = 0
        all_rel = []

        # Pre-convert every class's member types (needs the full registry).
        for cls in self.classes:
            self._convert_members(cls)

        # ---- Classes / interfaces / structs ----
        for cls in self.classes:
            parts = ns_to_parts(cls["namespace"])
            cpp_name = cls.get("cpp_name", cls["name"])
            rel = cls.get("rel_path")
            if not rel:
                sub = "/".join(parts)
                rel = f"{sub}/{cpp_name}.hpp" if sub else f"{cpp_name}.hpp"
            target_dir = self.gb_root / Path(*parts) if parts else self.gb_root
            target_dir.mkdir(parents=True, exist_ok=True)
            file_path = target_dir / f"{cpp_name}.hpp"
            all_rel.append(rel)

            with open(file_path, "w", encoding="utf-8") as f:
                f.write("#pragma once\n\n")
                f.write("#include <BNM/GameBinding.hpp>\n")
                for inc in cls["bnm_includes"]:
                    f.write(f"#include <{inc}>\n")
                for dep in cls["include_deps"]:
                    f.write(f'#include "{dep}"\n')
                f.write("\n")
                if cls["namespace"]:
                    f.write(f"// C# Namespace: {cls['namespace']}\n")
                f.write(f"// Assembly: {cls['image']}\n\n")

                # Forward-declare pointer-only dependencies (breaks include cycles).
                self._write_fwd(f, cls["fwd_deps"])

                self._open_ns(f, parts)

                base = cls["base_cpp"]
                is_generic_class = bool(cls.get("type_params"))
                is_renamed = cpp_name != cls["name"]
                # A base class with a comma (multi-argument template, e.g.
                # KeyedCollection<K, V>) splits the BNM_GAME_CLASS macro's
                # arguments, so that class is written out manually too.
                base_has_comma = "," in base
                if is_generic_class:
                    # Generic class -> manual template struct (BNM_GAME_CLASS
                    # cannot be a template; it stringifies the class name).
                    self._emit_generic_class_open(f, cls, base)
                elif is_renamed or base_has_comma:
                    # Non-generic class whose C++ name was arity-suffixed, or
                    # whose base type holds a comma -> manual struct so the
                    # macro's arguments stay intact and the IL2CPP name remains
                    # the true C# name.
                    self._emit_renamed_class_open(f, cls, base)
                elif cls["outer_class"]:
                    f.write(f"BNM_GAME_INNER_CLASS({cls['outer_class']}, {cpp_name}, {base})\n\n")
                elif cls["kind"] == "interface":
                    f.write(f'BNM_GAME_INTERFACE("{cls["namespace"]}", {cpp_name}, "{cls["image"]}")\n\n')
                else:
                    f.write(f'BNM_GAME_CLASS("{cls["namespace"]}", {cpp_name}, {base}, "{cls["image"]}")\n\n')

                if cls["fields"]:
                    f.write("    // --- Fields ---\n")
                    seen = set()
                    for fld in cls["fields"]:
                        if fld["name"] in seen:
                            continue
                        seen.add(fld["name"])
                        self._emit_field(f, fld)
                    f.write("\n")

                if cls["properties"]:
                    f.write("    // --- Properties ---\n")
                    seen = set()
                    for prop in cls["properties"]:
                        if prop["name"] in seen:
                            continue
                        seen.add(prop["name"])
                        self._emit_property(f, prop)
                    f.write("\n")

                if cls["methods"]:
                    f.write("    // --- Methods ---\n")
                    # BNM_BINDING_PROPERTY(Name, T) already emits Get<Name>() /
                    # Set<Name>(T). A C# method literally named "Set<Prop>" or
                    # "Get<Prop>" would collide with those accessors, so skip it
                    # (the property accessor already provides the binding).
                    prop_block = set()
                    for prop in cls["properties"]:
                        prop_block.add(f"Get{prop['name']}")
                        prop_block.add(f"Set{prop['name']}")
                    emitted = set()
                    # C++ cannot overload on return type alone. Two C# methods
                    # that share name + parameter types but differ only in return
                    # type (Decimal.Truncate(Decimal):Decimal vs private void,
                    # JToken.op_Explicit sets, ...) would be a compile error, so
                    # only the first is emitted.
                    emitted_by_params = set()
                    for m in cls["methods"]:
                        if m["name"] in prop_block:
                            continue
                        # Skip exact duplicate member signatures (defensive).
                        # Generic methods are keyed with their type params so a
                        # generic and a non-generic overload of the same name
                        # don't collapse into one. Dedup keys compare the
                        # normalised parameter types: intptr_t == int64_t / uintptr_t ==
                        # uint64_t on 64-bit targets, so C# assemblies that declare both
                        # spellings must not emit two members that C++ sees as one.
                        sig = (m["name"], m["return"],
                               tuple(self._dedup_type(p["type"]) for p in m["params"]),
                               tuple(m.get("method_type_params", [])))
                        if sig in emitted:
                            continue
                        params_sig = (m["name"], m["is_static"],
                                      tuple(self._dedup_type(p["type"]) for p in m["params"]))
                        if params_sig in emitted_by_params:
                            continue
                        emitted.add(sig)
                        emitted_by_params.add(params_sig)
                        if m.get("method_type_params"):
                            # Generic method -> template method (GetGeneric).
                            self._emit_generic_method(f, m)
                            continue
                        if m["name"] != m["raw_name"] or "," in m["return"] or m["name"] in ENV_MACROS:
                            # Explicit interface impl / keyword-named method, or a
                            # multi-arg generic return type (splits the macro's
                            # arguments), or an environment-macro name -> write
                            # the method out manually.
                            self._emit_plain_method(f, m)
                            continue
                        macro = "BNM_BINDING_STATIC_METHOD" if m["is_static"] else "BNM_BINDING_METHOD"
                        decl = "(" + ", ".join(f"{p['type']} {p['name']}" for p in m["params"]) + ")"
                        call = "(" + ", ".join(p["name"] for p in m["params"]) + ")";
                        f.write(f'    {macro}({m["raw_name"]}, {m["return"]}, {len(m["params"])}, {decl}, {call});\n')
                    f.write("\n")

                if is_generic_class:
                    f.write("BNM_GAME_GENERIC_CLASS_END\n")
                elif is_renamed or base_has_comma:
                    f.write("};\n")
                elif cls["outer_class"]:
                    f.write("BNM_GAME_INNER_CLASS_END\n")
                elif cls["kind"] == "interface":
                    f.write("BNM_GAME_INTERFACE_END\n")
                else:
                    f.write("BNM_GAME_CLASS_END\n")

                self._close_ns(f, parts)
            file_count += 1

        # ---- Enums ----
        for enm in self.enums:
            parts = ns_to_parts(enm["namespace"])
            cpp_name = enm.get("cpp_name", enm["name"])
            rel = enm.get("rel_path")
            if not rel:
                sub = "/".join(parts)
                rel = f"{sub}/{cpp_name}.hpp" if sub else f"{cpp_name}.hpp"
            target_dir = self.gb_root / Path(*parts) if parts else self.gb_root
            target_dir.mkdir(parents=True, exist_ok=True)
            file_path = target_dir / f"{cpp_name}.hpp"
            all_rel.append(rel)

            under = enm["underlying"] or "int"
            under_cpp = TYPE_MAP.get(under, under if under.endswith("_t") else "int")

            with open(file_path, "w", encoding="utf-8") as f:
                f.write("#pragma once\n\n")
                f.write("#include <cstdint>\n\n")
                if enm["namespace"]:
                    f.write(f"// C# Namespace: {enm['namespace']}\n")
                f.write(f"// Assembly: {enm['image']}\n\n")
                self._open_ns(f, parts)
                f.write(f"enum class {cpp_name} : {under_cpp} {{\n")
                for mem in enm["members"]:
                    pro, epi = env_macro_guard(mem["name"])
                    if pro:
                        f.write(pro)
                    if mem["val"]:
                        f.write(f"    {mem['name']} = {mem['val']},\n")
                    else:
                        f.write(f"    {mem['name']},\n")
                    if epi:
                        f.write(epi)
                f.write("};\n")
                self._close_ns(f, parts)
            file_count += 1

        # ---- Aggregate GameBinding.hpp ----
        agg = self.gb_root / "GameBinding.hpp"
        with open(agg, "w", encoding="utf-8") as f:
            f.write("#pragma once\n\n")
            f.write("// Aggregate header: includes every generated game binding.\n")
            f.write("// Include this single file to pull in all bindings.\n\n")
            f.write("#include <BNM/GameBinding.hpp>\n\n")
            for rel in sorted(set(all_rel)):
                f.write(f'#include "{rel}"\n')
        file_count += 1

        elapsed = time.time() - start_time
        print(f"[+] Generation complete! Created {file_count} headers (incl. GameBinding.hpp) in {elapsed:.2f}s.")

    # ------------------------------------------------------------------
    def _write_fwd(self, f, fwd_deps):
        """Writes forward declarations for pointer-only deps, grouped by namespace.

        fwd_deps entries are fully-qualified 'cpp_ns::Name' (e.g. 'GB::System::Array').
        Generic types are forward-declared with their template parameter list so
        references like MySingleton<PlayerController> * are valid.
        """
        if not fwd_deps:
            return
        groups = {}
        for qual in fwd_deps:
            ns, _, name = qual.rpartition("::")
            groups.setdefault(ns, []).append(name)
        f.write("// Forward declarations (pointer-only references).\n")
        for ns in sorted(groups):
            sub = ns.split("::")[1:]  # drop the leading GB
            DumpParser._open_ns(f, sub)
            for name in sorted(groups[ns]):
                info = self.by_qualified.get(f"{ns}::{name}")
                tps = info.get("type_params", []) if info else []
                if tps:
                    tparams = ", ".join(f"typename {p}" for p in tps)
                    f.write(f"template<{tparams}>\nstruct {name};\n")
                else:
                    f.write(f"struct {name};\n")
            DumpParser._close_ns(f, sub)
        f.write("\n")

    @staticmethod
    def _open_ns(f, parts):
        f.write(f"namespace {ROOT_NS} {{\n")
        for p in parts:
            f.write(f"namespace {p} {{\n")

    @staticmethod
    def _close_ns(f, parts):
        f.write("\n")
        for p in reversed(parts):
            f.write(f"}} // namespace {p}\n")
        f.write(f"}} // namespace {ROOT_NS}\n")


def main():
    if len(sys.argv) < 2:
        print("Usage: python BNM_GameBinding_Generator.py <path_to_dump.cs> [output_directory]")
        sys.exit(1)

    dump_path = sys.argv[1]
    out_dir = sys.argv[2] if len(sys.argv) > 2 else "GameBinding"

    if not os.path.exists(dump_path):
        print(f"[!] Error: File not found: {dump_path}")
        sys.exit(1)

    parser = DumpParser(dump_path, out_dir)
    parser.parse()
    parser.generate()


if __name__ == "__main__":
    main()
