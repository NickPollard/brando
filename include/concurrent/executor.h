// executor.h
#pragma once
#include "unit.h"
#include "concurrent/future.h"
#include "concurrent/queue.h"
#include "concurrent/task.h"
#include "immutable/list.h"

#include <range.h>

using std::thread;

namespace brando {
	namespace concurrent {

		typedef Task<Unit> Job;

		/*
			 Executor - a service that can execute work asynchronously
			 */
		struct Executor {
			template<typename T>
				auto execute(Task<T> t) -> Future<T> {
					auto p = Promise<T>(*this);
					auto job = t.map(function<Unit(T)>([=](T t){ p.complete(t); return unit(); }));
					executeImpl(job);
					return p.future();
				}
			virtual auto executeImpl(Job job) -> void = 0;
		};

		/*
			 ThreadPoolExecutor - an executor using a fixed size thread pool
			 */
		class ThreadPoolExecutor : public Executor {
			public:
				virtual auto executeImpl(Job job) -> void { jobs.push(job);(void)job;}
				ThreadPoolExecutor(int threadCount) { 
					// memory barrier - ensure that concurrent queue is initialized?
          std::atomic_thread_fence(std::memory_order_seq_cst); // Ensure that the Mutex is fully formed before other threads try to access it
					foreach(threadCount)([&]{ startThread(); }); 
				}
				~ThreadPoolExecutor() {
					// Don't kill the executor until all threads have finished
					//threads.foreach([](thread& t){ t.join(); });
				}
			private:
				ConcurrentQueue<Job> jobs;

				auto startThread() -> void {
					auto js = &jobs;
					auto t = new thread([js]{
						while (true) {
            /*
							if (!js->isEmpty()) {
								auto job = js->pop(); // Concurrently pop a single entry from the queue
								job.foreach(function<void(Job)>([](auto t){ t.run(); }));
							}
              */
						}
					});
					(void)t;
				}
		};

    //void executeFn( Executor& ex, function<void()> f ) {
      //ex.executeImpl( defer( f(); return unit(); ));
    //};
	}
}
