// task.cpp
#include <task.h>
#include <iostream>

//#define delay(BODY) FunctionFn<bool,int>(std::function<int(bool)>([](bool b){ (void)b; BODY; }))
#define delay(BODY) fn(std::function<int(bool)>([](bool b){ (void)b; BODY; }))
#define _delay(BODY) Task<int>(fn(std::function<int(bool)>([](bool b){ (void)b; BODY; })))

using brando::Task;
using brando::Fn;
using brando::fn;

void testTask() {
	//auto fn = FunctionFn<bool,int>(std::function<int(bool)>([](bool b){ (void)b; return 1; }));

	//auto lambda = [](bool b){ (void)b; return 0; };
	//auto fn = std::function<void(void)>(lambda);

	//Task<int> t = Task<int>(delay({ return 1; }));
	auto t = _delay({ return 1; });
	printf("Answer is %i.\n", t.run());

	auto ex = brando::ThreadPoolExecutor(4);
	auto futInt = t.runAsync(ex);
	(void)futInt;

	Fn<int,void> f = fn(std::function<void(int)>([](int i){ printf("%d", i); }));
}
