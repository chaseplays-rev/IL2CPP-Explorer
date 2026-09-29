#pragma once
#include "../includes.h"
#include <cstring>

namespace ObjectCache {
	struct RawArrayView {
		void* klass;
		void* monitor;
		void* bounds;
		uintptr_t maxLength;
		Object* vector[1];
	};

	inline bool IsClassOrSubclass(Class* klass, const char* namespc, const char* name) {
		for (int depth = 0; klass && depth < 64; depth++, klass = klass->GetParent()) {
			const char* currentName = klass->GetName();
			const char* currentNamespace = klass->GetNamespace();
			if (currentName && currentNamespace && strcmp(currentName, name) == 0 && strcmp(currentNamespace, namespc) == 0) return true;
		}
		return false;
	}

	inline void UpdateClassCapabilities() {
		Globals::selectedClassIsComponent = IsClassOrSubclass(Globals::selectedClass, "UnityEngine", "Component");
		Globals::selectedClassIsUnityObject = Globals::selectedClassIsComponent || IsClassOrSubclass(Globals::selectedClass, "UnityEngine", "Object");
	}

	inline void ClearSnapshot(bool clearStatus = true) {
		if (Globals::objectsHandle && gchandle_free) gchandle_free(Globals::objectsHandle);
		Globals::objectsHandle = nullptr;
		Globals::objectSnapshotCount = 0;
		if (clearStatus) Globals::objectSearchStatus.clear();
	}

	inline Array<Object*>* GetSnapshot() {
		if (!Globals::objectsHandle || !gchandle_get_target) return nullptr;
		return reinterpret_cast<Array<Object*>*>(gchandle_get_target(Globals::objectsHandle));
	}

	inline Object* GetObjectAt(int index) {
		Array<Object*>* snapshot = GetSnapshot();
		if (!snapshot || index < 0 || index >= Globals::objectSnapshotCount) return nullptr;
		RawArrayView* raw = reinterpret_cast<RawArrayView*>(snapshot);
		if (static_cast<uintptr_t>(index) >= raw->maxLength) return nullptr;
		return raw->vector[index];
	}

	inline Array<Object*>* SafeFindObjects(Type* type) {
		__try { return UObject::FindObjectsByType<Object*>(type); }
		__except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
	}

	inline Il2CppGCHandle SafeNewHandle(Il2CppObject* object) {
		__try { return gchandle_new ? gchandle_new(object, false) : nullptr; }
		__except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
	}

	inline Il2CppObject* SafeGetTarget(Il2CppGCHandle handle) {
		__try { return gchandle_get_target ? gchandle_get_target(handle) : nullptr; }
		__except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
	}

	inline uintptr_t SafeArrayLength(Il2CppArray* array) {
		__try { return array_length ? array_length(array) : 0; }
		__except (EXCEPTION_EXECUTE_HANDLER) { return reinterpret_cast<uintptr_t>(nullptr); }
	}

	inline bool SafeIsValid(Object* object) {
		if (!object) return false;
		__try { return reinterpret_cast<UObject*>(object)->IsValid(); }
		__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
	}

	inline bool Refresh() {
		ClearSnapshot(false);
		Globals::objectSearchStatus.clear();
		if (!Globals::selectedClass) { Globals::objectSearchStatus = "No class selected."; return false; }
		UpdateClassCapabilities();
		if (!Globals::selectedClassIsUnityObject) { Globals::objectSearchStatus = "Class does not inherit UnityEngine.Object."; return false; }
		if (!gchandle_new || !gchandle_get_target || !gchandle_free) { Globals::objectSearchStatus = "IL2CPP GC handle exports are unavailable."; return false; }
		Type* type = Globals::selectedClass->GetType();
		if (!type) { Globals::objectSearchStatus = "Could not resolve class Type."; return false; }
		Array<Object*>* objects = SafeFindObjects(type);
		if (!objects) { Globals::objectSearchStatus = "FindObjectsByType failed or returned no array."; return false; }
		Globals::objectsHandle = SafeNewHandle(reinterpret_cast<Il2CppObject*>(objects));
		if (!Globals::objectsHandle) { Globals::objectSearchStatus = "Failed to create GC handle for object snapshot."; return false; }
		Il2CppObject* rootedObject = SafeGetTarget(Globals::objectsHandle);
		if (!rootedObject) { ClearSnapshot(false); Globals::objectSearchStatus = "GC handle target is unavailable."; return false; }
		Globals::objectSnapshotCount = static_cast<int>(SafeArrayLength(reinterpret_cast<Il2CppArray*>(rootedObject)));
		if (Globals::objectSnapshotCount <= 0) { Globals::objectSearchStatus = "No Objects found."; return false; }
		Globals::objectSearchStatus = "Snapshot refreshed: " + std::to_string(Globals::objectSnapshotCount) + " object(s).";
		return true;
	}
}
