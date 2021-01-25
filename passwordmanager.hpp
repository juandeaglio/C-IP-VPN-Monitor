//Password expiration counter service
//when expires it calls outer updater 

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <libgen.h>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex>
#include <iterator>     // std::ostream_iterator

using namespace std;

string timeline;
string period;
string phoneNum;
bool file_read = false;
string susbdir;
static string cnt_path;
string expfile;
string usr_name;
string pwdpath;
char cntpath[50];
char xpncnt_dir[50];
char uname[50];
char pwdfile[50];
char origdir[100];
char currentIP[50];
char user_loggedin[50];
string phonemsg;
bool firstrun = true;
char notify_msg[50]; 
char notify_success[50];
char notify_update[50];
char regusr[50];
string ul = "";
bool pwd_found = false;
bool pwdupdated = false;
bool test = false;
string p2str(char *chr)
{
	stringstream myStreamString;
	myStreamString << chr;
	string myString = myStreamString.str();
	return myString;	
}
string shellCommand(string cmd) 
{
    string data;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    FILE *fp = popen(cmd.c_str(), "r");
    if (fp) 
	{
		while (!feof(fp))
		if (fgets(buffer, max_buffer, fp) != NULL) data.append(buffer);
		pclose(fp);
    }
    return data;
}
string GetRegusr()
{
	string usr = shellCommand("dir /home/ &");
	strcpy(regusr, usr.c_str());
	regusr [ strcspn(regusr, "\r\n") ] = 0;
	return usr;
}
string readpwd(string fname)
{
	string pass;
	string pwdfile;
	pwdfile = susbdir + "/"; 
	pwdfile += fname;
	cout<<"in readpwd(). opening "<<pwdfile<<endl;
	//read content of .pwd file
	ifstream myfile (pwdfile);
	if(!myfile.is_open())
	{
		printf("Failed to open .pwd file!\n"); 
		exit(0);
	}
	string line;
	while (getline(myfile,line))
	{
	  	pass = line;
		printf("old pass: %s\n", line.c_str());
		//exit(0);
	}
	myfile.close();
	return pass;
}

void writeData()
{
	ofstream file ((const char*)cntpath);
	if(!file.is_open())
		printf("Failed to open expcnt file!\n"); 
	for(int i=0; i<3; i++)
		if(i == 0)
			file<<time(NULL)<< endl;
		else if(i == 1)
			file<<period<<endl;
		else if(i == 2)
			file  << phoneNum;
	file.close();		
}
void readData()
{
		int i = 0;
		string line;

		std::ifstream file((const char*)cntpath);
		printf("in exired() cntpath: %s\n", cntpath);
		while(getline(file, line)) 
		{
			if(i == 0)
				timeline = line;
			else if(i == 1)
				period = line;
			else if(i == 2)
				phoneNum = line;
			i++;
		}
		file.close();
}

bool expired()
{	
	printf("in expired()\n");
	readData();

	long diff = time(NULL) - atol(timeline.c_str());
	printf("difference is: %d\n", diff);

	if(diff >= atol(period.c_str()))
		return true;
	else
		return false;
}
bool CheckForCurrentUsers()
{
	ul = shellCommand("users");
	strcpy(user_loggedin, ul.c_str());
	printf("ul.c_str() is: %s\n", ul.c_str());
	printf("user_loggedin before null terminator removal is: %s\n", user_loggedin);
	//remove '\n' char from user_loggedin
	user_loggedin [ strcspn(user_loggedin, "\r\n") ] = 0;
	printf("in if. user logged in is: %s\n", user_loggedin);
	return true;
}
bool IsUserLoggedIn()
{
	if(strcmp(user_loggedin, "") == 0)
		return false;	
	else
		return true;
}
void NotifyStart()
{	if(firstrun)
	{
		char notify_start[100];
		strcpy(notify_start, "DISPLAY=:0 sudo -u ");
		strcat(notify_start, regusr);
		strcat(notify_start, " notify-send '");
		strcat(notify_start, "xsvr successfully started'");
		FILE *fp = popen(notify_start, "r");
		pclose(fp);
		firstrun = false;
	}
}
string FindPasswordFile()
{		
	string emptystr;
	emptystr = "";
	string str = "find /run/media/ -name '.pwd' &";
	string path;
	path = shellCommand("find /run/media/ -name '.pwd'");
	if(path != "") //pwd file found. 
	{
		strcpy(pwdfile, path.c_str());
		printf("pwdfile is: %s\n", path.c_str());

		pwdfile [ strcspn(pwdfile, "\r\n") ] = 0;

		char * usbdir = dirname((char*)path.c_str());
		susbdir = p2str(usbdir);
		pwd_found = true;
		return susbdir;
	}
	else
	{
		FILE *fp = popen(notify_update, "r");
		pclose(fp);
		sleep(15); //.pwd not found - after 15 sec continue looping
		return "";
	}	
}
void TerminalCommandChangePassword(string username, string newpwd)
{
	//assemble change password command
	string part1 = "sudo echo -e '";
	string usrname = regusr;
	string part2 = "' | passwd ";
	//string oldpwd = oldpass; 
	//string newpwd = temp;
	string newline = "\n";
	string cmd;

	cmd = part1;
	cmd += newpwd;
	cmd += newline;
	cmd += newpwd;
	cmd += part2;
	cmd += username;

	cout<<"passwd command is: "<<cmd<<endl;
	//run change password command
	//system(cmd.c_str());
	FILE *fp = popen(cmd.c_str(), "r");
	pclose(fp);
}
string GenerateRandomPassword()
{
	//generate random password
	char temp[20];
	int i;
	srand((unsigned int)(time(NULL)));
	for(i = 0; i < 12; i++) 
	{
		temp[i] = 33 + rand() % 94;
		if(temp[i] == 36) //$
			temp[i] = 'S';
		if(temp[i] == 34) //dowble quote
			temp[i] = '%';
		if(temp[i] == 39) //single quote
			temp[i] = '&';
	}
	temp[i] = '\0';
	printf("%s\n",temp);
	string newpass = temp;
	return newpass;
}
bool WritePassword()
{
	string oldpwd;
	string oldrootpwd;
	string newpwd;
	string newrootpwd;
	oldpwd = readpwd(".pwd");
	oldrootpwd = readpwd(".rootpwd");
	if(!test)
	{
		TerminalCommandChangePassword(GetRegusr(), oldpwd);
	}
	newpwd = readpwd(".pwd");

	if(newpwd != oldpwd)
	{
		cout<<"old pwd - "<<oldpwd<<endl;
		cout<<"pwd is updated to "<<newpwd<<endl;
		pwdupdated = true;
	}
	newrootpwd = readpwd(".rootpwd");                          
	if(newrootpwd != oldrootpwd)
	{
		cout<<"old rootpwd - "<<newpwd<<endl;
		cout<<"rootpwd is updated to "<<newrootpwd<<endl;
	}
	printf("popping up notify_success msg - %s\n", notify_success);
	FILE *fp = popen(notify_success, "r");
	pclose(fp);
	writeData();
	return pwdupdated;
}
void SendPhoneAMsg()
{
		FILE *fp = popen(phonemsg.c_str(), "r");
		pclose(fp);
}
bool SetPassword()
{
	bool result = false;
	if(!pwd_found)
	{
		FindPasswordFile();
	}
	else
	{	
		WritePassword();
		result = true;
	}
	return result;
}
bool CheckForExpiration()
{
	if(expired())
	{
		return true;
	}
	else
	{
		if(!test)
			sleep(600); // Must be 10 min
		else
			sleep(10);
		printf("not expired. looping...\n");
		return false;
	}
}
void SetupPaths()
{
	strcpy(xpncnt_dir, "/home/");
	strcat(xpncnt_dir, regusr);
	strcat(xpncnt_dir, "/.cache/xpncnt/");

	strcpy(cntpath, xpncnt_dir);
	strcat(cntpath, "expcnt");
}

void ConstructMessages()
{
	strcpy(notify_update, "DISPLAY=:0 sudo -u ");
	strcat(notify_update, regusr);
	strcat(notify_update, " notify-send '");
	strcat(notify_update, "Passwords expired.' &");
	printf("notify_update assigned. cmd is: %s\n", notify_update);

	strcpy(notify_success, "DISPLAY=:0 sudo -u ");
	strcat(notify_success, regusr);
	strcat(notify_success, " notify-send '");
	strcat(notify_success, "Passwords successfully updated.' &");
	printf("notify_success assigned. cmd is: %s\n", notify_success);

	phonemsg = "echo '";
	phonemsg +=  regusr;
	phonemsg +=  " Password expired. Update plz.' | ssmtp ";
	phonemsg +=  phoneNum;
	phonemsg +=  "@txt.att.net";
}
bool ChangePasswordIfExpired()
{
	strcpy(pwdfile, "");
	getcwd(origdir, sizeof(origdir));
	GetRegusr();
	

	SetupPaths();
	ConstructMessages();
	readData();

	printf("before while\n");

	strcpy(user_loggedin, "");
	bool loggedIn = IsUserLoggedIn();
	while(!loggedIn)
	{
		if(!IsUserLoggedIn())
		{
			CheckForCurrentUsers();
			if(strcmp(user_loggedin, "") == 0 && strcmp(user_loggedin, "f32") != 0)
				return false;
			loggedIn = true;
		}
	}
	NotifyStart();
	bool isExpired = false;
	bool passwordChanged = false;
	while(!passwordChanged)
	{
		isExpired = CheckForExpiration();
		if(isExpired)
		{
			SendPhoneAMsg();
			if(SetPassword())
				passwordChanged = true;
			else
				return false;
			isExpired = false;
		}
		else
		{
			return false;
		}
	}
	return true;
}
