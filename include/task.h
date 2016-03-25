// task.h
#pragma once
#include <fn.h>
#include <future.h>
#include <memory>

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
				Fn<bool, T> f;
			public:
				auto run() -> T { return f(true); };
				auto runAsync(Executor& ex) -> Future<T>;

				Task(Fn<bool, T> f_) : f(f_) {}

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
				(void)t;
				executeImpl(t.map(fn(std::function<void(T)>([p](T t){ p->complete(t); }))));
				return p->future();
			}
		virtual auto executeImpl(Job job) -> void = 0;
	};

	template<typename T>
	 	struct Queue {
			auto enqueue(T t) -> void { (void)t; }
			auto operator += (T t) -> void { enqueue(t); }
		};

	class ThreadPoolExecutor : public Executor {
		private: 
			Queue<Job> jobs;
		public:
			virtual auto executeImpl(Job job) -> void { jobs += job; }

		ThreadPoolExecutor(int threadCount) {
			(void)threadCount;
			// Start n threads
			// foreach(threadCount)(startThread);
		}
		private:
			auto startThread() -> void {};
	};
};

template<typename T>
	auto brando::Task<T>::runAsync(Executor& ex) -> Future<T> { return ex.execute(*this); };
