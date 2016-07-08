// immutable/option.h
#pragma once
#include <stdint.h>
#include <functional>
#include <iostream>

using std::function;
using std::ostream;

typedef uint8_t byte;

namespace brando {
	namespace immutable {
		template <typename T> class Option {
			private:
				bool empty;
				byte storage[sizeof(T)];
				auto getT() const -> T         { return *(T*)(&storage[0]); }
				//auto setT(T t) -> void   { *(T*)(&storage[0]) = t; }
				auto setT(T t) -> void   { new ((T*)(&storage[0])) T(t); } // Placement new a T in `storage` using the copy constructor
			public:
				auto getOrElse(T _else) -> T { return empty ? _else : getT(); };
				auto isEmpty() -> bool { return empty; };
				auto nonEmpty() -> bool { return !empty; };
				
				template<typename U>
					void foreach(function<U(T)> f ) { if (!empty) f(getT()); }

				template<typename F> void foreach( F f ) { if (!empty) f(getT()); };
				//auto map()
				//auto flatMap()

				template<typename U>
					auto fold(function<U()> noneF, function<U(T)> someF) const -> U {
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

		template <typename T>
			ostream& operator << ( ostream& os, Option<T> const& value ) {
				value.fold(function<void()>([&]{ os << "None"; }),
									 function<void(T)>([&](T t){ os << "Some(" << t << ")"; }));
				return os;
			}
	}
}
