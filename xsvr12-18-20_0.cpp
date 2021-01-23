#include "passwordmanager.hpp"

using namespace std;

int main()
{
	strcpy(pwdfile, "");
	getcwd(origdir, sizeof(origdir));
	GetRegusr();
	

	SetupPaths();
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

