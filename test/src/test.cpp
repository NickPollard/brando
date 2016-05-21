// test.cpp
#include "immutable/option.h"
#include <stdio.h>

// Create a main() for the catch test library
#define CATCH_CONFIG_MAIN
#include "ext/catch.hpp"

using brando::immutable::some;
using brando::immutable::none;

TEST_CASE( "Option functions", "[option]" ) {
    REQUIRE( some(1).getOrElse(0) == 1 );
    REQUIRE( none<int>().getOrElse(0) == 0 );
}
