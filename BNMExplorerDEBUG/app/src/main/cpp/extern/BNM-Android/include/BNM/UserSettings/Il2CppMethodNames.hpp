#pragma once

// IL2CPP API symbol names resolved by BNM via dlsym.
// Names follow the canonical Unity IL2CPP exports (see nneonneo/Il2CppVersions api lists).
// You can override any name by defining the macro before including BNM.

#define BNM_IL2CPP_API_il2cpp_init "il2cpp_init"
#define BNM_IL2CPP_API_il2cpp_class_from_il2cpp_type "il2cpp_class_from_il2cpp_type"

#define BNM_IL2CPP_API_il2cpp_array_new_specific "il2cpp_array_new_specific"
#define BNM_IL2CPP_API_il2cpp_class_from_type "il2cpp_class_from_type"
#define BNM_IL2CPP_API_il2cpp_type_get_class_or_element_class "il2cpp_type_get_class_or_element_class"
#define BNM_IL2CPP_API_il2cpp_domain_get_assemblies "il2cpp_domain_get_assemblies"
#define BNM_IL2CPP_API_il2cpp_domain_assembly_open "il2cpp_domain_assembly_open"

#define BNM_IL2CPP_API_il2cpp_get_corlib "il2cpp_get_corlib"
#define BNM_IL2CPP_API_il2cpp_class_from_name "il2cpp_class_from_name"
#define BNM_IL2CPP_API_il2cpp_assembly_get_image "il2cpp_assembly_get_image"
#define BNM_IL2CPP_API_il2cpp_method_get_param_name "il2cpp_method_get_param_name"
#define BNM_IL2CPP_API_il2cpp_array_class_get "il2cpp_array_class_get"
#define BNM_IL2CPP_API_il2cpp_type_get_object "il2cpp_type_get_object"
#define BNM_IL2CPP_API_il2cpp_object_new "il2cpp_object_new"
#define BNM_IL2CPP_API_il2cpp_value_box "il2cpp_value_box"
#define BNM_IL2CPP_API_il2cpp_array_new "il2cpp_array_new"
#define BNM_IL2CPP_API_il2cpp_field_static_get_value "il2cpp_field_static_get_value"
#define BNM_IL2CPP_API_il2cpp_field_static_set_value "il2cpp_field_static_set_value"
#define BNM_IL2CPP_API_il2cpp_string_new "il2cpp_string_new"
#define BNM_IL2CPP_API_il2cpp_resolve_icall "il2cpp_resolve_icall"
#define BNM_IL2CPP_API_il2cpp_runtime_invoke "il2cpp_runtime_invoke"
#define BNM_IL2CPP_API_il2cpp_domain_get "il2cpp_domain_get"
#define BNM_IL2CPP_API_il2cpp_thread_current "il2cpp_thread_current"
#define BNM_IL2CPP_API_il2cpp_thread_attach "il2cpp_thread_attach"
#define BNM_IL2CPP_API_il2cpp_thread_detach "il2cpp_thread_detach"

// il2cpp_image_get_class is only available since Unity 2018.4
#if UNITY_VER >= 183
#define BNM_IL2CPP_API_il2cpp_image_get_class "il2cpp_image_get_class"
#endif

// il2cpp_gc_alloc_fixed / il2cpp_gc_free_fixed are only available since Unity 2021.2
#if UNITY_VER >= 212
#define BNM_IL2CPP_API_il2cpp_gc_alloc_fixed "il2cpp_gc_alloc_fixed"
#define BNM_IL2CPP_API_il2cpp_gc_free_fixed "il2cpp_gc_free_fixed"
#endif

// GC handles (canonical Unity names; BNM used to resolve non-existent il2cpp_gc_gchandle_* symbols)
#define BNM_IL2CPP_API_il2cpp_gchandle_new "il2cpp_gchandle_new"
#define BNM_IL2CPP_API_il2cpp_gchandle_free "il2cpp_gchandle_free"
#define BNM_IL2CPP_API_il2cpp_gchandle_get_target "il2cpp_gchandle_get_target"
#define BNM_IL2CPP_API_il2cpp_gchandle_new_weakref "il2cpp_gchandle_new_weakref"

// il2cpp_gchandle_foreach_get_target is only available since Unity 2019.3
#if UNITY_VER >= 193
#define BNM_IL2CPP_API_il2cpp_gchandle_foreach_get_target "il2cpp_gchandle_foreach_get_target"
#endif

// Class runtime helpers
#define BNM_IL2CPP_API_il2cpp_runtime_class_init "il2cpp_runtime_class_init"
#define BNM_IL2CPP_API_il2cpp_object_get_class "il2cpp_object_get_class"
#define BNM_IL2CPP_API_il2cpp_class_get_type "il2cpp_class_get_type"
#define BNM_IL2CPP_API_il2cpp_class_get_name "il2cpp_class_get_name"
#define BNM_IL2CPP_API_il2cpp_class_get_namespace "il2cpp_class_get_namespace"
#define BNM_IL2CPP_API_il2cpp_class_get_image "il2cpp_class_get_image"
#define BNM_IL2CPP_API_il2cpp_class_get_parent "il2cpp_class_get_parent"
#define BNM_IL2CPP_API_il2cpp_class_is_valuetype "il2cpp_class_is_valuetype"
#define BNM_IL2CPP_API_il2cpp_class_is_enum "il2cpp_class_is_enum"
#define BNM_IL2CPP_API_il2cpp_class_get_method_from_name "il2cpp_class_get_method_from_name"
#define BNM_IL2CPP_API_il2cpp_class_get_field_from_name "il2cpp_class_get_field_from_name"

// il2cpp_class_get_userdata_offset / il2cpp_class_set_userdata are only available since Unity 2019.1/2019.2
#if UNITY_VER >= 191
#define BNM_IL2CPP_API_il2cpp_class_get_userdata_offset "il2cpp_class_get_userdata_offset"
#define BNM_IL2CPP_API_il2cpp_class_set_userdata "il2cpp_class_set_userdata"
#endif

// String helpers
#define BNM_IL2CPP_API_il2cpp_string_new_len "il2cpp_string_new_len"
#define BNM_IL2CPP_API_il2cpp_string_new_utf16 "il2cpp_string_new_utf16"
