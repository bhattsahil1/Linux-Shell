#include<stdio.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/utsname.h> 
#include<stdbool.h>
#include <dirent.h>
typedef struct 
{
	int pid;
	char pname[100];
	char status[100];
}processes;
int process_count;
processes process_list[1000];
int rocketpid,launchpid;
char runproc[105];