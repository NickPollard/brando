// immutable/list.h
#pragma once

namespace brando {
	namespace immutable {
		template <typename T> class List {
			public:
				virtual auto head() -> Option<T>;
				virtual auto tail() -> Option<List<T>>;
				virtual auto size() -> int;
		};
		template <typename T> class Cons : public List<T> {
			T head;
			List<T> tail;
			public:
				virtual auto head() -> Option<T> { return Some(head); }
				virtual auto tail() -> Option<List<T>> { return Some(tail); }
				virtual auto size() -> int { return 1 + tail.size() }
		};
		template <typename T> class Nil : public List<T> {
			virtual auto head() -> Option<T> = { return none<T>(); }
			virtual auto tail() -> Option<List<T>> { return none<List<T>>(); }
			virtual auto size() -> int;
		};

		template <typename T>
			auto operator << (T, List<T>) -> List<T> {};
	}
}
