// test.cpp
#include "immutable/list.h"
#include "immutable/option.h"
#include "concurrent/future.h"
#include "concurrent/task.h"
#include "concurrent/executor.h"
#include "concurrent/time.h"
#include "functional/monoid.h"
#include <stdio.h>
#include <iostream>

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
using brando::concurrent::ThreadPoolExecutor;

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
}

TEST_CASE( "Futures", "[futures]" ) {
	REQUIRE( Promise<int>().future().completed() == false );
	REQUIRE( Promise<int>().future().get().isEmpty() == true );
	REQUIRE( future(42).get() == some(42) );
	REQUIRE( future(1).await(2, seconds) == some(1) );
	auto inc = function<int(int)>([](int i){ return i + 1; });
	REQUIRE( future(1).map(inc).await(2, seconds) == some(2) );
	REQUIRE( future(1).map(inc).map(inc).map(inc).await(2, seconds) == some(4) );
}

TEST_CASE( "Tasks", "[tasks]" ) {
	auto ex = ThreadPoolExecutor(1);
	REQUIRE( Task<int>([](){ return 42; }).run() == 42 );
	REQUIRE( Task<int>([](){ return 42; }).runAsync(ex).await(2, seconds) == some(42) );
}

TEST_CASE( "Monoid", "[monoids]" ) {
	REQUIRE( sum(nil<int>()) == 0 );
	REQUIRE( sum(nil<double>()) == 0.0 );
	REQUIRE( sum(1 << (2 << (3 << nil<int>()))) == 6 );
	REQUIRE( sum(1.0 << (2.0 << (3.0 << nil<double>()))) == 6.0 );
	REQUIRE( $(none<int>()) == 0 );
	REQUIRE( $(some(1)) == 1 );
}
