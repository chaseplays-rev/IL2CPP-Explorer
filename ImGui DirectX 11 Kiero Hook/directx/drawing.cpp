#include "drawing.h"
#include "object_cache.h"

namespace Drawing {
	void DrawCachedObjects(ImDrawList* pDrawList) {
		if (!pDrawList || !Globals::highlightObj || !Globals::selectedClass || !Globals::selectedClassIsComponent) return;
		Array<Object*>* objects = ObjectCache::GetSnapshot();
		if (!objects || Globals::objectSnapshotCount <= 0) return;
		Camera* camera = Camera::Get();
		if (!camera) return;
		const char* className = Globals::selectedClass->GetName();
		if (!className) className = "Object";
		for (int i = 0; i < Globals::objectSnapshotCount; i++) {
			Object* rawObject = ObjectCache::GetObjectAt(i);
			if (!rawObject) continue;
			if (!ObjectCache::SafeIsValid(rawObject)) continue;
			Component* component = reinterpret_cast<Component*>(rawObject);
			Transform* transform = component->GetTransform();
			if (!transform) continue;
			Vector3 position = transform->GetPosition();
			Vector2 screen;
			if (!camera->WorldToScreen(screen, position)) continue;
			pDrawList->AddText(ImVec2(screen.x, screen.y), IM_COL32(255, 255, 255, 255), className);
		}
	}

	void Render(ImDrawList* pDrawList) {
		DrawCachedObjects(pDrawList);
	}
}
