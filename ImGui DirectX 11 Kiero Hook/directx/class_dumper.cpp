#include "class_dumper.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace {
	constexpr uint32_t METHOD_ATTRIBUTE_STATIC_LOCAL = 0x0010;
	constexpr uint32_t FIELD_ATTRIBUTE_STATIC_LOCAL = 0x0010;
	constexpr uint32_t FIELD_ATTRIBUTE_LITERAL_LOCAL = 0x0040;
	std::string gStatus;
	bool gLastSucceeded = false;

	struct TypeInfo {
		std::string cpp;
		bool supported = true;
		std::string reason;
	};

	void SetStatus(bool success, const std::string& status) { gLastSucceeded = success; gStatus = status; }

	std::string StripAssemblyExtension(std::string name) {
		if (name.size() >= 4) {
			std::string suffix = name.substr(name.size() - 4);
			std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](char c) { return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
			if (suffix == ".dll" || suffix == ".exe") name.resize(name.size() - 4);
		}
		return name;
	}

	bool IsCppKeyword(const std::string& value) {
		static const std::set<std::string> keywords = {
			"alignas","alignof","and","and_eq","asm","auto","bitand","bitor","bool","break","case","catch","char","char8_t","char16_t","char32_t","class","compl","concept","const","consteval","constexpr","constinit","const_cast","continue","co_await","co_return","co_yield","decltype","default","delete","do","double","dynamic_cast","else","enum","explicit","export","extern","false","float","for","friend","goto","if","inline","int","long","mutable","namespace","new","noexcept","not","not_eq","nullptr","operator","or","or_eq","private","protected","public","register","reinterpret_cast","requires","return","short","signed","sizeof","static","static_assert","static_cast","struct","switch","template","this","thread_local","throw","true","try","typedef","typeid","typename","union","unsigned","using","virtual","void","volatile","wchar_t","while","xor","xor_eq"
		};
		return keywords.find(value) != keywords.end();
	}

	std::string SanitizeIdentifier(const std::string& raw, const char* fallback) {
		std::string value;
		value.reserve(raw.size() + 4);
		for (char c : raw) value += (std::isalnum(static_cast<unsigned char>(c)) || c == '_') ? c : '_';
		while (!value.empty() && value.back() == '_') value.pop_back();
		if (value.empty()) value = fallback;
		if (std::isdigit(static_cast<unsigned char>(value.front()))) value.insert(value.begin(), '_');
		if (IsCppKeyword(value)) value.insert(value.begin(), '_');
		return value;
	}

	std::string RemoveGenericArity(std::string name) {
		size_t tick = name.find('`');
		if (tick != std::string::npos) name.resize(tick);
		return name;
	}

	std::string ClassCppName(Class* klass) {
		if (!klass) return "Object";
		const char* rawName = klass->GetName();
		const char* rawNamespace = klass->GetNamespace();
		std::string name = rawName && *rawName ? rawName : "Object";
		std::string namespc = rawNamespace ? rawNamespace : "";
		if (namespc == "UnityEngine" && name == "Object") return "UObject";
		if (namespc == "System" && name == "String") return "String";
		if (namespc == "System" && name == "Type") return "Type";
		if (namespc == "System" && name == "Object") return "Object";
		if (namespc == "System" && name == "Boolean") return "bool";
		if (namespc == "System" && name == "Char") return "uint16_t";
		if (namespc == "System" && name == "SByte") return "int8_t";
		if (namespc == "System" && name == "Byte") return "uint8_t";
		if (namespc == "System" && name == "Int16") return "int16_t";
		if (namespc == "System" && name == "UInt16") return "uint16_t";
		if (namespc == "System" && name == "Int32") return "int";
		if (namespc == "System" && name == "UInt32") return "uint32_t";
		if (namespc == "System" && name == "Int64") return "int64_t";
		if (namespc == "System" && name == "UInt64") return "uint64_t";
		if (namespc == "System" && name == "Single") return "float";
		if (namespc == "System" && name == "Double") return "double";
		if (namespc == "System" && name == "IntPtr") return "intptr_t";
		if (namespc == "System" && name == "UIntPtr") return "uintptr_t";
		return SanitizeIdentifier(RemoveGenericArity(name), "Object");
	}

	std::string EscapeCppString(const std::string& value) {
		std::string out;
		out.reserve(value.size() + 8);
		for (char c : value) { if (c == '\\' || c == '"') out += '\\'; out += c; }
		return out;
	}

	void AddEnumDependency(std::vector<Class*>* enums, Class* klass) {
		if (!enums || !klass || !klass->IsEnum()) return;
		for (Class* current : *enums) if (current == klass) return;
		enums->push_back(klass);
	}

	TypeInfo TypeToCpp(Type* type, std::vector<Class*>* enums = nullptr) {
		if (!type) return { "void*", false, "type metadata unavailable" };
		Il2CppTypeEnum code = type->GetTypeEnum();
		switch (code) {
		case IL2CPP_API::IL2CPP_TYPE_VOID: return { "void", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_BOOLEAN: return { "bool", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_CHAR: return { "uint16_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_I1: return { "int8_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_U1: return { "uint8_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_I2: return { "int16_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_U2: return { "uint16_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_I4: return { "int", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_U4: return { "uint32_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_I8: return { "int64_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_U8: return { "uint64_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_R4: return { "float", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_R8: return { "double", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_STRING: return { "String*", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_I: return { "intptr_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_U: return { "uintptr_t", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_PTR: return { "void*", true, {} };
		case IL2CPP_API::IL2CPP_TYPE_BYREF: return { "void*", false, "by-ref parameter" };
		case IL2CPP_API::IL2CPP_TYPE_VAR:
		case IL2CPP_API::IL2CPP_TYPE_MVAR: return { "void*", false, "generic type parameter" };
		case IL2CPP_API::IL2CPP_TYPE_ARRAY:
		case IL2CPP_API::IL2CPP_TYPE_SZARRAY: return { "Array<Object*>*", false, "array signature requires element metadata support" };
		case IL2CPP_API::IL2CPP_TYPE_GENERICINST: return { "void*", false, "generic instance" };
		default: break;
		}

		Class* klass = type->GetClass();
		if (klass && klass->IsEnum()) { AddEnumDependency(enums, klass); return { ClassCppName(klass), true, {} }; }
		if (code == IL2CPP_API::IL2CPP_TYPE_CLASS || code == IL2CPP_API::IL2CPP_TYPE_OBJECT) return klass ? TypeInfo{ ClassCppName(klass) + "*", true, {} } : TypeInfo{ "Object*", false, "class metadata unavailable" };
		if (code == IL2CPP_API::IL2CPP_TYPE_VALUETYPE) return klass ? TypeInfo{ ClassCppName(klass), true, {} } : TypeInfo{ "void*", false, "value-type metadata unavailable" };
		return { "void*", false, "unsupported IL2CPP type" };
	}

	std::string MethodWrapperName(const std::string& methodName) {
		if (methodName == ".ctor") return "ctor";
		if (methodName == ".cctor") return "cctor";
		return SanitizeIdentifier(methodName, "Method");
	}

	bool IsUnityComponent(Class* klass) {
		if (!klass) return false;
		const char* name = klass->GetName();
		const char* namespc = klass->GetNamespace();
		return name && namespc && std::string(name) == "Component" && std::string(namespc) == "UnityEngine";
	}

	bool InheritsComponent(Class* klass) {
		if (!klass) return false;
		Class* parent = klass->GetParent();
		for (int depth = 0; parent && depth < 128; depth++, parent = parent->GetParent()) if (IsUnityComponent(parent)) return true;
		return false;
	}

	std::string ResolveDumpBase(Class* klass) {
		if (!klass || klass->IsValueType()) return {};
		if (InheritsComponent(klass)) return "Component";
		Class* parent = klass->GetParent();
		return parent ? ClassCppName(parent) : "";
	}

	std::string HexOffset(size_t offset) {
		std::ostringstream stream;
		stream << "0x" << std::uppercase << std::hex << offset;
		return stream.str();
	}

	std::string EnumValueToString(Type* baseType, const void* data) {
		if (!baseType || !data) return "0";
		switch (baseType->GetTypeEnum()) {
		case IL2CPP_API::IL2CPP_TYPE_I1: return std::to_string(static_cast<int>(*reinterpret_cast<const int8_t*>(data)));
		case IL2CPP_API::IL2CPP_TYPE_U1: return std::to_string(static_cast<unsigned int>(*reinterpret_cast<const uint8_t*>(data)));
		case IL2CPP_API::IL2CPP_TYPE_I2: return std::to_string(*reinterpret_cast<const int16_t*>(data));
		case IL2CPP_API::IL2CPP_TYPE_U2: return std::to_string(*reinterpret_cast<const uint16_t*>(data));
		case IL2CPP_API::IL2CPP_TYPE_I4: return std::to_string(*reinterpret_cast<const int32_t*>(data));
		case IL2CPP_API::IL2CPP_TYPE_U4: return std::to_string(*reinterpret_cast<const uint32_t*>(data));
		case IL2CPP_API::IL2CPP_TYPE_I8: return std::to_string(*reinterpret_cast<const int64_t*>(data));
		case IL2CPP_API::IL2CPP_TYPE_U8: return std::to_string(*reinterpret_cast<const uint64_t*>(data));
		default: return "0";
		}
	}

	std::string ReadEnumFieldValue(Field* field, Type* baseType) {
		if (!field || !baseType) return "0";
		Object* boxed = field->GetValueObject(nullptr);
		if (boxed && object_unbox) {
			void* data = object_unbox(boxed);
			if (data) return EnumValueToString(baseType, data);
		}
		alignas(8) unsigned char raw[8]{};
		if (field_static_get_value) field_static_get_value(field, raw);
		return EnumValueToString(baseType, raw);
	}

	std::string BuildEnumDefinition(Class* enumClass) {
		if (!enumClass || !enumClass->IsEnum()) return {};
		Type* baseType = class_enum_basetype ? reinterpret_cast<Type*>(class_enum_basetype(enumClass)) : nullptr;
		TypeInfo baseInfo = TypeToCpp(baseType, nullptr);
		if (!baseInfo.supported || baseInfo.cpp.empty()) baseInfo.cpp = "int";
		std::string out = "enum class " + ClassCppName(enumClass) + " : " + baseInfo.cpp + " {\n";
		bool wroteAny = false;
		for (Field* field : enumClass->GetFieldList()) {
			if (!field) continue;
			const char* rawName = field->GetName();
			if (!rawName || std::string(rawName) == "value__") continue;
			uint32_t flags = field->GetFlags();
			if ((flags & FIELD_ATTRIBUTE_STATIC_LOCAL) == 0 || (flags & FIELD_ATTRIBUTE_LITERAL_LOCAL) == 0) continue;
			out += "    " + SanitizeIdentifier(rawName, "Value") + " = " + ReadEnumFieldValue(field, baseType) + ",\n";
			wroteAny = true;
		}
		if (!wroteAny) out += "    // No enum literals were available.\n";
		out += "};\n\n";
		return out;
	}

	std::string BuildHeader(Class* klass) {
		if (!klass) return {};
		if (klass->IsEnum()) return "#pragma once\n\n" + BuildEnumDefinition(klass);

		const char* rawClassName = klass->GetName();
		const char* rawNamespace = klass->GetNamespace();
		Il2CppImage* image = klass->GetImage();
		const char* rawAssembly = image && image_get_name ? image_get_name(image) : nullptr;
		std::string className = rawClassName && *rawClassName ? rawClassName : "UnknownClass";
		std::string namespc = rawNamespace ? rawNamespace : "";
		std::string assembly = StripAssemblyExtension(rawAssembly ? rawAssembly : "");
		std::string cppClass = ClassCppName(klass);
		bool valueType = klass->IsValueType();
		std::string baseCpp = ResolveDumpBase(klass);
		std::vector<Class*> enumDependencies;

		struct MethodMeta {
			Method* method = nullptr;
			std::string name;
			std::string wrapperName;
			TypeInfo returnType;
			std::vector<TypeInfo> params;
			std::vector<std::string> exactParamTypes;
			std::vector<std::string> paramNames;
			uint32_t paramCount = 0;
			bool isStatic = false;
			bool useExactResolver = false;
			bool supported = true;
			std::string reason;
		};

		struct FieldMeta {
			Field* field = nullptr;
			std::string name;
			TypeInfo type;
			size_t offset = 0;
			uint32_t flags = 0;
			bool isStatic = false;
			bool supported = true;
			std::string reason;
		};

		std::vector<MethodMeta> methods;
		for (Method* method : klass->GetMethodList()) {
			if (!method) continue;
			MethodMeta meta;
			meta.method = method;
			const char* rawName = method->GetName();
			meta.name = rawName ? rawName : "UnknownMethod";
			meta.wrapperName = MethodWrapperName(meta.name);
			meta.paramCount = method->GetParamCount();
			meta.isStatic = (method->GetFlags() & METHOD_ATTRIBUTE_STATIC_LOCAL) != 0;
			meta.returnType = TypeToCpp(method->GetReturnType(), &enumDependencies);
			if (!meta.returnType.supported) { meta.supported = false; meta.reason = "return: " + meta.returnType.reason; }
			if (method->IsGeneric()) { meta.supported = false; meta.reason = "generic method"; }
			for (uint32_t i = 0; i < meta.paramCount; i++) {
				Type* rawParamType = method->GetParam(i);
				TypeInfo param = TypeToCpp(rawParamType, &enumDependencies);
				if (!param.supported && meta.supported) { meta.supported = false; meta.reason = "parameter " + std::to_string(i + 1) + ": " + param.reason; }
				meta.params.push_back(param);
				meta.exactParamTypes.push_back(rawParamType ? rawParamType->GetName() : "");
				const char* rawParamName = method->GetParamName(i);
				std::string fallbackName = "arg" + std::to_string(i);
				meta.paramNames.push_back(SanitizeIdentifier(rawParamName && *rawParamName ? std::string(rawParamName) : fallbackName, fallbackName.c_str()));
			}
			methods.push_back(meta);
		}

		for (size_t i = 0; i < methods.size(); i++) {
			for (size_t j = i + 1; j < methods.size(); j++) {
				if (methods[i].name == methods[j].name && methods[i].paramCount == methods[j].paramCount) {
					methods[i].useExactResolver = true;
					methods[j].useExactResolver = true;
				}
			}
		}

		std::vector<FieldMeta> fields;
		for (Field* field : klass->GetFieldList()) {
			if (!field) continue;
			FieldMeta meta;
			meta.field = field;
			const char* rawName = field->GetName();
			meta.name = SanitizeIdentifier(rawName && *rawName ? rawName : "field", "field");
			meta.type = TypeToCpp(field->GetType(), &enumDependencies);
			meta.offset = field->GetOffset();
			meta.flags = field->GetFlags();
			meta.isStatic = (meta.flags & FIELD_ATTRIBUTE_STATIC_LOCAL) != 0;
			meta.supported = meta.type.supported;
			if (!meta.supported) meta.reason = meta.type.reason;
			fields.push_back(meta);
		}
		std::stable_sort(fields.begin(), fields.end(), [](const FieldMeta& a, const FieldMeta& b) {
			if (a.isStatic != b.isStatic) return !a.isStatic;
			return a.offset < b.offset;
		});

		std::string out = "#pragma once\n\n";
		for (Class* enumClass : enumDependencies) if (enumClass && enumClass != klass) out += BuildEnumDefinition(enumClass);
		out += "// IL2CPP class: ";
		if (!namespc.empty()) out += namespc + ".";
		out += className + "\n// Assembly: " + assembly + "\n// Methods and fields declared directly on this class only.\n\n";
		if (valueType) out += "struct " + cppClass + " {\n";
		else {
			out += "class " + cppClass;
			if (!baseCpp.empty() && baseCpp != cppClass) out += " : public " + baseCpp;
			out += " {\npublic:\n";
		}

		for (const MethodMeta& meta : methods) {
			std::string declaration;
			if (meta.isStatic) declaration += "static ";
			declaration += meta.returnType.cpp.empty() ? "void*" : meta.returnType.cpp;
			declaration += " " + meta.wrapperName + "(";
			for (size_t i = 0; i < meta.params.size(); i++) {
				if (i) declaration += ", ";
				declaration += meta.params[i].cpp + " " + meta.paramNames[i];
			}
			declaration += ")";
			if (!meta.supported) { out += "    // Unsupported: " + declaration + " // " + meta.reason + "\n"; continue; }
			if (meta.wrapperName != meta.name) out += "    // IL2CPP method: " + meta.name + "\n";
			out += "    " + declaration + " {\n        ";
			if (meta.returnType.cpp != "void") out += "return ";
			out += meta.useExactResolver ? "Method::CallExact<" : "Method::Call<";
			out += meta.returnType.cpp + "(*)(";
			bool wrote = false;
			if (!meta.isStatic) { out += cppClass + "*"; wrote = true; }
			for (const TypeInfo& param : meta.params) { if (wrote) out += ", "; out += param.cpp; wrote = true; }
			out += ")>(\"" + EscapeCppString(assembly) + "\", \"" + EscapeCppString(namespc) + "\", \"" + EscapeCppString(className) + "\", \"" + EscapeCppString(meta.name) + "\", ";
			if (meta.useExactResolver) {
				out += "{";
				for (size_t i = 0; i < meta.exactParamTypes.size(); i++) { if (i) out += ", "; out += "\"" + EscapeCppString(meta.exactParamTypes[i]) + "\""; }
				out += "}";
			} else out += std::to_string(meta.paramCount);
			out += ")(";
			wrote = false;
			if (!meta.isStatic) { out += "this"; wrote = true; }
			for (const std::string& paramName : meta.paramNames) { if (wrote) out += ", "; out += paramName; wrote = true; }
			out += ");\n    }\n\n";
		}

		if (!fields.empty()) {
			out += "    // Fields\n";
			for (const FieldMeta& field : fields) {
				if (!field.supported) {
					out += "    // Unsupported field " + field.name;
					out += field.isStatic ? " // Static: " : " // " + HexOffset(field.offset) + ": ";
					out += field.reason + "\n";
					continue;
				}
				out += "    ";
				if (field.isStatic) out += "static ";
				out += field.type.cpp + " " + field.name + ";";
				out += field.isStatic ? " // Static\n" : " // " + HexOffset(field.offset) + "\n";
			}
		}
		out += "};\n";
		return out;
	}

	std::wstring Utf8ToWide(const std::string& value) {
		if (value.empty()) return {};
		int size = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
		if (size <= 1) return {};
		std::wstring out(static_cast<size_t>(size), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, out.data(), size);
		if (!out.empty() && out.back() == L'\0') out.pop_back();
		return out;
	}

	bool WriteUtf8File(const std::wstring& path, const std::string& content) {
		FILE* file = nullptr;
		if (_wfopen_s(&file, path.c_str(), L"wb") != 0 || !file) return false;
		size_t written = fwrite(content.data(), 1, content.size(), file);
		fclose(file);
		return written == content.size();
	}
}

bool ClassDumper::Dump(Class* klass)
{
	if (!klass) { SetStatus(false, "No class is selected."); return false; }
	const char* rawName = klass->GetName();
	if (!rawName || !*rawName) { SetStatus(false, "Selected class has no name."); return false; }
	std::string safeName = SanitizeIdentifier(RemoveGenericArity(rawName), "Class");
	std::string header = BuildHeader(klass);
	if (header.empty()) { SetStatus(false, "Failed to build class header."); return false; }
	wchar_t directory[MAX_PATH]{};
	DWORD length = GetCurrentDirectoryW(MAX_PATH, directory);
	if (!length || length >= MAX_PATH) { SetStatus(false, "Failed to resolve current directory."); return false; }
	std::wstring path = directory;
	if (!path.empty() && path.back() != L'\\') path += L'\\';
	path += Utf8ToWide(safeName) + L".h";
	if (!WriteUtf8File(path, header)) { SetStatus(false, "Failed to write " + safeName + ".h"); return false; }
	SetStatus(true, "Dumped " + safeName + ".h");
	return true;
}

const char* ClassDumper::GetStatus() { return gStatus.c_str(); }
bool ClassDumper::LastSucceeded() { return gLastSucceeded; }
