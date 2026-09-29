#pragma once

class Camera : public Component {
public:
	bool WorldToScreen(Vector2& screen, Vector3 world) {
		Vector3 ret = Method::CallVT<Vector3>("UnityEngine.CoreModule", "UnityEngine", "Camera", "WorldToScreenPoint", 1, this, &world);
		if (ret.z < 0)
			return false;
		screen.x = ret.x;
		screen.y = Screen::Height() - ret.y;

		return true;
	}
	static Camera* Get() {
		return Method::Call<Camera*(*)()>("UnityEngine.CoreModule", "UnityEngine", "Camera", "get_main", 0)();
	}
};