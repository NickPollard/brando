// future.cpp
#include <future.h>
#include <task.h>

void testFuture() {
		auto p = new Promise<int>();
		printf("%ld\n", (uintptr_t)p);
}
