#pragma once
#include "../includes.h"

namespace ClassDumper {
	bool Dump(Class* klass);
	const char* GetStatus();
	bool LastSucceeded();
}
