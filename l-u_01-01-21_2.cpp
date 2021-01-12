//developed from launcher-usb2_jan16-29.c working version
//added shutdown counter on internet outage
#include "l-u_01-01-21_2.hpp"

using namespace std;

typedef unsigned char byte;
int main()
{
	run();
}
void SetupVPN()
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
	}
}
int run()
{
	InitializePaths();
	printf(".llog file path: %s\n", logfile.c_str());

	if(!config.compare(""))	//empty string
	{
		locateConfig("/run/media", 0);
		sleep(1);
	}
	SetupVPN();
	while(1)
	{
		functionRun();
		sleep(delay);
	}
	return 0;
} 


void functionRun()
{
	int timeout = 0;
	char pidbuf[50];
	//Loop untill actual config file found


	currentIP = GetCurrentIP();
	//if(countTimes >= 10)
	//	currentIP = "";
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
		/*if(countTimes<5)
		{
			currentIP = "123.456.789.012";
		}
		else if(countTimes>=5 && countTimes<10)
		{
			currentIP = homeIP;
		}*/
		
		
		if(currentIP == serverIP)
		{
			if(!cleanedup)
			{
				string rmcmd = "rm autho.txt " + config;
				fp = popen(rmcmd.c_str(), "r");
				pclose(fp);
				cleanedup = true;
			}

		}
		else if(homeIP.compare(currentIP) == 0)
		{
			printf("ALERT!!! in home IP with VPN server ON - shut down computer!\n");
			ShutDown("ALERT (server ON in home IP)");
		}
		countTimes++;
		printf("ServerIP is: %s Keep checking...\n", serverIP.c_str());
		delay = 5;
	}

}
