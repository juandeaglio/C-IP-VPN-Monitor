#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "l-u_01-01-21_2.hpp"

TEST_CASE( "test", "[testing]" ) 
{
    REQUIRE(GetCurrentIP().compare("") != 0);
}