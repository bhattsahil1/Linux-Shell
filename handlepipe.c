#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include "parserfunc.h"
#include "redirhandler.h"
#include "executecmd.h"
int fds[2];
int handlepipe(char **args,char *homepath,char histcmd[20][100],int cnt)
{
    int i=0;
    int pipecount =0;
    int j,k;
    bool flag1,flag2;
    // char ***pipedcmds= malloc(1000 * sizeof(char**));;
    char *** pipedcmds = malloc(sizeof(char **) * 10);
    for(k = 0; k < 10; k++)
    {
        pipedcmds[k] = malloc(sizeof(char *) * 10);
        for(int j = 0; j < 10; j++)
        {
            pipedcmds[k][j] = malloc(10);
        }
    }
    int x=0;
    j=0;
    while(args[i]!=NULL)
    {
        while(args[i]!=NULL && strcmp(args[i],"|")!=0)
        {
            // printf("%s\n",args[i]);
            pipedcmds[x][j] = strcpy(pipedcmds[x][j],args[i]);
            // printf("%s\n",pipedcmds[x][j]);
            i++;
            j++;
            if(args[i]==NULL)
            {
                pipedcmds[x][j]=NULL;
            }
            // printf("%d\n",i);
        } 
        if(args[i]!=NULL && args[i][0]=='|')
        {
            pipedcmds[x][j] = NULL;
            pipecount++;
            i++;
            x++;
            j=0;
        }
    }
    if(pipecount == 0)
    {
        free(pipedcmds);
        return -1;
    }
    i=0;
    int saved_stdin = dup(0);
    int saved_stdout = dup(1);
    int pid,status;
    int r=0;
    while(i<=pipecount)
    {
        // printf("%s\n",pipedcmds[r]);
        flag1 = (bool)(!(i==0));
		if(flag1)
		{
			close(fds[1]);
			saved_stdin = dup(0);
            r++;
            int errval = dup2(fds[0],0);
			if (errval == -1)
            {
				printf("Error\n");
            }
			close(fds[0]);
		}
        flag2 = (bool)(!(i==pipecount));
		if(flag2)
		{
			pipe(fds);
			saved_stdout = dup(1);
            r++;
            int errval = dup2(fds[1],1);
			if (errval == -1)
            {
				printf("Error\n");
            }
		}
        
        pid = fork();
		if (pid == 0)
		{
			if(redirhandler(pipedcmds[i],homepath,histcmd,cnt)!=0)
            {
                exit(0);
            }
            else
            {
                status = cmdexec(pipedcmds[i],homepath,histcmd,cnt);
                exit(0);
            }
		}
		else 
		{
			waitpid(pid, &status, WUNTRACED);
			dup2(saved_stdout,1);
			dup2(saved_stdin,0);
		}
        i++;
	}
	return 0;
}
        