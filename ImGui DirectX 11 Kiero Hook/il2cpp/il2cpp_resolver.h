#pragma once
#include "il2cpp_types.h"

#define DO_EXPORT(t, c, e) t = reinterpret_cast<c>(IL2CPP_MEMORY.GetExport(e));

using namespace IL2CPP_API;

namespace IL2CPP_RESOLVER {
	inline void Initialize() {
		// Domain / thread
		DO_EXPORT(domain_get, t_domain_get, L"il2cpp_domain_get");
		DO_EXPORT(domain_get_assemblies, t_domain_get_assemblies, L"il2cpp_domain_get_assemblies");
		DO_EXPORT(thread_attach, t_thread_attach, L"il2cpp_thread_attach");
		DO_EXPORT(thread_detach, t_thread_detach, L"il2cpp_thread_detach");
		DO_EXPORT(thread_current, t_thread_current, L"il2cpp_thread_current");

		// Assembly / image
		DO_EXPORT(domain_assembly_open, t_domain_assembly_open, L"il2cpp_domain_assembly_open");
		DO_EXPORT(assembly_get_image, t_assembly_get_image, L"il2cpp_assembly_get_image");
		DO_EXPORT(image_get_name, t_image_get_name, L"il2cpp_image_get_name");
		DO_EXPORT(image_get_filename, t_image_get_filename, L"il2cpp_image_get_filename");
		DO_EXPORT(image_get_assembly, t_image_get_assembly, L"il2cpp_image_get_assembly");
		DO_EXPORT(image_get_class_count, t_image_get_class_count, L"il2cpp_image_get_class_count");
		DO_EXPORT(image_get_class, t_image_get_class, L"il2cpp_image_get_class");

		// Class
		DO_EXPORT(class_from_name, t_class_from_name, L"il2cpp_class_from_name");
		DO_EXPORT(class_from_type, t_class_from_type, L"il2cpp_class_from_type");
		DO_EXPORT(class_get_name, t_class_get_name, L"il2cpp_class_get_name");
		DO_EXPORT(class_get_namespace, t_class_get_namespace, L"il2cpp_class_get_namespace");
		DO_EXPORT(class_get_image, t_class_get_image, L"il2cpp_class_get_image");
		DO_EXPORT(class_get_parent, t_class_get_parent, L"il2cpp_class_get_parent");
		DO_EXPORT(class_get_interfaces, t_class_get_interfaces, L"il2cpp_class_get_interfaces");
		DO_EXPORT(class_get_methods, t_class_get_methods, L"il2cpp_class_get_methods");
		DO_EXPORT(class_get_method_from_name, t_class_get_method_from_name, L"il2cpp_class_get_method_from_name");
		DO_EXPORT(class_get_fields, t_class_get_fields, L"il2cpp_class_get_fields");
		DO_EXPORT(class_get_field_from_name, t_class_get_field_from_name, L"il2cpp_class_get_field_from_name");
		DO_EXPORT(class_get_properties, t_class_get_properties, L"il2cpp_class_get_properties");
		DO_EXPORT(class_get_property_from_name, t_class_get_property_from_name, L"il2cpp_class_get_property_from_name");
		DO_EXPORT(class_get_type, t_class_get_type, L"il2cpp_class_get_type");
		DO_EXPORT(class_is_valuetype, t_class_is_valuetype, L"il2cpp_class_is_valuetype");
		DO_EXPORT(class_is_enum, t_class_is_enum, L"il2cpp_class_is_enum");
		DO_EXPORT(class_enum_basetype, t_class_enum_basetype, L"il2cpp_class_enum_basetype");
		DO_EXPORT(class_instance_size, t_class_instance_size, L"il2cpp_class_instance_size");
		DO_EXPORT(class_value_size, t_class_value_size, L"il2cpp_class_value_size");
		DO_EXPORT(class_get_flags, t_class_get_flags, L"il2cpp_class_get_flags");

		// Method
		DO_EXPORT(method_get_name, t_method_get_name, L"il2cpp_method_get_name");
		DO_EXPORT(method_get_class, t_method_get_class, L"il2cpp_method_get_class");
		DO_EXPORT(method_get_return_type, t_method_get_return_type, L"il2cpp_method_get_return_type");
		DO_EXPORT(method_get_param_count, t_method_get_param_count, L"il2cpp_method_get_param_count");
		DO_EXPORT(method_get_param, t_method_get_param, L"il2cpp_method_get_param");
		DO_EXPORT(method_get_param_name, t_method_get_param_name, L"il2cpp_method_get_param_name");
		DO_EXPORT(method_get_flags, t_method_get_flags, L"il2cpp_method_get_flags");
		DO_EXPORT(method_is_generic, t_method_is_generic, L"il2cpp_method_is_generic");
		DO_EXPORT(method_is_inflated, t_method_is_inflated, L"il2cpp_method_is_inflated");
		DO_EXPORT(compile_method, t_compile_method, L"il2cpp_compile_method");

		// Field
		DO_EXPORT(field_get_name, t_field_get_name, L"il2cpp_field_get_name");
		DO_EXPORT(field_get_type, t_field_get_type, L"il2cpp_field_get_type");
		DO_EXPORT(field_get_parent, t_field_get_parent, L"il2cpp_field_get_parent");
		DO_EXPORT(field_get_offset, t_field_get_offset, L"il2cpp_field_get_offset");
		DO_EXPORT(field_get_flags, t_field_get_flags, L"il2cpp_field_get_flags");
		DO_EXPORT(field_get_value_object, t_field_get_value_object, L"il2cpp_field_get_value_object");
		DO_EXPORT(field_get_value, t_field_get_value, L"il2cpp_field_get_value");
		DO_EXPORT(field_set_value, t_field_set_value, L"il2cpp_field_set_value");
		DO_EXPORT(field_static_get_value, t_field_static_get_value, L"il2cpp_field_static_get_value");
		DO_EXPORT(field_static_set_value, t_field_static_set_value, L"il2cpp_field_static_set_value");

		// Property
		DO_EXPORT(property_get_name, t_property_get_name, L"il2cpp_property_get_name");
		DO_EXPORT(property_get_parent, t_property_get_parent, L"il2cpp_property_get_parent");
		DO_EXPORT(property_get_get_method, t_property_get_get_method, L"il2cpp_property_get_get_method");
		DO_EXPORT(property_get_set_method, t_property_get_set_method, L"il2cpp_property_get_set_method");
		DO_EXPORT(property_get_flags, t_property_get_flags, L"il2cpp_property_get_flags");

		// Type
		DO_EXPORT(type_get_name, t_type_get_name, L"il2cpp_type_get_name");
		DO_EXPORT(type_get_type, t_type_get_type, L"il2cpp_type_get_type");
		DO_EXPORT(type_is_byref, t_type_is_byref, L"il2cpp_type_is_byref");
		DO_EXPORT(type_get_attrs, t_type_get_attrs, L"il2cpp_type_get_attrs");
		DO_EXPORT(type_get_object, t_type_get_object, L"il2cpp_type_get_object");

		// Object / runtime
		DO_EXPORT(object_get_class, t_object_get_class, L"il2cpp_object_get_class");
		DO_EXPORT(object_get_size, t_object_get_size, L"il2cpp_object_get_size");
		DO_EXPORT(object_new, t_object_new, L"il2cpp_object_new");
		DO_EXPORT(runtime_object_init, t_runtime_object_init, L"il2cpp_runtime_object_init");
		DO_EXPORT(runtime_invoke, t_runtime_invoke, L"il2cpp_runtime_invoke");
		DO_EXPORT(runtime_invoke_convert_args, t_runtime_invoke_convert_args, L"il2cpp_runtime_invoke_convert_args");
		DO_EXPORT(object_unbox, t_object_unbox, L"il2cpp_object_unbox");
		DO_EXPORT(value_box, t_value_box, L"il2cpp_value_box");
		DO_EXPORT(object_to_string, t_object_to_string, L"il2cpp_object_to_string");

		// Strings
		DO_EXPORT(string_new, t_string_new, L"il2cpp_string_new");
		DO_EXPORT(string_new_utf16, t_string_new_utf16, L"il2cpp_string_new_utf16");
		DO_EXPORT(string_length, t_string_length, L"il2cpp_string_length");
		DO_EXPORT(string_chars, t_string_chars, L"il2cpp_string_chars");

		// Arrays
		DO_EXPORT(array_new, t_array_new, L"il2cpp_array_new");
		DO_EXPORT(array_new_specific, t_array_new_specific, L"il2cpp_array_new_specific");
		DO_EXPORT(array_length, t_array_length, L"il2cpp_array_length");
		DO_EXPORT(array_get_byte_length, t_array_get_byte_length, L"il2cpp_array_get_byte_length");

		// GC handles
		DO_EXPORT(gchandle_new, t_gchandle_new, L"il2cpp_gchandle_new");
		DO_EXPORT(gchandle_new_weakref, t_gchandle_new_weakref, L"il2cpp_gchandle_new_weakref");
		DO_EXPORT(gchandle_get_target, t_gchandle_get_target, L"il2cpp_gchandle_get_target");
		DO_EXPORT(gchandle_free, t_gchandle_free, L"il2cpp_gchandle_free");

		// Misc runtime helpers
		DO_EXPORT(resolve_icall, t_resolve_icall, L"il2cpp_resolve_icall");
		DO_EXPORT(il2cpp_free, t_free, L"il2cpp_free");
	}
}
