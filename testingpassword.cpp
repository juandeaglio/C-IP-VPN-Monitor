#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "passwordmanager.hpp"
using namespace std;

TEST_CASE( "UtilityFunctions", "[utility]" )
{	
    test = true;
    SECTION( "ShouldFindPasswordFile" )
    {
        string passwordFile = FindPasswordFile();
        REQUIRE(passwordFile.compare("") != 0);
    }
    SECTION( "ShouldGetRegUsr" )
    {
        REQUIRE(GetRegusr().compare("") != 0);
        REQUIRE(strcmp("f32", regusr) == 0);
        REQUIRE(strcmp("", regusr) != 0);
    }
}
TEST_CASE( "PasswordManagingFunctions", "[manager]")
{
    test = true;
    GetRegusr();
    SetupPaths();
    FindPasswordFile();
    period = "1";
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
        period = "2";
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
        period = "5";
        phoneNum = "7004003000";
        sleep(5);
        writeData();
        readData();
        REQUIRE(temp1.compare(period) != 0);
        REQUIRE(temp2.compare(phoneNum) != 0);
        REQUIRE(timeline.compare(temp3) != 0);
    }
    SECTION( "ShouldReadPassword" )
    {
        string password = readpwd(".pwd");
        REQUIRE(password.compare("") != 0);
    }
    SECTION( "ShouldGenerateRandomPassword" )
    {
        string password = readpwd(".pwd");
        string random = GenerateRandomPassword();
        REQUIRE(password.compare(random) != 0);
        REQUIRE(random.compare("") != 0);
    }
}
TEST_CASE( "MainLoop", "[main]" )
{
    REQUIRE(ChangePasswordIfExpired());
}