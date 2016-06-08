// monoid.h
#pragma once

template<typename T> struct not_void {
	static const bool value = true;
};

template<> struct not_void<void> {
	static const bool value = false;
};

template<typename T> struct Monoid {
	static auto zero() -> void;
};

template<> struct Monoid<int> {
	static auto append(int a, int b) { return a + b; }
	static auto zero() { return 0; }
};

template<> struct Monoid<double> {
	static auto append(double a, double b) { return a + b; }
	static auto zero() { return 0; }
};

template<typename T>
	auto sum(List<T> ts) -> T {
		//static_assert(Monoid<T>::exists, "Cannot sum; Type does not have a Monoid instance");
		static_assert(not_void<decltype(Monoid<T>::zero())>::value, "Cannot sum; Type does not have a Monoid instance");
		(void)ts;
		//return ts.foldLeft(Monoid<T>::zero(), Monoid<T>::append);

		T t(Monoid<T>::zero());
		while (!ts.isEmpty()) {
			ts.head().foreach(function<void(T)>([&](T tt){ t = Monoid<T>::append(t, tt); }));
			ts = ts.tail();
		}

		return t;
	};

