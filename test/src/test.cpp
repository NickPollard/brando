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

#define BRANDO_MAIN
#include "brando.h"

// Create a main() for the catch test library
#define CATCH_CONFIG_MAIN
#include "ext/catch.hpp"

using brando::immutable::some;
using brando::immutable::none;
using brando::immutable::List;
using brando::immutable::list;
using brando::immutable::nil;
using brando::concurrent::future;
using brando::concurrent::Future;
using brando::concurrent::Promise;
using brando::concurrent::seconds;
using brando::concurrent::milliseconds;
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
	REQUIRE( list(1,2,3).size() == 3);
	REQUIRE( list(1,2,3).head() == some(1));
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

ThreadPoolExecutor ex(4);

TEST_CASE( "Futures", "[futures]" ) {
	REQUIRE( Promise<int>(ex).future().completed() == false );
	REQUIRE( Promise<int>(ex).future().get().isEmpty() == true );
	REQUIRE( future(42, ex).get() == some(42) );
	REQUIRE( future(1, ex).await(100, milliseconds) == some(1) );
	auto inc = function<int(int)>([](int i){ return i + 1; });
	REQUIRE( future(1, ex).map(inc).await(100, milliseconds) == some(2) );
	REQUIRE( future(1, ex).map(inc).map(inc).map(inc).await(100, milliseconds) == some(4) );
}

TEST_CASE( "Sequence", "[sequence]" ) {
  auto l = list(future(1, ex), future(2, ex), future(3, ex));
	REQUIRE( sequenceFutures(l, ex).await(100, milliseconds) == some(list(1,2,3)) );
}

TEST_CASE( "Tasks", "[tasks]" ) {
	auto printInt = function<void(int)>([](int i){ printf("Result is %d.\n", i); });
	auto inc = function<int(int)>([](int i){ return i + 1; });
	REQUIRE( Task<int>([]{ return 42; }).run() == 42 );
	REQUIRE( Task<int>([]{ return 42; }).runAsync(ex).await(100, milliseconds) == some(42) );
	REQUIRE( Task<int>([]{ return 42; }).runAsync(ex).map(inc).await(100, milliseconds) == some(43) );

	Task<int>([]{ sleep_for(std::chrono::milliseconds(100)); return 1; }).runAsync(ex).foreach(printInt);
	Task<int>([]{ return 2; }).runAsync(ex).foreach(printInt);
	Task<int>([]{  sleep_for(std::chrono::milliseconds(200)); return 3; }).runAsync(ex).foreach(printInt);

	async( ex, {sleep_for(std::chrono::milliseconds(100)); return 5;} ).foreach(printInt);

	Task<int>([]{ return 4; }).runAsync(ex).foreach(printInt);

	sleep_for(std::chrono::milliseconds(300));
}

TEST_CASE( "defer", "[defer]" ) {
	REQUIRE( defer( return 42 ).run() == 42 );
	REQUIRE( defer( return 2.5 + 2.5 ).run() == 5.0 );
	REQUIRE( defer( return strlen("Hello") ).run() == 5 );
	REQUIRE( strlen( defer( return "Hello" ).run()) == 5 );
}

TEST_CASE( "Monoid", "[monoids]" ) {
	REQUIRE( sum(nil<int>()) == 0 );
	REQUIRE( sum(nil<double>()) == 0.0 );
	REQUIRE( sum(list(1,2,3)) == 6 );
	REQUIRE( sum(list(1.0,2.0,3.0)) == 6.0 );
	REQUIRE( $(none<int>()) == 0 );
	REQUIRE( $(some(1)) == 1 );
}
