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

#include "immutable/option.h"
#include "immutable/list.h"

using std::function;
using std::atomic;
using std::mutex;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::move;
using brando::immutable::Option;
using brando::immutable::none;
using brando::immutable::List;

namespace brando {
	namespace concurrent {

		template<typename T> struct Promise;
		template<typename T> struct Future;

		/*
			 Data struct for Future internals that can be swapped out with a single CAS
			 */
		template<typename T> class FutureData {
			typedef function<void(T)> handler;
			public:
				explicit FutureData(shared_ptr<List<handler>> hs, unique_ptr<T>&& t) : handlers(hs), value(move(t)) {}

				auto withValue(T t) -> FutureData<T>* {
					return new FutureData(handlers, make_unique<T>(t)); 
				}
				auto addHandler(handler h) -> FutureData<T>* {
					return new FutureData(h << handlers, value); // This cons here needs to be immtable/thread-safe
				}

				bool completed() { return value.get() != nullptr; }
				T* get() { return value; }

				/*
				void set(shared_ptr<List<handler>> hs, T* t) {
					handlers = hs;
					value.store(t);
				}
				*/

			private:
				shared_ptr<List<handler>> handlers;
				unique_ptr<T> value;
		};

		/*
			 Future<T> - a type that will, at some point, contain a value of type T, but may
			 not at present. Futures can be used to implement concurrency, and can be used
			 monadically to compose sequential asynchronous computations.

			 Futures are, underneath, a shared_ptr<Promise<T>>, with the Promise<T> allowing
			 the eventual T to be supplied
			 */
		template<typename T> struct Future {
			template<typename U> auto map( function<U(T)> f ) -> Future<U> {
				(void)f;
				auto next = new Promise<U>;
				//foreach{[a]{ next.completeWith(f(a)); }};
				return next->future();
			};
//			template<typename u, typename f> auto map( f f ) -> future<u> {
//				auto next = new promise<u>;
//				//foreach{[a]{ next.completewith(f(a)); }};
//				return next.future();
//			};

			explicit Future(Promise<T>* p) : promise(p) {}

			template<typename B> void foreach( function<B(T)> f ) { promise.foreach(f); };

			template<typename B> void andThen( function<B(T)> f ) { foreach(f); };

			auto completed() -> bool { return promise->completed(); }

			auto tryAwait() -> Option<T> { return none<T>(); }; // TODO

			// now() - Create an already completed future
			static Future<T> now(T t) {
				auto p = Promise<T>::make();
				p->complete(t);
				return p->future();
			};

			private:
			/*
				 Future must hold a reference to the underlying promise in order to run its functions

				 Since we don't want promise to be missing, this should be a shared_ptr<> ?
				 That means promises hang around until no-one has one anymore
				 Future is largely a wrapper around a shared_ptr<Promise<T>>

				 How we do we get futures from the promise? I guess we always want to deal with shared_ptr<> to promises, so that
				 it doesn't get revoked
				 which is the future - if we have one future we can make more
				 */
			shared_ptr<Promise<T>> promise;
		};

		/*
			 Promise<T> - underlying interface to produce Future<T> values
			 */
		template<typename T> struct Promise {
			static auto make() -> shared_ptr<Promise<T>> { return make_shared<Promise<T>>(); }

			bool completed() { return false; } // Strongly-ordered atomic memory-load
//			bool completed() { return atomicData.load()->completed(); } // Strongly-ordered atomic memory-load

			// Try to complete the future with the given value, return whether successful
			bool complete(T t) {
				FutureData<T>* current = atomicData.load(); // Strongly-ordered atomic memory-load
				// Keep attempting until we're successful or someone else completes it
				while (!current->completed()) {
					auto attempt = current->withValue(t);
					if (atomicData.compare_exchange_strong(current, attempt)) {
						delete current;
						// TODO - we've completed, so we need to run all the handlers
						return true;
					} else {
						delete attempt;
						current = atomicData.load(); // Strongly-ordered atomic memory-load
					}
				} 
			 	return false;
			}

			auto future() -> Future<T> { return Future<T>(this); }

			template<typename B> void foreach( function<B(T)> f ) { 
				FutureData<T>* current = atomicData.load(); // Strongly-ordered atomic memory-load
				FutureData<T>* attempt = nullptr;
				while (!current->completed()) {
					auto attempt = current->addHandler(f);
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
				f(current->get());
			}
			explicit Promise() {}

			private:
				atomic<FutureData<T>*> atomicData; // TODO does this need to be some kind of shared_ptr?
		};
	}
}
