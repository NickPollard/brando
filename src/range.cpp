// range.cpp
#include <range.h>

auto brando::foreach(int n, FunctionFn<bool, void> f) {
	for (int i = 0; i < n; ++i) f.apply(true);
}
