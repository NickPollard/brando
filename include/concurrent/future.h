// Future.h

/*
	 A C++ Monadic future implemtation.

	 A Future<T> represents a value of type T that will be ready at some point in the future,
	 but may not be ready now.
	 */

#pragma once
#include <functional>
#include <mutex>
#include <atomic>
#include <thread>

#include "immutable/option.h"
#include "immutable/list.h"
#include "concurrent/time.h"

using std::function;
using std::atomic;
using std::mutex;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::move;
using brando::immutable::Option;
using brando::immutable::some;
using brando::immutable::none;
using brando::immutable::List;
using brando::concurrent::Seconds;

namespace brando {
	namespace concurrent {

		template<typename T> struct Promise;
		template<typename T> struct PromiseImpl;

		/*
			 Future<T> - a type that will, at some point, contain a value of type T, but may
			 not at present. Futures can be used to implement concurrency, and can be used
			 monadically to compose sequential asynchronous computations.

			 Futures are, underneath, a shared_ptr<PromiseImpl<T>>, with the PromiseImpl<T> allowing
			 the eventual T to be supplied
			 */
		template<typename T> struct Future {
			template<typename U> auto map( function<U(T)> f ) -> Future<U> {
				Promise<U> next = Promise<U>();
				foreach([=](T t){ next.complete(f(t)); });
				return next.future();
			};

			/*
			template<typename F> auto map( F f ) {
				auto next = new Promise<decltype(f(T)>();
				foreach([=](T t){ next.complete(f(t)); });
				return next.future();
			};
			*/
			
			template<typename F> void foreach( F f ) const { promise->foreach(function<void(T)>(f)); };

			template<typename U> void foreach( function<U(T)> f ) const { promise->foreach(f); };

			template<typename U> void andThen( function<U(T)> f ) const { foreach(f); };

			auto completed() -> bool { return promise->completed(); }

			auto get() -> Option<T> { return promise->get(); }

			auto await(int seconds, Seconds unit) -> Option<T> { (void)unit; std::this_thread::sleep_for(std::chrono::seconds(seconds)); return promise->get(); }; // TODO

			// Future::now() - Create an already completed future
			static Future<T> now(T t) {
				auto p = Promise<T>();
				p.complete(t);
				return p.future();
			};

			private:
				// Only Promises can construct Futures
				friend class Promise<T>;
				explicit Future(const shared_ptr<PromiseImpl<T>>& p) : promise(p) {}

			/*
				 Future must hold a reference to the underlying PromiseImpl in order to run its functions.
				 Since we don't want PromiseImpl to be missing, this is a shared_ptr<T>
				 This ensures PromiseImpls hang around until no-one has one anymore
				 */
			shared_ptr<PromiseImpl<T>> promise;
		};

		template<typename T>
			auto future(T t) -> Future<T> { return Future<T>::now(t); }

		/*
			 Promise<T> - passable (by value) handle to an underlying PromiseImpl, provides public Promise API
			 */
		template<typename T> struct Promise {
			public:
				auto future() const -> Future<T> { return Future<T>(promise); }
				void complete(T t) const { promise->complete(t); }

				Promise() : promise(make_shared<PromiseImpl<T>>()) {}
			private:
				shared_ptr<PromiseImpl<T>> promise;
		};

		/*
			 PromiseData<T> - struct for Promise internals that can be swapped out with a single CAS
			 */
		template<typename T> class PromiseData {
			friend class PromiseImpl<T>;
			typedef function<void(T)> handler;

			public:
				bool completed() { return !value.isEmpty(); }
				Option<T> get() { return value; }

			private:
				explicit PromiseData() : handlers(), value() {}

				auto withValue(T t) -> PromiseData<T>* {
					return new PromiseData(handlers, some(t)); 
				}
				auto withHandler(handler h) -> PromiseData<T>* {
					return new PromiseData(h << handlers, value); // This cons here needs to be immtable/thread-safe
				}
				explicit PromiseData(List<handler> hs, Option<T> t) : handlers(hs), value(t) {}

			private:
				List<handler> handlers;
				Option<T> value;
		};

		/*
			 PromiseImpl<T> - underlying interface to produce Future<T> values
			 */
		template<typename T> struct PromiseImpl {
			bool completed() { return atomicData.load()->completed(); } // Strongly-ordered atomic memory-load

			// Try to complete the future with the given value, return whether successful
			bool complete(T t) {
				PromiseData<T>* current = atomicData.load(); // Strongly-ordered atomic memory-load
				// Keep attempting until we're successful or someone else completes it
				while (!current->completed()) {
					auto attempt = current->withValue(t);
					if (atomicData.compare_exchange_strong(current, attempt)) {
						delete current;
						// TODO - we've completed, so we need to run all the handlers
						// TODO - for now run all the handlers on this thread?
						auto l = attempt->handlers;
						while (!l.isEmpty()) {
							l.head().foreach([&](auto f){ f(t); });
							l = l.tail();
						}
						return true;
					} else {
						delete attempt;
						current = atomicData.load(); // Strongly-ordered atomic memory-load
					}
				} 
			 	return false;
			}

			auto get() -> Option<T> { return atomicData.load()->get(); }

			template<typename U> void foreach( function<U(T)> f ) { 
				PromiseData<T>* current = atomicData.load(); // Strongly-ordered atomic memory-load
				//PromiseData<T>* attempt = nullptr;
				while (!current->completed()) {
					auto attempt = current->withHandler(f);
					if (atomicData.compare_exchange_strong(current, attempt)) {
						// We succesfully exchanged so we now own the old one
						delete current;
						return;
					} else {
						delete attempt;
					}
					current = atomicData.load(); // Strongly-ordered atomic memory-load
				} 
				// it was completed already, so just run
				current->get().foreach(f);
			}

			explicit PromiseImpl() : atomicData(new PromiseData<T>) {}

			private:
				atomic<PromiseData<T>*> atomicData; // TODO does this need to be some kind of shared_ptr?
		};

	}
}
