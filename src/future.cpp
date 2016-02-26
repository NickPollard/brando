// future.cpp
#include <future.h>

void test() {
		auto p = new Promise<int>();
		printf("%ld\n", (uintptr_t)p);
}
