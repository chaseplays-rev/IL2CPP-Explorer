#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include <vector>
#include "kiero/kiero.h"
#include "kiero/minhook/include/MinHook.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

typedef uintptr_t PTR;

namespace Globals {
	inline int currentTab = 0;
}