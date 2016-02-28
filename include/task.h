// task.h
#pragma once

/*
	 Fn<A,B> - an opinionated function type from A -> B
	 */
template<typename A, typename B> 
	class Fn {
			auto apply(A a) -> B { return inner(a); };
	};

template<typename A, typename B> 
	class FunctionFn : Fn<A,B> {
			std::function<B(A)> inner;

			auto apply(A a) -> B { return inner(a); };
	};

/*
	 Task<T> - a computation that can be executed to produce a value of type T
	 */
template<typename T> 
	class Task {
			Fn<void, T> f;
			auto run() -> B { return f(); };
			auto runAsync(Executor& ex) -> Future<B> { return ex.execute(this); };
	};

/*
	 Executor - a service that can execute work asynchronously
	 */
class Executor {
	template<typename T>
			auto execute(Task<T> t) -> Future<T>;
}

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
			// Start n threads
			// foreach(threadCount)(startThread);
	}
	private:
		auto startThread() -> void {};
}

std::function<B(A)>

template<typename A, typename B> 
	auto apply(Fn<A,B> f, A a) -> B;

template<typename A, typename B> 
	auto delay(Fn<A,B> f, A a) -> Task<B>;

template<typename T> 
	auto run(Task<T> task) -> T;

template<typename T> 
	auto runAsync(Task<T> task) -> Future<T>;
