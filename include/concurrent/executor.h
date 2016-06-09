// executor.h
#pragma once
#include "concurrent/future.h"
#include "concurrent/task.h"
#include "immutable/list.h"

#include <range.h>

namespace brando {
	namespace concurrent {

		struct Unit {};
		Unit unit() { return Unit(); }
		typedef Task<Unit> Job;

		/*
			 Executor - a service that can execute work asynchronously
			 */
		struct Executor {
			template<typename T>
				auto execute(Task<T> t) -> Future<T> {
					auto p = Promise<T>();
					executeImpl(t.map(function<Unit(T)>([=](T t){ p.complete(t); return unit(); })));
					return p.future();
				}
			virtual auto executeImpl(Job job) -> void = 0;
		};

		class ThreadPoolExecutor : public Executor {
			public:
				virtual auto executeImpl(Job job) -> void { jobs = job << jobs; }
				ThreadPoolExecutor(int threadCount) { foreach(threadCount)([&](){ startThread();}); }
				~ThreadPoolExecutor() {
					// Don't kill the executor until all threads have finished
					//threads.foreach([](thread& t){ t.join(); });
				}
			private:
				List<Job> jobs;
				//list<thread> threads;

				auto startThread() -> void {
					auto thread = new std::thread([&](){
						auto jobs = this->jobs;
						while (true) if (!jobs.isEmpty()) {
							auto job = jobs.head();
							job.foreach(function<void(Job)>([](auto t){ t.run(); }));
						}
					});
					(void)thread;
				}
		};
	}
}
