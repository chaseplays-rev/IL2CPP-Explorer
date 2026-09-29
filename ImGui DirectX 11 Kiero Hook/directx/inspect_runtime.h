#pragma once
#include "../includes.h"
#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <climits>
#include <string>
#include <vector>

namespace InspectRuntime {
	inline Object* GetHandleObject(Il2CppGCHandle handle) {
		return handle && gchandle_get_target ? reinterpret_cast<Object*>(gchandle_get_target(handle)) : nullptr;
	}

	inline void ClearArgumentHandles() {
		if (gchandle_free) for (Il2CppGCHandle handle : Globals::methodArgumentHandles) if (handle) gchandle_free(handle);
		Globals::methodArgumentHandles.clear();
	}

	inline void ClearMethodContext() {
		ClearArgumentHandles();
		if (Globals::methodContextHandle && gchandle_free) gchandle_free(Globals::methodContextHandle);
		Globals::methodContextHandle = nullptr;
		Globals::methodContextTabId = 0;
		Globals::methodContextClass = nullptr;
		Globals::inspectedMethod = nullptr;
		Globals::methodInspectorOpen = false;
		Globals::methodArguments[0] = '\0';
		Globals::methodCallResult.clear();
	}

	inline void FreeTab(Globals::InspectTab& tab) {
		if (tab.objectHandle && gchandle_free) gchandle_free(tab.objectHandle);
		tab.objectHandle = nullptr;
		tab.klass = nullptr;
	}

	inline void Reset() {
		ClearMethodContext();
		for (Globals::InspectTab& tab : Globals::inspectTabs) FreeTab(tab);
		Globals::inspectTabs.clear();
		Globals::activeInspectTab = -1;
		Globals::nextInspectTabId = 1;
		Globals::selectedObject = nullptr;
	}

	inline Object* GetRootObject() {
		if (Globals::inspectTabs.empty()) return nullptr;
		return GetHandleObject(Globals::inspectTabs[0].objectHandle);
	}

	inline Class* GetActiveClass() {
		if (Globals::activeInspectTab < 0 || Globals::activeInspectTab >= static_cast<int>(Globals::inspectTabs.size())) return nullptr;
		return Globals::inspectTabs[Globals::activeInspectTab].klass;
	}

	inline Object* GetActiveObject() {
		if (Globals::activeInspectTab < 0 || Globals::activeInspectTab >= static_cast<int>(Globals::inspectTabs.size())) return nullptr;
		return GetHandleObject(Globals::inspectTabs[Globals::activeInspectTab].objectHandle);
	}

	inline uint64_t GetActiveTabId() {
		if (Globals::activeInspectTab < 0 || Globals::activeInspectTab >= static_cast<int>(Globals::inspectTabs.size())) return 0;
		return Globals::inspectTabs[Globals::activeInspectTab].id;
	}

	inline bool OpenClass(Class* klass, Object* object) {
		if (!klass || !object || !gchandle_new || !gchandle_get_target || !gchandle_free) return false;
		for (size_t i = 0; i < Globals::inspectTabs.size(); i++) {
			if (Globals::inspectTabs[i].klass == klass && GetHandleObject(Globals::inspectTabs[i].objectHandle) == object) {
				Globals::activeInspectTab = static_cast<int>(i);
				return true;
			}
		}
		Il2CppGCHandle handle = gchandle_new(reinterpret_cast<Il2CppObject*>(object), false);
		if (!handle) return false;
		Globals::InspectTab tab;
		tab.klass = klass;
		tab.objectHandle = handle;
		tab.id = Globals::nextInspectTabId++;
		Globals::inspectTabs.push_back(tab);
		Globals::activeInspectTab = static_cast<int>(Globals::inspectTabs.size()) - 1;
		return true;
	}

	inline bool OpenObject(Object* object) {
		if (!object) return false;
		Class* klass = object->GetClass();
		return klass ? OpenClass(klass, object) : false;
	}

	inline bool SyncRoot(Object* object) {
		if (!object) return false;
		Object* currentRoot = GetRootObject();
		if (currentRoot == object && !Globals::inspectTabs.empty()) {
			Globals::activeInspectTab = 0;
			Globals::selectedObject = object;
			return true;
		}
		Reset();
		if (!OpenObject(object)) return false;
		Globals::selectedObject = object;
		return true;
	}

	inline void CloseTab(size_t index) {
		if (index == 0 || index >= Globals::inspectTabs.size()) return;
		uint64_t closingId = Globals::inspectTabs[index].id;
		bool closingActive = static_cast<int>(index) == Globals::activeInspectTab;
		if (Globals::methodContextTabId == closingId) ClearMethodContext();
		FreeTab(Globals::inspectTabs[index]);
		Globals::inspectTabs.erase(Globals::inspectTabs.begin() + index);
		if (Globals::inspectTabs.empty()) Globals::activeInspectTab = -1;
		else if (closingActive) Globals::activeInspectTab = static_cast<int>(index < Globals::inspectTabs.size() ? index : Globals::inspectTabs.size() - 1);
		else if (static_cast<int>(index) < Globals::activeInspectTab) Globals::activeInspectTab--;
	}

	inline void SetMethodContext(Method* method) {
		ClearMethodContext();
		if (!method) return;
		Globals::inspectedMethod = method;
		Globals::methodInspectorOpen = true;
		Globals::methodContextTabId = GetActiveTabId();
		Globals::methodContextClass = GetActiveClass();
		Object* object = GetActiveObject();
		if (object && gchandle_new) Globals::methodContextHandle = gchandle_new(reinterpret_cast<Il2CppObject*>(object), false);
	}

	inline Object* GetSavedMethodObject() {
		return GetHandleObject(Globals::methodContextHandle);
	}

	inline Object* GetMethodContext(Method* method) {
		if (!method || method != Globals::inspectedMethod || method->IsStatic()) return nullptr;
		return GetSavedMethodObject();
	}

	inline std::string Trim(const std::string& value) {
		size_t first = 0;
		while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) first++;
		size_t last = value.size();
		while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) last--;
		return value.substr(first, last - first);
	}

	inline bool SplitArguments(const char* text, uint32_t expected, std::vector<std::string>& output, std::string& error) {
		output.clear();
		std::string source = text ? text : "";
		if (expected == 0) {
			if (!Trim(source).empty()) { error = "This method takes no parameters."; return false; }
			return true;
		}
		bool quoted = false;
		bool escaped = false;
		size_t start = 0;
		for (size_t i = 0; i <= source.size(); i++) {
			char ch = i < source.size() ? source[i] : ',';
			if (quoted) {
				if (escaped) escaped = false;
				else if (ch == '\\') escaped = true;
				else if (ch == '"') quoted = false;
				continue;
			}
			if (ch == '"') { quoted = true; continue; }
			if (ch == ',') {
				output.push_back(Trim(source.substr(start, i - start)));
				start = i + 1;
			}
		}
		if (quoted) { error = "Argument string has an unterminated quote."; return false; }
		if (output.size() != expected) { error = "Expected " + std::to_string(expected) + " argument(s), received " + std::to_string(output.size()) + "."; return false; }
		return true;
	}

	inline bool ParseString(const std::string& token, std::string& value) {
		std::string text = Trim(token);
		if (text.size() < 2 || text.front() != '"' || text.back() != '"') return false;
		value.clear();
		for (size_t i = 1; i + 1 < text.size(); i++) {
			char ch = text[i];
			if (ch == '\\' && i + 1 < text.size() - 1) {
				char next = text[++i];
				switch (next) { case 'n': value += '\n'; break; case 'r': value += '\r'; break; case 't': value += '\t'; break; case '\\': value += '\\'; break; case '"': value += '"'; break; default: value += next; break; }
			} else value += ch;
		}
		return true;
	}

	inline bool ParseSigned(const std::string& token, long long minimum, long long maximum, long long& value) {
		std::string text = Trim(token);
		if (text.empty()) return false;
		errno = 0;
		char* end = nullptr;
		long long parsed = std::strtoll(text.c_str(), &end, 0);
		if (errno == ERANGE || !end || *end != '\0' || parsed < minimum || parsed > maximum) return false;
		value = parsed;
		return true;
	}

	inline bool ParseUnsigned(const std::string& token, unsigned long long maximum, unsigned long long& value) {
		std::string text = Trim(token);
		if (text.empty() || text[0] == '-') return false;
		errno = 0;
		char* end = nullptr;
		unsigned long long parsed = std::strtoull(text.c_str(), &end, 0);
		if (errno == ERANGE || !end || *end != '\0' || parsed > maximum) return false;
		value = parsed;
		return true;
	}

	inline Object* RootArgument(Object* object) {
		if (!object) return nullptr;
		if (!gchandle_new) return object;
		Il2CppGCHandle handle = gchandle_new(reinterpret_cast<Il2CppObject*>(object), false);
		if (!handle) return nullptr;
		Globals::methodArgumentHandles.push_back(handle);
		return GetHandleObject(handle);
	}

	template <typename T>
	inline Object* BoxValue(Class* klass, T value) {
		if (!klass || !value_box) return nullptr;
		return RootArgument(reinterpret_cast<Object*>(value_box(klass, &value)));
	}

	inline bool BuildBoxedArgument(Type* type, const std::string& token, Object*& value, std::string& error) {
		value = nullptr;
		if (!type) { error = "Unknown parameter type."; return false; }
		if (type->IsByRef()) { error = "By-ref parameters are not supported yet."; return false; }
		Class* klass = type->GetClass();
		Il2CppTypeEnum typeEnum = type->GetTypeEnum();
		std::string text = Trim(token);
		if (klass && klass->IsEnum() && class_enum_basetype) {
			Type* baseType = reinterpret_cast<Type*>(class_enum_basetype(klass));
			if (baseType) typeEnum = baseType->GetTypeEnum();
		}
		long long signedValue = 0;
		unsigned long long unsignedValue = 0;
		switch (typeEnum) {
		case IL2CPP_TYPE_BOOLEAN: {
			std::string lower = text;
			std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			if (lower != "true" && lower != "false" && lower != "1" && lower != "0") { error = "Boolean expects true, false, 1, or 0."; return false; }
			uint8_t v = lower == "true" || lower == "1" ? 1 : 0; value = BoxValue(klass, v); break;
		}
		case IL2CPP_TYPE_CHAR: {
			uint16_t v = 0;
			if (text.size() == 3 && text.front() == '\'' && text.back() == '\'') v = static_cast<uint8_t>(text[1]);
			else { if (!ParseUnsigned(text, UINT16_MAX, unsignedValue)) { error = "Char expects 'A' or a numeric value."; return false; } v = static_cast<uint16_t>(unsignedValue); }
			value = BoxValue(klass, v); break;
		}
		case IL2CPP_TYPE_I1: if (!ParseSigned(text, INT8_MIN, INT8_MAX, signedValue)) { error = "SByte is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<int8_t>(signedValue)); break;
		case IL2CPP_TYPE_U1: if (!ParseUnsigned(text, UINT8_MAX, unsignedValue)) { error = "Byte is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<uint8_t>(unsignedValue)); break;
		case IL2CPP_TYPE_I2: if (!ParseSigned(text, INT16_MIN, INT16_MAX, signedValue)) { error = "Int16 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<int16_t>(signedValue)); break;
		case IL2CPP_TYPE_U2: if (!ParseUnsigned(text, UINT16_MAX, unsignedValue)) { error = "UInt16 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<uint16_t>(unsignedValue)); break;
		case IL2CPP_TYPE_I4: if (!ParseSigned(text, INT32_MIN, INT32_MAX, signedValue)) { error = "Int32 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<int32_t>(signedValue)); break;
		case IL2CPP_TYPE_U4: if (!ParseUnsigned(text, UINT32_MAX, unsignedValue)) { error = "UInt32 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<uint32_t>(unsignedValue)); break;
		case IL2CPP_TYPE_I8: if (!ParseSigned(text, LLONG_MIN, LLONG_MAX, signedValue)) { error = "Int64 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<int64_t>(signedValue)); break;
		case IL2CPP_TYPE_U8: if (!ParseUnsigned(text, ULLONG_MAX, unsignedValue)) { error = "UInt64 is invalid or out of range."; return false; } value = BoxValue(klass, static_cast<uint64_t>(unsignedValue)); break;
		case IL2CPP_TYPE_I: if (!ParseSigned(text, LLONG_MIN, LLONG_MAX, signedValue)) { error = "IntPtr is invalid."; return false; } value = BoxValue(klass, static_cast<intptr_t>(signedValue)); break;
		case IL2CPP_TYPE_U: if (!ParseUnsigned(text, ULLONG_MAX, unsignedValue)) { error = "UIntPtr is invalid."; return false; } value = BoxValue(klass, static_cast<uintptr_t>(unsignedValue)); break;
		case IL2CPP_TYPE_R4: {
			errno = 0; char* end = nullptr; float v = std::strtof(text.c_str(), &end); if (errno == ERANGE || !end || *end != '\0') { error = "Single is invalid."; return false; } value = BoxValue(klass, v); break;
		}
		case IL2CPP_TYPE_R8: {
			errno = 0; char* end = nullptr; double v = std::strtod(text.c_str(), &end); if (errno == ERANGE || !end || *end != '\0') { error = "Double is invalid."; return false; } value = BoxValue(klass, v); break;
		}
		case IL2CPP_TYPE_STRING: {
			if (text == "null") { value = nullptr; return true; }
			std::string stringValue; if (!ParseString(text, stringValue)) { error = "String expects a quoted value, for example \"hello\"."; return false; }
			value = RootArgument(reinterpret_cast<Object*>(String::New(stringValue.c_str()))); break;
		}
		case IL2CPP_TYPE_CLASS:
		case IL2CPP_TYPE_OBJECT:
		case IL2CPP_TYPE_ARRAY:
		case IL2CPP_TYPE_SZARRAY:
		case IL2CPP_TYPE_GENERICINST:
			if (klass && klass->IsValueType()) { error = "Non-enum value-type parameters are not supported yet."; return false; }
			if (text == "null") { value = nullptr; return true; }
			if (text == "this") { value = GetSavedMethodObject(); return value != nullptr; }
			if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
				errno = 0; char* end = nullptr; uintptr_t address = static_cast<uintptr_t>(std::strtoull(text.c_str(), &end, 16));
				if (errno == ERANGE || !end || *end != '\0' || !address) { error = "Object pointer is invalid."; return false; }
				value = reinterpret_cast<Object*>(address); return true;
			}
			error = "Reference parameters accept null, this, or a 0x address."; return false;
		default:
			error = "This parameter type is not supported yet."; return false;
		}
		if (!value) { error = "Failed to allocate/box the argument."; return false; }
		return true;
	}

	inline std::string ExceptionText(Il2CppException* exception) {
		if (!exception) return {};
		if (!object_to_string) return "Managed exception returned.";
		Il2CppException* stringifyException = nullptr;
		Il2CppString* text = object_to_string(reinterpret_cast<Il2CppObject*>(exception), &stringifyException);
		if (!text || stringifyException) return "Managed exception returned.";
		return reinterpret_cast<String*>(text)->GetString();
	}

	inline std::string FormatReturn(Method* method, Object* result) {
		if (!method) return "Call failed.";
		Type* returnType = method->GetReturnType();
		if (!returnType) return result ? "Returned object." : "Returned: null";
		Il2CppTypeEnum typeEnum = returnType->GetTypeEnum();
		std::string typeName = returnType->GetName();
		if (typeEnum == IL2CPP_TYPE_VOID) return "Returned: void";
		if (!result) return "Returned: null";
		if (typeEnum == IL2CPP_TYPE_STRING) return "Returned: \"" + reinterpret_cast<String*>(result)->GetString() + "\"";
		void* data = nullptr;
		char buffer[256]{};
		if ((typeEnum >= IL2CPP_TYPE_BOOLEAN && typeEnum <= IL2CPP_TYPE_R8) || typeEnum == IL2CPP_TYPE_I || typeEnum == IL2CPP_TYPE_U)
			data = object_unbox ? object_unbox(reinterpret_cast<Il2CppObject*>(result)) : nullptr;
		switch (typeEnum) {
		case IL2CPP_TYPE_BOOLEAN: if (data) return *reinterpret_cast<uint8_t*>(data) ? "Returned: true" : "Returned: false"; break;
		case IL2CPP_TYPE_CHAR: if (data) { sprintf_s(buffer, "Returned: %u", static_cast<unsigned>(*reinterpret_cast<uint16_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_I1: if (data) { sprintf_s(buffer, "Returned: %d", static_cast<int>(*reinterpret_cast<int8_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_U1: if (data) { sprintf_s(buffer, "Returned: %u", static_cast<unsigned>(*reinterpret_cast<uint8_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_I2: if (data) { sprintf_s(buffer, "Returned: %d", static_cast<int>(*reinterpret_cast<int16_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_U2: if (data) { sprintf_s(buffer, "Returned: %u", static_cast<unsigned>(*reinterpret_cast<uint16_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_I4: if (data) { sprintf_s(buffer, "Returned: %d", *reinterpret_cast<int32_t*>(data)); return buffer; } break;
		case IL2CPP_TYPE_U4: if (data) { sprintf_s(buffer, "Returned: %u", *reinterpret_cast<uint32_t*>(data)); return buffer; } break;
		case IL2CPP_TYPE_I8: if (data) { sprintf_s(buffer, "Returned: %lld", static_cast<long long>(*reinterpret_cast<int64_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_U8: if (data) { sprintf_s(buffer, "Returned: %llu", static_cast<unsigned long long>(*reinterpret_cast<uint64_t*>(data))); return buffer; } break;
		case IL2CPP_TYPE_R4: if (data) { sprintf_s(buffer, "Returned: %.6f", *reinterpret_cast<float*>(data)); return buffer; } break;
		case IL2CPP_TYPE_R8: if (data) { sprintf_s(buffer, "Returned: %.6f", *reinterpret_cast<double*>(data)); return buffer; } break;
		default: break;
		}
		Class* resultClass = result->GetClass();
		const char* className = resultClass ? resultClass->GetName() : nullptr;
		sprintf_s(buffer, "Returned: %s [%p]", className ? className : (typeName.empty() ? "Object" : typeName.c_str()), result);
		return buffer;
	}

	inline bool InvokeInspectedMethod() {
		Globals::methodCallResult.clear();
		Method* method = Globals::inspectedMethod;
		if (!method) { Globals::methodCallResult = "No method selected."; return false; }
		Object* instance = method->IsStatic() ? nullptr : GetMethodContext(method);
		if (!method->IsStatic() && !instance) { Globals::methodCallResult = "The object instance for this method tab is no longer available."; return false; }
		uint32_t paramCount = method->GetParamCount();
		std::vector<std::string> tokens;
		std::string error;
		if (!SplitArguments(Globals::methodArguments, paramCount, tokens, error)) { Globals::methodCallResult = error; return false; }
		ClearArgumentHandles();
		std::vector<Il2CppObject*> arguments(paramCount, nullptr);
		for (uint32_t i = 0; i < paramCount; i++) {
			Object* boxed = nullptr;
			if (!BuildBoxedArgument(method->GetParam(i), tokens[i], boxed, error)) {
				Globals::methodCallResult = "Argument " + std::to_string(i + 1) + ": " + error;
				ClearArgumentHandles();
				return false;
			}
			arguments[i] = reinterpret_cast<Il2CppObject*>(boxed);
		}
		Il2CppException* exception = nullptr;
		Object* result = nullptr;
		if (paramCount > 0) {
			if (!runtime_invoke_convert_args) { Globals::methodCallResult = "il2cpp_runtime_invoke_convert_args is unavailable."; ClearArgumentHandles(); return false; }
			result = method->InvokeConvert(instance, arguments.data(), static_cast<int>(paramCount), &exception);
		} else result = method->Invoke(instance, nullptr, &exception);
		ClearArgumentHandles();
		if (exception) { Globals::methodCallResult = "Call failed: " + ExceptionText(exception); return false; }
		Globals::methodCallResult = FormatReturn(method, result);
		if (result) {
			Class* resultClass = result->GetClass();
			if (resultClass && !resultClass->IsValueType() && (resultClass != Globals::methodContextClass || result != instance)) {
				if (OpenClass(resultClass, result)) {
					const char* name = resultClass->GetName();
					Globals::methodCallResult += " | Opened ";
					Globals::methodCallResult += name ? name : "returned class";
					Globals::methodCallResult += " tab.";
				}
			}
		}
		return true;
	}
}
