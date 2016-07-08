// traverse.h
#pragma once

#include "concurrent/future.h"
#include "immutable/list.h"

namespace brando {
	namespace functional {

		using concurrent::future;
		using concurrent::Future;
		using concurrent::Promise;
		using immutable::List;
		using immutable::nil;

		template<typename T>
			auto sequenceFutures(List<Future<T>> fs) -> Future<List<T>> {
				auto initial = future(nil<T>());
				auto fn = function<Future<List<T>>(Future<T>, Future<List<T>>)>([=](Future<T> h, Future<List<T>> t){
					auto p = Promise<List<T>>();
					// when both t and h done, complete p
					t.foreach(function<void(List<T>)>([=](List<T> tail){ h.foreach(function<void(T)>([=](T head){ p.complete(head << tail); })); }));
					return p.future();
				});
				return fs.foldRight(initial, fn);
			};

	}
}
