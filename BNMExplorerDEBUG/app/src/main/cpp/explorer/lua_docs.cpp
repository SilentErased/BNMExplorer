#include "lua_docs.hpp"

const char k_llms_txt[] = R"TXT(# BNMExplorer and LuaBNM

> BNMExplorer runs a web based inspector inside an IL2CPP Unity game on Android or VR. LuaBNM is its embedded Lua 5.1 runtime: scripts can call any IL2CPP class, field, property and method by name, hook native methods with Lua callbacks, read XR controller input, send HTTP and HTTPS requests, parse JSON, read and write workspace files and raw memory. Docs site: https://silenterased.github.io/BNMExplorer/ - the mod itself also serves this exact file at http://DEVICE_IP:8080/llms.txt

This file documents everything: the web console, the startup manager, every LuaBNM function, the limits, the errors and a complete working VR mod menu at the end. Everything here is written against the actual C++ source, so names and behavior match what runs inside the game.

## What BNMExplorer is

BNMExplorer is a native mod (.so) you inject into a patched Unity IL2CPP game. On startup it detects the Unity version, resolves IL2CPP exports and starts an HTTP server on port 8080. You open `http://DEVICE_IP:8080` in a browser on the same network (on a VR headset browser use `https://127.0.0.1:8080`).

Tabs in the web UI:

- Scene - live hierarchy, inspector, component editor, scene dump.
- Class - assembly explorer with fields, properties and methods.
- Instances - live instances of a class.
- Inspector - inspect any object by address or static class.
- Watcher - hook game methods and log every call with arguments and callers.
- Loops - repeat field writes on an interval.
- Console - the LuaBNM editor, workspace files and output.
- Freeze - block Destroy and scene loads, with a tracker of blocked calls.
- Manager - freeze the game at startup so hooks can be installed before the game runs, plus Auto Lua.

## Getting started

1. Patch the target game so it loads the BNMExplorer native library.
2. Launch the game and find the device IP, for example with `adb shell ip addr show wlan0`.
3. Open `http://DEVICE_IP:8080` in a browser.
4. Open the Console tab, edit `main.lua` and press Run.

Everything a script prints shows up in the Output panel. Output from tasks that keep running after the chunk finished goes to the same panel over time.

## The web console and the workspace

Scripts live as files in a workspace folder on the device. The Console tab has a file tree, a tabbed editor with Lua syntax highlighting and autocomplete, and an output panel.

- Run - saves every dirty file, then runs the selected file. Run always executes `main.lua` unless you run a specific file from its tab.
- Stop - kills all tasks started by the last run and disables hook callbacks.
- Reset - closes the whole Lua state. The next Run starts fresh.
- New file / New folder - creates workspace entries. Files can have any extension.
- Upload - uploads a file of any type into the workspace (1 MB per file limit).
- Download - saves a workspace file back to your computer.
- Import ZIP - replaces the whole workspace with the contents of a zip. The old files are deleted, so save your work first.
- Export ZIP - downloads the workspace as a zip.
- Save On Device / Device ZIPs - stores and loads zips on the device storage (usually `/sdcard/BNMExplorer/zips/`).

Workspace limits: 1 MB per file, 8 MB total on one zip import, 500 files per zip, 8 folder levels deep.

### require(module_name)

Workspace modules work through `require`. It accepts these forms and looks in the workspace folder:

```lua
local movements = require("movements.lua")
local utils = require("lib.utils")
local utils2 = require("lib/utils")
local init = require("lib/init.lua")
local folder = require("movements")
```

- Dots in a module name become folder separators, so `"lib.utils"` loads `lib/utils.lua`.
- A name ending in `.lua` is used as is.
- If the name points at a folder, `folder/init.lua` is loaded.
- A module runs once per Run. Its return value (any Lua value) is cached and returned to every later `require` of the same module. A module that returns nothing gets `true`.
- Requiring a module from itself raises a "circular require" error.
- Each Run clears the module cache, so edits to required files apply on the next Run.

```lua
-- main.lua
local movements = require("movements.lua")
movements.hello()

-- movements.lua
local movements = {}
function movements.hello()
  print("module loaded")
end
return movements
```

## Startup Manager

The Manager tab can freeze the game at startup. BNM loads inside the IL2CPP `il2cpp_init` call, on the Unity main thread, before any scene or game code exists. When Freeze is armed, the mod pauses the main thread at that exact point. The web server keeps running, so you can:

- install method hooks with `hook_method`
- run scripts that only need IL2CPP classes (no scene objects yet)
- read the Manager tab status and last Auto Lua output

Pressing Resume lets the main thread continue and Unity finishes starting up, now with your hooks already installed.

Settings:

- Freeze on next launch - arms the pause for the next game start. The setting persists in `manager.json` next to the workspace folder (it survives workspace replacement).
- Timeout in seconds - auto resume after N seconds, 0 waits forever.
- Auto Lua on launch - runs a workspace file (default `main.lua`) at the same early point on every launch, right after the freeze is released. Hooks installed by that script apply before the first game function runs.
- Test run now - executes the Auto Lua file immediately from the browser.

Do not touch the device screen while the game is frozen - Android can show an Application Not Responding dialog for a frozen main thread. The freeze itself is safe; input is what triggers ANR.

## Lua runtime facts

- Language: Lua 5.1. Numbers are doubles, there is no integer type, no goto, no bitwise operators (use the `bit` library).
- Standard libraries are open except these removed globals: `dofile`, `loadfile`, `module`, `package`, `io`, `os`, `debug`, `newproxy`, `gcinfo`. `require` exists and loads workspace modules (see above).
- One persistent Lua state. Global variables and closures survive between Runs until Reset.
- Chunk timeout: 5 seconds. Scheduler callback timeout: 1 second per call. A timeout raises the error "script timeout".
- `print` joins values with tabs. Values returned at the end of a chunk print one per line.
- Errors come back as "SyntaxError: ..." (compile stage) or "RuntimeError: ..." (runtime). An error inside a task prints "RuntimeError: ..." and kills that task. An error inside a hook callback prints "[Hook N Class::Method] ..." and the original method runs.
- Output from tasks and hooks goes to an async buffer capped at 65536 characters, oldest lines drop first.
- Running a new file stops all tasks from the previous run, disables hook callbacks (the native trampolines stay installed but forward to the original methods) and clears the module cache.

## Value conversion between Lua and IL2CPP

Method arguments convert by the declared IL2CPP parameter type. Return values convert back the same way.

- `bool` - Lua boolean. Any truthy value is accepted when passing.
- `int8, uint8, int16, uint16, int32, uint32, int64, uint64, float, double, char, IntPtr` - Lua number. Enums are plain numbers.
- `string` - Lua string. Passing nil gives a null string, a null string comes back as nil.
- `Vector2, Vector3, Vector4, Quaternion, Color` - tables from `vec2`, `vec3`, `vec4`, `quat`, `color`. Any table with `x, y, z, w` or `r, g, b, a` number fields is accepted too. A Color with fewer than 4 components gets alpha 1.
- Object parameters - a reference, nil for null, a string (converted to a managed string), or in loose matching a raw address number.
- Other value types (structs) - pass a boxed struct reference or a raw address to copy from. Struct returns are boxed and come back as a reference whose fields you can read and write normally:

```lua
local bounds = some_collider.bounds
print(bounds.center.x, bounds.size.y)
```

- `ref` and `out` parameters (byref) - pass an address number or a reference; the parameter arrives as a raw pointer. Read and write the pointed-to data with the `memory` library.
- Methods returning void return nothing. Overloads are picked by argument count first, then by matching Lua argument types against the declared parameter types.
- A wrong argument count raises "method 'Name' expects N arguments, got M".

## Vectors, quaternions and colors

Constructors:

- `vec2(x, y) -> vector`
- `vec3(x, y, z) -> vector`
- `vec4(x, y, z, w) -> vector`
- `quat(x, y, z, w) -> quaternion` - missing values default to 0 except w which defaults to 1
- `color(r, g, b, a) -> color` - missing values default to 0 except a which defaults to 1

Fields: `vec2/vec3/vec4` and `quat` use `x, y, z, w`. `color` uses `r, g, b, a`. The tables are mutable.

Operators, component-wise with the shorter operand deciding the result length:

- `a + b`, `a - b`, `a * b`, `a / b` (division by zero gives 0)
- `vector * number`, `number * vector`, `vector / number`
- `-a`, `a == b` (equal only when the length matches and every component matches)
- `tostring(v)` gives `(x, y, z)`

Methods:

- `v:length() -> number`
- `v:normalized() -> vector` - a zero vector stays zero
- `v:dot(other) -> number`
- `v:distance(other) -> number`

```lua
local dir = (target_pos - hand_pos):normalized()
local half_red = color(1, 0, 0, 0.5)
```

## Object references

`find_class`, `wrap`, component lookups, method returns and field reads give back reference userdata. A reference stores a class and, for instances, an object pointer protected by a GC handle, so the object cannot be collected while Lua holds it. A class reference (from `find_class`) has no instance and reads as the static side.

Special members on every reference:

- `ref.addr -> number` - object address, 0 for class references
- `ref.class_name -> string` - class name without namespace
- `ref.full_name -> string` - namespace-qualified class name
- `ref:is_valid() -> boolean`
- `ref:get_class() -> class reference` - class of this object
- `ref:get_field(name) -> value` - field read by name, instance or static
- `ref:set_field(name, value)` - field write by name
- `ref:get_method(name, [argc]) -> method handle`
- `ref:get_component(type) -> reference or nil`
- `ref:add_component(type) -> reference`
- `ref:get_component_in_children(type) -> reference or nil`
- `ref:destroy([delay_seconds])`
- `ref:find_objects() -> table` - all live objects of this class
- `ref:new(...) -> reference` - constructs an instance, picking the `.ctor` overload by arguments
- `ref:debug_info() -> string` - address, class, cached pointer, validity
- reflection sugar (see the Reflection section): `ref:list_methods([filter, inherited])`, `ref:list_fields()`, `ref:list_properties()`, `ref:list_members()`, `ref:list_parents()`, `ref:list_interfaces()`, `ref:list_nested()`, `ref:class_info()`, `ref:dump()`

The `type` argument of the component functions is a full class name string like `"UnityEngine.Rigidbody"` or a class reference.

Everything else is looked up by member name on the class and its parents:

- Properties: `ref.Name` reads through the getter, `ref.Name = value` writes through the setter.
- Fields: `ref.Name` reads the field, `ref.Name = value` writes it.
- Methods: `ref:Name(args)` for instance methods, `ref.Name(args)` for statics. Calling an instance method with a dot and no self also works.
- Unknown names raise "member 'Name' not found".

References compare with `==` when they point at the same object. `tostring(ref)` gives `Namespace.Class<ObjectName @ 0xADDRESS>`.

```lua
local player = game_object.Find("GorillaPlayer")
local body = player:get_component("UnityEngine.Rigidbody")
body.velocity = vec3(0, 10, 0)
player.transform.localScale = vec3(1.5, 1.5, 1.5)
local pos = player.transform.position
print(pos.x, pos.y, pos.z)
```

Struct fields and struct returns (like the `bounds` example above) are boxed IL2CPP objects. Reading `boxed.field` works, writing `boxed.field = value` works, and writing the boxed value back into the original field is not needed - struct field reads box a copy, so assign through the owning object when you want to change the owner:

```lua
local t = some_object.transform
local p = t.localPosition
p.y = p.y + 1
t.localPosition = p
```

### is_valid(value) -> boolean

- nil -> false
- non-reference values -> true
- reference with an unresolved class -> false (`find_class` returns such a reference when the class does not exist)
- class reference -> true
- instance reference -> true only when the pointer is plausible and, for UnityEngine.Object subclasses, `m_CachedPtr` is non-null. Destroyed Unity objects return false.

Check `is_valid` before touching objects that can be destroyed at any time.

### type_name(value) -> string

- reference -> full class name, for example "UnityEngine.GameObject"
- method handle -> "method"
- vector or color table -> "vec"
- anything else -> the Lua type name

### wrap(address) -> reference or nil

Casts a numeric address into a reference after plausibility checks on the pointer and its class pointer. Returns nil when the address looks invalid.

### find_class(full_name, [assembly_name]) -> class reference

Looks up a class by namespace-qualified name. The text after the last dot is the class name, the rest is the namespace. The optional assembly name restricts the lookup, for example "Assembly-CSharp.dll". Results are cached. A missing class does not raise - the returned reference is just invalid.

```lua
local photon = find_class("Photon.Pun.PhotonNetwork")
if is_valid(photon) then photon.Disconnect() end
```

### assembly(name)

Prelude helper for assembly-scoped lookups:

- `assembly(name):namespace(ns):class(class_name, [address]) -> reference` - wraps `address` instead of looking up when a nonzero address is given
- `assembly(name):class(full_class_name, [address]) -> reference` - same with a name that already includes the namespace

```lua
local player_class = assembly("Assembly-CSharp.dll"):namespace("GorillaLocomotion"):class("Player")
```

### Method handles

`ref:get_method(name, [argc]) -> handle` returns a method bound to that reference's instance. Class references give static handles. A missing method gives an invalid handle.

- `handle(args...)` or `handle:invoke(args...)` - calls the method
- `handle:is_valid() -> boolean` - exists and has compiled code
- `handle:get_name() -> string`
- `handle:get_param_count() -> number`
- `handle:get_return_type() -> string`
- `handle:set_instance(ref_or_nil)` - rebinds the instance, nil for static

```lua
local method = player:get_method("Awake", 0)
if method:is_valid() then method() end
```

### get_builtin_resource(type, path) -> reference or nil

Loads a built-in Unity resource through `UnityEngine.Resources.GetBuiltinResource`. Common font paths: "Arial.ttf", "LegacyRuntime.ttf".

```lua
local font = get_builtin_resource("UnityEngine.Font", "LegacyRuntime.ttf")
```

## Scene queries

- `scene.get_objects() -> table` - every live `UnityEngine.GameObject`
- `scene.find_object(name) -> reference or nil` - `GameObject.Find`, active objects only
- `scene.find_objects_of_type(type) -> table` - references of that class
- `scene.SomeName` or `scene["Some Name"]` - shorthand for `find_object`
- `find_object(name) -> reference or nil` and `find_objects_of_type(type) -> table` exist as globals too
- `destroy(object_or_address, [delay_seconds])` - `UnityEngine.Object.Destroy`, also `ref:destroy([delay])`

```lua
local camera = scene["Main Camera"]
for _, body in ipairs(find_objects_of_type("UnityEngine.Rigidbody")) do
  print(body.gameObject.name)
end
destroy(camera, 2.0)
```

## Scheduler and the main thread

All timed callbacks share one task list. Every task has a numeric id.

- `on_update(fn) -> id` - repeats every pump, zero interval
- `every(seconds, fn) -> id` - repeats every N seconds, negatives clamp to 0
- `after(seconds, fn) -> id` - one shot after N seconds
- `cancel(id) -> boolean` - true when the id existed
- `cancel_all()` - kills every task
- `get_time() -> number` - monotonic seconds, only differences are meaningful
- `on_game_start(fn) -> id` - waits until the first Unity frame rendered (`Time.frameCount > 0`, checked every 0.25 s) and then runs `fn` once. Use it for code that needs a live scene.
- `main_thread(fn)` - queues `fn` onto the Unity main thread. The queue is pumped from Unity frames once a frame hook exists (see below); if no frame hook can be found the worker thread runs the callback after 3 seconds. Errors print as "MainThreadError: ...".

Pumping and threads:

- By default a background worker thread pumps due tasks about every 8 ms. Unity API calls from that thread work for many things but can crash on others.
- `set_tick_hook(full_class_name, method_name, [assembly_name]) -> boolean` installs a native hook on a zero argument void method (a game class Update or FixedUpdate for example). After it succeeds, tasks and the `main_thread` queue pump on the Unity main thread from inside that method, throttled to once per 4 ms, and the background worker stops pumping.
- The web UI also installs Unity frame hooks for scene browsing (Camera OnPreCull and friends). When those exist, tasks pump on the main thread even without `set_tick_hook`.
- Only one tick hook can exist and it cannot be removed. Guard it with `has_tick_hook()`.
- `set_tick_hook` errors: "tick hook is already installed", "class 'X' not found", "method 'Y' without arguments not found", "method 'Y' must return void", "failed to hook 'Y'".
- Re-running a script does not remove old tasks by itself - Run does that for you. When you rerun code manually inside one chunk, call `cancel_all()` first.

```lua
if not has_tick_hook() then
  pcall(set_tick_hook, "GorillaLocomotion.Player", "FixedUpdate", "Assembly-CSharp.dll")
end

on_game_start(function()
  print("first frame", time.frameCount)
end)

main_thread(function()
  scene["Main Camera"].tag = "EditorOnly"
end)
```

## Native method hooks

`hook_method(class_name, method_name, selector, callback, [assembly_name]) -> hook_id`

Installs a native hook on a compiled IL2CPP method. The Lua callback runs synchronously on the calling thread of the hooked method. Lua state access is serialized with a mutex, so hooks and the console never run at the same time.

The selector picks the overload:

- a number - exact parameter count, not counting self
- a string - signature like `"float,string"` using short type names (int, uint, long, ulong, short, ushort, byte, sbyte, float, bool map to their full names)
- nil - only valid when exactly one overload exists

When several overloads match, the error lists every candidate signature.

Callback arguments: instance methods get `self` first, then the declared parameters. Statics get only their parameters. Supported parameter kinds:

- booleans, all integer types, enums - numbers
- strings - Lua strings
- objects - references
- `float` and `double` - numbers
- `Vector2`, `Vector3`, `Vector4`, `Color` - vec tables
- `ref` / `out` (byref) parameters - raw addresses as numbers; use `memory.read*` / `memory.write*` on them

Limits:

- Android ARM64 only.
- At most 16 parameters, and everything must fit the calling convention registers: 8 integer (self counts on instance methods) and 8 float. Arguments that would go on the stack are rejected.
- Struct parameters by value, generic methods and abstract bodies are rejected.
- Slot budget: 256 hooks for everything returning a word sized value (numbers, bools, enums, strings, objects, void), 48 hooks for methods returning `float`, `double`, `Vector2`, `Vector3`, `Vector4` or `Color`. Hooking the same address again replaces the callback and resets its call counter.

Return behavior:

- void method - return `true` or `"original"` to also run the original, return `false` or `nil` to skip it.
- non-void method - return `nil` or `"original"` to run the original, return a compatible value to replace the result.
- numbers for numeric and enum returns, `float` and `double` take numbers
- strings for string returns
- objects for object returns: a reference, a string (creates a new managed string, handy because System.String is an object), or `{addr = 0x...}` for any pointer you built
- vec tables for `Vector2/3/4` and `Color` returns, a color with 3 components gets alpha 1
- `HOOK_NULL` - a null reference for string and object returns

Hook management:

- `unhook_method(hook_id) -> boolean` - also named `disable_method_hook(hook_id)`. Disables the callback, the trampoline stays and forwards to the original.
- `unhook_all_methods() -> number` - disables every active callback, returns the count.
- `has_method_hook(hook_id) -> boolean`
- `hook_list() -> table` - array of `{id, name, active, calls}` for every installed hook
- `get_hook(hook_id) -> table or nil` - `{id, name, active, calls, is_static, param_count, return_type, address}`. `return_type` is one of "void", "bool", "int8".."uint64", "float", "double", "string", "vec2", "vec3", "vec4", "color", "enum", "object"
- `set_hook_callback(hook_id, fn) -> boolean` - swaps the callback on an installed hook without reinstalling, resets the call counter and reactivates the hook
- `hook_original(...)` - inside a callback, calls the original method and returns its result. With no arguments it forwards the current arguments. Pass replacements for any prefix of the arguments to modify what the original sees. Byref and vector arguments take addresses and vec tables.
- `hook_caller() -> string` - inside a callback, describes the caller (managed method or module+offset)

A Lua error inside a callback prints "[Hook N Class::Method] error text" and the original method runs.

```lua
local id = hook_method("PlayerHealth", "TakeDamage", "float", function(self, amount)
  print(hook_caller())
  return hook_original(self, amount * 0.5)
end)

hook_method("GameManager", "GetPlayerName", 0, function(self)
  return "LuaPlayer"
end)

hook_method("CameraRig", "GetOffset", 0, function(self)
  return vec3(0, 2, -5)
end)

hook_method("TeamManager", "GetPlayers", 0, function(self)
  return make_array("UnityEngine.GameObject", { scene["Player 1"], scene["Player 2"] })
end)

print(get_hook(id).return_type, hook_list()[1].calls)
set_hook_callback(id, function(self, amount) return 0 end)
```

## Networking

`http.request(options) -> response`

Performs a blocking request. Call it from the console, from `main_thread` or from tasks - it blocks the calling thread for up to the timeout. Options table:

- `url` - required, "http://" or "https://"
- `method` - "GET" (default), "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"
- `headers` - table of strings
- `body` - request body string
- `content_type` - defaults to "application/octet-stream", a default User-Agent "LuaBNM" is always sent
- `timeout` - seconds, default 10
- `follow_redirects` - default true

Successful response: `{ok = true, status, body, headers}` - body is the raw response string, headers maps lowercased names as sent by the server. Failed connection: `{ok = false, error, error_code}`.

HTTPS needs a build of the mod compiled with `BNM_ENABLE_HTTPS` (OpenSSL linked in). On a plain build an https URL raises a clear error.

Shortcuts:

- `http.get(url, [headers_table], [timeout_seconds]) -> response`
- `http.post(url, [body], [headers_table], [content_type]) -> response` - body as a string defaults the content type to "application/json"

```lua
local r = http.get("http://example.com/api/version")
if r.ok and r.status == 200 then
  print(r.body)
end

local r2 = http.post("http://example.com/api/log", json.encode({name = "Lua", hp = 100}))
if r2.ok then
  local data = json.decode(r2.body)
  print(data and data.name)
end
```

## JSON

- `json.encode(value) -> string` - converts a Lua value to JSON. Tables with sequential numeric keys become arrays, tables with string keys become objects, nil inside objects drops the key, functions and userdata fail with an error. Avoid nil holes in arrays - `{1, nil, 3}` has an undefined length in Lua.
- `json.decode(text) -> value, error` - parses JSON into Lua tables. Arrays become 1-based tables, null becomes nil. On bad input returns nil and an error message.
- `json.stringify` is an alias for `json.encode`, `json.parse` is an alias for `json.decode`.

## Files

The workspace holds files of any type: .lua scripts, .png textures, .json data, raw .bin blobs.

- `fs.list([dir]) -> table` - array of entries `{path, name, dir, lua, size}`. `path` is workspace relative, `dir` is true for folders, `lua` is true for .lua files, `size` is bytes. Without arguments it lists the root.
- `fs.read(path) -> string or nil, error` - raw bytes as a Lua string, binary safe.
- `fs.write(path, data) -> boolean` - writes bytes, creates parent folders. 1 MB limit.
- `fs.exists(path) -> boolean`
- `fs.size(path) -> number` - errors when the file is missing
- `fs.delete(path) -> boolean`
- `fs.mkdir(path) -> boolean` - errors on an invalid folder name

Paths are workspace relative, `/` separated, no `..`, case sensitive.

```lua
for _, e in ipairs(fs.list()) do
  print(e.path, e.size)
end

local png = fs.read("icon.png")
print(#png, base64.encode(png:sub(1, 16)))

local tex = find_class("UnityEngine.Texture2D"):new(2, 2, 4, false)
tex:LoadImage(make_byte_array(png))
```

`base64`:

- `base64.encode(data_string) -> string`
- `base64.decode(text) -> string` - stops at padding, ignores invalid characters

## Memory

Raw memory access by address. Reads probe the memory first and return nil when unreadable. Writes do not probe - a bad address will crash the game, that is on you.

- `memory.read8(addr) -> number or nil`
- `memory.read16(addr) -> number or nil`
- `memory.read32(addr) -> number or nil`
- `memory.read64(addr) -> number or nil`
- `memory.read_float(addr) -> number or nil`
- `memory.read_double(addr) -> number or nil`
- `memory.read_bytes(addr, length) -> string or nil` - up to 65536 bytes
- `memory.read_string(addr, [max_length]) -> string or nil` - reads a C string until a zero byte, default max 256
- `memory.write8(addr, value)`
- `memory.write16(addr, value)`
- `memory.write32(addr, value)`
- `memory.write64(addr, value)`
- `memory.write_float(addr, value)`
- `memory.write_double(addr, value)`
- `memory.write_bytes(addr, data_string)` - up to 65536 bytes
- `memory.nop(addr, length) -> boolean` - fills whole instructions with ARM64 NOPs (1..1024 bytes), makes the page writable with mprotect and clears the instruction cache

Integer reads are signed, 64 bit values pass through doubles so precision is limited to 2^53 like everywhere else in Lua.

```lua
local hp = memory.read32(0x7ABC1230)
memory.write_float(0x7ABC1234, 99.5)
local name = memory.read_string(0x7ABC1240, 64)
local data = memory.read_bytes(wrap(0x7ABC1000).addr, 16)
```

## bit

Works on 64 bit values. Results come back as doubles, so values above 2^53 lose precision.

- `bit.band(a, ...)` - and of all arguments
- `bit.bor(a, ...)`
- `bit.bxor(a, ...)`
- `bit.lshift(value, shift)`
- `bit.rshift(value, shift)` - logical, fills with zeros
- `bit.arshift(value, shift)` - arithmetic, fills with the sign bit
- `bit.bnot(value)`

```lua
local flags = bit.band(bit.bor(0x1, 0x4), 0xFF)
```

## Enums

- `enum_values(class) -> table` - `{Name = value}` for every constant, class from a name string or reference. Statics only, the backing `value__` field is skipped.
- `enum_name(class, value) -> string or nil` - first name matching the value.

```lua
local platform = enum_values("UnityEngine.RuntimePlatform")
print(platform.WindowsPlayer, enum_name("UnityEngine.RuntimePlatform", 13))
```

## Reflection and search

Search across every loaded assembly. All of these take an optional `path` filter shaped like `assembly/namespace/class` where each part is a partial case-insensitive match, and an optional `limit` (default 200, max 5000). A search text may use `Class::member` or `Class.member` to scope by class.

- `find_classes(text, [path, limit]) -> table, truncated` - class references
- `find_methods(text, [path, limit]) -> table, truncated` - hits `{kind, name, type, params, class, namespace, assembly, static, address}`
- `find_fields(text, [path, limit]) -> table, truncated`
- `find_properties(text, [path, limit]) -> table, truncated`
- `find_members(text, [path, limit]) -> table, truncated` - methods plus fields plus properties
- `list_assemblies() -> table` - assembly names
- `list_classes([path, limit]) -> table` - class references, default limit 2000
- `dump_class(name) -> string` - one class as C# shaped text

The second return value is true when the result list hit the limit.

Class references also carry per-class reflection:

- `ref:list_methods([filter, inherited]) -> table`
- `ref:list_fields([filter, inherited]) -> table`
- `ref:list_properties([filter, inherited]) -> table`
- `ref:list_members([filter, inherited]) -> table`
- `ref:list_parents() -> table` - parent class names
- `ref:list_interfaces() -> table`
- `ref:list_nested() -> table` - nested class references
- `ref:class_info() -> table` - `{name, namespace, full_name, assembly, kind, parent, abstract, method_count, field_count, property_count, interface_count}`, kind is "class", "struct", "enum" or "interface"
- `ref:dump() -> string`

```lua
for _, m in ipairs(find_class("PlayerHealth"):list_methods("damage", true)) do
  print(m.class .. "." .. m.name .. "(" .. m.params .. ") : " .. m.type .. " @ " .. hex(m.address))
end
print(dump_class("UnityEngine.GameObject"))
```

## Arrays and lists

- `to_table(object) -> table or nil` - converts an IL2CPP array (`T[]`) or `List<T>` into a 1-based Lua table. Elements convert by their element class: numbers, bools, strings, references, structs get boxed.
- `make_array(element_class, lua_table) -> reference` - creates a real IL2CPP array. Elements can be references, strings, numbers (numeric element classes) or boxed structs. Pass the result anywhere an array parameter is expected or return it from a hook.
- `make_byte_array(data) -> reference` - a `byte[]` from a Lua string (binary safe) or a table of numbers.

```lua
local bodies = to_table(find_objects_of_type("UnityEngine.Rigidbody"))
for i, body in ipairs(bodies) do print(i, body.name) end

local names = make_array("System.String", {"a", "b", "c"})
some_ref:SetNames(names)
```

## XR input

Reads `UnityEngine.XR.InputDevices`. Needs the game to run Unity XR.

Side argument: "left" or "right" (only the first letter matters, anything starting with l is left). A number also works: 0 is left, anything else is right.

Feature names ignore case, underscores and spaces, so `grip_button`, `GripButton` and "grip button" are the same.

- `xr.is_available() -> boolean`
- `xr.get_bool_feature(name, side) -> boolean`
- `xr.get_float_feature(name, side) -> number`
- `xr.get_vector2_feature(name, side) -> vector` - `{x, y}`
- `xr.send_haptic_impulse(side, amplitude, duration) -> boolean` - channel 0

Bool features: `primary_button`, `primary_touch`, `secondary_button`, `secondary_touch`, `grip_button`, `trigger_button`, `menu_button`, `primary_2d_axis_click`, `primary_2d_axis_touch`, `secondary_2d_axis_click`, `secondary_2d_axis_touch`.

Float features: `trigger`, `grip`. Vector2 features: `primary_2d_axis`, `secondary_2d_axis`.

An unknown feature raises an error. A missing device returns false, 0 or `vec2(0, 0)`. On Quest style controllers `primary_button` is A/X and `secondary_button` is B/Y.

```lua
if xr.get_bool_feature("grip_button", "left") then
  xr.send_haptic_impulse("left", 0.5, 0.1)
end
```

## Utility functions

- `hex(number) -> string` - formats as "0x..."
- `parse_hex(text) -> number` - parses "0x1A2B" or "1A2B"
- `set_timescale(value)` - writes `Time.timeScale`
- `get_timescale() -> number`
- `log_info(...)`, `log_warn(...)`, `log_error(...)` - like print but also writes to the Android logcat with a matching priority
- `il2cpp_loaded() -> boolean` - true once BNM finished loading IL2CPP (always true inside a working script)
- `destroy(object_or_address, [delay])` - destroy a Unity object

Prelude globals defined before every script:

- `game_object` - UnityEngine.GameObject class reference
- `unity_object` - UnityEngine.Object
- `time` - UnityEngine.Time, so `time.frameCount` and `time.deltaTime` work
- `shader` - UnityEngine.Shader, `shader.Find(name)`
- `quaternion`, `mathf`, `physics` - matching UnityEngine classes
- `force_mode` - `{force = 0, impulse = 1, velocity_change = 2, acceleration = 5}`
- `primitive_type` - `{sphere = 0, capsule = 1, cylinder = 2, cube = 3, plane = 4, quad = 5}`

Prelude helpers:

- `find_shader([name]) -> reference or nil` - with a name it calls `shader.Find(name)`. Without one it tries "Standard", "GorillaTag/UberShader", "Universal Render Pipeline/Lit", "Unlit/Color" and returns the first hit.
- `create_primitive(kind, [color], [shader_name]) -> reference` - creates a `GameObject.CreatePrimitive`, assigns a shader, applies the color, enables the renderer.

```lua
local cube = create_primitive(primitive_type.cube, color(1, 0, 0, 1), "Unlit/Color")
cube.transform.position = vec3(0, 2, 0)
cube:destroy(5.0)
```

## API reference

Quick list of every global. Detailed behavior lives in the sections above.

Core and values:

- `print(...) -> nil`
- `type_name(value) -> string`
- `is_valid(value) -> boolean`
- `wrap(address) -> reference or nil`
- `get_time() -> number`
- `hex(number) -> string`
- `parse_hex(text) -> number`
- `il2cpp_loaded() -> boolean`
- `vec2(x, y) -> vector`
- `vec3(x, y, z) -> vector`
- `vec4(x, y, z, w) -> vector`
- `quat(x, y, z, w) -> quaternion`
- `color(r, g, b, a) -> color`

Classes and objects:

- `find_class(full_name, [assembly_name]) -> reference`
- `assembly(name) -> assembly helper`
- `get_builtin_resource(type, path) -> reference or nil`
- `destroy(object_or_address, [delay_seconds])`
- `find_object(name) -> reference or nil`
- `find_objects_of_type(type) -> table`

Scene:

- `scene.get_objects() -> table`
- `scene.find_object(name) -> reference or nil`
- `scene.find_objects_of_type(type) -> table`
- `scene.AnyName -> reference or nil`

Scheduler:

- `on_update(fn) -> id`
- `every(seconds, fn) -> id`
- `after(seconds, fn) -> id`
- `cancel(id) -> boolean`
- `cancel_all()`
- `on_game_start(fn) -> id`
- `main_thread(fn)`
- `set_tick_hook(full_class_name, method_name, [assembly_name]) -> boolean`
- `has_tick_hook() -> boolean`

Hooks:

- `hook_method(class_name, method_name, selector, callback, [assembly_name]) -> hook_id`
- `unhook_method(hook_id) -> boolean`
- `disable_method_hook(hook_id) -> boolean`
- `unhook_all_methods() -> number`
- `has_method_hook(hook_id) -> boolean`
- `hook_list() -> table`
- `get_hook(hook_id) -> table or nil`
- `set_hook_callback(hook_id, fn) -> boolean`
- `hook_original([...]) -> value`
- `hook_caller() -> string`
- `HOOK_NULL -> userdata`

Networking and data:

- `http.request(options) -> response`
- `http.get(url, [headers, timeout]) -> response`
- `http.post(url, [body, headers, content_type]) -> response`
- `json.encode(value) -> string`
- `json.decode(text) -> value, error`
- `fs.list([dir]) -> table`
- `fs.read(path) -> string or nil, error`
- `fs.write(path, data) -> boolean`
- `fs.exists(path) -> boolean`
- `fs.size(path) -> number`
- `fs.delete(path) -> boolean`
- `fs.mkdir(path) -> boolean`
- `base64.encode(data) -> string`
- `base64.decode(text) -> string`

Memory and bits:

- `memory.read8(addr) -> number or nil`
- `memory.read16(addr) -> number or nil`
- `memory.read32(addr) -> number or nil`
- `memory.read64(addr) -> number or nil`
- `memory.read_float(addr) -> number or nil`
- `memory.read_double(addr) -> number or nil`
- `memory.read_bytes(addr, length) -> string or nil`
- `memory.read_string(addr, [max]) -> string or nil`
- `memory.write8(addr, value)`
- `memory.write16(addr, value)`
- `memory.write32(addr, value)`
- `memory.write64(addr, value)`
- `memory.write_float(addr, value)`
- `memory.write_double(addr, value)`
- `memory.write_bytes(addr, data)`
- `memory.nop(addr, length) -> boolean`
- `bit.band(a, ...) -> number`
- `bit.bor(a, ...) -> number`
- `bit.bxor(a, ...) -> number`
- `bit.lshift(value, shift) -> number`
- `bit.rshift(value, shift) -> number`
- `bit.arshift(value, shift) -> number`
- `bit.bnot(value) -> number`

Reflection:

- `find_classes(text, [path, limit]) -> table, truncated`
- `find_methods(text, [path, limit]) -> table, truncated`
- `find_fields(text, [path, limit]) -> table, truncated`
- `find_properties(text, [path, limit]) -> table, truncated`
- `find_members(text, [path, limit]) -> table, truncated`
- `list_assemblies() -> table`
- `list_classes([path, limit]) -> table`
- `dump_class(name) -> string`
- `enum_values(class) -> table`
- `enum_name(class, value) -> string or nil`
- `to_table(object) -> table or nil`
- `make_array(element_class, table) -> reference`
- `make_byte_array(data) -> reference`

Reference and handle members (colon syntax):

- `ref.addr -> number`, `ref.class_name -> string`, `ref.full_name -> string`
- `ref:is_valid() -> boolean`
- `ref:get_class() -> reference`
- `ref:get_field(name) -> value`
- `ref:set_field(name, value)`
- `ref:get_method(name, [argc]) -> handle`
- `ref:get_component(type) -> reference or nil`
- `ref:add_component(type) -> reference`
- `ref:get_component_in_children(type) -> reference or nil`
- `ref:destroy([delay_seconds])`
- `ref:find_objects() -> table`
- `ref:new(...) -> reference`
- `ref:debug_info() -> string`
- `ref:list_methods([filter, inherited]) -> table`
- `ref:list_fields([filter, inherited]) -> table`
- `ref:list_properties([filter, inherited]) -> table`
- `ref:list_members([filter, inherited]) -> table`
- `ref:list_parents() -> table`
- `ref:list_interfaces() -> table`
- `ref:list_nested() -> table`
- `ref:class_info() -> table`
- `ref:dump() -> string`
- `handle:invoke(args...) -> value`
- `handle:is_valid() -> boolean`
- `handle:get_name() -> string`
- `handle:get_param_count() -> number`
- `handle:get_return_type() -> string`
- `handle:set_instance(ref_or_nil)`

XR:

- `xr.is_available() -> boolean`
- `xr.get_bool_feature(name, side) -> boolean`
- `xr.get_float_feature(name, side) -> number`
- `xr.get_vector2_feature(name, side) -> vector`
- `xr.send_haptic_impulse(side, amplitude, duration) -> boolean`

Utility:

- `find_shader([name]) -> reference or nil`
- `create_primitive(kind, [color], [shader_name]) -> reference`
- `set_timescale(value)`
- `get_timescale() -> number`
- `log_info(...)`, `log_warn(...)`, `log_error(...)`

## Common patterns

Safe cached lookups:

```lua
local hands = {}
local function get_hand(side)
  if not is_valid(hands[side]) then
    hands[side] = game_object.Find(side == "left" and "LeftHand Controller" or "RightHand Controller")
  end
  if is_valid(hands[side]) then return hands[side] end
  return nil
end
```

Cleanup that survives script reloads - leave a global function behind and call the old one on the next run:

```lua
local previous_cleanup = my_mod_cleanup

my_mod_cleanup = function()
  if is_valid(menu_root) then destroy(menu_root) end
end

if previous_cleanup then pcall(previous_cleanup) end
```

Fetching and applying remote config once per session:

```lua
on_game_start(function()
  local r = http.get("https://example.com/mod_config.json", nil, 5)
  if not r.ok then return end
  local cfg = json.decode(r.body)
  if cfg then set_timescale(cfg.timescale or 1) end
end)
```

## Troubleshooting

- "member 'X' not found" - wrong name or the member lives on another class in the hierarchy. Inspect with `ref:get_class()`, `ref:class_info()` and `ref:list_members()`.
- "method 'X' expects N arguments, got M" - IL2CPP arities are exact. Check overloads with `ref:list_methods()`.
- "unsupported parameter type" - a hook hit a by-value struct parameter it cannot marshal. Struct parameters work in method calls (boxed or by address) but not in hooks.
- "too many integer/float arguments for a register hook" - the method passes arguments on the stack, hooks only cover register arguments.
- "no overload of 'X' matches that signature" - pass the right selector. The error lists every overload.
- "script timeout" - a chunk or callback ran past its limit. Move long loops into `every()` tasks.
- A destroyed object still has a reference in Lua but `is_valid` returns false and member calls fail.
- Unity calls from the worker thread can crash. Use `set_tick_hook` or `main_thread` for anything that touches Unity objects every frame.
- Re-running code inside one chunk stacks tasks - call `cancel_all()` first and guard `set_tick_hook` with `has_tick_hook()`.
- `find_class` on a missing class does not raise, check `is_valid`.
- An https URL fails on a plain build - rebuild the mod with `BNM_ENABLE_HTTPS`.

## Complete example: Silents Menu (3D VR mod menu)

This is a full, working LuaBNM script with every line explained. It targets a Gorilla Tag style Unity game, but the techniques apply to any Unity IL2CPP game: world-space UI built from primitives, controller input through the xr table, physics changes through Rigidbody, and a main-thread loop through set_tick_hook.

How it works in short:
1. Reload safety: the script saves the previous run's global cleanup function in previous_cleanup and calls it once, so re-executing never leaves duplicate menus or platforms.
2. Main loop: set_tick_hook hooks GorillaLocomotion.Player.FixedUpdate so tasks run on the Unity main thread. cancel_all() then on_update(tick) registers one loop that runs every pump. pcall around tick keeps errors from killing the task.
3. Opening the menu: tick() reads xr.get_bool_feature("secondary_button", side). While it is held the menu exists and follows the hand; when released it is destroyed.
4. Building the menu: the menu is made only from primitive cubes plus a world-space UnityEngine.Canvas for text. Button positions are stored as hit boxes in the menu's local space.
5. Pressing buttons: a tiny sphere (the "finger") is attached to the opposite hand. Each frame its world position is converted into the menu's local space with InverseTransformPoint and compared with every hit box (axis-aligned box test with margins). A cooldown debounces presses. After a press the menu is rebuilt so the new state is drawn.
6. Data-driven entries: the categories table lists buttons. Toggles have on_enable, on_disable and on_tick. run_enabled_entries calls the on_tick of every enabled toggle every frame, even when the menu is closed, so mods keep running.
7. Mods: fly sets Rigidbody.velocity from the hand direction while the trigger is held; car follows the head direction on the horizontal plane; slingshot keeps adding force; long arms scales the player; platforms spawn cubes under the hands while grip is held.
8. Rainbow: hsv_to_color converts a hue to a Color. Objects that should animate are registered with register_rainbow, and update_rainbow rewrites their material color every frame using get_time().

```lua
-- ============================================================================
-- SILENTS MENU - a 3D VR mod menu written in LuaBNM
-- ============================================================================
-- What it does:
--   * Hold the secondary button (B/Y) on the menu hand to show a world-space
--     menu attached to that hand.
--   * Touch a menu button with a small sphere on the OTHER hand to press it.
--   * Buttons are toggles (movement mods) or actions (change category/page).
--   * Background and enabled buttons shimmer through a smooth rainbow.
--
-- How it runs:
--   * A native hook on GorillaLocomotion.Player.FixedUpdate pumps Lua tasks on
--     the Unity main thread. A single on_update task runs tick() every pump.
--   * tick() polls the controller, runs enabled mods, and builds/updates/
--     destroys the menu objects.
-- ============================================================================

-- Global left behind by a previous run of this script (nil on the first run).
-- Calling it destroys the old menu and spawned platforms so reloading the
-- script never leaves stale objects in the scene.
local previous_cleanup = silents_menu_cleanup

-- ---------------------------------------------------------------------------
-- Configuration
-- ---------------------------------------------------------------------------
local menu_title = "SILENTS MENU"
-- Size of the invisible root cube. Children are positioned in this cube's
-- LOCAL space, so every button position below is multiplied by this scale.
local menu_scale = vec3(0.1, 0.3, 0.3825)
-- Size of the visible background cube relative to the root.
local menu_size = vec3(0.1, 1.0, 1.0)
local buttons_per_page = 8
-- Seconds to ignore presses after one registers (debounce).
local press_cooldown = 0.4
-- Extra tolerance added around each button's hit box, per local axis.
local press_margin_x = 0.05
local press_margin_y = 0.0167
local press_margin_z = 0.0131
-- Rainbow: hue cycles per second, and hue offset between neighbouring buttons.
local rainbow_speed = 0.15
local rainbow_spread = 0.12
local dark_button = color(0.03, 0.03, 0.03, 1)
local white_text = color(1, 1, 1, 1)
-- Index 1 = normal text, index 2 = text on enabled toggles.
local text_colors = { white_text, white_text }
-- Shaders tried in order; the first that exists is used so colors show up.
local shader_names = { "Unlit/Color", "Universal Render Pipeline/Lit", "Sprites/Default" }
local identity = quat(0, 0, 0, 1)

-- ---------------------------------------------------------------------------
-- State shared by the whole script
-- ---------------------------------------------------------------------------
local state = {
  menu = nil,            -- root GameObject of the menu (nil when closed)
  canvas_obj = nil,      -- world-space canvas that holds all text labels
  reference = nil,       -- small sphere on the other hand used as the "finger"
  fps_text = nil,        -- Text component of the FPS label
  hit_boxes = {},        -- list of pressable areas, rebuilt with the menu
  rainbow_items = {},    -- objects whose color is animated every frame
  category = "main",     -- which entry in `categories` is shown
  page = 0,              -- zero-based page inside the category
  right_handed = false,  -- menu on right hand instead of left
  disconnect_button = true,
  fps_counter = true,
  last_press = 0,        -- time of the last registered press
  fps_value = 0,
  fps_time = nil,
  fps_frames = 0
}

local visible_shader                       -- cached result of get_visible_shader
local hands = {}                           -- cached controller GameObjects
local player_object                       -- cached player GameObject
local platform_store = { left = nil, right = nil }  -- spawned thin platforms
local thick_store = { left = nil, right = nil }     -- spawned thick platforms
local set_category                         -- forward declarations: the
local recreate_menu                        -- functions are defined later

-- ---------------------------------------------------------------------------
-- Color helpers (rainbow)
-- ---------------------------------------------------------------------------

-- Converts hue/saturation/value (all 0..1) into a UnityEngine.Color.
-- The hue wheel is split into 6 sectors; inside each sector one channel is
-- max, one is min and one fades linearly.
local function hsv_to_color(h, s, v, a)
  h = h % 1
  local sector = math.floor(h * 6)
  local f = h * 6 - sector
  local p = v * (1 - s)
  local q = v * (1 - f * s)
  local t = v * (1 - (1 - f) * s)
  sector = sector % 6
  if sector == 0 then return color(v, t, p, a) end
  if sector == 1 then return color(q, v, p, a) end
  if sector == 2 then return color(p, v, t, a) end
  if sector == 3 then return color(p, q, v, a) end
  if sector == 4 then return color(t, p, v, a) end
  return color(v, p, q, a)
end

-- The current rainbow color. Hue is driven by get_time() so every caller
-- sees the same smooth cycle; `offset` shifts the hue per object.
local function rainbow_color(offset, value, alpha)
  return hsv_to_color(get_time() * rainbow_speed + offset, 1, value, alpha)
end

-- ---------------------------------------------------------------------------
-- Rendering helpers
-- ---------------------------------------------------------------------------

-- Finds a shader that exists in this game (result is cached).
local function get_visible_shader()
  if visible_shader then return visible_shader end
  for _, name in ipairs(shader_names) do
    local found = shader.Find(name)
    if found then
      visible_shader = found
      return found
    end
  end
  return nil
end

-- Gives an object a visible shader and a color. Returns the material so the
-- caller can keep animating it. Returns nil if the object has no renderer.
local function apply_color(obj, col)
  local rend = obj:get_component("UnityEngine.Renderer")
  if not is_valid(rend) then return nil end
  local mat = rend.material
  local found = get_visible_shader()
  if found then mat.shader = found end
  mat.color = col
  return mat
end

-- Colors an object with the rainbow AND remembers it so update_rainbow can
-- keep changing its color. `value` = brightness, `alpha` = opacity.
local function register_rainbow(obj, offset, value, alpha)
  local mat = apply_color(obj, rainbow_color(offset, value, alpha))
  if mat then
    state.rainbow_items[#state.rainbow_items + 1] = { obj = obj, mat = mat, offset = offset, value = value, alpha = alpha }
  end
end

-- Called every frame while the menu is open: rewrites the color of every
-- registered object. Objects that were destroyed are skipped via is_valid.
local function update_rainbow()
  for _, item in ipairs(state.rainbow_items) do
    if is_valid(item.obj) then
      item.mat.color = rainbow_color(item.offset, item.value, item.alpha)
    end
  end
end

-- Destroys a component of the given type on an object, if it has one.
local function remove_component(obj, type_name)
  local comp = obj:get_component(type_name)
  if is_valid(comp) then destroy(comp) end
end

-- ---------------------------------------------------------------------------
-- Font loading
-- ---------------------------------------------------------------------------

-- Tries built-in font files first, then an OS font. Every attempt is wrapped
-- in pcall because missing resources raise errors instead of returning nil.
local function load_font()
  local font_class = find_class("UnityEngine.Font")
  for _, name in ipairs({ "Arial.ttf", "LegacyRuntime.ttf" }) do
    local ok, font = pcall(get_builtin_resource, font_class, name)
    if ok and is_valid(font) then return font end
  end
  local ok, font = pcall(function() return font_class.CreateDynamicFontFromOSFont("Arial", 16) end)
  if ok and is_valid(font) then return font end
  return nil
end

local font
local font_tried = false

-- Loads the font only once, even if it failed (avoids retrying every frame).
local function ensure_font()
  if font_tried then return end
  font_tried = true
  font = load_font()
  print("font_loaded", tostring(font ~= nil))
end

-- ---------------------------------------------------------------------------
-- Rotation math (Lua has no Quaternion.Euler here, so it is built by hand)
-- ---------------------------------------------------------------------------

-- Hamilton product of two quaternions given as {x,y,z,w} tables.
local function quat_multiply(a, b)
  return {
    x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
    y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
    z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
    w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
  }
end

-- Euler angles in degrees -> quaternion, using Unity's Z * X * Y order
-- (rotate around Z first, then X, then Y).
local function euler_to_quat(ex, ey, ez)
  local rx = math.rad(ex) * 0.5
  local ry = math.rad(ey) * 0.5
  local rz = math.rad(ez) * 0.5
  local qx = { x = math.sin(rx), y = 0, z = 0, w = math.cos(rx) }
  local qy = { x = 0, y = math.sin(ry), z = 0, w = math.cos(ry) }
  local qz = { x = 0, y = 0, z = math.sin(rz), w = math.cos(rz) }
  local q = quat_multiply(quat_multiply(qy, qx), qz)
  return quat(q.x, q.y, q.z, q.w)
end

-- Rotation applied to every text label so it faces the player when the menu
-- is held on the hand.
local text_rotation = euler_to_quat(180, 90, 90)

-- ---------------------------------------------------------------------------
-- Scene lookups (cached, always re-validated)
-- ---------------------------------------------------------------------------

-- Returns the "LeftHand Controller" / "RightHand Controller" object.
-- Scene lookups are slow, so the object is cached and only searched again
-- if the cached one was destroyed (for example after a scene change).
local function get_hand(side)
  if not is_valid(hands[side]) then
    hands[side] = game_object.Find(side == "left" and "LeftHand Controller" or "RightHand Controller")
  end
  if is_valid(hands[side]) then return hands[side] end
  return nil
end

local function get_player()
  if not is_valid(player_object) then
    player_object = game_object.Find("GorillaPlayer")
  end
  if is_valid(player_object) then return player_object end
  return nil
end

-- The player's Rigidbody; mods set its velocity to move the player.
local function get_player_body()
  local player = get_player()
  if not player then return nil end
  local body = player:get_component("UnityEngine.Rigidbody")
  if is_valid(body) then return body end
  return nil
end

-- ---------------------------------------------------------------------------
-- Platforms (hold grip to spawn a solid block under that hand)
-- ---------------------------------------------------------------------------

-- Creates a cube with a collider (CreatePrimitive adds one) at a position.
local function spawn_cube(position, scale, rotation, col)
  local obj = game_object.CreatePrimitive(primitive_type.cube)
  apply_color(obj, col)
  obj.transform.localScale = scale
  obj.transform.position = position
  if rotation then obj.transform.rotation = rotation end
  return obj
end

-- Thin platforms fall away: add a Rigidbody (gravity), remove the collider so
-- the player cannot stand on it, and delete it after 2 seconds.
local function release_platform(obj)
  if not is_valid(obj) then return end
  obj:add_component("UnityEngine.Rigidbody")
  remove_component(obj, "UnityEngine.Collider")
  destroy(obj, 2.0)
end

-- Thick platforms simply vanish.
local function remove_platform(obj)
  if is_valid(obj) then destroy(obj) end
end

-- Generic grip logic shared by both platform mods. For each hand:
--   grip held and no platform yet -> build one with `build`
--   grip released and a platform exists -> `release` it and forget it
local function grip_platforms(store, build, release)
  for _, side in ipairs({ "left", "right" }) do
    if xr.get_bool_feature("grip_button", side) then
      if not is_valid(store[side]) then
        local hand = get_hand(side)
        if hand then store[side] = build(side, hand) end
      end
    elseif store[side] then
      release(store[side])
      store[side] = nil
    end
  end
end

-- Releases every platform in a store (used when a mod is switched off).
local function clear_platforms(store, release)
  for _, side in ipairs({ "left", "right" }) do
    if store[side] then
      release(store[side])
      store[side] = nil
    end
  end
end

-- Large flat block slightly below the hand. The right side uses a different
-- size so both hands cover a similar area.
local function build_thick_platform(side, hand)
  local position = hand.transform.position + vec3(0, -0.1, 0)
  local col = rainbow_color(0, 0.8, 1)
  if side == "right" then
    return spawn_cube(position, vec3(0.289351, 0.03870832, 0.4166668), nil, col)
  end
  return spawn_cube(position, vec3(0.4, 0.05, 0.4), nil, col)
end

-- Thin slab that copies the hand's rotation, like a paddle.
local function build_platform(side, hand)
  local position = hand.transform.position + vec3(0, -0.07, 0)
  return spawn_cube(position, vec3(0.025, 0.3, 0.4), hand.transform.rotation, rainbow_color(0, 0.8, 0.95))
end

local function thick_platforms_tick()
  grip_platforms(thick_store, build_thick_platform, remove_platform)
end

local function thick_platforms_stop()
  clear_platforms(thick_store, remove_platform)
end

local function platforms_tick()
  grip_platforms(platform_store, build_platform, release_platform)
end

local function platforms_stop()
  clear_platforms(platform_store, release_platform)
end

-- ---------------------------------------------------------------------------
-- Movement mods (each *_tick runs every frame while its toggle is enabled)
-- ---------------------------------------------------------------------------

-- Fly: while the right trigger is held, velocity points where the hand points.
local function hand_fly_tick()
  if xr.get_float_feature("trigger", "right") <= 0.5 then return end
  local hand = get_hand("right")
  local body = get_player_body()
  if not hand or not body then return end
  body.velocity = hand.transform.forward * 15.0
end

-- Car: like fly but follows the head direction and ignores the vertical part,
-- so you drive along the ground plane.
local function car_tick()
  if xr.get_float_feature("trigger", "right") <= 0.5 then return end
  local camera = game_object.Find("Main Camera")
  local body = get_player_body()
  if not is_valid(camera) or not body then return end
  local forward = camera.transform.forward * 15.0
  body.velocity = vec3(forward.x, 0, forward.z)
end

-- Slingshot: instead of overwriting velocity, keeps adding velocity change in
-- the hand direction, so speed builds up while the trigger is held.
local function slingshot_fly_tick()
  if xr.get_float_feature("trigger", "right") <= 0.5 then return end
  local hand = get_hand("right")
  local body = get_player_body()
  if not hand or not body then return end
  body:AddForce(hand.transform.forward * 1.0, force_mode.velocity_change)
end

-- Long arms: scales the whole player up; the stop function resets it.
local function long_arms_tick()
  local player = get_player()
  if player then player.transform.localScale = vec3(1.5, 1.5, 1.5) end
end

local function long_arms_stop()
  local player = get_player()
  if player then player.transform.localScale = vec3(1, 1, 1) end
end

-- Calls the static PhotonNetwork.Disconnect() method (leaves the lobby).
local function disconnect_photon()
  local photon = find_class("Photon.Pun.PhotonNetwork")
  if photon then photon.Disconnect() end
end

-- Destroys the finger sphere. It is recreated on the next menu open, which is
-- needed after switching hands because it lives on the opposite hand.
local function reset_reference()
  if is_valid(state.reference) then destroy(state.reference) end
  state.reference = nil
end

-- ---------------------------------------------------------------------------
-- Menu content (pure data: the menu builder turns this into 3D buttons)
-- ---------------------------------------------------------------------------
-- Each entry is a table:
--   text        label shown on the button
--   tooltip     text printed to the console when pressed
--   action      function run when pressed (non-toggle buttons)
--   toggle      true if the button switches on/off
--   enabled     initial state of a toggle (nil = off)
--   on_enable / on_disable   run once when the toggle changes
--   on_tick     run every frame while the toggle is on
local categories = {
  main = {
    { text = "Settings", tooltip = "Opens the settings", action = function() set_category("settings") end },
    { text = "Movement", tooltip = "Opens movement", action = function() set_category("movement") end }
  },
  settings = {
    { text = "back", tooltip = "Goes back to main menu", action = function() set_category("main") end },
    {
      text = "Right hand menu",
      tooltip = "Sets the menu to right hand",
      toggle = true,
      on_enable = function() state.right_handed = true reset_reference() end,
      on_disable = function() state.right_handed = false reset_reference() end
    },
    {
      text = "Disconnect button",
      tooltip = "Toggles the disconnect button",
      toggle = true,
      enabled = true,
      on_enable = function() state.disconnect_button = true end,
      on_disable = function() state.disconnect_button = false end
    },
    {
      text = "FPS counter",
      tooltip = "Toggles the fps counter",
      toggle = true,
      enabled = true,
      on_enable = function() state.fps_counter = true end,
      on_disable = function() state.fps_counter = false end
    }
  },
  movement = {
    { text = "back", tooltip = "Goes back to main menu", action = function() set_category("main") end },
    { text = "PlatForms thicker", tooltip = "Head Fly", toggle = true, on_tick = thick_platforms_tick, on_disable = thick_platforms_stop },
    { text = "PlatForms", tooltip = "Plats", toggle = true, on_tick = platforms_tick, on_disable = platforms_stop },
    { text = "Fly", tooltip = "Hand Fly", toggle = true, on_tick = hand_fly_tick },
    { text = "Long Arms", tooltip = "Longer Arms", toggle = true, on_tick = long_arms_tick, on_disable = long_arms_stop },
    { text = "Car", tooltip = "Driving in my car right after a beer.", toggle = true, on_tick = car_tick },
    { text = "SlingshotFly", tooltip = "luanches yourself", toggle = true, on_tick = slingshot_fly_tick }
  }
}

-- Switches category and goes back to its first page.
set_category = function(name)
  state.category = name
  state.page = 0
end

-- ---------------------------------------------------------------------------
-- Paging
-- ---------------------------------------------------------------------------

-- Zero-based index of the last page of the current category.
local function last_page()
  local count = #categories[state.category]
  return math.max(0, math.ceil(count / buttons_per_page) - 1)
end

-- Moves by +1 / -1 and wraps around at both ends.
local function change_page(delta)
  state.page = state.page + delta
  if state.page < 0 then state.page = last_page() end
  if state.page > last_page() then state.page = 0 end
end

-- Runs when a menu entry is pressed. Toggles flip `enabled` and fire the
-- enable/disable callbacks; plain buttons just run `action`. pcall keeps a
-- broken callback from crashing the menu loop.
local function press_entry(entry)
  if entry.toggle then
    entry.enabled = not entry.enabled
    if entry.enabled then
      print("[ENABLE] " .. entry.tooltip)
      if entry.on_enable then pcall(entry.on_enable) end
    else
      print("[DISABLE] " .. entry.tooltip)
      if entry.on_disable then pcall(entry.on_disable) end
    end
  else
    print("[ENABLE] " .. entry.tooltip)
    if entry.action then pcall(entry.action) end
  end
end

-- ---------------------------------------------------------------------------
-- Building the 3D menu
-- ---------------------------------------------------------------------------

-- Creates a UI Text label under `parent_transform` and returns the Text
-- component plus its RectTransform (used to position and size it).
-- alignment 4 = middle-center; style 0 normal, 1 bold, 2 italic.
-- Font size 1 with "best fit" lets Unity scale the text to the rect size,
-- which is required because the canvas is tiny in world space.
local function create_text(parent_transform, content, col, rich, style)
  local text_obj = game_object.new()
  text_obj.transform:SetParent(parent_transform)
  local label = text_obj:add_component("UnityEngine.UI.Text")
  if font then label.font = font end
  label.text = content
  label.fontSize = 1
  label.color = col
  label.supportRichText = rich
  label.fontStyle = style
  label.alignment = 4
  label.resizeTextForBestFit = true
  label.resizeTextMinSize = 0
  local rect = text_obj:get_component("UnityEngine.RectTransform")
  return label, rect
end

-- Creates one pressable button:
--   * a cube child of the menu (the visible button, a trigger collider)
--   * a text label on the canvas
--   * a hit box record used by check_presses
-- position/scale are in the menu's local space; text_position is where the
-- label sits on the canvas. `rainbow_on` = animated color, otherwise dark.
-- `index` shifts the rainbow hue so neighbouring buttons differ.
local function create_button(id, action, label, position, scale, text_position, text_size, rainbow_on, text_col, rich, style, index)
  local cube = game_object.CreatePrimitive(primitive_type.cube)
  remove_component(cube, "UnityEngine.Rigidbody")
  local collider = cube:get_component("UnityEngine.BoxCollider")
  if is_valid(collider) then collider.isTrigger = true end
  cube.transform:SetParent(state.menu.transform)
  cube.transform.localRotation = identity
  cube.transform.localScale = vec3(scale[1], scale[2], scale[3])
  cube.transform.localPosition = vec3(position[1], position[2], position[3])
  if rainbow_on then
    register_rainbow(cube, (index or 0) * rainbow_spread + 0.5, 0.55, 1)
  else
    apply_color(cube, dark_button)
  end

  local _, rect = create_text(state.canvas_obj.transform, label, text_col, rich, style)
  rect.localPosition = vec3(0, 0, 0)
  rect.sizeDelta = vec2(text_size[1], text_size[2])
  rect.localPosition = vec3(text_position[1], text_position[2], text_position[3])
  rect.rotation = text_rotation

  -- Store the button's box in local space for touch detection.
  state.hit_boxes[#state.hit_boxes + 1] = {
    id = id,
    action = action,
    px = position[1], py = position[2], pz = position[3],
    sx = scale[1], sy = scale[2], sz = scale[3]
  }
end

-- Builds the whole menu from scratch: root, background, canvas, title, FPS
-- label, fixed buttons (disconnect, page arrows) and the entries of the
-- current page. It is called again after every press, so the new toggle
-- states and page appear immediately.
local function create_menu()
  ensure_font()
  state.hit_boxes = {}
  state.rainbow_items = {}

  -- Invisible root: no physics, no collider, no renderer. Everything else is
  -- parented to it so moving/rotating the root moves the whole menu.
  local menu = game_object.CreatePrimitive(primitive_type.cube)
  remove_component(menu, "UnityEngine.Rigidbody")
  remove_component(menu, "UnityEngine.BoxCollider")
  remove_component(menu, "UnityEngine.Renderer")
  menu.transform.localScale = menu_scale
  state.menu = menu

  -- Rainbow background panel.
  local background = game_object.CreatePrimitive(primitive_type.cube)
  remove_component(background, "UnityEngine.Rigidbody")
  remove_component(background, "UnityEngine.BoxCollider")
  background.transform:SetParent(menu.transform)
  background.transform.rotation = identity
  background.transform.localScale = menu_size
  background.transform.position = vec3(0.05, 0, 0)
  register_rainbow(background, 0, 0.35, 0.95)

  -- World-space canvas for all text. renderMode 2 = WorldSpace. A high
  -- dynamicPixelsPerUnit keeps the tiny text sharp.
  local canvas_obj = game_object.new()
  canvas_obj.transform:SetParent(menu.transform)
  state.canvas_obj = canvas_obj
  local canvas = canvas_obj:add_component("UnityEngine.Canvas")
  local scaler = canvas_obj:add_component("UnityEngine.UI.CanvasScaler")
  pcall(function() canvas_obj:add_component("UnityEngine.UI.GraphicRaycaster") end)
  canvas.renderMode = 2
  scaler.dynamicPixelsPerUnit = 1000

  -- Title, with the current page number.
  local _, title_rect = create_text(canvas_obj.transform, menu_title .. " [" .. tostring(state.page + 1) .. "]", text_colors[1], true, 1)
  title_rect.localPosition = vec3(0, 0, 0)
  title_rect.sizeDelta = vec2(0.28, 0.05)
  title_rect.position = vec3(0.06, 0, 0.165)
  title_rect.rotation = text_rotation

  -- Optional FPS label (its text is updated by update_fps).
  state.fps_text = nil
  if state.fps_counter then
    local fps_label, fps_rect = create_text(canvas_obj.transform, "FPS: " .. tostring(math.floor(state.fps_value + 0.5)), text_colors[1], true, 2)
    fps_rect.localPosition = vec3(0, 0, 0)
    fps_rect.position = vec3(0.06, 0, 0.135)
    fps_rect.sizeDelta = vec2(0.28, 0.02)
    fps_rect.rotation = text_rotation
    state.fps_text = fps_label
  end

  -- Optional disconnect button at the top.
  if state.disconnect_button then
    create_button("disconnect", disconnect_photon, "Disconnect",
      { 0.56, 0, 0.6 }, { 0.09, 0.9, 0.08 },
      { 0.064, 0, 0.23 }, { 0.2, 0.03 },
      false, text_colors[1], false, 0)
  end

  -- Page arrows on the left and right edges.
  create_button("previous_page", function() change_page(-1) end, "<",
    { 0.56, 0.65, 0 }, { 0.09, 0.2, 0.9 },
    { 0.064, 0.195, 0 }, { 0.2, 0.03 },
    false, text_colors[1], false, 0)

  create_button("next_page", function() change_page(1) end, ">",
    { 0.56, -0.65, 0 }, { 0.09, 0.2, 0.9 },
    { 0.064, -0.195, 0 }, { 0.2, 0.03 },
    false, text_colors[1], false, 0)

  -- Entry buttons for the current page. `first..last` is the slice of the
  -- category list that fits on this page; each row is moved down by 0.1.
  -- Enabled toggles get the animated rainbow, everything else stays dark.
  local list = categories[state.category]
  local first = state.page * buttons_per_page + 1
  local last = math.min(#list, first + buttons_per_page - 1)
  for i = first, last do
    local entry = list[i]
    local offset = (i - first) * 0.1
    local on = entry.toggle and entry.enabled
    create_button("entry_" .. i, function() press_entry(entry) end, entry.text,
      { 0.56, 0, 0.28 - offset }, { 0.09, 0.9, 0.08 },
      { 0.064, 0, 0.111 - offset / 2.6 }, { 0.2, 0.03 },
      on and true or false,
      text_colors[2],
      true, 2, i - first)
  end
end

-- Sticks the menu to the menu hand every frame. The right-hand version is
-- flipped 180 degrees around Z so the text still reads correctly.
local function recenter_menu()
  local side = state.right_handed and "right" or "left"
  local hand = get_hand(side)
  if not hand or not is_valid(state.menu) then return end
  state.menu.transform.position = hand.transform.position
  if state.right_handed then
    local angles = hand.transform.eulerAngles
    state.menu.transform.rotation = euler_to_quat(angles.x, angles.y, angles.z + 180)
  else
    state.menu.transform.rotation = hand.transform.rotation
  end
end

-- Creates the "finger": a tiny white sphere parented to the OPPOSITE hand,
-- slightly below it. Its position is what check_presses tests against.
local function create_reference()
  local other = get_hand(state.right_handed and "left" or "right")
  if not other then return end
  local ref = game_object.CreatePrimitive(primitive_type.sphere)
  ref.transform:SetParent(other.transform)
  apply_color(ref, white_text)
  ref.transform.localPosition = vec3(0, -0.1, 0)
  ref.transform.localScale = vec3(0.01, 0.01, 0.01)
  remove_component(ref, "UnityEngine.Collider")
  state.reference = ref
end

-- Destroys and rebuilds the menu in place (after a press).
recreate_menu = function()
  if is_valid(state.menu) then destroy(state.menu) end
  create_menu()
  recenter_menu()
end

-- Removes every menu object and clears references (menu closed).
local function destroy_menu()
  if is_valid(state.menu) then destroy(state.menu) end
  reset_reference()
  state.menu = nil
  state.canvas_obj = nil
  state.fps_text = nil
  state.hit_boxes = {}
  state.rainbow_items = {}
end

-- ---------------------------------------------------------------------------
-- Touch detection
-- ---------------------------------------------------------------------------

-- Converts the finger sphere's world position into the menu's local space
-- (InverseTransformPoint), then tests it against each button's box. Because
-- both are in local space, rotation and position of the hand do not matter.
-- A hit needs |p - center| <= half size + margin on all three axes. The first
-- matching button runs its action and the menu is rebuilt; the cooldown stops
-- one touch from firing many times.
local function check_presses()
  if not is_valid(state.menu) or not is_valid(state.reference) then return end
  local now = get_time()
  if now < state.last_press + press_cooldown then return end
  local p = state.menu.transform:InverseTransformPoint(state.reference.transform.position)
  for _, box in ipairs(state.hit_boxes) do
    if math.abs(p.x - box.px) <= box.sx * 0.5 + press_margin_x
      and math.abs(p.y - box.py) <= box.sy * 0.5 + press_margin_y
      and math.abs(p.z - box.pz) <= box.sz * 0.5 + press_margin_z then
      state.last_press = now
      pcall(box.action)
      recreate_menu()
      return
    end
  end
end

-- ---------------------------------------------------------------------------
-- FPS counter
-- ---------------------------------------------------------------------------

-- Counts frames over a 0.5 second window: fps = frames passed / seconds
-- passed (time.frameCount is Unity's global frame counter). The label is
-- refreshed twice per second.
local function update_fps()
  local now = get_time()
  local frames = time.frameCount
  if not state.fps_time then
    state.fps_time = now
    state.fps_frames = frames
    return
  end
  if now - state.fps_time < 0.5 then return end
  state.fps_value = (frames - state.fps_frames) / (now - state.fps_time)
  state.fps_time = now
  state.fps_frames = frames
  if state.fps_counter and is_valid(state.fps_text) then
    state.fps_text.text = "FPS: " .. tostring(math.floor(state.fps_value + 0.5))
  end
end

-- ---------------------------------------------------------------------------
-- Main loop
-- ---------------------------------------------------------------------------

-- Runs the on_tick function of every enabled toggle, in every category, even
-- while the menu is closed (mods keep working after you let go of the button).
local function run_enabled_entries()
  for _, list in pairs(categories) do
    for _, entry in ipairs(list) do
      if entry.enabled and entry.on_tick then pcall(entry.on_tick) end
    end
  end
end

-- One frame of logic:
--   1. On the first frame after a reload, run the previous script's cleanup.
--   2. Read the open button (secondary button on the menu hand).
--   3. Run active mods.
--   4. If the menu is closed: open it when the button is held.
--      If it is open: while held, follow the hand, animate colors, check
--      presses and update FPS; when released, destroy it.
local function tick()
  if previous_cleanup then
    pcall(previous_cleanup)
    previous_cleanup = nil
  end
  local open = xr.get_bool_feature("secondary_button", state.right_handed and "right" or "left")
  run_enabled_entries()
  if not is_valid(state.menu) then
    if open then
      create_menu()
      recenter_menu()
      if not is_valid(state.reference) then create_reference() end
    end
    return
  end
  if open then
    recenter_menu()
    update_rainbow()
    check_presses()
    update_fps()
  else
    destroy_menu()
  end
end

-- Exposed as a global so the NEXT run of this script can call it (see
-- previous_cleanup at the top). Removes the menu, platforms and resets scale.
silents_menu_cleanup = function()
  destroy_menu()
  clear_platforms(platform_store, remove_platform)
  clear_platforms(thick_store, remove_platform)
  long_arms_stop()
end

-- Run Lua tasks from the Unity main thread by hooking FixedUpdate. Only one
-- tick hook can exist, so install it only if none is active yet.
if not has_tick_hook() then
  local ok, err = pcall(set_tick_hook, "GorillaLocomotion.Player", "FixedUpdate", "Assembly-CSharp.dll")
  if not ok then print("hook", err) end
end

-- Remove tasks from earlier runs, then register this script's loop. pcall
-- keeps one error from killing the task (an uncaught error would cancel it).
cancel_all()
on_update(function()
  local ok, err = pcall(tick)
  if not ok then print("tick_error", err) end
end)
```

## Links

- Docs site: https://silenterased.github.io/BNMExplorer/
- This file on GitHub Pages: https://silenterased.github.io/BNMExplorer/llms.txt
- The mod serves the same file at http://DEVICE_IP:8080/llms.txt
- Source repository: https://github.com/SilentErased/BNMExplorer)TXT";

const char* get_llms_txt() {
    return k_llms_txt;
}
