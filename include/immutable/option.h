// immutable/option.h
#pragma once
#include <stdint.h>
#include <functional>

using std::function;

typedef uint8_t byte;

namespace brando {
	namespace immutable {
		template <typename T> class Option {
			private:
				bool empty;
				byte storage[sizeof(T)];
				auto getT() -> T         { return *(T*)(&storage[0]); }
				//auto setT(T t) -> void   { *(T*)(&storage[0]) = t; }
				auto setT(T t) -> void   { new ((T*)(&storage[0])) T(t); } // Placement new a T in `storage` using the copy constructor
			public:
				auto getOrElse(T _else) -> T { return empty ? _else : getT(); };
				auto isEmpty() -> bool { return empty; };
				auto nonEmpty() -> bool { return !empty; };
				
				//auto foreach()
				//auto map()
				//auto flatMap()

				template<typename U>
				auto fold(function<U()> noneF, function<U(T)> someF) -> U {
					if (empty)
						return noneF();
					else
						return someF(getT());
				}
				bool operator ==(Option<T> other) {
					return (empty == other.isEmpty() && (empty || getT() == other.getT()));
				}

				Option(T t) : empty(false) { setT(t); }
				Option() : empty(true) {}
		};

		template <typename T> 
			auto some(T t) -> Option<T> { return Option<T>(t); }
		template <typename T> 
			auto none() -> Option<T> { return Option<T>(); }
	}
}
