// task.h
#pragma once
#include <fn.h>
#include "concurrent/future.h"

#include <memory>
#include <thread>

#include <iostream>

using std::shared_ptr;

namespace brando {
	namespace concurrent {
		// *** Forward Declaration
		template<typename T> class Task;
		struct Executor;

		/*
			 Task<T> - a computation that can be executed to produce a value of type T
			 */
		template<typename T>
			class Task {
				public:
					auto run() -> T { return f(); };
					auto runAsync(Executor& ex) -> Future<T>;

					Task(function<T()> f_) : f(f_) {}

					static auto delay(function<T()> df) { return Task(df); }

					/*
						 Task probably needs to be a free Monad that is interpreted as Id or Future when run
						 */
					template<typename U>
						auto map(function<U(T)> ff) -> Task<U> {
							auto thisF = f;
							return Task<U>([=] {
								auto t = thisF();
								return ff(t);
								}); };

					//auto bind(Fn<T,Task<U>>) -> Task<U>
				private:
					function<T()> f;
			};

		template <typename F>
  		auto task(F f) -> Task<decltype(f())> {
				return Task<decltype(f())>(f);
			};

		#define defer( body ) (task([=]{ body; }))
		#define async( ex, body ) (task([=]{ body; }).runAsync(ex))

	};
}

#include "concurrent/executor.h"
template<typename T>
//	auto brando::concurrent::Task<T>::runAsync(brando::concurrent::Executor& ex) -> Future<T> { (void)ex; return future(f()); };
	auto brando::concurrent::Task<T>::runAsync(brando::concurrent::Executor& ex) -> Future<T> { return ex.execute(*this); };

