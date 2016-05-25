// test.cpp
#include "immutable/option.h"
#include "concurrent/future.h"
#include <stdio.h>

// Create a main() for the catch test library
#define CATCH_CONFIG_MAIN
#include "ext/catch.hpp"

using brando::immutable::some;
using brando::immutable::none;
using brando::concurrent::Future;
using brando::concurrent::Promise;

TEST_CASE( "Option functions", "[option]" ) {
  REQUIRE( some(1).getOrElse(0) == 1 );
  REQUIRE( none<int>().getOrElse(0) == 0 );
}

TEST_CASE( "Futures", "[futures]" ) {
		REQUIRE( Promise<int>::make().get() != nullptr );
		REQUIRE( Promise<int>::make()->future().completed() == false );

//	REQUIRE( Future<int>::now(2).completed() == true );
//	REQUIRE( Future<int>::now(3).tryAwait() == some(3) );
//	REQUIRE( Future<int>::now(3).map(std::function<int(int)>([](auto i){ return i + 1; })).tryAwait() == some(4) );
}
