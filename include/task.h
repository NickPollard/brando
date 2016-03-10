// task.h
#pragma once
#include <fn.h>
#include <future.h>

namespace brando {
	// Forward Declaration
	template<typename T> class Task;

	/*
	 	Executor - a service that can execute work asynchronously
	 	*/
	class Executor {
		template<typename T>
			auto execute(Task<T> t) -> Future<T>;
	};
	
	/*
	 	Task<T> - a computation that can be executed to produce a value of type T
	 	*/
	template<typename T> 
		class Task {
			private:
				FunctionFn<bool, T> f;
			public:
				auto run() -> T { return f(true); };
				//auto runAsync(Executor& ex) -> Future<T> { return ex.execute(this); };

				Task(FunctionFn<bool, T> f_) : f(f_) {}

				/*
					 Task probably needs to be a free Monad that is interpreted as Id or Future when run
					 */
				//auto map(Fn<T,U>) -> Task<U>
				//auto bind(Fn<T,Task<U>>) -> Task<U>
		};

#if 0
class ThreadPoolExecutor : Executor {
	typedef Task<void> job;
	private:
		// Queue<job> jobs;
	public:
	template<typename T>
		auto execute(Task<T> t) -> Future<T> {
			// auto p = new Promise<T>();
			// auto work = t.foreach(p.complete);
			// jobs.enqueue(work);
			// return p.future;
		}

	ThreadPoolExecutor(int threadCount) {
		(void)threadCount;
		// Start n threads
		// foreach(threadCount)(startThread);
	}
	private:
		auto startThread() -> void {};
};

//std::function<B(A)>

template<typename A, typename B> 
	auto apply(Fn<A,B> f, A a) -> B;

template<typename A, typename B> 
	auto delay(Fn<A,B> f, A a) -> Task<B>;

template<typename T> 
	auto run(Task<T> task) -> T;

template<typename T> 
	auto runAsync(Task<T> task) -> Future<T>;
#endif
};


