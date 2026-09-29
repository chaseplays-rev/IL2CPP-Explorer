#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include <vector>
#include "kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "sdk/sdk.hpp"

namespace Globals {
	struct InspectTab {
		IL2CPP::Class* klass = nullptr;
		IL2CPP_API::Il2CppGCHandle objectHandle = nullptr;
		uint64_t id = 0;
	};

	inline int currentTab = 0;
	inline IL2CPP::Class* selectedClass = nullptr;
	inline IL2CPP::Object* selectedObject = nullptr;
	inline IL2CPP_API::Il2CppGCHandle objectsHandle = nullptr;
	inline int objectSnapshotCount = 0;
	inline bool highlightObj = false;
	inline bool selectedClassIsUnityObject = false;
	inline bool selectedClassIsComponent = false;
	inline bool styleApplied = false;
	inline char searchAssembly[128] = "Assembly-CSharp";
	inline char searchNamespace[192] = "";
	inline char searchClass[128] = "";
	inline std::string searchStatus;
	inline std::string objectSearchStatus;

	inline std::vector<InspectTab> inspectTabs;
	inline int activeInspectTab = -1;
	inline uint64_t nextInspectTabId = 1;
	inline IL2CPP::Method* inspectedMethod = nullptr;
	inline bool methodInspectorOpen = false;
	inline IL2CPP_API::Il2CppGCHandle methodContextHandle = nullptr;
	inline uint64_t methodContextTabId = 0;
	inline IL2CPP::Class* methodContextClass = nullptr;
	inline std::vector<IL2CPP_API::Il2CppGCHandle> methodArgumentHandles;
	inline char methodArguments[1024] = "";
	inline std::string methodCallResult;
}
