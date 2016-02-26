// list.cpp
#include "list.h"

	void test() {
		auto emp = List<int>::empty();
		auto a = List<int>::cons(1, emp);
		auto b = List<int>::cons(2, a);
		printf("%d\n", b->head());
	}
