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

string shellCommand(string cmd) 
{
    string data;
    //FILE * stream;
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

	//construct log file text based on incoming message.
	logtext = "Poweroff called due to ";
	logtext += cmd;

	//write log file text
	//WriteToLog(logtext);
	

	//construct phone message command based on incoming message.
	phonemsg = "echo ";
	phonemsg += "'Poweroff called due to ";
	phonemsg += cmd;
	phonemsg += "' | ssmtp ";//8054276925@txt.att.net";
	phonemsg +=  phoneNum;
	phonemsg +=  "@txt.att.net";

	//send ssmtp message on my cellphone
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
	printf("Current time is %s\n", c_time_string);
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

	//Remove all HTML stuff and null terminator
    regex_replace(ostream_iterator<char>(result), text.begin(), text.end(), vowels, "");

	//return string retreived from stream
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
void SetPaths()
{
	
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
void functionRun();
string StartServer()
{
	printf("first loop run in home IP. lets start server\n");
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
void WaitForVPNConnection()
{
	while(!serverOn)
	{
		if(currentIP.compare(homeIP) == 0)
		{
			currentIP = GetCurrentIP();
			sleep(1);
		}
		
		else
		{
			serverIP = currentIP;
			printf("server ON. Current IP is: %s, server IP is: %s\n", currentIP.c_str(), serverIP.c_str());
			serverOn = true;
			string notify_success = "DISPLAY=:0 sudo -u ";
			notify_success += user;
			notify_success += " notify-send '";
			notify_success += "VPN launch success! IP is: ";								
			notify_success += serverIP;
			notify_success += "' &";

			cout<<notify_success<<endl;
			fp = popen(notify_success.c_str(), "r");
			pclose(fp);
		}
	}
}
int run();