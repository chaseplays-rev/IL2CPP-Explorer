#include "il2cpp.h"

Assembly* IL2CPP_API::Assembly::Resolve(const char* Asm)
{
    return (Assembly*)domain_assembly_open(Domain, Asm);
}

Il2CppImage* IL2CPP_API::Assembly::GetImage()
{
    return assembly_get_image(this);
}

Class* IL2CPP_API::Class::Resolve(const char* Assembly, const char* Namespace, const char* Klass)
{
    return (Class*)class_from_name(Assembly::Resolve(Assembly)->GetImage(), Namespace, Klass);
}

Method* IL2CPP_API::Method::Resolve(const char* Asm, const char* Namespace, const char* Klass, const char* Name, int pCount)
{
    return Class::Resolve(Asm, Namespace, Klass)->GetMethod(Name, pCount);
}

Method* IL2CPP_API::Class::GetMethod(const char* Name, int pCount)
{
    return (Method*)class_get_method_from_name(this, Name, pCount);
}

Field* IL2CPP_API::Class::GetField(const char* Name)
{
    return (Field*)class_get_field_from_name(this, Name);
}

Type* IL2CPP_API::Class::GetType()
{
    return (Type*)class_get_type(this);
}

void IL2CPP_API::Class::GetMethods()
{
    void* iterator = NULL;
    Method* method = NULL;

    while ((method = (Method*)class_get_methods(this, &iterator)) != NULL) {
        std::cout << method->GetName() << "\n";
    }
}

const char* IL2CPP_API::Class::GetName()
{
    return class_get_name(this);
}

const char* IL2CPP_API::Field::GetName() {
    return field_get_name(this);
}

void* IL2CPP_API::Method::GetAddress()
{
    return this->MethodAddress;
}

const char* IL2CPP_API::Method::GetName()
{
    return method_get_name(this);
}

void IL2CPP_API::Method::Hook(void* vHk, void* vOrg)
{
    MH_CreateHook(this->GetAddress(), vHk, (void**)vOrg);
    MH_EnableHook(this->GetAddress());
}

Class* IL2CPP_API::Object::GetClass()
{
    return (Class*)object_get_class(this);
}

Field* IL2CPP_API::Object::GetField(const char* Name)
{
    return this->GetClass()->GetField(Name);
}

Type* IL2CPP_API::Type::Resolve(const char* Asm, const char* Namespace, const char* Klass)
{
    return Class::Resolve(Asm, Namespace, Klass)->GetType();
}

Class* IL2CPP_API::Type::GetClass()
{
    return this->GetTypeObject()->GetClass();
}

Object* IL2CPP_API::Type::GetTypeObject()
{
    return (Object*)type_get_object(this);
}

String* IL2CPP_API::String::New(const char* Text)
{
    return (String*)string_new(Text);
}

size_t IL2CPP_API::String::GetLength()
{
    return string_length(this);
}

const wchar_t* IL2CPP_API::String::GetChars()
{
    return string_chars(this);
}

std::string IL2CPP_API::String::GetString()
{
    std::wstring wstr(this->GetChars());
    return std::string(wstr.begin(), wstr.end());
}
