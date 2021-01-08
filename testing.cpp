#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE( "test", "[testing]" ) 
{
    REQUIRE(1 != 0);
}