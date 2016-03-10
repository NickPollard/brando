// range.h
#pragma once

#include <fn.h>

namespace brando {
	// TODO - should be Fn not FunctionFn
	auto foreach(int n, FunctionFn<bool, void> f);
}
