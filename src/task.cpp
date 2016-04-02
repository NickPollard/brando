// task.cpp
#include <task.h>
#include <iostream>
#include "immutable/option.h"

//#define delay(BODY) FunctionFn<bool,int>(std::function<int(bool)>([](bool b){ (void)b; BODY; }))
#define delay(BODY) fn0(std::function<int()>([](){ BODY; }))
#define _delay(BODY) Task<int>(fn0(std::function<int()>([](){ BODY; })))

using brando::Task;
using brando::Fn;
using brando::fn;
using brando::Fn0;
using brando::fn0;
using brando::immutable::some;

void testTask() {

	//auto lambda = [](bool b){ (void)b; return 0; };
	//auto fn = std::function<void(void)>(lambda);

	auto t = _delay({ return 1; });
	printf("Answer is %i.\n", t.run());

	auto ex = brando::ThreadPoolExecutor(4);
	auto futInt = t.runAsync(ex);
	(void)futInt;

	Fn<int,void> f = fn(std::function<void(int)>([](int i){ printf("%d", i); }));

	// Option
	auto o = some(13);
	int i = o->getOrElse(0);
	printf("%d\n", i);
}
