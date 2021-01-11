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

TEST_CASE( "ShouldGetPhone", "[testing]" ) 
{
    SetCntFile(homedir + "/.cache/xpncnt/expcnt");
    string phone = GetPhone();
    cout<<"phone: "<<phone.c_str()<<endl;
    REQUIRE(phone.compare("") != 0);
}