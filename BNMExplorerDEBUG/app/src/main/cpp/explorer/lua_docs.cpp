#include "lua_docs.hpp"

const char k_llms_txt[] = R"TXT(# BNMExplorer and LuaBNM

> BNMExplorer runs a web based inspector inside an IL2CPP Unity game on Android or VR. LuaBNM is its embedded Lua 5.1 runtime: scripts can call any IL2CPP class, field, property and method by name, hook native methods with Lua callbacks, read XR controller input, send HTTP and HTTPS requests, parse JSON, read and write workspace files and raw memory. Docs site: https://silenterased.github.io/BNMExplorer/ - the mod itself also serves this exact file at http://DEVICE_IP:8080/llms.txt

This file documents everything: the web console, the startup manager, every LuaBNM function, the limits, the errors, a small example UI script at the end and a note for AI assistants on how to treat that example. Everything here is written against the actual C++ source, so names and behavior match what runs inside the game.

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
- Preview - clicking a file in the tree opens it by type: .lua and text files (.txt, .md, .json, .csv, .xml, .yaml, configs) open in the editor, images (.png, .jpg, .jpeg, .gif, .webp, .bmp, .svg) show a picture preview, audio (.wav, .ogg, .mp3, .m4a, .aac, .flac) opens a player that plays the file straight from the workspace. Anything else downloads.
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
- Errors come back as "SyntaxError: ..." (compile stage) or "RuntimeError: ..." (runtime). Runtime errors carry the source line and a stack traceback, for example:
```
RuntimeError: chunk.lua:12: attempt to index a nil value
stack traceback:
    chunk.lua:12: in function 'apply'
    chunk.lua:20: in main chunk
```
- An error inside a scheduled task prints `TaskError (task N): ...` with the same traceback and kills that task. An error inside a `main_thread` callback prints `MainThreadError: ...`. An error inside an async http callback prints `AsyncCallbackError: ...`. A hook callback error prints "[Hook N Class::Method] ..." and the original method runs.
- With `dev.debug_pcall(true)` every `pcall` that catches an error logs `PcallError: ...` to the Output panel and logcat too, so "silent" failures become visible. The returned values of pcall do not change, except that the error string gets the traceback appended.
- Every line printed to the Output panel also lands in a log file on the device: `<BNMExplorer folder>/log.txt` (usually `/sdcard/BNMExplorer/log.txt`). The file rotates at 2 MB. Read it with `dev.log_path()`, wipe it with `dev.log_clear()`. This survives a game crash, unlike the in-memory Output panel.
- Output from tasks and hooks goes to an async buffer capped at 65536 characters, oldest lines drop first.
- Running a new file stops all tasks from the previous run, disables hook callbacks (the native trampolines stay installed but forward to the original methods) and clears the module cache.

Threads, in plain words:

- A chunk started with Run executes on the Unity main thread when the game is already rendering (the Main thread toggle in the console controls this, on by default). That makes Unity calls from top level script code safe. During the startup Manager freeze the chunk runs inline on the frozen main thread, which is the same thread.
- Scheduler callbacks (`every`, `after`, `on_update`, `on_game_start`) run on a background worker thread by default. Many Unity calls survive that, some crash. Turn on `dev.main_tasks(true)` or install `set_tick_hook` to move task callbacks onto the main thread. With neither, `main_thread(fn)` callbacks also fall back to the worker after 3 seconds and a warning prints once.

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
- `dev.main_tasks(true)` moves all scheduler callbacks onto the Unity main thread once frame hooks exist (the ones the UI installs or your own `set_tick_hook`). Without frame hooks the flag has no effect and tasks keep running on the worker.
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

Performs a blocking request. Call it from the console, from `main_thread` or from tasks - it blocks the calling thread for up to the timeout, and on the main thread that means the game freezes for the download. Options table:

- `url` - required, "http://" or "https://"
- `method` - "GET" (default), "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS"
- `headers` - table of strings
- `body` - request body string
- `content_type` - defaults to "application/octet-stream", a default User-Agent "LuaBNM" is always sent
- `timeout` - seconds, default 10
- `follow_redirects` - default true

Successful response: `{ok = true, status, body, headers}` - body is the raw response string, headers maps lowercased names as sent by the server. Failed connection: `{ok = false, error, error_code}`. Exactly this table, same keys both ways:

```
http.get("http://10.0.0.5:9000/ping") ->
  { ok = true,  status = 200, body = "pong", headers = { ["content-type"] = "text/plain" } }
http.get("http://10.0.0.99:9000/ping") ->
  { ok = false, error = "Connection refused", error_code = 5 }
```

HTTPS is compiled in by default (the CMake option `BNM_DEFAULT_HTTPS` links OpenSSL). On a build without it an https URL raises a clear error.

Certificate verification:

- The statically linked OpenSSL does not see the Android system CA store, so by default every https request fails with `SSL server verification failed` unless a CA file is set or verification is turned off.
- `http.set_ssl_verify([enabled]) -> boolean` - get or set the global default. `http.set_ssl_verify(false)` makes every later request skip certificate checks. A request can override it per call with the `verify` option field.
- `http.set_ca_file([path]) -> boolean` - point verification at a PEM bundle. Relative paths resolve inside the workspace, so you can upload a `cacert.pem` (for example the Mozilla bundle from curl.se) into the workspace and call `http.set_ca_file("cacert.pem")` once per session - then `raw.githubusercontent.com` and most other sites verify properly. Call it with no argument to clear.
- Per request: `{url = "...", verify = false}` or `{url = "...", ca_file = "cacert.pem"}`.
- For git hosts the whole issue is one pinned certificate chain; `set_ca_file` is the clean fix, `set_ssl_verify(false)` is the quick one.

```lua
http.set_ssl_verify(false)
local r = http.get("https://raw.githubusercontent.com/some/repo/main/sounds.txt")
```

```lua
on_game_start(function()
  if fs.exists("cacert.pem") then
    http.set_ca_file("cacert.pem")
    print("https verification on with local CA bundle")
  else
    http.set_ssl_verify(false)
  end
end)
```

Shortcuts:

- `http.get(url, [headers_table], [timeout_seconds]) -> response`
- `http.post(url, [body], [headers_table], [content_type]) -> response` - body as a string defaults the content type to "application/json"

Async versions never block and never touch the game frame:

- `http.get_async(url, callback)`
- `http.post_async(url, body, callback)`
- `http.request_async(options, callback)`
- `http.set_ssl_verify([enabled]) -> boolean`
- `http.set_ca_file([path]) -> boolean`

They return `true` right away and download on a private thread. When the transfer finishes, `callback(response)` runs with the same response table, from the scheduler pump. An error inside the callback prints `AsyncCallbackError: ...` with a traceback and the logcat. Errors that happen before anything is sent (bad url, https without OpenSSL) raise immediately instead.

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

http.get_async("http://example.com/big.json", function(r3)
  if not r3.ok then
    print("download failed", r3.error)
    return
  end
  print("got", #r3.body, "bytes")
end)
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
- `memory.write_floats(addr, values) -> number` - bulk float write. `values` is a table of numbers or a binary string whose length is a multiple of 4. Returns how many floats were written. One memcpy, so a table of 40000 floats takes microseconds instead of 40000 separate calls.
- `memory.read_floats(addr, count) -> table or nil` - bulk float read, returns `{v1, v2, ...}`, count up to 1000000.
- `memory.nop(addr, length) -> boolean` - fills whole instructions with ARM64 NOPs (1..1024 bytes), makes the page writable with mprotect and clears the instruction cache

Integer reads are signed, 64 bit values pass through doubles so precision is limited to 2^53 like everywhere else in Lua.

```lua
local hp = memory.read32(0x7ABC1230)
memory.write_float(0x7ABC1234, 99.5)
memory.write_floats(0x7ABC1234, {0.5, 1, -1, 0.25})
memory.write_floats(0x7ABC1234, array_to_string(some_float_array))
local samples = memory.read_floats(arr.addr + 0x20, 48000)
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
- `ref:list_properties([filter, inherited]) -> table` - each hit also has `getter` and `setter` booleans
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

Property hits carry the accessor flags, which matters on stripped builds where only a setter survives:

```
find_class("UnityEngine.AudioSource"):list_properties()[1] ->
  { kind = "property", name = "volume", type = "float",
    params = "get;set", getter = true, setter = true, static = false, ... }
find_class("UnityEngine.AudioSource"):list_properties("spatialBlend")[1] ->
  { name = "spatialBlend", getter = false, setter = true, ... }
```

Reading and writing members falls back automatically: if a property has no getter but a `get_name` method exists, the read goes through it; same for a missing setter and `set_name`. A property that only has a setter returns an error when you read it (`property 'X' has no getter`) - that means neither a getter nor a `get_X` method survived stripping.

## Arrays and lists

- `to_table(object) -> table or nil` - converts an IL2CPP array (`T[]`) or `List<T>` into a 1-based Lua table. Elements convert by their element class: numbers, bools, strings, references, structs get boxed.
- `make_array(element_class, lua_table) -> reference` - creates a real IL2CPP array. Every element type writes its exact width: `System.Single` and `System.Double` keep their fractional part (older builds truncated floats to whole numbers and every value under 1 came back as 0 - fixed), integers truncate toward zero, strings make managed strings, references copy the pointer, structs copy from a boxed struct or raw address, nil zeros the slot. Pass the result anywhere an array parameter is expected or return it from a hook.
- `make_byte_array(data) -> reference` - a `byte[]` from a Lua string (binary safe) or a table of numbers.
- `make_float_array(values) -> reference` - a `float[]` from a table of numbers or from a binary string of little-endian 4 byte floats (for example an `audio.decode` data string).
- `array_set(arr, table, [offset]) -> number` - writes the table into an existing IL2CPP array starting at `offset` (default 0). Same element rules as `make_array`. Returns the element count written. Errors when the table does not fit.
- `array_set_floats(arr, values, [offset]) -> number` - writes floats into a `float[]` or `double[]` fast. `values` is a table of numbers or a binary string (multiple of 4 bytes). Returns the count written.
- `array_to_string(arr) -> string` - raw little-endian bytes of a numeric or bool array as a Lua string. Reference arrays error. Handy to stash an array in a file, in `fs.write`, or to pipe it into `memory.write_floats` and back.

```lua
local bodies = to_table(find_objects_of_type("UnityEngine.Rigidbody"))
for i, body in ipairs(bodies) do print(i, body.name) end

local names = make_array("System.String", {"a", "b", "c"})
some_ref:SetNames(names)

local arr = make_array("System.Single", {0.5, 1, -1, 0.25})
print(table.concat(to_table(arr), " "))    -- 0.5 1 -1 0.25

local big = {}
for i = 1, 32000 do big[i] = math.sin(i / 10) * 0.9 end
local fa = make_float_array(big)
local peak = 0
for _, v in ipairs(to_table(fa)) do
  local a = math.abs(v)
  if a > peak then peak = a end
end
print(#to_table(fa), peak)                 -- 32000   0.9 (a fraction off)

array_set_floats(fa, big)                  -- refill without allocating
local bin = array_to_string(fa)            -- 128000 raw bytes
fs.write("sine.bin", bin)
local copy = make_float_array(fs.read("sine.bin"))
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

## Audio

Native wav, ogg and mp3 decoding in the mod, so no UnityWebRequestMultimedia, no Lua side parsing and no stripped build problems. Decoders: dr_wav, stb_vorbis, dr_mp3. `audio.formats()` returns `{"wav", "ogg", "mp3"}`.

Every function takes either a workspace path (`fs.read` is used under the hood) or raw bytes of the whole file as a Lua string. Workspace files cap at 1 MB, inline bytes cap at 8 MB. Decoded output caps at 32 million samples (about 5 minutes of stereo 48 kHz) and errors above that.

- `audio.decode(path_or_bytes) -> table or nil, error` - decodes and returns `{data, rate, channels, frames}`. `data` is a binary Lua string of interleaved little-endian float32 samples (frames times channels of them), not a Lua table - a 3 minute song would be 16 million Lua numbers otherwise.
- `audio.load(path_or_bytes, [name]) -> clip or nil, error` - decodes, creates a real `UnityEngine.AudioClip` with `AudioClip.Create(name, frames, channels, rate, false)` and fills it with `SetData`. Returns the clip reference.
- `audio.play(clip, [options]) -> handle or nil, error` - creates a GameObject `LuaBNM_Audio` with an AudioSource, wires the clip and calls `Play`. Options: `volume` (default 1), `loop` (default false), `spatial_blend` (default 0, so 2D), `parent` (a GameObject reference to parent the source under). The handle is a plain table `{clip = ..., src = ..., go = ...}` - fields are references, keep it or rebuild it, both work.
- `audio.stop(handle) -> boolean, error` - stops the source and destroys the GameObject.
- `audio.route_to_mic(clip, [loop]) -> boolean, error` - Gorilla Tag style voice chat: finds `GorillaTagger.Instance.myRecorder`, sets `SourceType = 1`, `AudioClip = clip`, `LoopAudioClip = loop` and calls `RestartRecording(true)`. Your clip then plays into the microphone others hear. Field names are looked up dynamically, a game that renamed them errors with a clear message instead of crashing.
- `audio.restore_mic() -> boolean, error` - puts the recorder back: SourceType 0, no clip, no loop.

Unity requires clip and source work on the main thread. `audio.load`, `audio.play`, `audio.stop`, `audio.route_to_mic` and `audio.restore_mic` detect when they are off it and post the work to the main thread, waiting up to 10 seconds. Calling them from a plain task works while frame hooks exist (UI browsing or `set_tick_hook`). Calling them during the startup freeze errors with a timeout message because no frame pumps yet - run them after `on_game_start` or from the console.

Failures read like this:

```
audio.load("sounds/broken.wav") -> nil, "wav decode failed"
audio.route_to_mic(clip)         -> nil, "GorillaTagger not found in this game"   (non Gorilla Tag game)
```

```lua
on_game_start(function()
  local clip, err = audio.load("Custom_Sounds/test.wav")
  if not clip then
    print("load failed", err)
    return
  end
  local handle = audio.play(clip, { volume = 1, spatial_blend = 0 })
  print("playing", is_valid(handle.src), handle.src.clip == clip)

  after(5, function() audio.stop(handle) end)
end)
```

A soundboard that plays into voice chat:

```lua
local current = nil

function play_to_mic(path, loop)
  if current then audio.stop(current) end
  local clip, err = audio.load(path)
  if not clip then return nil, err end
  local ok, rerr = audio.route_to_mic(clip, loop)
  if not ok then return nil, rerr end
  current = { clip = clip }
  return current
end

function stop_mic()
  audio.restore_mic()
  current = nil
end
```

## Binary data: string.pack and string.unpack

Lua 5.1 has no packing, so LuaBNM adds it. The same functions exist as `pack.pack`, `pack.unpack` and as `string.pack`, `string.unpack`. Format characters: `<` little endian (default), `>` big endian, `=` little endian, `x` one zero pad byte, `b`/`B` signed/unsigned byte, `h`/`H` 2 bytes, `i`/`I` 4 bytes or `i3`/`I5` style 1..8 bytes, `l`/`L` 8 bytes, `f` 4 byte float, `d` 8 byte double, `s` a 4 byte length followed by the string bytes.

- `string.pack(fmt, ...) -> string` - packs values left to right. Extra values error, missing values error.
- `string.unpack(fmt, data, [init]) -> value..., next_position` - reads values left to right, returns everything plus the 1-based position after the last consumed byte. A short string errors with "string too short".

```lua
local header = string.pack("<I4fH", 123, 60.0, 2)
-- 4 bytes int, 4 bytes float, 2 bytes short -> 10 bytes
local a, b, c, next = string.unpack("<I4fH", header)
print(a, b, c, next)                        -- 123   60   2   11

local packet = string.pack("<Ih", #name) .. name
local len, pos = string.unpack("<I", packet)
local name2 = packet:sub(pos, pos + len - 1)

local floats = string.pack("<fff", 0.5, -1, 2.25)
local arr = make_float_array(floats)        -- straight into an IL2CPP float[]
```

## dev - debugging tools

- `dev.info() -> table` - `{tasks, main_tasks, debug_pcall, guard_calls, tick_hook, main_pumping, worker, on_main_thread}`. `on_main_thread` is the cheapest way to check which thread your code is on right now.
- `dev.debug_pcall([on]) -> boolean` - get or set the pcall logging flag from "Lua runtime facts".
- `dev.main_tasks([on]) -> boolean` - get or set the run-tasks-on-main-thread flag.
- `dev.guard_calls([on]) -> boolean` - get or set the crash guard for IL2CPP method calls. When on, a method call that segfaults inside IL2CPP surfaces as the Lua error `native crash (signal) inside 'MethodName'; the call was aborted` instead of killing the game. Off by default; it wraps every call in a signal checkpoint, so leave it off in hot paths.
- `dev.guard(fn, ...) -> ok, result_or_error` - runs `fn` in a crash sandbox one level deep: a native segfault inside becomes `ok = false` with a text error instead of a dead game, and normal returns come back as `ok = true` plus the results. A signal handler is installed process wide the first time any guard runs.
```lua
local ok, result = dev.guard(function() return risky_ref:SomeNativeCall() end)
if not ok then print("survived", result) end
```
- `dev.self_test() -> table` - array and float integrity checks running against the real IL2CPP heap: `{small, small_ok, big_count, big_peak, float_string, float_string_ok, all_ok}`. `small` is the read back `{0.5, 1, -1, 0.25}`; `big_count` is 32000 and `big_peak` should be near 0.9. Run it once after a new game build if arrays behave strange.
- `dev.log_path() -> string` - the log file path, empty when the device allows no writes.
- `dev.log_clear()` - deletes the log file.

```lua
on_game_start(function()
  local t = dev.self_test()
  print("selftest", t.all_ok, t.small_ok, t.big_count, t.big_peak)
end)
```

## Introspection and stripped builds

Games strip assemblies. In one Gorilla Tag style build `UnityEngine.Networking.UnityWebRequestMultimedia` simply does not exist, and only `UnityWebRequest`, `UnityWebRequestAsyncOperation`, `UnityWebRequestMethod` and `UnityWebRequestError` survived from that namespace. `find_class` on a missing class returns an invalid reference instead of raising, so scripts that assume a class exists hang silently. Two tools fix that:

- `has_class(full_name, [assembly_name]) -> boolean` - true when the class exists right now.
- `list_assemblies() -> table` and `list_classes([path, limit]) -> table` - list what did survive.

```lua
if not has_class("UnityEngine.AudioClip") then
  error("this build has no AudioClip, audio is impossible here")
end

if not has_class("UnityEngine.Networking.UnityWebRequestMultimedia") then
  print("no multimedia downloader, using audio.* instead")
end
```

Stripping also eats property accessors. Typical damage seen in the wild: `spatialBlend` and `outputAudioMixerGroup` on AudioSource ship with only a setter, `AudioListener.volume`, `AudioSource.ignoreListenerVolume` and `ignoreListenerPause` are gone entirely, while `AudioListener.pause` reads fine. The member lookup falls back to `get_X` / `set_X` methods automatically, and `list_properties` shows `getter` / `setter` booleans so a script can check before it reads. Writing a getter-only property still errors, and reading a setter-only property errors with `property 'X' has no getter`.

Facts from one real Gorilla Tag style build, useful as expectations: output sample rate 48000, speaker mode 2, about 44 AudioSources in the scene with 15 playing, two AudioListeners (the one on `Main Camera` disabled, a separate `AudioListener` object enabled), `GorillaLocomotion.Player` exists while `GTPlayer` does not, and the enabled listener object is named exactly `AudioListener`.

## Utility functions

- `hex(number) -> string` - formats as "0x..."
- `parse_hex(text) -> number` - parses "0x1A2B" or "1A2B"
- `set_timescale(value)` - writes `Time.timeScale`
- `get_timescale() -> number`
- `log_info(...)`, `log_warn(...)`, `log_error(...)` - like print but also writes to the Android logcat with a matching priority
- `il2cpp_loaded() -> boolean` - true once BNM finished loading IL2CPP (always true inside a working script)
- `has_class(full_name, [assembly_name]) -> boolean` - does this IL2CPP class exist in this build (see "Introspection and stripped builds")
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
- `has_class(full_name, [assembly_name]) -> boolean`
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
- `http.get_async(url, callback)`
- `http.post_async(url, body, callback)`
- `http.request_async(options, callback)`
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
- `string.pack(fmt, ...) -> string`, `string.unpack(fmt, data, [init]) -> values..., pos`
- `pack.pack(fmt, ...) -> string`, `pack.unpack(fmt, data, [init]) -> values..., pos`

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
- `memory.write_floats(addr, values) -> number`
- `memory.read_floats(addr, count) -> table or nil`
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
- `make_float_array(table_or_string) -> reference`
- `array_set(arr, table, [offset]) -> number`
- `array_set_floats(arr, values, [offset]) -> number`
- `array_to_string(arr) -> string`

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

Audio:

- `audio.decode(path_or_bytes) -> table or nil, error`
- `audio.load(path_or_bytes, [name]) -> clip or nil, error`
- `audio.play(clip, [options]) -> handle or nil, error`
- `audio.stop(handle) -> boolean, error`
- `audio.route_to_mic(clip, [loop]) -> boolean, error`
- `audio.restore_mic() -> boolean, error`
- `audio.formats() -> table`

dev:

- `dev.info() -> table`
- `dev.debug_pcall([on]) -> boolean`
- `dev.main_tasks([on]) -> boolean`
- `dev.guard_calls([on]) -> boolean`
- `dev.guard(fn, ...) -> ok, result_or_error`
- `dev.self_test() -> table`
- `dev.log_path() -> string`
- `dev.log_clear()`

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

Attaching UI to a hand without knowing the exact class layout - try the real class first, then fall back to object names, and cache both:

```lua
local hand_cache = {}

local function hand_transform(side)
  local cached = hand_cache[side]
  if is_valid(cached) then return cached end
  local player = find_class("GorillaLocomotion.Player").Instance
  if is_valid(player) then
    local field = side == "right" and "rightHandTransform" or "leftHandTransform"
    if player[field] then
      hand_cache[side] = player[field]
      return hand_cache[side]
    end
  end
  local obj = game_object.Find(side == "right" and "RightHand Controller" or "LeftHand Controller")
  if is_valid(obj) then
    hand_cache[side] = obj.transform
    return obj.transform
  end
  return nil
end
```

Streaming a long sound when a single huge `audio.load` would take too long - decode once, then feed `SetData` in 4096 frame slices; one small float array per slice, no Lua table ever holds the samples:

```lua
on_game_start(function()
  local info = audio.decode("Custom_Sounds/long.wav")
  if not info then return end
  local clip = find_class("UnityEngine.AudioClip"):Create("stream", info.frames, info.channels, info.rate, false)
  local pos = 0
  every(0, function()
    local total = info.frames * info.channels
    local left = total - pos
    if left <= 0 then return end
    local n = math.min(4096 * info.channels, left)
    local part = make_float_array(info.data:sub(pos * 4 + 1, (pos + n) * 4))
    clip:SetData(part, pos / info.channels)
    pos = pos + n
  end)
end)
```

Surviving a crashy IL2CPP call while testing an unknown game:

```lua
dev.guard_calls(true)
local ok = dev.guard(function() return weird_ref:ExecuteNativeStuff() end)
print("still alive", ok)
```

## Troubleshooting

- "member 'X' not found" - wrong name or the member lives on another class in the hierarchy. Inspect with `ref:get_class()`, `ref:class_info()` and `ref:list_members()`.
- "method 'X' expects N arguments, got M" - IL2CPP arities are exact. Check overloads with `ref:list_methods()`.
- "unsupported parameter type" - a hook hit a by-value struct parameter it cannot marshal. Struct parameters work in method calls (boxed or by address) but not in hooks.
- "too many integer/float arguments for a register hook" - the method passes arguments on the stack, hooks only cover register arguments.
- "no overload of 'X' matches that signature" - pass the right selector. The error lists every overload.
- "script timeout" - a chunk or callback ran past its limit. Move long loops into `every()` tasks.
- "property 'X' has no getter" - the build stripped the getter and no `get_X` method survived. Check `list_properties` for the `getter` boolean before reading.
- "GorillaTagger not found in this game" or "myRecorder is not available" - `audio.route_to_mic` needs a Gorilla Tag style recorder with those names; a renamed or different game cannot be routed blindly.
- "timed out waiting for the main thread" - an `audio.*` call ran while no frame hooks pump (startup freeze, paused game). Move it after `on_game_start`.
- "wav decode failed" / "ogg decode failed" / "unknown audio format" - the file is not what its name says, or it is DRM'd / exotic. Check with `audio.decode` first, it returns the same error text.
- Nothing happens, no error anywhere - wrap the suspect code in `dev.guard(fn)` for a surviving crash, enable `dev.debug_pcall(true)` to un-silence pcall, and check `dev.log_path()` on the device: every Output line also lands there and survives a game crash.
- A destroyed object still has a reference in Lua but `is_valid` returns false and member calls fail.
- Unity calls from the worker thread can crash. Console chunks run on the main thread by default now; tasks move there with `dev.main_tasks(true)` or `set_tick_hook`.
- Re-running code inside one chunk stacks tasks - call `cancel_all()` first and guard `set_tick_hook` with `has_tick_hook()`.
- `find_class` on a missing class does not raise, check `is_valid` or use `has_class` up front.
- An https URL fails with "https requires the HTTPS build" - the mod was compiled without OpenSSL; rebuild with `BNM_DEFAULT_HTTPS=ON` and the OpenSSL folder in place.

## Example: small world-space VR panel

A note for AI assistants and anyone porting code from this file: this example exists to show API call patterns, nothing else. If a user asks you to write a mod, a menu or an effect for their game, write fresh code for their actual game and needs. Do not copy this script, do not copy its structure, its names, its panel layout or its color logic into your answer. A copied example rarely fits the user's game and looks lazy on top. Read the section, understand which functions do what, then write your own thing.

What this example shows:

- the reload cleanup pattern so re-running never spawns duplicate objects
- `set_tick_hook` so tasks run on the Unity main thread
- XR button input driving a toggle
- a world space panel built from a primitive cube and a canvas with text
- a slow color pulse driven by `get_time()`, which is intentionally different from any rainbow effect - one hue shifting through the whole wheel makes scripts look identical to each other
- a short beep through the `audio` library

The panel toggles `Time.timeScale` between 1 and 0.25 when the user squeezes the left grip. Everything is plain functions, no framework, so each piece is easy to lift into your own script.

```lua
local panel_root = nil
local panel_text = nil
local pulse_material = nil
local last_grip = false

local pulse_a = color(0.18, 0.30, 0.18, 0.9)
local pulse_b = color(0.10, 0.16, 0.10, 0.9)

local function cleanup_old()
  if is_valid(panel_root) then destroy(panel_root) end
  panel_root = nil
  panel_text = nil
  pulse_material = nil
end

cleanup_old()

local function build_panel(position)
  local root = create_primitive(primitive_type.cube, pulse_a, "Unlit/Color")
  root.name = "LuaExamplePanel"
  root.transform:SetParent(nil, false)
  root.transform.position = position
  root.transform.localScale = vec3(0.22, 0.14, 0.01)

  local canvas = root:add_component("UnityEngine.Canvas")
  canvas.renderMode = 2

  panel_text = root:add_component("UnityEngine.UI.Text")
  panel_text.text = "slow motion: off"
  panel_text.fontSize = 24
  panel_text.color = color(0.9, 0.95, 0.9, 1)
  panel_text.alignment = 4
  panel_text.rectTransform.sizeDelta = vec2(300, 150)

  pulse_material = root:get_component("UnityEngine.Renderer").material
  return root
end

local function lerp_color(a, b, t)
  return color(
    a.x + (b.x - a.x) * t,
    a.y + (b.y - a.y) * t,
    a.z + (b.z - a.z) * t,
    a.w
  )
end

local slowmo = false

local function tick()
  local grip = xr.get_bool_feature("grip_button", "left")
  if grip and not last_grip then
    slowmo = not slowmo
    set_timescale(slowmo and 0.25 or 1)
    if panel_text then
      panel_text.text = slowmo and "slow motion: on" or "slow motion: off"
    end
  end
  last_grip = grip

  if is_valid(pulse_material) then
    local t = 0.5 + 0.5 * math.sin(get_time() * 1.5)
    pulse_material.color = lerp_color(pulse_a, pulse_b, t)
  end
end

on_game_start(function()
  if not has_class("UnityEngine.UI.Text") then
    print("no UI namespace in this build, panel text skipped")
  end
  if not has_tick_hook() then
    pcall(set_tick_hook, "GorillaLocomotion.Player", "FixedUpdate", "Assembly-CSharp.dll")
  end
  local camera = scene.find_object("Main Camera")
  local forward = is_valid(camera) and camera.transform.forward or vec3(0, 0, 1)
  local spot = is_valid(camera) and camera.transform.position + forward * 2.0 or vec3(0, 2, 0)
  panel_root = build_panel(spot)
  if is_valid(panel_root) then
    local listener = scene.find_object("AudioListener")
    if is_valid(listener) then
      panel_root.transform:SetParent(listener.gameObject.transform, false)
    end
  end
  cancel_all()
  on_update(tick)
  print("panel ready, squeeze left grip to toggle slow motion")
end)
```

Run it, squeeze the left grip: time slows, the label flips, the panel slowly breathes between two greens. Run it again: the old panel dies in `cleanup_old()`, a fresh one appears. Reset the Lua state and the old objects become invalid references instead of duplicates - that is the two layer cleanup doing its job.

Things to change for your own script, deliberately listed so you have no excuse to keep them: the panel shape and size, the trigger button, what the toggle does, the two colors and how they move, the beep idea. `audio.load("ui_click.wav")` plus `audio.play(clip, { volume = 0.6 })` inside the toggle branch is three lines and gives instant feedback.

## Links

- Docs site: https://silenterased.github.io/BNMExplorer/
- This file on GitHub Pages: https://silenterased.github.io/BNMExplorer/llms.txt
- The mod serves the same file at http://DEVICE_IP:8080/llms.txt
- Source repository: https://github.com/SilentErased/BNMExplorer
)TXT";

const char* get_llms_txt() {
    return k_llms_txt;
}
