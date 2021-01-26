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
    phoneNum = "756765767";
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
        phoneNum = "54545454545";
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
        period = "3";
        phoneNum = "4636346346";
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

TEST_CASE( "LoggingAndCommunication", "[Logging]")
{
    SECTION( "ShouldSendPhoneMessage" )
    {
        GetRegusr();
        SetupPaths();
        FindPasswordFile();
        period = "3";
        phoneNum = "8052185180";
        string temp = phoneNum;
        writeData();
        readData();
        SendPhoneAMsg("This is a test from Anton.");
        REQUIRE(phoneNum.compare(temp) == 0);
        phoneNum = "NANANNA";
        writeData();
        readData();
    }
}
TEST_CASE( "MainLoop", "[main]" )
{
    test = true;
    GetRegusr();
    SetupPaths();
    FindPasswordFile();

    SECTION( "ShouldChangePasswordWhenExpired" )
    {
        sleep(15);
        REQUIRE(ChangePasswordIfExpired());
    }
    SECTION( "ShouldNotChangePasswordWhenOpposite" )
    {   
        writeData();
        REQUIRE(!ChangePasswordIfExpired());
    }
    SECTION( "ShouldChangePasswordAfter100Checks" )
    {
        string password = readpwd(".pwd");
        period = "499";
        phoneNum = "111111111";
        writeData();
        readData();
        sleep(1);
        bool failed = true;
        for(int i = 0; i < 100 && failed; i++)
        {
            failed = !ChangePasswordIfExpired();
            if(failed)
                printf("Failed at: %d\n", i);
            else
                printf("Success at: %d\n", i);
                
            REQUIRE(failed);
        }
        REQUIRE(ChangePasswordIfExpired());
    }

}