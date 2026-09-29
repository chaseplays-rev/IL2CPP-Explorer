#pragma once

class UObject : public Object {
public:
	template <typename T>
	static Array<T>* FindObjectsByType(Type* pType) {
		return Method::Call<Array<T>*(*)(Object*, int, int)>("UnityEngine.CoreModule", "UnityEngine", "Object", "FindObjectsByType", 3)(pType->GetObjectType(), 0, 0);
	}
    bool IsValid() {
        return Method::Call<bool(*)(Object*)>("UnityEngine.CoreModule", "UnityEngine", "Object", "op_Implicit", 1)(this);
    }
};