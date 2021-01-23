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
    GetRegusr();
    SetupPaths();
    FindPasswordFile();
    period = "100000000000";
    phoneNum = "9004003000";
    SECTION( "ShouldReadData" )
    {
        readData();
        REQUIRE(period.compare("") != 0);
        REQUIRE(phoneNum.compare("") != 0);
        REQUIRE(timeline.compare("") != 0);
    }
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
        REQUIRE(timeline.compare("") != 0);
    }
    SECTION( "ShouldReadOldDataAndWriteNewDataAndReadNewData" )
    {
        readData();
        string temp3 = timeline;
        string temp1 = period;
        string temp2 = phoneNum;
        period = "50000000000";
        phoneNum = "7004003000";
        sleep(5);
        writeData();
        readData();
        REQUIRE(temp1.compare(period) != 0);
        REQUIRE(temp2.compare(phoneNum) != 0);
        REQUIRE(timeline.compare(temp3) != 0);
    }
}