#pragma once

class Transform : public Object {
public:
	int ChildCount() {
		return Method::Call<int(*)(Transform*)>("UnityEngine.CoreModule", "UnityEngine", "Transform", "get_childCount", 0)(this);
	}

	Vector3 GetPosition() {
		return Method::Call<Vector3(*)(Transform*)>("UnityEngine.CoreModule", "UnityEngine", "Transform", "get_position", 0)(this);
	}
};
