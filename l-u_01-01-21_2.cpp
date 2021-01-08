//developed from launcher-usb2_jan16-29.c working version
//added shutdown counter on internet outage


using namespace std;

typedef unsigned char byte;


int run()
{
	bool init = false;
	bool serverOn = false;
	bool cleanedup = false;
	int delay = 1;	
	string serverFile = "";
	string homeIP;
	string serverIP;
	string workdir;

////////////////////////////////////////////////////////////////////////////////

	//Define main variables - homedir, user etc.
	string output = shellCommand("users");
    const regex vowels("\n+");
	string result = std::regex_replace(output, vowels, "");
	user = result;
	homedir = "home/" + user;
	logfile = homedir + "/.llog";
	cntfile = homedir + "/.cache/xpncnt/expcnt";
	phoneNum = getPhone();
	//cout<<"phoneNum is: "<<phoneNum<<endl;
	char cwd[100];
	char * maindir = getcwd(cwd, sizeof(cwd));
	workdir = maindir;
	printf("current dir is: %s\n", maindir);
	
////////////////////////////////////////////////////////////////////////////////

	printf(".llog file path: %s\n", logfile.c_str());

	int timeout = 0;
	char pidbuf[50];

	//main loop
	while(1)
	{
		//Loop untill actual config file found
		if(!config.compare(""))	//empty string
		{
			locateConfig("/run/media", 0);
			sleep(1);
		}
		else
		{
			string currentIP = GetCurrentIP(); 
			cout<<"current IP: "<<currentIP<<endl;
			timeout = 0;

			if(currentIP == "")
			{
				delay = 1;
				//On internet outage 
				if(serverOn)
				{ 	
					if(timeout >= 15)	//inet outage too long, shutting down
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
					if(shellCommand("pidof qbittorrent") != "")
					{
						ShutDown("Qbittorrent ON on home IP");
					}

					if(shellCommand("pidof transmission-gtk") != "")
					{
						ShutDown("Transmission ON in home IP");
					}

					//let's run the server
					if(serverOn == false) 
					{
						printf("first loop run in home IP. lets start server\n");


						//copy autho.txt file from usb to current working dir
						string line;
						printf("configDir is: %s\n", configDir.c_str());
						ifstream myfile (configDir + "/autho.txt");
						ofstream myfile1 ("autho.txt");

						while (getline(myfile,line))
						{
						  myfile1 << line << '\n';
						}
						myfile.close();
						myfile1.close();

						//copy server file from usb to current working dir
						string configPath = configDir + "/";
						configPath += config;
						ifstream myfile2 (configPath);
						ofstream myfile3 (config);

						while (getline(myfile2,line))
						{
						  myfile3 << line << '\n';
						}
						myfile2.close();
						myfile3.close();
						
						printf("before command\n");
						string runcmd = "sudo openvpn " + config;
						runcmd += " &";	
						cout<<"runcmd is: "<<runcmd<<endl;
						fp = popen(runcmd.c_str(), "r");
						pclose(fp);
						sleep(3); //wait until VPN starts

						//let's check wheather server launch succeeded
						while(1)
						{
							//if(strcmp(homeIP, currentIP) == 0) //still home IP
							if(currentIP == homeIP)
							{
								currentIP = GetCurrentIP();
								sleep(1);
							}
							else	//not home IP = server IP
							{
								serverIP = currentIP;
								printf("server ON. IP is: %s\n", currentIP.c_str());
								serverIP = currentIP;
								serverOn = true;
								printf("server ON. IP is: %s\n", serverIP.c_str());
							
	//"DISPLAY=:0 sudo -u laptop notify-send 'Message'"		

								//notify launch success
								string notify_success = "DISPLAY=:0 sudo -u ";
								notify_success += user;
								notify_success += " notify-send '";
								notify_success += "VPN launch success! IP is: ";								
								notify_success += serverIP;
								notify_success += "' &";

								cout<<notify_success<<endl;
								fp = popen(notify_success.c_str(), "r");
								pclose(fp);
								break;
							}
						}
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
	}			// end of while loop
	return 0;
} 

