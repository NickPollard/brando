// task.cpp
#include <task.h>
#include <iostream>

//#define delay(BODY) FunctionFn<bool,int>(std::function<int(bool)>([](bool b){ (void)b; BODY; }))
#define delay(BODY) makeFn(std::function<int(bool)>([](bool b){ (void)b; BODY; }))
//#define delay(BODY) makeFn(makeFunction([](bool b){ (void)b; BODY; }))

using brando::Task;

void testTask() {
	//auto fn = FunctionFn<bool,int>(std::function<int(bool)>([](bool b){ (void)b; return 1; }));

	//auto lambda = [](bool b){ (void)b; return 0; };
	//auto fn = std::function<void(void)>(lambda);

	Task<int> t = Task<int>(delay({ return 1; }));
	//auto fn = delay({ return 1; });
	//Task<int> t = Task<int>(fn);
	int i = t.run();
	(void)i;
}
