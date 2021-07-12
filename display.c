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
#include<limits.h>
#include<dirent.h>
#include<sys/wait.h>
#include "display.h"
#define PATH_MAX 4096
char **parselona(char *commands)
{
    char **tokens = malloc(1000 * sizeof(char*));
    char *token;
    token = strtok(commands,"/");
    int i=0;
    while(token!=NULL)
    {
        tokens[i] = token;
        i++;
        token = strtok(NULL,"/");
    }
    tokens[i]=NULL;
    return tokens;
}
char *invoke(char *arg)
{

    // printf("%s\n",arg);
    char pwd[PATH_MAX];
    char *newpwd = (char*)calloc(PATH_MAX,sizeof(char));
    getcwd(pwd,sizeof(pwd));
    newpwd = strcpy(newpwd,pwd);
    char **rand = parselona(arg);   
    int i =0;
    char *pathconstructor = (char*)calloc(PATH_MAX,sizeof(char));
    while(rand[i]!=NULL)
    {
        i++;
    }
    pathconstructor= strcpy(pathconstructor,pwd);
    int j =0;
    // printf("%d\n",i);
    while(j!=i-1)
    {
        // printf("j is %d\n",j);
        pathconstructor = strcat(pathconstructor,"/");
        pathconstructor = strcat(pathconstructor,rand[j]);
        j++;
    }
    int x = strlen(pathconstructor);
    // printf("%d\n",x);
    // printf("%s\n",pathconstructor);
    if(pathconstructor[x-1]=='.')
    {
        pathconstructor[x-1] = '\0';
        if(pathconstructor[x-2 ]== '/')
        {
            pathconstructor[x-2]= '\0';
        }
    }
    // printf("%s\n",pathconstructor);
    // x = strlen(pathconstructor);  
    // printf("%d\n",x);
    return pathconstructor;
}
void display(char *pathconstructor)
{
    struct utsname usernamedata;
    uname(&usernamedata);
    char *user = (char*)calloc(100,sizeof(char));
    user = strcpy(user,"<");
    char *p = getenv("USER");
    char *q = usernamedata.sysname;
    user = strcat(user,p);
    user = strcat(user,"@");
    user = strcat(user,q);
    user = strcat(user,":");
    char pwd[PATH_MAX];
    char *newpwd = (char*)calloc(PATH_MAX,sizeof(char));
    getcwd(pwd,sizeof(pwd));
    newpwd = strcpy(newpwd,pwd);
    // printf("PATHCONSTRUCTOR is %s\n",pathconstructor);
    int y = strcmp(pwd,pathconstructor);
    if(y<0)
    {
        // printf("Case1\n");
        newpwd = strcpy(newpwd,newpwd);
    }
    else if(y==0)
    {
        // printf("Case2\n");
        newpwd = strcpy(newpwd,"~");
    }
    else
    {
        // printf("Case3\n");
        char tmpwd[1000];
        int i;
        tmpwd[0] = '~';
        tmpwd[1] = '/';
        int x = strlen(pathconstructor);
        for(i=2;i<strlen(pwd);i++)
        {
            tmpwd[i] = newpwd[x+i-1];
        }
        tmpwd[i] = '\0';
        // printf("%s\n",tmpwd);
        newpwd = strcpy(newpwd,tmpwd);
    }
    user = strcat(user,newpwd);
    user = strcat(user,">");
    printf("\033[1;34m");
    printf("%s",user);
    printf("\033[0m");
}

char* linkformatter(char *pathconstructor,char *dir)
{
    char *newpwd = (char*)calloc(PATH_MAX,sizeof(char));
    newpwd = strcpy(newpwd,dir);
    // printf("PATHCONSTRUCTOR is %s\n",pathconstructor);
    int y = strcmp(dir,pathconstructor);
    if(y<0)
    {
        // printf("Case1\n");
        newpwd = strcpy(newpwd,newpwd);
    }
    else if(y==0)
    {
        // printf("Case2\n");
        newpwd = strcpy(newpwd,"~");
    }
    else
    {
        // printf("Case3\n");
        char tmpwd[1000];
        int i;
        tmpwd[0] = '~';
        tmpwd[1] = '/';
        int x = strlen(pathconstructor);
        for(i=2;i<strlen(dir);i++)
        {
            tmpwd[i] = newpwd[x+i-1];
        }
        tmpwd[i] = '\0';
        // printf("%s\n",tmpwd);
        newpwd = strcpy(newpwd,tmpwd);
    }
    return newpwd;
}