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
using namespace std;
typedef unsigned char byte;

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
void ShutDown(string cmd)
{
	string phonemsg;
	string logtext;

	//Leave a record in log file
	time_t current_time;
	char* c_time_string;
	current_time = time(NULL);
	c_time_string = ctime(&current_time);

	printf("Current time is %s\n", c_time_string);

	//construct log file text based on incoming message.
	logtext = "Poweroff called due to ";
	logtext += cmd;
	logtext += " at %s\n ";

	//write log file text
	fp = fopen(logfile.c_str(), "a");
	fprintf(fp, logtext.c_str(), c_time_string);
	fclose(fp);

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

	//shutdown comp
	fp = popen("poweroff", "r");
	pclose(fp);
	//return 0;
}

string GetCurrentIP()
{
	const string text = shellCommand("curl -s http://checkip.dyndns.org/");

    const regex vowels("[a-zA-Z<>/:'\n' ]");
    stringstream result;

	//Remove all HTML stuff and null terminator
    regex_replace(ostream_iterator<char>(result), text.begin(), text.end(), vowels, "");

	//return string retreived from stream
	return result.str();
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

string getPhone()
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
int run();
