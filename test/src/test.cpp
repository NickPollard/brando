// test.cpp
#include "immutable/option.h"
#include <stdio.h>

using brando::immutable::some;
using brando::immutable::none;

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;
	// Option
	printf("-- Beginning Option Tests --\n");
	auto op = some(14);
	auto op2 = none<int>();
	printf("  some(14) => %d, none => %d\n", op.getOrElse(0), op2.getOrElse(0));
	return 0;
}
