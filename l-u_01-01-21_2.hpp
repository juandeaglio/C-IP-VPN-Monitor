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

string shellCommand(string cmd);
void ShutDown(string cmd);
string GetCurrentIP();
void locateConfig(const char *name, int indent);
string getPhone();
int run();
