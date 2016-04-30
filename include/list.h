// list.h
#pragma once
#include <memory>

/* 
	 An immutable, refcounted, singly-linked list
	 */

using std::shared_ptr;

template <typename T> struct List;

template <typename T> struct ListElem {
		T head;
		shared_ptr<List<T>> tail;
		ListElem(T h, shared_ptr<List<T>> t) : head(h), tail(t) {}
};

template <typename T> struct List {
	bool isEmpty() { return elem == nullptr; }
	bool nonEmpty() { return elem != nullptr; }

	List<T> tail() { return elem->tail; }
	T head() { return elem->head; }
	
	static auto empty() -> shared_ptr<List<T>> { return std::make_shared<List<T>>(nullptr); }
	
	static auto cons(T head, shared_ptr<List<T>> tail) -> shared_ptr<List<T>> {
			auto l = std::make_shared<List<T>>(new ListElem<T>(head, tail));
			return l;
	}

	private:
		ListElem<T>* elem;
		
	public:
		List(ListElem<T>* e) : elem(e) {}
};
