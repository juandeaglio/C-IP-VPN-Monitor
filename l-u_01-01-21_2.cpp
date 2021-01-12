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

		if(currentIP.compare("") == 0)
		{
			delay = 1;
			if(serverOn)
			{ 	
				if(timeout >= 15);
					ShutDown("(over 15 sec) internet outage.");

				timeout++;
			}
		}
		else
		{
			InitializeHomeIP();
			if(homeIP.compare(currentIP) == 0)
			{
				delay = 1;
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
				else
				{
					printf("ALERT!!! in home IP with VPN server ON - shut down computer!\n");
					ShutDown("ALERT (server ON in home IP)");
				}
			}
			if(currentIP == serverIP)
				if(!cleanedup)
				{
					string rmcmd = "rm autho.txt " + config;
					fp = popen(rmcmd.c_str(), "r");
					pclose(fp);
					cleanedup = true;
				}
				printf("ServerIP is: %s Keep checking...\n", serverIP.c_str());
				delay = 5;
		}
	}
	sleep(delay);
}
