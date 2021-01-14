#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "l-u_01-01-21_2.hpp"
using namespace std;

void SetupVPNIPAndHomeIP()
{
    InitializePaths();
    InitializeHomeIP("123.456.789.012");
    currentIP = "47.144.17.23";
    WaitForVPNConnection();
}

TEST_CASE( "ShouldSendShellCommand", "[testing]" )
{
    const string text = shellCommand("curl -s http://checkip.dyndns.org/");
    REQUIRE(text.compare("")!= 0);
}
TEST_CASE( "ShouldCheckIfProcessExists", "[testing] ")
{
    REQUIRE(CheckIfPIDExists("systemd"));
}
TEST_CASE( "ShouldStartServer", "[testing]" )
{
    string runcmd = StartServer();
    REQUIRE(runcmd.substr(0,12).compare("sudo openvpn") == 0);
}
TEST_CASE( "ShouldGetCurrentIP", "[testing]" ) 
{
    string ip = GetCurrentIP();
    REQUIRE(ip.compare("") != 0);        
}
TEST_CASE( "ShouldLocateConfig", "[testing]" )
{
    locateConfig("/run/media", 0);
    InitializePaths();
    REQUIRE(configDir.substr(0,14).compare("/run/media/"+user) == 0);
    REQUIRE(entryName.compare("nordvpn_start") == 0);
}
TEST_CASE( "ShouldTurnServerOnAndChangeIP", "[testing]" )
{
    SetupVPNIPAndHomeIP();
    REQUIRE(serverOn);
    REQUIRE(homeIP.compare(serverIP) != 0);
    REQUIRE(serverIP.compare(currentIP) == 0);
}
TEST_CASE( "ShouldShutDownWithNoIP", "[testing]" ) 
{
    SetPaths();
    string ip = GetNoIP();
    REQUIRE(ip.compare("") == 0);    
    ShutDown("(over 15 sec) internet outage.");
    REQUIRE(isShuttingDown);
}
TEST_CASE( "ShouldShutdownIfClientOn", "[testing]" )
{
    if(CheckIfPIDExists("systemd"))
    {
        ShutDown("systemd ON on home IP");
    }
    REQUIRE(isShuttingDown);
}
TEST_CASE( "ShouldTurnServerOnAndCheckVPN", "[testing]" )
{
    SetupVPNIPAndHomeIP();
    for(int i = 0; i < 10; i++)
    {
        printf("Server ip is equal to ");
        IsVPNActive();
    }
    REQUIRE(IsVPNActive());
}
TEST_CASE( "ShouldShutdownServerAfterIPChange", "[testing]")
{
    SetupVPNIPAndHomeIP();
    for(int i = 0; i < 10; i++)
    {
        if(i >= 9)
        {
            homeIP = "47.144.17.23";
            serverIP = "123.456.789.012";
            printf("Home IP is now equal to %s\n", homeIP.c_str());
        }
        else
        {
            printf("Server ip is equal to ");
        }
        
        IsVPNActive();
    }
    REQUIRE(!IsVPNActive());
    REQUIRE(isShuttingDown);
}

/*
TEST_CASE( "ShouldWriteToLog", "[testing]")
{
    SetPaths();
    REQUIRE(WriteToLog("This is a test."));
}


TEST_CASE( "ShouldGetPhone", "[testing]" ) 
{ 
    SetUser();
    cout<<"user: "<<homedir<<endl;
    cout<<"path: " << homedir + "/.cache/xpncnt/expcnt"<<endl;
    SetCntFile(homedir + "/.cache/xpncnt/expcnt");
    string phone = GetPhone();
    cout<<"phone: "<<phone.c_str()<<endl;
    REQUIRE(phone.compare("") != 0);
}*/