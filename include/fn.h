// fn.h
#pragma once
#include <functional>
#include <memory>
#include <prelude.h>

using std::shared_ptr;
using std::make_shared;

namespace brando {
	// *** Forward declaration
	template <typename A, typename B> struct Fn;
	template <typename A, typename B> auto fn(std::function<B(A)> f) -> Fn<A,B>;

	/*
	 	Fn<A,B> - an opinionated function type from A -> B
	
	 	How do we pass [Fn]s around? By value? By reference? By unique/shared?
	 	If Fn wraps a std::function, then it probably has either a pointer to a function object, a function pointer, or an 'inlined' function obj
	
	 	std::function, when copied, may copy the func obj it points to.
	 	
	 	Fn
	 		- on copy: No heap allocations?
				- simple function pointer, is inline so copied by value
				- if heavier - i.e. func object, including lambda - then use a shared reference
	
	 	If we copy, then we have to copy the std::function, or else have some kind of shared handle?
	 	What if FN is just a Shared<function>?
	 	Then Fn can be passed by value, simplifying all Fn taking functions?
	 	*/
	template<typename A, typename B> 
		struct Fn {
			private:
				shared_ptr<std::function<B(A)>> inner;
			public:
				B apply(A a) const { return (*inner)(a); };
				B operator ()(A a) const { return (*inner)(a); };
	
				template<typename C>
					auto andThen(Fn<B,C> g) -> Fn<A,C> {
						auto f = *this;
						return fn(std::function<C(A)>([f,g](A a) { return g(f(a)); }));
				};
			
				Fn(std::function<B(A)> f) : inner(make_shared<std::function<B(A)>>(f)) {}
	
		};
	
	/*
	template<typename A, typename B> 
		struct FunctionFn : Fn<A,B> {
			private:
				std::function<B(A)> inner;
			public:
				virtual auto apply(A a) -> B const { return inner(a); };
				virtual auto operator ()(A a) -> B const { return inner(a); };
	
				FunctionFn(std::function<B(A)> f) : inner(f) {}
		};
  	*/
	
	template <typename A, typename B>
  	auto fn(std::function<B(A)> f) -> Fn<A,B> { return Fn<A,B>(f); };
	
	template <typename A, typename B, typename F>
  	auto fn(F f) -> Fn<A,B> { return Fn<A,B>(std::function<B(A)>(f)); };
	
	template<typename A, typename B, typename C>
		auto andThen(Fn<A,B> f, Fn<B,C> g) -> Fn<A,C> {
			return fn(std::function<C(A)>([f,g](A a) { return g(f(a)); }));
		};
};
