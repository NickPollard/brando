// task.h
#pragma once
#include <fn.h>
#include <future.h>
#include <queue.h>
#include <range.h>
#include <memory>
#include <thread>

using std::shared_ptr;

namespace brando {
	// *** Forward Declaration
	template<typename T> class Task;
	struct Executor;

	/*
	 	Task<T> - a computation that can be executed to produce a value of type T
	 	*/
	template<typename T> 
		class Task {
			private:
				Fn0<T> f;
			public:
				auto run() -> T { return f(); };
				auto runAsync(Executor& ex) -> Future<T>;

				Task(Fn0<T> f_) : f(f_) {}

				/*
					 Task probably needs to be a free Monad that is interpreted as Id or Future when run
					 */
				template<typename U>
				auto map(Fn<T,U> ff) -> Task<U> { return Task<U>(f.andThen(ff)); };
				//auto bind(Fn<T,Task<U>>) -> Task<U>
		};
	
	typedef Task<void> Job;

	/*
	 	Executor - a service that can execute work asynchronously
	 	*/
	struct Executor {
		template<typename T>
			auto execute(Task<T> t) -> Future<T> {
				auto p = new Promise<T>();
				executeImpl(t.map(fn<T,void>([p](T t){ p->complete(t); })));
				return p->future();
			}
		virtual auto executeImpl(Job job) -> void = 0;
	};

	class ThreadPoolExecutor : public Executor {
		public:
			virtual auto executeImpl(Job job) -> void { jobs += job; }
			ThreadPoolExecutor(int threadCount) { foreach(threadCount)([this](){ this->startThread();}); }
			~ThreadPoolExecutor() {
				// Don't kill the executor until all threads have finished
				//threads.foreach([](thread& t){ t.join(); });
			}
		private:
			Queue<Job> jobs;
			//list<thread> threads;

			auto startThread() -> void {
				auto thread = std::thread([this](){
					auto jobs = this->jobs;
					while (true) if (!jobs.empty()) {
						auto job = jobs.head();
						job.run();
					}
				});
			}
	};
};

template<typename T>
	auto brando::Task<T>::runAsync(Executor& ex) -> Future<T> { return ex.execute(*this); };
