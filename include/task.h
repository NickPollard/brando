// task.h
#pragma once
#include <fn.h>
#include <future.h>

namespace brando {
	// Forward Declaration
	template<typename T> class Task;
	struct Executor;

	template<typename A, typename B, typename C>
		auto andThen(const Fn<A,B>& f, const Fn<B,C>& g) -> FunctionFn<A,C> {
			return makeFn(std::function<C(A)>([f, g](A a) { return g(f(a)); }));
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
				auto runAsync(Executor& ex) -> Future<T>;

				Task(FunctionFn<bool, T> f_) : f(f_) {}

				/*
					 Task probably needs to be a free Monad that is interpreted as Id or Future when run
					 */
				template<typename U>
				auto map(const Fn<T,U>& ff) -> Task<U> {
					//auto combined = Fn<bool,U>(std::function<bool,U>([](
					auto combined = andThen(f, ff);
					return Task<U>(combined);
				};
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
				executeImpl(t.map(makeFn(std::function<void(T)>([p](T t){ p->complete(t); }))));
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
