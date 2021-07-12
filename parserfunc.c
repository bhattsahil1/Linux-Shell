#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/utsname.h>
#include<sys/stat.h>
#include <sys/sysmacros.h>
#include<time.h>
#include<limits.h>
#include<dirent.h>
#include<sys/wait.h>
#include<grp.h>
#include<pwd.h>
int min(int a,int b)
{
    if(a<b)
        return a;
    else
    {
        return b;
    }
}

char *lineread(void)
{
    char *commands = NULL;
    ssize_t buf = 0;
    getline(&commands,&buf,stdin);
    return commands;
}


char **parseline(char *commands)
{
    char **tokens = malloc(1000 * sizeof(char*));
    char *token;
    token = strtok(commands," \t\r\n\a");
    int i=0;
    while(token!=NULL)
    {
        tokens[i] = token;
        token = strtok(NULL," \t\r\n\a");
        i++;
    }
    tokens[i]=NULL;
    return tokens;
}
char **parser(char *commands, char *c)
{
    char **tokens = malloc(1000 * sizeof(char*));
    char *token;
    token = strtok(commands,c);
    int i=0;
    while(token!=NULL)
    {
        tokens[i] = token;
        token = strtok(NULL,c);
        i++;
    }
    tokens[i]=NULL;
    return tokens;
}
