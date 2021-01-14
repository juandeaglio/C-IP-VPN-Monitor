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

	if(!config.compare(""))	//empty string
	{
		locateConfig("/run/media", 0);
		sleep(1);
	}
	SetupVPN();
	while(1)
	{
		IsVPNActive();
		sleep(delay);
	}
	return 0;
} 



