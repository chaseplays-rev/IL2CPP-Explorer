#pragma once
#include "il2cpp_resolver.h"
#include <string>
#include <vector>
#include <initializer_list>

namespace IL2CPP {
	inline Il2CppDomain* Domain = nullptr;

	class Assembly;
	class Class;
	class Method;
	class Object;
	class Field;
	class Property;
	class Type;

	class Assembly : public Il2CppAssembly {
	public:
		static Assembly* Resolve(const char* Assembly);
		Il2CppImage* GetImage();
		const char* GetName();
	};

	class Class : public Il2CppClass {
	public:
		static Class* Resolve(const char* Asm, const char* Namespace, const char* Klass);
		Method* GetMethod(const char* Name, int pCount);
		Method* GetMethodExact(const char* Name, const std::vector<std::string>& ParamTypes);
		Field* GetField(const char* Name);
		Property* GetProperty(const char* Name);
		Type* GetType();
		void GetMethods();
		std::vector<Method*> GetMethodList();
		std::vector<Field*> GetFieldList();
		std::vector<Property*> GetPropertyList();
		std::vector<Class*> GetInterfaces();
		const char* GetName();
		const char* GetNamespace();
		Il2CppImage* GetImage();
		Class* GetParent();
		bool IsValueType();
		bool IsEnum();
	};

	class Method : public Il2CppMethod {
	private:
		void* MethodAddress;
	public:
		static Method* Resolve(const char* Asm, const char* Namespace, const char* Klass, const char* Name, int pCount);
		static Method* ResolveExact(const char* Asm, const char* Namespace, const char* Klass, const char* Name, const std::vector<std::string>& ParamTypes);
		void* GetAddress();
		const char* GetName();
		Class* GetClass();
		Type* GetReturnType();
		uint32_t GetParamCount();
		Type* GetParam(uint32_t index);
		const char* GetParamName(uint32_t index);
		uint32_t GetFlags(uint32_t* iflags = nullptr);
		bool IsStatic();
		bool IsGeneric();
		bool IsInflated();
		Object* Invoke(Object* instance, void** params, Il2CppException** exception = nullptr);
		Object* InvokeConvert(Object* instance, Il2CppObject** params, int paramCount, Il2CppException** exception = nullptr);
		void Hook(void* vHk, void* vOrg);

		template <class T>
		T InternalCall() {
			return reinterpret_cast<T>(this->GetAddress());
		}

		template <class T>
		static T Call(const char* Asm, const char* Namespace, const char* Klass, const char* Name, int pCount) {
			Method* method = Method::Resolve(Asm, Namespace, Klass, Name, pCount);
			return method ? reinterpret_cast<T>(method->GetAddress()) : nullptr;
		}

		template <class T>
		static T CallExact(const char* Asm, const char* Namespace, const char* Klass, const char* Name, std::initializer_list<const char*> ParamTypes) {
			std::vector<std::string> types;
			types.reserve(ParamTypes.size());
			for (const char* type : ParamTypes) types.emplace_back(type ? type : "");
			Method* method = Method::ResolveExact(Asm, Namespace, Klass, Name, types);
			return method ? reinterpret_cast<T>(method->GetAddress()) : nullptr;
		}
	};

	class Field : public Il2CppField {
	public:
		const char* GetName();
		Type* GetType();
		Class* GetParent();
		size_t GetOffset();
		uint32_t GetFlags();
		Object* GetValueObject(Object* obj = nullptr);

		template <class T>
		T GetValue(Object* obj) {
			T val{};
			field_get_value(obj, this, &val);
			return val;
		}

		template <class T>
		void SetValue(Object* obj, T val) {
			field_set_value(obj, this, &val);
		}

		template <class T>
		T GetStaticValue() {
			T val{};
			field_static_get_value(this, &val);
			return val;
		}

		template <class T>
		void SetStaticValue(T val) {
			field_static_set_value(this, &val);
		}
	};

	class Property : public Il2CppProperty {
	public:
		const char* GetName();
		Class* GetParent();
		Method* GetGetter();
		Method* GetSetter();
		uint32_t GetFlags();
	};

	class Object : public Il2CppObject {
	public:
		Class* GetClass();
		Field* GetField(const char* Name);

		template <class T>
		static T* Create(Class* klass) {
			T* ptr = reinterpret_cast<T*>(object_new(klass));
			if (ptr) runtime_object_init(ptr);
			return ptr;
		}
	};

	class Type : public Il2CppType {
	public:
		static Type* Resolve(const char* Asm, const char* Namespace, const char* Klass);
		Class* GetClass();
		Object* GetTypeObject();
		std::string GetName();
		Il2CppTypeEnum GetTypeEnum();
		bool IsByRef();
		uint32_t GetAttrs();
	};

	class String : public Il2CppString {
	public:
		static String* New(const char* Text);
		size_t GetLength();
		const wchar_t* GetChars();
		std::string GetString();
	};

	template <class T>
	class Array : public Object {
	public:
		static Array<T>* CreateInstance(Object* pType, int length) {
			typedef Array<T>* (__cdecl* CreateInstance)(Object*, int);
			auto ctor = reinterpret_cast<CreateInstance>(IL2CPP_MEMORY.GetModuleEntry().modBaseAddr + 0x1FBC130);
			return ctor(pType, length);
		}

		T GetValue(int index) {
			return Method::Call<T(*)(Array<T>*, int)>("mscorlib", "System", "Array", "GetValueImpl", 1)(this, index - this->GetLowerBound(0));
		}

		void SetValue(int index, T obj) {
			return Method::Call<void(*)(Array<T>*, T, int)>("mscorlib", "System", "Array", "SetValueImpl", 2)(this, obj, index);
		}

		int GetLength() {
			return Method::Call<int(*)(Array<T>*)>("mscorlib", "System", "Array", "get_Length", 0)(this);
		}

		int GetLowerBound(int bound) {
			return Method::Call<int(*)(Array<T>*, int)>("mscorlib", "System", "Array", "GetLowerBound", 1)(this, bound);
		}

		bool isEmpty() {
			return this->GetLength() == 0;
		}
	};

	inline void Attach() {
		Domain = domain_get();
		if (Domain) thread_attach(Domain);
	}

	inline void Initialize() {
		IL2CPP_MEMORY.Initialize();
		IL2CPP_RESOLVER::Initialize();
		Attach();
		MH_Initialize();
	}
}

using namespace IL2CPP;
