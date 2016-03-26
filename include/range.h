// range.h
#pragma once

#include <fn.h>

namespace brando {
	struct ForeachBuilder {
		int iterations;
		ForeachBuilder(int n) : iterations(n) {}
		template<typename T>
			auto operator ()(Fn<bool, T> f) -> void { for (int i = iterations; i > 0; --i) f(true); }
	};

	auto foreach(int n) -> ForeachBuilder { return ForeachBuilder(n); }
};
