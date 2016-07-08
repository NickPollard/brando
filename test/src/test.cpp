// test.cpp
#include "immutable/list.h"
#include "immutable/option.h"
#include "concurrent/future.h"
#include "concurrent/task.h"
#include "concurrent/executor.h"
#include "concurrent/time.h"
#include "functional/monoid.h"
#include "functional/traverse.h"

#include <stdio.h>
#include <iostream>
#include <thread>

// Create a main() for the catch test library
#define CATCH_CONFIG_MAIN
#include "ext/catch.hpp"

using brando::immutable::some;
using brando::immutable::none;
using brando::immutable::List;
using brando::immutable::nil;
using brando::concurrent::future;
using brando::concurrent::Future;
using brando::concurrent::Promise;
using brando::concurrent::seconds;
using brando::concurrent::Task;
using brando::concurrent::task;
using brando::concurrent::ThreadPoolExecutor;
using brando::functional::sequenceFutures;
using std::this_thread::sleep_for;

TEST_CASE( "Option functions", "[option]" ) {
  REQUIRE( some(1).getOrElse(0) == 1 );
  REQUIRE( none<int>().getOrElse(0) == 0 );
}

TEST_CASE( "Lists", "[lists]" ) {
	REQUIRE( List<int>::empty().head().isEmpty() == true );
	REQUIRE( nil<int>().head().isEmpty() == true );
	REQUIRE( nil<int>().isEmpty() == true );
	REQUIRE( (1 << nil<int>()).head().isEmpty() == false );
	REQUIRE( (1 << nil<int>()).head() == some(1) );
	REQUIRE( (1 << (2 << nil<int>())).head() == some(1) );
	REQUIRE( (1 << (2 << nil<int>())).tail().head() == some(2) );
	REQUIRE( (1 << nil<int>()).tail().tail().head().isEmpty() == true );
	REQUIRE( (1 << nil<int>()).size() == 1 );
	REQUIRE( (1 << (2 << nil<int>())).foldLeft(0, function<int(int, int)>([](int a, int b){ return a + b; })) == 3 );
}

TEST_CASE( "Futures", "[futures]" ) {
	REQUIRE( Promise<int>().future().completed() == false );
	REQUIRE( Promise<int>().future().get().isEmpty() == true );
	REQUIRE( future(42).get() == some(42) );
	REQUIRE( future(1).await(0, seconds) == some(1) );
	auto inc = function<int(int)>([](int i){ return i + 1; });
	REQUIRE( future(1).map(inc).await(0, seconds) == some(2) );
	REQUIRE( future(1).map(inc).map(inc).map(inc).await(0, seconds) == some(4) );
}

TEST_CASE( "Sequence", "[sequence]" ) {
	REQUIRE( sequenceFutures(future(1) << (future(2) << (future(3) << nil<Future<int>>()))).await(0, seconds) == some(1 << (2 << (3 << nil<int>()))) );
}

ThreadPoolExecutor ex(4);
TEST_CASE( "Tasks", "[tasks]" ) {
	auto printInt = function<void(int)>([](int i){ printf("Result is %d.\n", i); });
	auto inc = function<int(int)>([](int i){ return i + 1; });
	REQUIRE( Task<int>([]{ return 42; }).run() == 42 );
	REQUIRE( defer( return 42 ).run() == 42 );
	REQUIRE( defer( return 2.5 + 2.5 ).run() == 5.0 );
	REQUIRE( defer( return strlen("Hello") ).run() == 5 );
	REQUIRE( strlen( defer( return "Hello" ).run()) == 5 );
	REQUIRE( Task<int>([]{ return 42; }).runAsync(ex).await(1, seconds) == some(42) );
	REQUIRE( Task<int>([]{ return 42; }).runAsync(ex).map(inc).await(1, seconds) == some(43) );

	Task<int>([]{ sleep_for(std::chrono::seconds(1)); return 1; }).runAsync(ex).foreach(printInt);
	Task<int>([]{ return 2; }).runAsync(ex).foreach(printInt);
	Task<int>([]{  sleep_for(std::chrono::seconds(2)); return 3; }).runAsync(ex).foreach(printInt);

	async( ex, {sleep_for(std::chrono::seconds(1)); return 5;} ).foreach(printInt);

	Task<int>([]{ return 4; }).runAsync(ex).foreach(printInt);

	sleep_for(std::chrono::seconds(3));
}

TEST_CASE( "Monoid", "[monoids]" ) {
	REQUIRE( sum(nil<int>()) == 0 );
	REQUIRE( sum(nil<double>()) == 0.0 );
	REQUIRE( sum(1 << (2 << (3 << nil<int>()))) == 6 );
	REQUIRE( sum(1.0 << (2.0 << (3.0 << nil<double>()))) == 6.0 );
	REQUIRE( $(none<int>()) == 0 );
	REQUIRE( $(some(1)) == 1 );
}
