#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "l-u_01-01-21_2.hpp"
using namespace std;

TEST_CASE( "ShouldGetCurrentIP", "[testing]" ) 
{
    string ip = GetCurrentIP();
    cout<<"current IP: "<<ip<<endl;
    REQUIRE(ip.compare("") != 0);        
}
TEST_CASE( "ShouldShutDownWithNoIP", "[testing]" ) 
{
    SetPaths();
    cout<<"logfile: " << logfile<<endl;
    string ip = GetNoIP();
    cout<<"current IP: "<<ip<<endl;
    REQUIRE(ip.compare("") == 0);    
    ShutDown("(over 15 sec) internet outage.");
    REQUIRE(isShuttingDown);
}
TEST_CASE( "ShouldLocateConfig", "[testing]" )
{
    locateConfig("/run/media", 0);
    cout<<"ConfigDir: "<<configDir<<"entryName: "<<endl<<entryName<<endl;
    REQUIRE(configDir.compare("")!= 0);
    REQUIRE(entryName.compare("")!= 0);
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
TEST_CASE( "ShouldStopWaitingForConnectionWhenCurrentIPChanges", "[testing]" )
{
    homeIP = "47.144.17.23";
    currentIP = "123.456";
    
    WaitForVPNConnection();
    REQUIRE(serverOn);
    REQUIRE(homeIP.compare(serverIP) != 0);
    REQUIRE(serverIP.compare(currentIP) == 0);
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