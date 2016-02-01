// Future.h

/*
   A C++ Monadic future implemtation.

   A Future<T> represents a value of type T that will be ready at some point in the future,
   but may not be ready now.
   */

#pragma once
#include <functional>

using std::function;
using std::atomic;
using std::mutex;

template<typename T> using list = std::forward_list<T>;

template<typename T> struct Future {
	typedef function<void(T)> handler;

	template<typename B> auto 
		map( function<B(T)> f ) -> Future<B> {
			auto next = new Promise<B>;
			foreach{[a]{ next.completeWith(f(a)); };
			return next.future;
		};

	template<typename B> void foreach( function<B(T)> f ) { 
		FutureData<T>*& previous = data.load(); // Strongly-ordered atomic memory-load
		if (previous.value != nullptr) // If we know it's already completed for sure, we can just run our handler with no worries about synchronisation // TODO - possibly run on a different thread
			f(previous.value)
		else {
			FutureData<T>* replaceWith = nullptr;
			do {
				if (previous.value != nullptr) { // it's been completed whilst we were waiting
					f(previous.value);
					if (replaceWith) delete replaceWith;
					break;
				}	
				auto hs = previous.handlers.cons(f);
				if (!replaceWith) replaceWith = new FutureData<T>( hs, previous.value );
				else              replaceWith.set( hs, previous.value );
			} while (!data.compare_exchange_strong(previous, replaceWith))
			// We succesfully exchanged so we now own the old one
			delete previous;
		}
	};

	template<typename B> void 
		andThen( function<B(T)> f ) { foreach(f); };

	// Is the future complete yet?
	auto isComplete() -> bool { value.load() != nullptr; }

	private:
	/*
		atomic<T*> value;
		list<handler> handlers;
		mutex m;
		*/
		atomic<FutureData<T>*> data;
};

template<typename T> class FutureData {
	list<handler> handlers;
	atomic<T*> value;

	FutureData(list<handler> hs, T* v) : handlers(hs), value(v) {}

	void set(list<handler> hs, T* v) {
		handlers = hs;
		value.store(v);
	}
};

template<typename T> struct CompletedFuture : public Future<T> {
};

template<typename T> struct PromiseFuture : public Future<T> {
};

template<typename T> struct Promise {
	PromiseFuture<T> future;

	// Try to complete the future with the given value, return whether successful
	auto completeWith( const T& t ) -> bool {
		lock_guard(m);
		value = t; // set the value in the mutex
		handlers.foreach(run)
	}

	static completed(T t) -> unique_ptr<Promise<T>> {
		auto p = make_unique<Promise<T>>();
		p->completeWith(t);
		p
	}

	private:
		mutex m;
};

