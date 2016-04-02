// queue.h
#include <list> 

namespace brando {
	template<typename T>
	 	struct Queue {
			std::list<T> in;
			std::list<T> out;
			auto enqueue(T t) -> void { (in.size() == 0 ? in : out).push_front(t); } // Is there a list.empty()?
			auto operator += (T t) -> void { enqueue(t); }
			auto head() -> T { return out.front(); }
			auto tail() -> Queue<T> { }
			auto empty() -> bool { return false; /* TODO */ }
		};
};
