#include "il2cpp.h"
#include <iostream>
#include <string>
#include <vector>

namespace {
	struct AssemblyCacheEntry { std::string name; Assembly* assembly = nullptr; };
	struct ImageCacheEntry { Assembly* assembly = nullptr; Il2CppImage* image = nullptr; };
	struct ClassCacheEntry { std::string assembly; std::string namespc; std::string name; Class* klass = nullptr; };
	struct MethodCacheEntry { Class* klass = nullptr; std::string name; int paramCount = 0; Method* method = nullptr; };
	struct ExactMethodCacheEntry { Class* klass = nullptr; std::string name; std::vector<std::string> paramTypes; Method* method = nullptr; };
	struct FieldCacheEntry { Class* klass = nullptr; std::string name; Field* field = nullptr; };
	struct PropertyCacheEntry { Class* klass = nullptr; std::string name; Property* property = nullptr; };
	struct TypeCacheEntry { Class* klass = nullptr; Type* type = nullptr; };

	std::vector<AssemblyCacheEntry> gAssemblyCache;
	std::vector<ImageCacheEntry> gImageCache;
	std::vector<ClassCacheEntry> gClassCache;
	std::vector<MethodCacheEntry> gMethodCache;
	std::vector<ExactMethodCacheEntry> gExactMethodCache;
	std::vector<FieldCacheEntry> gFieldCache;
	std::vector<PropertyCacheEntry> gPropertyCache;
	std::vector<TypeCacheEntry> gTypeCache;

	const char* SafeString(const char* value) { return value ? value : ""; }
}

Assembly* IL2CPP::Assembly::Resolve(const char* Asm)
{
	const std::string name = SafeString(Asm);
	for (const auto& entry : gAssemblyCache) if (entry.name == name) return entry.assembly;
	Assembly* assembly = reinterpret_cast<Assembly*>(domain_assembly_open(Domain, name.c_str()));
	if (assembly) gAssemblyCache.push_back({ name, assembly });
	return assembly;
}

Il2CppImage* IL2CPP::Assembly::GetImage()
{
	for (const auto& entry : gImageCache) if (entry.assembly == this) return entry.image;
	Il2CppImage* image = assembly_get_image(this);
	if (image) gImageCache.push_back({ this, image });
	return image;
}

const char* IL2CPP::Assembly::GetName()
{
	Il2CppImage* image = GetImage();
	return image && image_get_name ? image_get_name(image) : nullptr;
}

Class* IL2CPP::Class::Resolve(const char* Asm, const char* Namespace, const char* Klass)
{
	const std::string assemblyName = SafeString(Asm);
	const std::string namespaceName = SafeString(Namespace);
	const std::string className = SafeString(Klass);
	for (const auto& entry : gClassCache) if (entry.assembly == assemblyName && entry.namespc == namespaceName && entry.name == className) return entry.klass;
	Assembly* assembly = Assembly::Resolve(assemblyName.c_str());
	if (!assembly) return nullptr;
	Il2CppImage* image = assembly->GetImage();
	if (!image) return nullptr;
	Class* klass = reinterpret_cast<Class*>(class_from_name(image, namespaceName.c_str(), className.c_str()));
	if (klass) gClassCache.push_back({ assemblyName, namespaceName, className, klass });
	return klass;
}

Method* IL2CPP::Method::Resolve(const char* Asm, const char* Namespace, const char* Klass, const char* Name, int pCount)
{
	Class* klass = Class::Resolve(Asm, Namespace, Klass);
	return klass ? klass->GetMethod(Name, pCount) : nullptr;
}

Method* IL2CPP::Method::ResolveExact(const char* Asm, const char* Namespace, const char* Klass, const char* Name, const std::vector<std::string>& ParamTypes)
{
	Class* klass = Class::Resolve(Asm, Namespace, Klass);
	return klass ? klass->GetMethodExact(Name, ParamTypes) : nullptr;
}

Method* IL2CPP::Class::GetMethod(const char* Name, int pCount)
{
	const std::string methodName = SafeString(Name);
	for (const auto& entry : gMethodCache) if (entry.klass == this && entry.name == methodName && entry.paramCount == pCount) return entry.method;
	Method* method = reinterpret_cast<Method*>(class_get_method_from_name(this, methodName.c_str(), pCount));
	if (method) gMethodCache.push_back({ this, methodName, pCount, method });
	return method;
}

Method* IL2CPP::Class::GetMethodExact(const char* Name, const std::vector<std::string>& ParamTypes)
{
	const std::string methodName = SafeString(Name);
	for (const auto& entry : gExactMethodCache) if (entry.klass == this && entry.name == methodName && entry.paramTypes == ParamTypes) return entry.method;
	if (!class_get_methods) return nullptr;
	void* iterator = nullptr;
	Method* method = nullptr;
	while ((method = reinterpret_cast<Method*>(class_get_methods(this, &iterator))) != nullptr) {
		const char* currentName = method->GetName();
		if (!currentName || methodName != currentName || method->GetParamCount() != ParamTypes.size()) continue;
		bool matches = true;
		for (uint32_t i = 0; i < method->GetParamCount(); i++) {
			Type* type = method->GetParam(i);
			if (!type || type->GetName() != ParamTypes[i]) { matches = false; break; }
		}
		if (!matches) continue;
		gExactMethodCache.push_back({ this, methodName, ParamTypes, method });
		return method;
	}
	return nullptr;
}

Field* IL2CPP::Class::GetField(const char* Name)
{
	const std::string fieldName = SafeString(Name);
	for (const auto& entry : gFieldCache) if (entry.klass == this && entry.name == fieldName) return entry.field;
	Field* field = reinterpret_cast<Field*>(class_get_field_from_name(this, fieldName.c_str()));
	if (field) gFieldCache.push_back({ this, fieldName, field });
	return field;
}

Property* IL2CPP::Class::GetProperty(const char* Name)
{
	const std::string propertyName = SafeString(Name);
	for (const auto& entry : gPropertyCache) if (entry.klass == this && entry.name == propertyName) return entry.property;
	Property* property = class_get_property_from_name ? reinterpret_cast<Property*>(class_get_property_from_name(this, propertyName.c_str())) : nullptr;
	if (property) gPropertyCache.push_back({ this, propertyName, property });
	return property;
}

Type* IL2CPP::Class::GetType()
{
	for (const auto& entry : gTypeCache) if (entry.klass == this) return entry.type;
	Type* type = reinterpret_cast<Type*>(class_get_type(this));
	if (type) gTypeCache.push_back({ this, type });
	return type;
}

void IL2CPP::Class::GetMethods()
{
	for (Method* method : GetMethodList()) if (method) std::cout << method->GetName() << "\n";
}

std::vector<Method*> IL2CPP::Class::GetMethodList()
{
	std::vector<Method*> methods;
	if (!class_get_methods) return methods;
	void* iterator = nullptr;
	Method* method = nullptr;
	while ((method = reinterpret_cast<Method*>(class_get_methods(this, &iterator))) != nullptr) {
		methods.push_back(method);
		const char* name = method->GetName();
		int paramCount = method_get_param_count ? static_cast<int>(method_get_param_count(method)) : -1;
		if (name && paramCount >= 0) {
			bool exists = false;
			for (const auto& entry : gMethodCache) if (entry.klass == this && entry.name == name && entry.paramCount == paramCount) { exists = true; break; }
			if (!exists) gMethodCache.push_back({ this, name, paramCount, method });
		}
	}
	return methods;
}

std::vector<Field*> IL2CPP::Class::GetFieldList()
{
	std::vector<Field*> fields;
	if (!class_get_fields) return fields;
	void* iterator = nullptr;
	Field* field = nullptr;
	while ((field = reinterpret_cast<Field*>(class_get_fields(this, &iterator))) != nullptr) {
		fields.push_back(field);
		const char* name = field->GetName();
		if (name) {
			bool exists = false;
			for (const auto& entry : gFieldCache) if (entry.klass == this && entry.name == name) { exists = true; break; }
			if (!exists) gFieldCache.push_back({ this, name, field });
		}
	}
	return fields;
}

std::vector<Property*> IL2CPP::Class::GetPropertyList()
{
	std::vector<Property*> properties;
	if (!class_get_properties) return properties;
	void* iterator = nullptr;
	Property* property = nullptr;
	while ((property = reinterpret_cast<Property*>(class_get_properties(this, &iterator))) != nullptr) {
		properties.push_back(property);
		const char* name = property->GetName();
		if (name) {
			bool exists = false;
			for (const auto& entry : gPropertyCache) if (entry.klass == this && entry.name == name) { exists = true; break; }
			if (!exists) gPropertyCache.push_back({ this, name, property });
		}
	}
	return properties;
}

std::vector<Class*> IL2CPP::Class::GetInterfaces()
{
	std::vector<Class*> interfaces;
	if (!class_get_interfaces) return interfaces;
	void* iterator = nullptr;
	Il2CppClass* klass = nullptr;
	while ((klass = class_get_interfaces(this, &iterator)) != nullptr) interfaces.push_back(reinterpret_cast<Class*>(klass));
	return interfaces;
}

const char* IL2CPP::Class::GetName() { return class_get_name ? class_get_name(this) : nullptr; }
const char* IL2CPP::Class::GetNamespace() { return class_get_namespace ? class_get_namespace(this) : nullptr; }
Il2CppImage* IL2CPP::Class::GetImage() { return class_get_image ? class_get_image(this) : nullptr; }
Class* IL2CPP::Class::GetParent() { return class_get_parent ? reinterpret_cast<Class*>(class_get_parent(this)) : nullptr; }
bool IL2CPP::Class::IsValueType() { return class_is_valuetype ? class_is_valuetype(this) : false; }
bool IL2CPP::Class::IsEnum() { return class_is_enum ? class_is_enum(this) : false; }

const char* IL2CPP::Field::GetName() { return field_get_name ? field_get_name(this) : nullptr; }
Type* IL2CPP::Field::GetType() { return field_get_type ? reinterpret_cast<Type*>(field_get_type(this)) : nullptr; }
Class* IL2CPP::Field::GetParent() { return field_get_parent ? reinterpret_cast<Class*>(field_get_parent(this)) : nullptr; }
size_t IL2CPP::Field::GetOffset() { return field_get_offset ? field_get_offset(this) : 0; }
uint32_t IL2CPP::Field::GetFlags() { return field_get_flags ? field_get_flags(this) : 0; }
Object* IL2CPP::Field::GetValueObject(Object* obj) { return field_get_value_object ? reinterpret_cast<Object*>(field_get_value_object(this, obj)) : nullptr; }

const char* IL2CPP::Property::GetName() { return property_get_name ? property_get_name(this) : nullptr; }
Class* IL2CPP::Property::GetParent() { return property_get_parent ? reinterpret_cast<Class*>(property_get_parent(this)) : nullptr; }
Method* IL2CPP::Property::GetGetter() { return property_get_get_method ? reinterpret_cast<Method*>(property_get_get_method(this)) : nullptr; }
Method* IL2CPP::Property::GetSetter() { return property_get_set_method ? reinterpret_cast<Method*>(property_get_set_method(this)) : nullptr; }
uint32_t IL2CPP::Property::GetFlags() { return property_get_flags ? property_get_flags(this) : 0; }

void* IL2CPP::Method::GetAddress() { return this->MethodAddress; }
const char* IL2CPP::Method::GetName() { return method_get_name ? method_get_name(this) : nullptr; }
Class* IL2CPP::Method::GetClass() { return method_get_class ? reinterpret_cast<Class*>(method_get_class(this)) : nullptr; }
Type* IL2CPP::Method::GetReturnType() { return method_get_return_type ? reinterpret_cast<Type*>(method_get_return_type(this)) : nullptr; }
uint32_t IL2CPP::Method::GetParamCount() { return method_get_param_count ? method_get_param_count(this) : 0; }
Type* IL2CPP::Method::GetParam(uint32_t index) { return method_get_param ? reinterpret_cast<Type*>(method_get_param(this, index)) : nullptr; }
const char* IL2CPP::Method::GetParamName(uint32_t index) { return method_get_param_name ? method_get_param_name(this, index) : nullptr; }
uint32_t IL2CPP::Method::GetFlags(uint32_t* iflags) { uint32_t local = 0; return method_get_flags ? method_get_flags(this, iflags ? iflags : &local) : 0; }
bool IL2CPP::Method::IsStatic() { return (GetFlags() & 0x0010) != 0; }
bool IL2CPP::Method::IsGeneric() { return method_is_generic ? method_is_generic(this) : false; }
bool IL2CPP::Method::IsInflated() { return method_is_inflated ? method_is_inflated(this) : false; }
Object* IL2CPP::Method::Invoke(Object* instance, void** params, Il2CppException** exception) { return runtime_invoke ? reinterpret_cast<Object*>(runtime_invoke(this, instance, params, exception)) : nullptr; }
Object* IL2CPP::Method::InvokeConvert(Object* instance, Il2CppObject** params, int paramCount, Il2CppException** exception) { return runtime_invoke_convert_args ? reinterpret_cast<Object*>(runtime_invoke_convert_args(this, instance, params, paramCount, exception)) : nullptr; }

void IL2CPP::Method::Hook(void* vHk, void* vOrg)
{
	MH_CreateHook(this->GetAddress(), vHk, reinterpret_cast<void**>(vOrg));
	MH_EnableHook(this->GetAddress());
}

Class* IL2CPP::Object::GetClass() { return object_get_class ? reinterpret_cast<Class*>(object_get_class(this)) : nullptr; }
Field* IL2CPP::Object::GetField(const char* Name) { Class* klass = this->GetClass(); return klass ? klass->GetField(Name) : nullptr; }

Type* IL2CPP::Type::Resolve(const char* Asm, const char* Namespace, const char* Klass)
{
	Class* klass = Class::Resolve(Asm, Namespace, Klass);
	return klass ? klass->GetType() : nullptr;
}

Class* IL2CPP::Type::GetClass() { return class_from_type ? reinterpret_cast<Class*>(class_from_type(this)) : nullptr; }
Object* IL2CPP::Type::GetTypeObject() { return type_get_object ? reinterpret_cast<Object*>(type_get_object(this)) : nullptr; }
std::string IL2CPP::Type::GetName() { char* raw = type_get_name ? type_get_name(this) : nullptr; if (!raw) return {}; std::string name = raw; if (il2cpp_free) il2cpp_free(raw); return name; }
Il2CppTypeEnum IL2CPP::Type::GetTypeEnum() { return type_get_type ? type_get_type(this) : IL2CPP_API::IL2CPP_TYPE_END; }
bool IL2CPP::Type::IsByRef() { return type_is_byref ? type_is_byref(this) : false; }
uint32_t IL2CPP::Type::GetAttrs() { return type_get_attrs ? type_get_attrs(this) : 0; }

String* IL2CPP::String::New(const char* Text) { return reinterpret_cast<String*>(string_new(Text)); }
size_t IL2CPP::String::GetLength() { return string_length ? static_cast<size_t>(string_length(this)) : 0; }
const wchar_t* IL2CPP::String::GetChars() { return string_chars ? string_chars(this) : nullptr; }

std::string IL2CPP::String::GetString()
{
	const wchar_t* chars = this->GetChars();
	if (!chars) return {};
	std::wstring wstr(chars, chars + this->GetLength());
	return std::string(wstr.begin(), wstr.end());
}
