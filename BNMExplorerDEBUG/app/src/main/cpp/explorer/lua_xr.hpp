#pragma once
#include <algorithm>
#include "lua_objects.hpp"

struct xr_input_device {
    uint64_t device_id;
    uint64_t extra;
};

struct xr_vec2 {
    float x;
    float y;
};

static std::string normalize_feature_name(const char* s) {
    std::string out;
    for (; *s; s++) {
        if (*s == '_' || *s == ' ') continue;
        out += (char)std::tolower((unsigned char)*s);
    }
    return out;
}

static const char* resolve_feature_name(const std::unordered_map<std::string, const char*>& table, const char* name) {
    auto it = table.find(normalize_feature_name(name));
    return it == table.end() ? nullptr : it->second;
}

static const std::unordered_map<std::string, const char*>& xr_bool_features() {
    static const std::unordered_map<std::string, const char*> table = {
        {"primarybutton", "PrimaryButton"},
        {"primarytouch", "PrimaryTouch"},
        {"secondarybutton", "SecondaryButton"},
        {"secondarytouch", "SecondaryTouch"},
        {"gripbutton", "GripButton"},
        {"triggerbutton", "TriggerButton"},
        {"menubutton", "MenuButton"},
        {"primary2daxisclick", "Primary2DAxisClick"},
        {"primary2daxistouch", "Primary2DAxisTouch"},
        {"secondary2daxisclick", "Secondary2DAxisClick"},
        {"secondary2daxistouch", "Secondary2DAxisTouch"}
    };
    return table;
}

static const std::unordered_map<std::string, const char*>& xr_float_features() {
    static const std::unordered_map<std::string, const char*> table = {
        {"trigger", "Trigger"},
        {"grip", "Grip"}
    };
    return table;
}

static const std::unordered_map<std::string, const char*>& xr_vec2_features() {
    static const std::unordered_map<std::string, const char*> table = {
        {"primary2daxis", "Primary2DAxis"},
        {"secondary2daxis", "Secondary2DAxis"}
    };
    return table;
}

static int xr_side_from_arg(lua_State* L, int idx) {
    if (lua_type(L, idx) == LUA_TNUMBER) return lua_tonumber(L, idx) == 0 ? 0 : 1;
    const char* s = luaL_checkstring(L, idx);
    return (s[0] == 'l' || s[0] == 'L') ? 0 : 1;
}

static bool xr_get_device(int side, uint64_t& device_id) {
    Class input_devices("UnityEngine.XR", "InputDevices");
    if (!input_devices.IsValid()) return false;
    Method<xr_input_device> get_device = input_devices.GetMethod("GetDeviceAtXRNode", 1);
    if (!get_device.IsValid()) return false;
    xr_input_device dev = get_device(side == 0 ? 4 : 5);
    device_id = dev.device_id;
    return true;
}

template <typename T>
static bool xr_try_get(const char* method_name, const char* extern_name, uint64_t device_id, const char* usage, T* out) {
    Class input_devices("UnityEngine.XR", "InputDevices");
    if (!input_devices.IsValid()) return false;
    Method<bool> method = input_devices.GetMethod(method_name, 3);
    if (method.IsValid()) return method(device_id, CreateMonoString(usage), out);
    auto fn = (bool (*)(uint64_t, BNM::Structures::Mono::String*, T*))BNM::GetExternMethod(extern_name);
    if (!fn) return false;
    return fn(device_id, CreateMonoString(usage), out);
}

static int l_xr_is_available(lua_State* L) {
    Class input_devices("UnityEngine.XR", "InputDevices");
    lua_pushboolean(L, input_devices.IsValid());
    return 1;
}

static int l_xr_get_bool_feature(lua_State* L) {
    const char* usage = resolve_feature_name(xr_bool_features(), luaL_checkstring(L, 1));
    if (!usage) return luaL_error(L, "unknown bool feature '%s'", lua_tostring(L, 1));
    int side = xr_side_from_arg(L, 2);
    uint64_t device_id = 0;
    bool value = false;
    if (!xr_get_device(side, device_id) ||
        !xr_try_get<bool>("TryGetFeatureValue_bool", "UnityEngine.XR.InputDevices::TryGetFeatureValue_bool", device_id, usage, &value))
        value = false;
    lua_pushboolean(L, value);
    return 1;
}

static int l_xr_get_float_feature(lua_State* L) {
    const char* usage = resolve_feature_name(xr_float_features(), luaL_checkstring(L, 1));
    if (!usage) return luaL_error(L, "unknown float feature '%s'", lua_tostring(L, 1));
    int side = xr_side_from_arg(L, 2);
    uint64_t device_id = 0;
    float value = 0.0f;
    if (!xr_get_device(side, device_id) ||
        !xr_try_get<float>("TryGetFeatureValue_float", "UnityEngine.XR.InputDevices::TryGetFeatureValue_float", device_id, usage, &value))
        value = 0.0f;
    lua_pushnumber(L, value);
    return 1;
}

static int l_xr_get_vector2_feature(lua_State* L) {
    const char* usage = resolve_feature_name(xr_vec2_features(), luaL_checkstring(L, 1));
    if (!usage) return luaL_error(L, "unknown vector2 feature '%s'", lua_tostring(L, 1));
    int side = xr_side_from_arg(L, 2);
    uint64_t device_id = 0;
    xr_vec2 value = {0.0f, 0.0f};
    if (!xr_get_device(side, device_id) ||
        !xr_try_get<xr_vec2>("TryGetFeatureValue_Vector2f", "UnityEngine.XR.InputDevices::TryGetFeatureValue_Vector2f", device_id, usage, &value))
        value = {0.0f, 0.0f};
    float out[2] = {value.x, value.y};
    push_vec(L, out, 2, false);
    return 1;
}

static int l_xr_send_haptic_impulse(lua_State* L) {
    int side = xr_side_from_arg(L, 1);
    float amplitude = (float)luaL_checknumber(L, 2);
    float duration = (float)luaL_checknumber(L, 3);
    uint64_t device_id = 0;
    if (!xr_get_device(side, device_id)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    Class input_devices("UnityEngine.XR", "InputDevices");
    Method<void> method = input_devices.GetMethod("SendHapticImpulse", 4);
    if (method.IsValid()) {
        method(device_id, 0u, amplitude, duration);
        lua_pushboolean(L, 1);
        return 1;
    }
    auto fn = (void (*)(uint64_t, uint32_t, float, float))BNM::GetExternMethod("UnityEngine.XR.InputDevices::SendHapticImpulse");
    if (!fn) {
        lua_pushboolean(L, 0);
        return 1;
    }
    fn(device_id, 0u, amplitude, duration);
    lua_pushboolean(L, 1);
    return 1;
}

static void register_xr(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, l_xr_is_available);
    lua_setfield(L, -2, "is_available");
    lua_pushcfunction(L, l_xr_get_bool_feature);
    lua_setfield(L, -2, "get_bool_feature");
    lua_pushcfunction(L, l_xr_get_float_feature);
    lua_setfield(L, -2, "get_float_feature");
    lua_pushcfunction(L, l_xr_get_vector2_feature);
    lua_setfield(L, -2, "get_vector2_feature");
    lua_pushcfunction(L, l_xr_send_haptic_impulse);
    lua_setfield(L, -2, "send_haptic_impulse");
    lua_setglobal(L, "xr");
}
