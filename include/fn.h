// fn.h
#pragma once
#include <functional>

#include <prelude.h>

/*
	 Fn<A,B> - an opinionated function type from A -> B
	 */
template<typename A, typename B> 
	struct Fn {
		virtual auto apply(A a) -> B = 0;
		virtual auto operator ()(A a) -> B = 0;
	};

template<typename A, typename B> 
	struct FunctionFn : Fn<A,B> {
		private:
			std::function<B(A)> inner;
		public:
			virtual auto apply(A a) -> B { return inner(a); };
			virtual auto operator ()(A a) -> B { return inner(a); };

			FunctionFn(std::function<B(A)> f) : inner(f) {}
	};

template <typename A, typename B>
  auto makeFn(std::function<B(A)> f) -> FunctionFn<A,B> { return FunctionFn<A,B>(f); };

template <typename A, typename B>
  auto makeFunction(bool f) -> std::function<B(A)> { return std::function<B(A)>(f); };
