// traverse.h
#pragma once

template<typename T>
	auto sequenceFutures(List<Future<T>> fs) -> Future<List<T>> {
		fs.foldLeft(future(nil<T>())([](f, ff){
			auto p = Promise<List<T>>()
			// when both f and ff done, complete p
			f.foreach([](l){ ff.foreach([](ll){ p.complete(l << ll); })};
			return p.future();
		});
	};
