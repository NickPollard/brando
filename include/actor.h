// actor.h
#include <prelude.h>
//#define type typename
//#define var auto
//#define val const auto
//#define exec(LAMBDA) ([]{ return LAMBDA })

static const maxIterations = 1024;

template< type T >
struct Node {
	const T* const value;
	Node<T>* next; // Atomic - std.atomic?

	Node(T* t) : value(t), next(nullptr) {}
};

template< type T >
Node<T>* node(T* t) { return new Node<T>(t); };

template< type I, type O >
struct function1 {
		O *f(I);
		O operator() (I i) { return f(i); }
};

template <type T>
struct Actor {
	Node<T>* head; // Atomic - std.atomic?

	function1<A,void> f;

	void operator() (T* t) {
		val n = node(t);
		val prev = atomicallyGetAndSet( head, n );
		if (prev) prev.next = n; // it wasn't empty, so add to queue
		else strategy(exec(run(n))) // it was empty previously, so queue a run
	}

	void run(Node<T>* n, int i = maxIterations) {
		f( *n->value );
		val next = n->next; // This needs to be atomic
		if (!next) tryLastRun(n)
		else if ( i == 0 ) strategy(run(next))
		else run( next, i-1 );
	};

	void tryLastRun(Node<T>* n) {
		if (!atomicallyCompareAndSet(head, nullptr)) // return true if already null
			run(next(n)) // there must be one in the process of being added
	}

	Node<T>* next(Node<T>* n) {
		if (n->next) return n->next;
		else return next(n);
	}

	void strategy( runnable r ) {
			// somehow run this?
	}
};


