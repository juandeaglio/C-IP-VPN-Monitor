#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <regex>
#include <libgen.h>
#include <sys/types.h>
#include <dirent.h>
#include <future>
#include <thread>
#include <chrono>

using namespace std;

typedef unsigned char byte;

bool isShuttingDown = false;
FILE *fp;
string homedir;
string user;
string logfile;
string cntfile;
string configDir = "";
string entryName = "";
string creds = "";
string config = "";
int i = 0;
string susbdir;
string phoneNum;
char* c_time_string;
string workdir;
bool init = false;
bool serverOn = false;
bool cleanedup = false;
int delay = 1;	
string serverFile = "";
string homeIP;
string serverIP;
string currentIP;

int countTimes = 0;
void NotifySend(string message)
{
	string notify_success = "DISPLAY=:0 sudo -u ";
	notify_success += user;
	notify_success += " notify-send '";
	notify_success += message;
	notify_success += "' &";

	cout<<notify_success<<endl;
	fp = popen(notify_success.c_str(), "r");
	pclose(fp);
}
string shellCommand(string cmd) 
{
    string data;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    fp = popen(cmd.c_str(), "r");
    if (fp) 
	{
		while (!feof(fp))
		if (fgets(buffer, max_buffer, fp) != NULL) data.append(buffer);
		pclose(fp);
    }
    return data;
}
string GetWebRequest(string website)
{
	return shellCommand("curl -s " + website);
}
void ShutDown(string cmd)
{
	string phonemsg;
	string logtext;

	logtext = "Poweroff called due to ";
	logtext += cmd;

	phonemsg = "echo ";
	phonemsg += "'Poweroff called due to ";
	phonemsg += cmd;
	phonemsg += "' | ssmtp ";//8054276925@txt.att.net";
	phonemsg +=  phoneNum;
	phonemsg +=  "@txt.att.net";

	fp = popen(phonemsg.c_str(), "r");
	pclose(fp);

	sleep(1);

	isShuttingDown = true;
}
bool WriteToLog(string logtext)
{	
	time_t current_time;
	char* c_time_string;
	current_time = time(NULL);
	c_time_string = ctime(&current_time);
	logtext += c_time_string;
	fp = fopen(logfile.c_str(), "a");
	int status = fprintf(fp, logtext.c_str());
	fclose(fp);
	return status >= 0;
}
string GetCurrentIP()
{
	const string text = GetWebRequest("http://checkip.dyndns.org/");

    const regex vowels("[a-zA-Z<>/:'\r\n' ]");
    stringstream result;

    regex_replace(ostream_iterator<char>(result), text.begin(), text.end(), vowels, "");

	return result.str();
}
string GetNoIP()
{
	return "";
}

void locateConfig(const char *name, int indent)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) 
	{
		char path[1024];
        if (entry->d_type == DT_DIR) 
		{
            //char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            //printf("%*s[%s]\n", indent, "", entry->d_name);
			if(!strcmp(entry->d_name, "nordvpn_start"))
			{
				configDir = path;
				entryName = entry->d_name;
				
            	locateConfig(path, indent + 2);
				break;
			}
            locateConfig(path, indent + 2);
        } 
		else 
		{
			if(entryName == "nordvpn_start")
			{
				//list files in directory
				if(!i)
					creds = entry->d_name;
				else
					config = entry->d_name;	
				i++;			
				//cout<<entry->d_name<<endl;
			}
        }
    }
    closedir(dir);
}

string GetPhone()
{
	int i = 0;
	string line;
	string number;

	//reading from expcnt file
	ifstream file(cntfile);
	while(getline(file, line)) 
	{
		if(i == 2)
			number = line;
		i++;
	}
	file.close();
	return number;
}
string GetUsers()
{
 	return shellCommand("users");
}
bool CheckIfPIDExists(string processName)
{
	return shellCommand("pidof qbittorrent").compare("") == 0;
}
void SetCntFile(string fileDir)
{
	cntfile = fileDir;
}
void InitializePaths()
{
	string output = GetUsers();
    const regex vowels("\n+");
	string result = std::regex_replace(output, vowels, "");
	user = result;
	homedir = "home/" + user;
	logfile = homedir + "/.llog";
	SetCntFile(homedir + "/.cache/xpncnt/expcnt");
	phoneNum = GetPhone();
	//cout<<"phoneNum is: "<<phoneNum<<endl;
	char cwd[100];
	char * maindir = getcwd(cwd, sizeof(cwd));
	workdir = maindir;
	printf("current dir is: %s\n", maindir);
}
string StartServer()
{
	printf("first loop run in home IP. lets start server\n");
	string line;
	ifstream myfile (configDir + "/autho.txt");
	ofstream myfile1 ("autho.txt");

	while (getline(myfile,line))
	{
		myfile1 << line << '\n';
	}
	myfile.close();
	myfile1.close();

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
	return runcmd;
}
bool WaitForVPNConnection()
{
	while(!serverOn)
	{
		string currentIP = GetCurrentIP();
		if(currentIP.compare(homeIP) != 0)
		{
			string ip = currentIP;
			serverIP = ip;
			serverOn = true;
			NotifySend("VPN launch success! IP is: " + serverIP);
			return true;
		}
	}
	return false;
}
void InitializeHomeIP()
{
	if(init == false)
	{
		homeIP = GetCurrentIP();
		init = true;
		cout<<"homeIP initiated as "<<homeIP<<endl;
	}
}
void InitializeHomeIP(string ip)
{
	if(init == false)
	{
		if(ip.compare("") != 0)
		{
			homeIP = ip;
			init = true;
			cout<<"homeIP initiated as "<<homeIP<<endl;
		}
	}
}
bool IsVPNActive()
{
	int timeout = 0;
	char pidbuf[50];
	currentIP = GetCurrentIP();
	cout<<"current IP: "<<currentIP<<endl;
	timeout = 0;
	if(currentIP.compare("") == 0)
	{
		delay = 1;
		if(serverOn)
		{ 	
			if(timeout >= 15);
			{
				ShutDown("(over 15 sec) internet outage.");
				return false;
			}
			timeout++;
		}
	}
	else
	{	
		if(currentIP == serverIP)
		{
			if(!cleanedup)
			{
				string rmcmd = "rm autho.txt " + config;
				fp = popen(rmcmd.c_str(), "r");
				pclose(fp);
				cleanedup = true;
			}
			return true;
		}
		else if(homeIP.compare(currentIP) == 0)
		{
			printf("ALERT!!! in home IP with VPN server ON - shut down computer!\n");
			ShutDown("ALERT (server ON in home IP)");
			return false;
		}
		countTimes++;
		printf("ServerIP is: %s Keep checking...\n", serverIP.c_str());
		delay = 5;
	}
	return false;
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
		else if(CheckIfPIDExists("pidof transmission-gtk"))
		{
			ShutDown("Transmission ON in home IP");
		}
		else if(serverOn == false) 
		{
			StartServer();
			WaitForVPNConnection();
		}
	}
}
int run();