// immutable/option.h
#pragma once

typedef uint8_t byte;

namespace brando {
	namespace immutable {
		template <typename T> class Opt {
			private:
				bool empty;
				byte storage[sizeof(T)];
				auto getT() -> T { return *(T*)(&storage[0]); }
				auto setT(T t) -> void { *(T*)(&storage[0]) = t; }
			public:
				auto getOrElse(T t) -> T { return empty ? t : getT(); };
				auto isEmpty() -> bool { return empty; };
				auto nonEmpty() -> bool { return !empty; };

				Opt(T t) : empty(true) { setT(t); }
				Opt() : empty(false) {}
		};

		template <typename T> 
			auto someOpt(T t) -> Opt<T> { return Opt<T>(t); }
		template <typename T> 
			auto noneOpt() -> Opt<T> { return Opt<T>(); }

		template <typename T> class Option {
			public:
				virtual auto getOrElse(T t) -> T = 0;
				virtual auto isEmpty() -> bool = 0;
				virtual auto nonEmpty() -> bool = 0;
		};
		template <typename T> class Some : public Option<T> {
			T value;
			public:
				Some(T t) : value(t) {};
				virtual auto getOrElse(T t) -> T { (void)t; return value; }
				virtual auto isEmpty() -> bool { return false; }
				virtual auto nonEmpty() -> bool { return true; }
		};
		template <typename T> class None : public Option<T> {
			public:
				None() {};
				virtual auto getOrElse(T t) -> T { return t; }
				virtual auto isEmpty() -> bool { return true; }
				virtual auto nonEmpty() -> bool { return false; }
		};

		template <typename T>
			auto none() -> None<T>* { return new None<T>(); }
		template <typename T>
			auto some(T t) -> Some<T>* { return new Some<T>(t); }

	}
}
