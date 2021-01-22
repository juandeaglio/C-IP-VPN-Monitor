#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "passwordmanager.hpp"
using namespace std;

TEST_CASE( "ShouldFindPasswordFile", "[utility]" )
{	
    string passwordFile = FindPasswordFile();
    REQUIRE(passwordFile.compare("") != 0);
}
TEST_CASE( "PasswordManagingFunctions", "[manager]")
{
    SetupPaths();
    FindPasswordFile();
    
    SECTION( "ShouldWriteAndReadData" )
    {
        period = "10000000000";
        phoneNum = "8004003000";
        string temp1 = period;
        string temp2 = phoneNum;
        writeData();
        readData();
        REQUIRE(temp1.compare(period) == 0);
        REQUIRE(temp2.compare(phoneNum) == 0);
        printf("timeline: %s\n",timeline);
        REQUIRE(timeline.compare("") != 0);
    }
}