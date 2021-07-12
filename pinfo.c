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
#include "pinfo.h"
#include "display.h"
#define PATH_MAX 4096
char **parsiline(char *commands)
{
    char **tokens = malloc(1000 * sizeof(char*));
    char *token;
    token = strtok(commands," ");
    int i=0;
    while(token!=NULL)
    {
        tokens[i] = token;
        i++;
        token = strtok(NULL," ");
    }
    tokens[i]=NULL;
    return tokens;
}
void p_info(char **arguments,char *homepath)
{
    if(arguments[1]==NULL)
    {
        char *infopath = (char *)calloc(PATH_MAX,sizeof(char));
        char id[50];
        sprintf(id,"%d",getpid());
        // printf("%s\n",id);
        infopath = strcpy(infopath,"/proc/");
        infopath = strcat(infopath,id);
        infopath = strcat(infopath,"/stat");
        char *exepath = (char *)calloc(PATH_MAX,sizeof(char));
        exepath = strcpy(exepath,"/proc/");
        exepath = strcat(exepath,id);
        exepath = strcat(exepath,"/exe");
        int fd = open(infopath,O_RDONLY);
        if(fd ==-1)
        {
            printf("Sorry,either we can't access data for this process or this process doesn't exist\n");
        }
        else
        {
            char *buf = (char*)calloc(1000,sizeof(char));
            read(fd,buf,1000);
            // printf("%s\n",buf);
            char ** values = parsiline(buf);
            char *cuf = (char*)calloc(1000,sizeof(char));
            int x= readlink(exepath,cuf,1000);
            // printf("%d\n",x);
            printf("pid -- %s\n",id);
            printf("Process status -- {%s} \n",values[2]);
            printf("memory -- %s {Virtual memory}\n",values[22]);
            if(x==-1)
            {
                printf("The executable path for this process cannot be accessed\n");
            }
            else
            {
                printf("Executable Path -- %s\n",linkformatter(homepath,cuf));                
            }
        }
        
    }
    else
    {
        char *infopath = (char *)calloc(PATH_MAX,sizeof(char));
        infopath = strcpy(infopath,"/proc/");
        infopath = strcat(infopath,arguments[1]);
        infopath = strcat(infopath,"/stat");

        char *exepath = (char *)calloc(PATH_MAX,sizeof(char));
        exepath = strcpy(exepath,"/proc/");
        exepath = strcat(exepath,arguments[1]);
        exepath = strcat(exepath,"/exe");

        int fd = open(infopath,O_RDONLY);
        if(fd ==-1)
        {
            printf("Sorry,either we can't access data for this process or this process doesn't exist\n");
        }
        else
        {
            char *buf = (char*)calloc(1000,sizeof(char));
            read(fd,buf,1000);
            // printf("%s\n",buf);
            char ** values = parsiline(buf);
            char *cuf = (char*)calloc(1000,sizeof(char));
            int x= readlink(exepath,cuf,1000);
            // printf("%d\n",x);
            printf("pid -- %s\n",arguments[1]);
            printf("Process status -- {%s} \n",values[2]);
            printf("memory -- %s {Virtual memory}\n",values[22]);
            if(x==-1)
            {
                printf("The executable path for this process cannot be accessed\n");
            }
            else
            {
                printf("Executable Path -- %s\n",linkformatter(homepath,cuf));
            }
        }

    }
}
