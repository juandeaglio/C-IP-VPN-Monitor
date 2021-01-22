#include "l-u_01-01-21_2.hpp"

using namespace std;

typedef unsigned char byte;
int main()
{
	run();
}
void* CheckVPNEveryNSeconds()
{
	IsVPNActive();
	sleep(delay);
	pthread_exit(NULL);
}
void run()
{
	InitializePaths();
	if(!config.compare(""))	
	{
		locateConfig("/run/media", 0);
		sleep(1);
	}
	SetupVPN();
	printf("setup\n");
	pthread_t vpnThread;
	int rc;
	int i = 0;
	while(1)
	{
		if (pthread_create(&vpnThread, NULL, CheckVPNEveryNSeconds, (void*)NULL) != 0) 
		{
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
      	}
	}
	pthread_exit(NULL);
}