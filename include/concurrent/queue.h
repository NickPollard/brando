// queue.h
#pragma once

#include <cassert>
#include <mutex>
#include <queue>
#include <iostream>

using std::queue;
using std::lock_guard;
using std::mutex;

namespace brando {
	namespace concurrent {

		template <typename T>
			struct ConcurrentQueue {
				auto pop() -> Option<T> { 
					// Double-checked locking - we need to guarantee when we're in here that we *do* have a value
					lock_guard<mutex> lock(m);
					if (storage.empty()) {
						return none<T>();
					}
					else {
						auto t = some(storage.front()); // Invokes copy constructor on t
						storage.pop();
						return t;
					}
				} // Blocking pop

				void push(T t) { 
					lock_guard<mutex> lock(m);
					storage.push(t);
				} // Blocking push

				auto isEmpty() -> bool { 
					lock_guard<mutex> lock(m);
					return storage.empty();
				}

				ConcurrentQueue() : storage() { 
				 	lock_guard<mutex> lock(m);
			 	}

				private:
					mutex m;
					queue<T> storage;
			};

	}
}
