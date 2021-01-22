#include "passwordmanager.hpp"

using namespace std;

int main()
{
	strcpy(pwdfile, "");
	getcwd(origdir, sizeof(origdir));
	string usr = shellCommand("dir /home/ &");
	strcpy(regusr, usr.c_str());
	regusr [ strcspn(regusr, "\r\n") ] = 0;
	printf("username is: %s\n", regusr);

	ConstructMessages();
	readData();

	printf("before while\n");

	strcpy(user_loggedin, "");
	bool loggedIn = false;
	while(!loggedIn)
	{
		if(!IsUserLoggedIn())
		{
			CheckForCurrentUsers();
			loggedIn = true;
		}
	}
	NotifyStart();
	while(1)
	{
		CheckForExpiration();
	}
	return 0;
}

