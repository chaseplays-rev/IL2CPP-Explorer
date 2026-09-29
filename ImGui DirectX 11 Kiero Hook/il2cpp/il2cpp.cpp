#include "il2cpp.h"
#include <string>
#include <vector>

namespace {
    struct AssemblyCacheEntry { std::string name; Assembly* assembly = nullptr; };
    struct ImageCacheEntry { Assembly* assembly = nullptr; Il2CppImage* image = nullptr; };
    struct ClassCacheEntry { std::string assembly; std::string namespc; std::string name; Class* klass = nullptr; };
    struct MethodCacheEntry { Class* klass = nullptr; std::string name; int paramCount = 0; Method* method = nullptr; };
    struct FieldCacheEntry { Class* klass = nullptr; std::string name; Field* field = nullptr; };
    struct TypeCacheEntry { Class* klass = nullptr; Type* type = nullptr; };

    std::vector<AssemblyCacheEntry> gAssemblyCache;
    std::vector<ImageCacheEntry> gImageCache;
    std::vector<ClassCacheEntry> gClassCache;
    std::vector<MethodCacheEntry> gMethodCache;
    std::vector<FieldCacheEntry> gFieldCache;
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

Class* IL2CPP::Class::Resolve(const char* Asm, const char* Namespace, const char* Klass)
{
    const std::string assemblyName = SafeString(Asm);
    const std::string namespaceName = SafeString(Namespace);
    const std::string className = SafeString(Klass);

    for (const auto& entry : gClassCache) {
        if (entry.assembly == assemblyName && entry.namespc == namespaceName && entry.name == className) return entry.klass;
    }

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

Method* IL2CPP::Class::GetMethod(const char* Name, int pCount)
{
    const std::string methodName = SafeString(Name);
    for (const auto& entry : gMethodCache) {
        if (entry.klass == this && entry.name == methodName && entry.paramCount == pCount) return entry.method;
    }

    Method* method = reinterpret_cast<Method*>(class_get_method_from_name(this, methodName.c_str(), pCount));
    if (method) gMethodCache.push_back({ this, methodName, pCount, method });
    return method;
}

Field* IL2CPP::Class::GetField(const char* Name)
{
    const std::string fieldName = SafeString(Name);
    for (const auto& entry : gFieldCache) if (entry.klass == this && entry.name == fieldName) return entry.field;

    Field* field = reinterpret_cast<Field*>(class_get_field_from_name(this, fieldName.c_str()));
    if (field) gFieldCache.push_back({ this, fieldName, field });
    return field;
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
    void* iterator = nullptr;
    Method* method = nullptr;
    while ((method = reinterpret_cast<Method*>(class_get_methods(this, &iterator))) != nullptr) std::cout << method->GetName() << "\n";
}

const char* IL2CPP::Class::GetName()
{
    return class_get_name(this);
}

const char* IL2CPP::Field::GetName()
{
    return field_get_name(this);
}

void* IL2CPP::Method::GetAddress()
{
    return this->MethodAddress;
}

const char* IL2CPP::Method::GetName()
{
    return method_get_name(this);
}

void IL2CPP::Method::Hook(void* vHk, void* vOrg)
{
    MH_CreateHook(this->GetAddress(), vHk, reinterpret_cast<void**>(vOrg));
    MH_EnableHook(this->GetAddress());
}

Class* IL2CPP::Object::GetClass()
{
    return reinterpret_cast<Class*>(object_get_class(this));
}

Field* IL2CPP::Object::GetField(const char* Name)
{
    Class* klass = this->GetClass();
    return klass ? klass->GetField(Name) : nullptr;
}

Type* IL2CPP::Type::Resolve(const char* Asm, const char* Namespace, const char* Klass)
{
    Class* klass = Class::Resolve(Asm, Namespace, Klass);
    return klass ? klass->GetType() : nullptr;
}

Class* IL2CPP::Type::GetClass()
{
    Object* typeObject = this->GetTypeObject();
    return typeObject ? typeObject->GetClass() : nullptr;
}

Object* IL2CPP::Type::GetTypeObject()
{
    return reinterpret_cast<Object*>(type_get_object(this));
}

String* IL2CPP::String::New(const char* Text)
{
    return reinterpret_cast<String*>(string_new(Text));
}

size_t IL2CPP::String::GetLength()
{
    return string_length(this);
}

const wchar_t* IL2CPP::String::GetChars()
{
    return string_chars(this);
}

std::string IL2CPP::String::GetString()
{
    std::wstring wstr(this->GetChars());
    return std::string(wstr.begin(), wstr.end());
}
