//developed from launcher-usb2_jan16-29.c working version
//added shutdown counter on internet outage
#include "l-u_01-01-21_2.hpp"

using namespace std;

typedef unsigned char byte;
int main()
{
	run();
}

int run()
{
	InitializePaths();
	printf(".llog file path: %s\n", logfile.c_str());

	while(1)
	{
		functionRun();
	}
	return 0;
} 

void functionRun()
{
	int timeout = 0;
	char pidbuf[50];
	//Loop untill actual config file found
	if(!config.compare(""))	//empty string
	{
		locateConfig("/run/media", 0);
		sleep(1);
	}
	else
	{
		currentIP = GetCurrentIP(); 
		cout<<"current IP: "<<currentIP<<endl;
		timeout = 0;

		if(currentIP == "")
		{
			delay = 1;
			//On internet outage 
			if(serverOn)
			{ 	
				if(timeout >= 15);	//inet outage too long, shutting down
					ShutDown("(over 15 sec) internet outage.");

				timeout++;// = timeout + 500;
			}	//end of if(serverOn)
		}
		else //Got IP
		{
			//initiate homeIP on first appearance of internet
			if(init == false)
			{
				homeIP = currentIP;
				init = true;
				cout<<"homeIP initiated as "<<homeIP<<endl;
			}
			//Home IP... 
			if(homeIP == currentIP)
			{
				//sleep delay - 1 sec for HOME IP
				delay = 1;

				//any crazy case if qbittorrent or transmission on when home IP - shot system down
				if(CheckIfPIDExists("qbittorrent"))
				{
					ShutDown("Qbittorrent ON on home IP");
				}

				if(CheckIfPIDExists("pidof transmission-gtk"))
				{
					ShutDown("Transmission ON in home IP");
				}

				//let's run the server
				if(serverOn == false) 
				{
					StartServer();
					WaitForVPNConnection();
				}
				else	//ALERT!!! in home IP with VPN server ON - shut down computer!
				{
					printf("ALERT!!! in home IP with VPN server ON - shut down computer!\n");
					ShutDown("ALERT (server ON in home IP)");
				}
			}
			//in server IP. keep checking IP. (serverIP is assigned in GetCurrentIP())
			if(currentIP == serverIP)
				//Clean up. delete server and autho.txt files from working dir
				if(!cleanedup)
				{
					string rmcmd = "rm autho.txt " + config;
					fp = popen(rmcmd.c_str(), "r");
					pclose(fp);
					cleanedup = true;
				}
				printf("ServerIP is: %s Keep checking...\n", serverIP.c_str());
				delay = 5;	//let's keep it checking every 5 sec
		}		//end of IP else
	}
	sleep(delay);
}
