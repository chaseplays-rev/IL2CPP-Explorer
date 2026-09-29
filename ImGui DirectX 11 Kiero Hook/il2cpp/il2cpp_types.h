#pragma once
#include "il2cpp_memory.h"
#include <cstddef>
#include <cstdint>

#define IL2TYPE(n) typedef class n{}n;
#define IL2FUNC(r, n, b, ...) typedef r(*n)(__VA_ARGS__); inline n b;
#define IL2FUNCT(r, n, b) typedef r(*n)(); inline n b;
#define DO_EXPORT(t, c, e) IL2CPP_API::t = reinterpret_cast<IL2CPP_API::c>(IL2CPP_MEMORY.GetExport(e));

namespace IL2CPP_API {
	IL2TYPE(Il2CppDomain);
	IL2TYPE(Il2CppAssembly);
	IL2TYPE(Il2CppImage);
	IL2TYPE(Il2CppClass);
	IL2TYPE(Il2CppMethod);
	IL2TYPE(Il2CppField);
	IL2TYPE(Il2CppProperty);
	IL2TYPE(Il2CppString);
	IL2TYPE(Il2CppObject);
	IL2TYPE(Il2CppArray);
	IL2TYPE(Il2CppType);
	IL2TYPE(Il2CppException);
	IL2TYPE(Il2CppThread);

	using Il2CppChar = wchar_t;
	using Il2CppGCHandle = void*;

	enum Il2CppTypeEnum {
		IL2CPP_TYPE_END = 0x00,
		IL2CPP_TYPE_VOID = 0x01,
		IL2CPP_TYPE_BOOLEAN = 0x02,
		IL2CPP_TYPE_CHAR = 0x03,
		IL2CPP_TYPE_I1 = 0x04,
		IL2CPP_TYPE_U1 = 0x05,
		IL2CPP_TYPE_I2 = 0x06,
		IL2CPP_TYPE_U2 = 0x07,
		IL2CPP_TYPE_I4 = 0x08,
		IL2CPP_TYPE_U4 = 0x09,
		IL2CPP_TYPE_I8 = 0x0A,
		IL2CPP_TYPE_U8 = 0x0B,
		IL2CPP_TYPE_R4 = 0x0C,
		IL2CPP_TYPE_R8 = 0x0D,
		IL2CPP_TYPE_STRING = 0x0E,
		IL2CPP_TYPE_PTR = 0x0F,
		IL2CPP_TYPE_BYREF = 0x10,
		IL2CPP_TYPE_VALUETYPE = 0x11,
		IL2CPP_TYPE_CLASS = 0x12,
		IL2CPP_TYPE_VAR = 0x13,
		IL2CPP_TYPE_ARRAY = 0x14,
		IL2CPP_TYPE_GENERICINST = 0x15,
		IL2CPP_TYPE_TYPEDBYREF = 0x16,
		IL2CPP_TYPE_I = 0x18,
		IL2CPP_TYPE_U = 0x19,
		IL2CPP_TYPE_FNPTR = 0x1B,
		IL2CPP_TYPE_OBJECT = 0x1C,
		IL2CPP_TYPE_SZARRAY = 0x1D,
		IL2CPP_TYPE_MVAR = 0x1E,
		IL2CPP_TYPE_CMOD_REQD = 0x1F,
		IL2CPP_TYPE_CMOD_OPT = 0x20,
		IL2CPP_TYPE_INTERNAL = 0x21,
		IL2CPP_TYPE_MODIFIER = 0x40,
		IL2CPP_TYPE_SENTINEL = 0x41,
		IL2CPP_TYPE_PINNED = 0x45,
		IL2CPP_TYPE_ENUM = 0x55
	};

	// Domain / thread
	IL2FUNCT(Il2CppDomain*, t_domain_get, domain_get);
	IL2FUNC(const Il2CppAssembly**, t_domain_get_assemblies, domain_get_assemblies, Il2CppDomain*, size_t*);
	IL2FUNC(Il2CppThread*, t_thread_attach, thread_attach, Il2CppDomain*);
	IL2FUNC(void, t_thread_detach, thread_detach, Il2CppThread*);
	IL2FUNCT(Il2CppThread*, t_thread_current, thread_current);

	// Assembly / image
	IL2FUNC(Il2CppAssembly*, t_domain_assembly_open, domain_assembly_open, Il2CppDomain*, const char*);
	IL2FUNC(Il2CppImage*, t_assembly_get_image, assembly_get_image, Il2CppAssembly*);
	IL2FUNC(const char*, t_image_get_name, image_get_name, Il2CppImage*);
	IL2FUNC(const char*, t_image_get_filename, image_get_filename, Il2CppImage*);
	IL2FUNC(Il2CppAssembly*, t_image_get_assembly, image_get_assembly, Il2CppImage*);
	IL2FUNC(size_t, t_image_get_class_count, image_get_class_count, Il2CppImage*);
	IL2FUNC(Il2CppClass*, t_image_get_class, image_get_class, Il2CppImage*, size_t);

	// Class
	IL2FUNC(Il2CppClass*, t_class_from_name, class_from_name, Il2CppImage*, const char*, const char*);
	IL2FUNC(Il2CppClass*, t_class_from_type, class_from_type, Il2CppType*);
	IL2FUNC(const char*, t_class_get_name, class_get_name, Il2CppClass*);
	IL2FUNC(const char*, t_class_get_namespace, class_get_namespace, Il2CppClass*);
	IL2FUNC(Il2CppImage*, t_class_get_image, class_get_image, Il2CppClass*);
	IL2FUNC(Il2CppClass*, t_class_get_parent, class_get_parent, Il2CppClass*);
	IL2FUNC(Il2CppClass*, t_class_get_interfaces, class_get_interfaces, Il2CppClass*, void**);
	IL2FUNC(Il2CppMethod*, t_class_get_methods, class_get_methods, Il2CppClass*, void**);
	IL2FUNC(Il2CppMethod*, t_class_get_method_from_name, class_get_method_from_name, Il2CppClass*, const char*, int);
	IL2FUNC(Il2CppField*, t_class_get_fields, class_get_fields, Il2CppClass*, void**);
	IL2FUNC(Il2CppField*, t_class_get_field_from_name, class_get_field_from_name, Il2CppClass*, const char*);
	IL2FUNC(Il2CppProperty*, t_class_get_properties, class_get_properties, Il2CppClass*, void**);
	IL2FUNC(Il2CppProperty*, t_class_get_property_from_name, class_get_property_from_name, Il2CppClass*, const char*);
	IL2FUNC(Il2CppType*, t_class_get_type, class_get_type, Il2CppClass*);
	IL2FUNC(bool, t_class_is_valuetype, class_is_valuetype, Il2CppClass*);
	IL2FUNC(bool, t_class_is_enum, class_is_enum, Il2CppClass*);
	IL2FUNC(Il2CppType*, t_class_enum_basetype, class_enum_basetype, Il2CppClass*);
	IL2FUNC(int32_t, t_class_instance_size, class_instance_size, Il2CppClass*);
	IL2FUNC(int32_t, t_class_value_size, class_value_size, Il2CppClass*, uint32_t*);
	IL2FUNC(uint32_t, t_class_get_flags, class_get_flags, Il2CppClass*);

	// Method
	IL2FUNC(const char*, t_method_get_name, method_get_name, Il2CppMethod*);
	IL2FUNC(Il2CppClass*, t_method_get_class, method_get_class, Il2CppMethod*);
	IL2FUNC(Il2CppType*, t_method_get_return_type, method_get_return_type, Il2CppMethod*);
	IL2FUNC(uint32_t, t_method_get_param_count, method_get_param_count, Il2CppMethod*);
	IL2FUNC(Il2CppType*, t_method_get_param, method_get_param, Il2CppMethod*, uint32_t);
	IL2FUNC(const char*, t_method_get_param_name, method_get_param_name, Il2CppMethod*, uint32_t);
	IL2FUNC(uint32_t, t_method_get_flags, method_get_flags, Il2CppMethod*, uint32_t*);
	IL2FUNC(bool, t_method_is_generic, method_is_generic, Il2CppMethod*);
	IL2FUNC(bool, t_method_is_inflated, method_is_inflated, Il2CppMethod*);
	IL2FUNC(void*, t_compile_method, compile_method, Il2CppMethod*);

	// Field
	IL2FUNC(const char*, t_field_get_name, field_get_name, Il2CppField*);
	IL2FUNC(Il2CppType*, t_field_get_type, field_get_type, Il2CppField*);
	IL2FUNC(Il2CppClass*, t_field_get_parent, field_get_parent, Il2CppField*);
	IL2FUNC(size_t, t_field_get_offset, field_get_offset, Il2CppField*);
	IL2FUNC(uint32_t, t_field_get_flags, field_get_flags, Il2CppField*);
	IL2FUNC(Il2CppObject*, t_field_get_value_object, field_get_value_object, Il2CppField*, Il2CppObject*);
	IL2FUNC(void, t_field_get_value, field_get_value, Il2CppObject*, Il2CppField*, void*);
	IL2FUNC(void, t_field_set_value, field_set_value, Il2CppObject*, Il2CppField*, void*);
	IL2FUNC(void, t_field_static_get_value, field_static_get_value, Il2CppField*, void*);
	IL2FUNC(void, t_field_static_set_value, field_static_set_value, Il2CppField*, void*);

	// Property
	IL2FUNC(const char*, t_property_get_name, property_get_name, Il2CppProperty*);
	IL2FUNC(Il2CppClass*, t_property_get_parent, property_get_parent, Il2CppProperty*);
	IL2FUNC(Il2CppMethod*, t_property_get_get_method, property_get_get_method, Il2CppProperty*);
	IL2FUNC(Il2CppMethod*, t_property_get_set_method, property_get_set_method, Il2CppProperty*);
	IL2FUNC(uint32_t, t_property_get_flags, property_get_flags, Il2CppProperty*);

	// Type
	IL2FUNC(char*, t_type_get_name, type_get_name, Il2CppType*);
	IL2FUNC(Il2CppTypeEnum, t_type_get_type, type_get_type, Il2CppType*);
	IL2FUNC(bool, t_type_is_byref, type_is_byref, Il2CppType*);
	IL2FUNC(uint32_t, t_type_get_attrs, type_get_attrs, Il2CppType*);
	IL2FUNC(Il2CppObject*, t_type_get_object, type_get_object, Il2CppType*);

	// Object / runtime
	IL2FUNC(Il2CppClass*, t_object_get_class, object_get_class, Il2CppObject*);
	IL2FUNC(uint32_t, t_object_get_size, object_get_size, Il2CppObject*);
	IL2FUNC(Il2CppObject*, t_object_new, object_new, Il2CppClass*);
	IL2FUNC(void, t_runtime_object_init, runtime_object_init, Il2CppObject*);
	IL2FUNC(Il2CppObject*, t_runtime_invoke, runtime_invoke, Il2CppMethod*, void*, void**, Il2CppException**);
	IL2FUNC(Il2CppObject*, t_runtime_invoke_convert_args, runtime_invoke_convert_args, Il2CppMethod*, void*, Il2CppObject**, int, Il2CppException**);
	IL2FUNC(void*, t_object_unbox, object_unbox, Il2CppObject*);
	IL2FUNC(Il2CppObject*, t_value_box, value_box, Il2CppClass*, void*);
	IL2FUNC(Il2CppString*, t_object_to_string, object_to_string, Il2CppObject*, Il2CppException**);

	// Strings
	IL2FUNC(Il2CppString*, t_string_new, string_new, const char*);
	IL2FUNC(Il2CppString*, t_string_new_utf16, string_new_utf16, const Il2CppChar*, int32_t);
	IL2FUNC(int32_t, t_string_length, string_length, Il2CppString*);
	IL2FUNC(const Il2CppChar*, t_string_chars, string_chars, Il2CppString*);

	// Arrays
	IL2FUNC(Il2CppArray*, t_array_new, array_new, Il2CppClass*, uintptr_t);
	IL2FUNC(Il2CppArray*, t_array_new_specific, array_new_specific, Il2CppClass*, uintptr_t);
	IL2FUNC(uintptr_t, t_array_length, array_length, Il2CppArray*);
	IL2FUNC(uint32_t, t_array_get_byte_length, array_get_byte_length, Il2CppArray*);

	// GC handles
	IL2FUNC(Il2CppGCHandle, t_gchandle_new, gchandle_new, Il2CppObject*, bool);
	IL2FUNC(Il2CppGCHandle, t_gchandle_new_weakref, gchandle_new_weakref, Il2CppObject*, bool);
	IL2FUNC(Il2CppObject*, t_gchandle_get_target, gchandle_get_target, Il2CppGCHandle);
	IL2FUNC(void, t_gchandle_free, gchandle_free, Il2CppGCHandle);

	// Misc runtime helpers
	IL2FUNC(void*, t_resolve_icall, resolve_icall, const char*);
	IL2FUNC(void, t_free, il2cpp_free, void*);
}
