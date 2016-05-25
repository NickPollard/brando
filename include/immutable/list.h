// immutable/list.h
#pragma once
#include "immutable/option.h"
#include <memory>

using std::shared_ptr;

/*
	 Immutable list

	 Use shared_ptr for structural sharing - allowing us to do efficient copying operations, with automatic cleanup after

	 How to implement? Inheritance? Composition? Sub element class?
	 Can we limit people to only ever handling access to the shared_ptr handle? (Yes?)
	 */

namespace brando {
	namespace immutable {

	/*
		 Value-semantics wrapper around a shared_ptr to a list;
		 Hide implementation of List (so shared_ptr semantics enforced)
		 */

	template<typename T> class ListImpl;

	template<typename T>
	struct List {
		public:
			auto head() -> Option<T> { return inner->head(); }
			auto tail() -> List<T> { return inner->tail(); }
			auto isEmpty() -> bool { return inner->_head.isEmpty(); }
			auto size() -> int { return (isEmpty() ? 0 : 1 + tail().size()); }

			static List empty() { return List(new ListImpl<T>()); }
			
		public:
			List(T head, List<T> tail): inner(new ListImpl<T>(head, tail)) {}
		private: 
			List(ListImpl<T>* ptr) : inner(ptr) {}

			shared_ptr<ListImpl<T>> inner;
						 /*
								What operations to support?
								Map -> List<U>
								Foreach -> Unit
								size() -> Int
								zip -> List<T,U>
								zipWithIndex -> List<T,Int>
								take -> List
								drop -> List
								*/
	};

	template<typename T>
			auto operator <<(T head, List<T> tail) -> List<T> { 
				return List<T>(head, tail);
			}

	template<typename T>
	struct ListImpl {
		friend class List<T>; // List needs to be able to access private members in the impl
		public:
			auto head() -> Option<T> { return _head; }
			auto tail() -> List<T> { return _tail.getOrElse(List<T>::empty()); }

			ListImpl() : _head(none<T>()), _tail(none<List<T>>()) {}
			ListImpl(T t, List<T> tail) : _head(some(t)), _tail(some(tail)) {}
		private:
			Option<T> _head;
			Option<List<T>> _tail;
	};

	template<typename T>
		auto nil() -> List<T> { return List<T>::empty(); }
	}
}
