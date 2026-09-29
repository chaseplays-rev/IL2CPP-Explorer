#include "menu.h"
#include "class_dumper.h"
#include "object_cache.h"
#include "inspect_runtime.h"
#include <cstring>
#include <string>
#include <vector>

namespace {
	void SetupModernStyle() {
		if (Globals::styleApplied) return;
		Globals::styleApplied = true;
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(0.0f, 0.0f);
		style.FramePadding = ImVec2(10.0f, 7.0f);
		style.ItemSpacing = ImVec2(8.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.WindowRounding = 10.0f;
		style.ChildRounding = 8.0f;
		style.FrameRounding = 6.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 8.0f;
		style.GrabRounding = 6.0f;
		style.TabRounding = 6.0f;
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.94f, 0.97f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.48f, 0.55f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.055f, 0.060f, 0.075f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.072f, 0.078f, 0.095f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.065f, 0.070f, 0.085f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.15f, 0.17f, 0.21f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.095f, 0.105f, 0.125f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.14f, 0.15f, 0.19f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.15f, 0.35f, 0.70f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.43f, 0.85f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.12f, 0.30f, 0.63f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.15f, 0.35f, 0.70f, 0.35f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.40f, 0.80f, 0.55f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.45f, 0.90f, 0.70f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.60f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.25f, 0.50f, 0.95f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.17f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.40f, 0.80f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.20f, 0.45f, 0.90f, 0.60f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.25f, 0.50f, 1.00f, 0.90f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.055f, 0.060f, 0.075f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.28f, 0.34f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.34f, 0.42f, 1.00f);
	}

	bool SidebarButton(const char* label, bool selected) {
		ImVec2 size(145.0f, 40.0f);
		if (selected) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.70f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		} else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.075f, 0.080f, 0.095f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.65f, 0.68f, 0.75f, 1.0f));
		}
		bool clicked = ImGui::Button(label, size);
		ImGui::PopStyleColor(2);
		return clicked;
	}

	std::string TypeName(Type* type) {
		if (!type) return "unknown";
		std::string name = type->GetName();
		return name.empty() ? "unknown" : name;
	}

	std::string MethodSignature(Method* method) {
		if (!method) return "<invalid method>";
		std::string signature;
		if (method->IsStatic()) signature += "static ";
		signature += TypeName(method->GetReturnType()) + " ";
		const char* methodName = method->GetName();
		signature += methodName ? methodName : "<unknown>";
		signature += "(";
		uint32_t count = method->GetParamCount();
		for (uint32_t i = 0; i < count; i++) {
			if (i) signature += ", ";
			signature += TypeName(method->GetParam(i));
			const char* paramName = method->GetParamName(i);
			if (paramName && *paramName) signature += " " + std::string(paramName);
		}
		signature += ")";
		return signature;
	}

	const char* ImageName(Class* klass) {
		if (!klass || !image_get_name) return "";
		Il2CppImage* image = klass->GetImage();
		return image ? image_get_name(image) : "";
	}

	void DrawClassType(Class* klass, const char* prefix) {
		if (!klass) return;
		const char* name = klass->GetName();
		const char* namespc = klass->GetNamespace();
		if (namespc && *namespc) ImGui::Text("%s %s.%s    [%p]", prefix, namespc, name ? name : "<unknown>", klass);
		else ImGui::Text("%s %s    [%p]", prefix, name ? name : "<unknown>", klass);
	}

	void DrawBaseTypesAndInterfaces(Class* klass) {
		if (!klass || !ImGui::CollapsingHeader("Base Type and Interfaces")) return;
		Class* parent = klass->GetParent();
		if (parent) {
			ImGui::TextDisabled("Base Types");
			ImGui::Indent(12.0f);
			int depth = 0;
			while (parent && depth++ < 64) { DrawClassType(parent, "[C]"); parent = parent->GetParent(); }
			ImGui::Unindent(12.0f);
		} else ImGui::TextDisabled("Base Types: none");

		std::vector<Class*> interfaces = klass->GetInterfaces();
		if (!interfaces.empty()) {
			ImGui::Dummy(ImVec2(0, 4));
			ImGui::TextDisabled("Interfaces");
			ImGui::Indent(12.0f);
			for (Class* interfaceClass : interfaces) DrawClassType(interfaceClass, "[I]");
			ImGui::Unindent(12.0f);
		} else ImGui::TextDisabled("Interfaces: none");
	}

	void SearchClass() {
		InspectRuntime::Reset();
		ObjectCache::ClearSnapshot();
		Globals::highlightObj = false;
		std::string namespc = Globals::searchNamespace;
		if (namespc == "-") namespc.clear();
		Globals::selectedClass = Class::Resolve(Globals::searchAssembly, namespc.c_str(), Globals::searchClass);
		ObjectCache::UpdateClassCapabilities();
		if (Globals::selectedClass) Globals::searchStatus = "Class found.";
		else Globals::searchStatus = "Class not found.";
	}

	void DrawSearchTab() {
		ImGui::SetCursorPosX(20);
		ImGui::TextColored(ImVec4(0.95f, 0.96f, 1.0f, 1.0f), "Search");
		ImGui::SetCursorPosX(20);
		ImGui::TextDisabled("Resolve an IL2CPP class using the same assembly / namespace / class format as the SDK.");
		ImGui::Dummy(ImVec2(0, 14));
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##SearchBox", ImVec2(ImGui::GetContentRegionAvail().x - 20, 205), true);
		ImGui::Text("Class Search");
		ImGui::Dummy(ImVec2(0, 5));
		ImGui::TextDisabled("Assembly");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextWithHint("##AssemblySearch", "Assembly-CSharp", Globals::searchAssembly, IM_ARRAYSIZE(Globals::searchAssembly));
		ImGui::TextDisabled("Namespace");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextWithHint("##NamespaceSearch", "Global namespace: leave blank or use -", Globals::searchNamespace, IM_ARRAYSIZE(Globals::searchNamespace));
		ImGui::TextDisabled("Class");
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90);
		bool enterPressed = ImGui::InputTextWithHint("##ClassSearch", "Item", Globals::searchClass, IM_ARRAYSIZE(Globals::searchClass), ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine();
		bool searchPressed = ImGui::Button("Search", ImVec2(75, 0));
		if (enterPressed || searchPressed) SearchClass();
		ImGui::EndChild();

		ImGui::Dummy(ImVec2(0, 12));
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##SearchResults", ImVec2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 20), true);
		if (Globals::selectedClass) {
			const char* name = Globals::selectedClass->GetName();
			const char* namespc = Globals::selectedClass->GetNamespace();
			ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.0f), "Found");
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::TextDisabled("Class"); ImGui::SameLine(130); ImGui::Text("%s", name ? name : "");
			ImGui::TextDisabled("Namespace"); ImGui::SameLine(130); ImGui::Text("%s", namespc && *namespc ? namespc : "<Global>");
			ImGui::TextDisabled("Assembly"); ImGui::SameLine(130); ImGui::Text("%s", ImageName(Globals::selectedClass));
			ImGui::TextDisabled("Address"); ImGui::SameLine(130); ImGui::Text("%p", Globals::selectedClass);
		} else if (!Globals::searchStatus.empty()) ImGui::TextDisabled("%s", Globals::searchStatus.c_str());
		else ImGui::TextDisabled("Search for a class to begin.");
		ImGui::EndChild();
	}

	void DrawClassTab() {
		if (!Globals::selectedClass) { Globals::currentTab = 1; return; }
		const char* name = Globals::selectedClass->GetName();
		const char* namespc = Globals::selectedClass->GetNamespace();
		ImGui::SetCursorPosX(20);
		ImGui::TextColored(ImVec4(0.95f, 0.96f, 1.0f, 1.0f), "Class");
		ImGui::SetCursorPosX(20);
		ImGui::TextDisabled("Read-only IL2CPP class metadata and SDK dump.");
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##ClassDetails", ImVec2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 20), true);
		ImGui::Text("Class Information");
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));
		ImGui::TextDisabled("Class"); ImGui::SameLine(140); ImGui::Text("%s", name ? name : "");
		ImGui::TextDisabled("Namespace"); ImGui::SameLine(140); ImGui::Text("%s", namespc && *namespc ? namespc : "<Global>");
		ImGui::TextDisabled("Assembly"); ImGui::SameLine(140); ImGui::Text("%s", ImageName(Globals::selectedClass));
		ImGui::TextDisabled("Class Address"); ImGui::SameLine(140); ImGui::Text("%p", Globals::selectedClass);
		ImGui::TextDisabled("Value Type"); ImGui::SameLine(140); ImGui::Text("%s", Globals::selectedClass->IsValueType() ? "Yes" : "No");
		ImGui::TextDisabled("Enum"); ImGui::SameLine(140); ImGui::Text("%s", Globals::selectedClass->IsEnum() ? "Yes" : "No");
		ImGui::Dummy(ImVec2(0, 8));
		if (ImGui::Button("Dump", ImVec2(90.0f, 30.0f))) ClassDumper::Dump(Globals::selectedClass);
		const char* dumpStatus = ClassDumper::GetStatus();
		if (dumpStatus && *dumpStatus) {
			ImGui::SameLine();
			if (ClassDumper::LastSucceeded()) ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.0f), "%s", dumpStatus);
			else ImGui::TextDisabled("%s", dumpStatus);
		}
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 5));
		DrawBaseTypesAndInterfaces(Globals::selectedClass);

		if (ImGui::CollapsingHeader("Methods", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (Method* method : Globals::selectedClass->GetMethodList()) {
				std::string signature = MethodSignature(method);
				ImGui::TextWrapped("%s", signature.c_str());
			}
		}

		if (ImGui::CollapsingHeader("Fields")) {
			for (Field* field : Globals::selectedClass->GetFieldList()) {
				if (!field) continue;
				const char* fieldName = field->GetName();
				std::string typeName = TypeName(field->GetType());
				ImGui::Text("%s : %s    [0x%zX]", fieldName ? fieldName : "<unknown>", typeName.c_str(), field->GetOffset());
			}
		}

		if (ImGui::CollapsingHeader("Properties")) {
			for (Property* property : Globals::selectedClass->GetPropertyList()) {
				if (!property) continue;
				const char* propertyName = property->GetName();
				ImGui::Text("%s", propertyName ? propertyName : "<unknown>");
			}
		}
		ImGui::EndChild();
	}

	void DrawUtilitiesTab() {
		if (!Globals::selectedClass) { Globals::currentTab = 1; return; }
		ImGui::SetCursorPosX(20);
		ImGui::TextColored(ImVec4(0.95f, 0.96f, 1.0f, 1.0f), "Utilities");
		ImGui::SetCursorPosX(20);
		ImGui::TextDisabled("Utilities for the currently selected IL2CPP class.");
		ImGui::Dummy(ImVec2(0, 14));
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##Utilities", ImVec2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 20), true);
		ImGui::Text("Selected Class");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.35f, 0.65f, 1.0f, 1.0f), "%s", Globals::selectedClass->GetName() ? Globals::selectedClass->GetName() : "");
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 8));

		if (!Globals::selectedClassIsUnityObject) {
			ImGui::TextDisabled("Object search unavailable: this class does not inherit UnityEngine.Object.");
			ImGui::EndChild();
			return;
		}

		if (Globals::selectedClassIsComponent) ImGui::Checkbox("Highlight Instances", &Globals::highlightObj);
		else { Globals::highlightObj = false; ImGui::TextDisabled("Highlighting requires a class derived from UnityEngine.Component."); }
		ImGui::Dummy(ImVec2(0, 6));
		if (ImGui::Button("Search Valid Objects", ImVec2(170.0f, 32.0f))) ObjectCache::Refresh();
		if (!Globals::objectSearchStatus.empty()) {
			ImGui::SameLine();
			ImGui::TextDisabled("%s", Globals::objectSearchStatus.c_str());
		}

		Array<Object*>* objects = ObjectCache::GetSnapshot();
		if (objects && Globals::objectSnapshotCount > 0) {
			ImGui::Dummy(ImVec2(0, 10));
			ImGui::BeginChild("##ValidObjects", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
			int validCount = 0;
			for (int i = 0; i < Globals::objectSnapshotCount; i++) {
				Object* object = ObjectCache::GetObjectAt(i);
				if (!object) continue;
				if (!ObjectCache::SafeIsValid(object)) continue;
				validCount++;
			}
			ImGui::Text("Valid Objects: %d", validCount);
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 5));
			for (int i = 0; i < Globals::objectSnapshotCount; i++) {
				Object* object = ObjectCache::GetObjectAt(i);
				if (!object) continue;
				if (!ObjectCache::SafeIsValid(object)) continue;
				char label[128];
				sprintf_s(label, "Index %d    [%p]", i, object);
				bool selected = InspectRuntime::GetRootObject() == object;
				ImGui::PushID(i);
				if (ImGui::Selectable(label, selected)) {
					if (InspectRuntime::SyncRoot(object)) {
						Globals::selectedObject = object;
						Globals::currentTab = 0;
					}
				}
				ImGui::PopID();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

	void DrawInspectTabBar() {
		if (Globals::inspectTabs.empty()) return;
		int closeIndex = -1;
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##InspectTabs", ImVec2(ImGui::GetContentRegionAvail().x - 20, 38.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (size_t i = 0; i < Globals::inspectTabs.size(); i++) {
			Globals::InspectTab& tab = Globals::inspectTabs[i];
			const char* name = tab.klass ? tab.klass->GetName() : nullptr;
			bool active = static_cast<int>(i) == Globals::activeInspectTab;
			ImGui::PushID(static_cast<int>(tab.id));
			if (active) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.35f, 0.70f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			}
			if (ImGui::Button(name ? name : "<Unknown>", ImVec2(0, 28.0f))) Globals::activeInspectTab = static_cast<int>(i);
			if (active) ImGui::PopStyleColor(2);
			if (i > 0) {
				ImGui::SameLine(0.0f, 2.0f);
				if (ImGui::Button("x##Close", ImVec2(24.0f, 28.0f))) closeIndex = static_cast<int>(i);
			}
			ImGui::PopID();
			if (i + 1 < Globals::inspectTabs.size()) ImGui::SameLine(0.0f, 6.0f);
		}
		ImGui::EndChild();
		if (closeIndex > 0) InspectRuntime::CloseTab(static_cast<size_t>(closeIndex));
	}

	void DrawInspectableClass(Class* klass, Object* object, const char* prefix) {
		if (!klass || !object) return;
		const char* name = klass->GetName();
		const char* namespc = klass->GetNamespace();
		char label[512];
		if (namespc && *namespc) sprintf_s(label, "%s %s.%s    [%p]", prefix, namespc, name ? name : "<unknown>", klass);
		else sprintf_s(label, "%s %s    [%p]", prefix, name ? name : "<unknown>", klass);
		ImGui::PushID(klass);
		ImGui::Selectable(label, false);
		if (ImGui::BeginPopupContextItem("##InspectTypeContext")) {
			if (ImGui::Selectable("Open Class Tab", false, 0, ImVec2(150.0f, 28.0f))) InspectRuntime::OpenClass(klass, object);
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	void DrawInspectHierarchy(Class* klass, Object* object) {
		if (!klass || !object || !ImGui::CollapsingHeader("Base Type and Interfaces")) return;
		Class* parent = klass->GetParent();
		if (parent) {
			ImGui::TextDisabled("Base Types");
			ImGui::Indent(12.0f);
			int depth = 0;
			while (parent && depth++ < 64) {
				DrawInspectableClass(parent, object, "[C]");
				parent = parent->GetParent();
			}
			ImGui::Unindent(12.0f);
		} else ImGui::TextDisabled("Base Types: none");
		std::vector<Class*> interfaces = klass->GetInterfaces();
		if (!interfaces.empty()) {
			ImGui::Dummy(ImVec2(0, 4));
			ImGui::TextDisabled("Interfaces");
			ImGui::Indent(12.0f);
			for (Class* interfaceClass : interfaces) DrawInspectableClass(interfaceClass, object, "[I]");
			ImGui::Unindent(12.0f);
		} else ImGui::TextDisabled("Interfaces: none");
	}

	void OpenMethodInspector(Method* method) {
		if (!method) return;
		InspectRuntime::SetMethodContext(method);
	}

	void DrawMethodInspector() {
		if (!Globals::methodInspectorOpen || !Globals::inspectedMethod) return;
		Method* method = Globals::inspectedMethod;
		std::string signature = MethodSignature(method);
		const char* methodName = method->GetName();
		Class* ownerClass = method->GetClass();
		const char* className = ownerClass ? ownerClass->GetName() : nullptr;
		uint32_t paramCount = method->GetParamCount();
		bool isStatic = method->IsStatic();
		if (!Globals::methodInspectorOpen) return;
		ImGui::SetNextWindowSize(ImVec2(540.0f, paramCount ? 430.0f : 350.0f), ImGuiCond_FirstUseEver);
		bool open = Globals::methodInspectorOpen;
		if (!ImGui::Begin("Method Inspector", &open, ImGuiWindowFlags_NoCollapse)) {
			Globals::methodInspectorOpen = open;
			ImGui::End();
			return;
		}
		if (!open) {
			ImGui::End();
			InspectRuntime::ClearMethodContext();
			return;
		}
		ImGui::TextWrapped("%s", signature.c_str());
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 6));
		ImGui::BeginChild("##MethodInformation", ImVec2(0, 145.0f), true);
		ImGui::TextDisabled("Method"); ImGui::SameLine(120); ImGui::Text("%s", methodName ? methodName : "");
		ImGui::TextDisabled("Class"); ImGui::SameLine(120); ImGui::Text("%s", className ? className : "");
		ImGui::TextDisabled("Static"); ImGui::SameLine(120); ImGui::Text("%s", isStatic ? "Yes" : "No");
		ImGui::TextDisabled("Parameters"); ImGui::SameLine(120); ImGui::Text("%u", paramCount);
		ImGui::TextDisabled("Instance"); ImGui::SameLine(120); ImGui::Text("%p", isStatic ? nullptr : InspectRuntime::GetMethodContext(method));
		ImGui::TextDisabled("MethodInfo"); ImGui::SameLine(120); ImGui::Text("%p", method);
		ImGui::EndChild();
		if (paramCount > 0) {
			ImGui::Dummy(ImVec2(0, 8));
			ImGui::Text("Arguments");
			for (uint32_t i = 0; i < paramCount; i++) {
				std::string typeName = TypeName(method->GetParam(i));
				const char* paramName = method->GetParamName(i);
				ImGui::TextDisabled("[%u] %s %s", i, typeName.c_str(), paramName ? paramName : "");
			}
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			ImGui::InputTextWithHint("##MethodArguments", "Example: 123, true, \"text\", this, null", Globals::methodArguments, IM_ARRAYSIZE(Globals::methodArguments));
			ImGui::TextDisabled("Reference args: null, this, or 0xADDRESS. Enums currently use their numeric value.");
		}
		ImGui::Dummy(ImVec2(0, 8));
		bool canCall = isStatic || InspectRuntime::GetMethodContext(method) != nullptr;
		if (canCall) {
			if (ImGui::Button("Call", ImVec2(160.0f, 36.0f))) InspectRuntime::InvokeInspectedMethod();
		} else ImGui::TextDisabled("The object instance saved for this method is no longer available.");
		if (!Globals::methodCallResult.empty()) {
			ImGui::Dummy(ImVec2(0, 8));
			ImGui::Separator();
			ImGui::TextWrapped("%s", Globals::methodCallResult.c_str());
		}
		ImGui::End();
	}

	void DrawInspectTab() {
		if (Globals::inspectTabs.empty()) { Globals::currentTab = Globals::selectedClass ? 2 : 1; return; }
		Class* klass = InspectRuntime::GetActiveClass();
		Object* object = InspectRuntime::GetActiveObject();
		if (!klass || !object) {
			ImGui::SetCursorPosX(20);
			ImGui::TextDisabled("The selected object is no longer available.");
			return;
		}
		ImGui::SetCursorPosX(20);
		ImGui::TextColored(ImVec4(0.95f, 0.96f, 1.0f, 1.0f), "Inspector");
		ImGui::SetCursorPosX(20);
		ImGui::TextDisabled("Each class tab keeps its own rooted object instance for method calls.");
		ImGui::Dummy(ImVec2(0, 8));
		DrawInspectTabBar();
		klass = InspectRuntime::GetActiveClass();
		object = InspectRuntime::GetActiveObject();
		if (!klass || !object) return;
		ImGui::SetCursorPosX(20);
		ImGui::BeginChild("##InspectorDetails", ImVec2(ImGui::GetContentRegionAvail().x - 20, ImGui::GetContentRegionAvail().y - 20), true);
		const char* name = klass->GetName();
		const char* namespc = klass->GetNamespace();
		ImGui::Text("Class Information");
		ImGui::Separator();
		ImGui::TextDisabled("Class"); ImGui::SameLine(140); ImGui::Text("%s", name ? name : "");
		ImGui::TextDisabled("Namespace"); ImGui::SameLine(140); ImGui::Text("%s", namespc && *namespc ? namespc : "<Global>");
		ImGui::TextDisabled("Object"); ImGui::SameLine(140); ImGui::Text("%p", object);
		ImGui::TextDisabled("Class Address"); ImGui::SameLine(140); ImGui::Text("%p", klass);
		ImGui::Dummy(ImVec2(0, 8));
		DrawInspectHierarchy(klass, object);
		if (ImGui::CollapsingHeader("Methods", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (Method* method : klass->GetMethodList()) {
				if (!method) continue;
				std::string signature = MethodSignature(method);
				ImGui::PushID(method);
				ImGui::Selectable(signature.c_str(), Globals::inspectedMethod == method && Globals::methodContextTabId == InspectRuntime::GetActiveTabId());
				if (ImGui::BeginPopupContextItem("##MethodContext")) {
					if (ImGui::Selectable("Inspect Method", false, 0, ImVec2(150.0f, 28.0f))) OpenMethodInspector(method);
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
		}
		if (ImGui::CollapsingHeader("Fields")) {
			for (Field* field : klass->GetFieldList()) {
				if (!field) continue;
				const char* fieldName = field->GetName();
				std::string typeName = TypeName(field->GetType());
				ImGui::Text("%s : %s    [0x%zX]", fieldName ? fieldName : "<unknown>", typeName.c_str(), field->GetOffset());
			}
		}
		if (ImGui::CollapsingHeader("Properties")) {
			for (Property* property : klass->GetPropertyList()) {
				if (!property) continue;
				const char* propertyName = property->GetName();
				ImGui::Text("%s", propertyName ? propertyName : "<unknown>");
			}
		}
		ImGui::EndChild();
	}

	void DrawSidebar(const ImVec2& windowSize) {
		ImGui::BeginChild("##Sidebar", ImVec2(175.0f, windowSize.y), true);
		ImGui::Dummy(ImVec2(0, 10));
		ImGui::SetCursorPosX(18);
		ImGui::TextColored(ImVec4(0.35f, 0.65f, 1.0f, 1.0f), "IL2CPP");
		ImGui::SetCursorPosX(18);
		ImGui::Text("RUNTIME EXPLORER");
		ImGui::Dummy(ImVec2(0, 18));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 12));
		ImGui::SetCursorPosX(14);
		if (SidebarButton("Search", Globals::currentTab == 1)) Globals::currentTab = 1;
		if (Globals::selectedClass) {
			ImGui::SetCursorPosX(14);
			if (SidebarButton("Class", Globals::currentTab == 3)) Globals::currentTab = 3;
		}
		if (!Globals::inspectTabs.empty()) {
			ImGui::SetCursorPosX(14);
			if (SidebarButton("Inspect", Globals::currentTab == 0)) Globals::currentTab = 0;
		}
		if (Globals::selectedClass) {
			ImGui::SetCursorPosX(14);
			if (SidebarButton("Utilities", Globals::currentTab == 2)) Globals::currentTab = 2;
		}
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 75);
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0, 8));
		ImGui::SetCursorPosX(18);
		ImGui::TextColored(ImVec4(0.30f, 0.85f, 0.50f, 1.0f), "●");
		ImGui::SameLine();
		ImGui::TextDisabled("Runtime attached");
		ImGui::EndChild();
	}

	void DrawCurrentTab() {
		if (!Globals::selectedClass && Globals::currentTab != 1) Globals::currentTab = 1;
		if (Globals::inspectTabs.empty() && Globals::currentTab == 0) Globals::currentTab = Globals::selectedClass ? 3 : 1;
		switch (Globals::currentTab) {
		case 0: DrawInspectTab(); break;
		case 1: DrawSearchTab(); break;
		case 2: DrawUtilitiesTab(); break;
		case 3: DrawClassTab(); break;
		default: Globals::currentTab = 1; DrawSearchTab(); break;
		}
	}
}

void Menu::Draw()
{
	SetupModernStyle();
	if (!Globals::selectedClass && Globals::currentTab == 0) Globals::currentTab = 1;
	ImGui::SetNextWindowSize(ImVec2(900, 620), ImGuiCond_FirstUseEver);
	ImGui::Begin("IL2CPP Runtime Explorer", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 windowSize = ImGui::GetContentRegionAvail();
	DrawSidebar(windowSize);
	ImGui::SameLine();
	ImGui::BeginChild("##MainContent", ImVec2(0, windowSize.y), false);
	ImGui::Dummy(ImVec2(0, 12));
	DrawCurrentTab();
	ImGui::EndChild();
	ImGui::End();
	DrawMethodInspector();
}
