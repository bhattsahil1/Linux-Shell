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
#include<signal.h>
#include "display.h"
#include "ls.h"
#include "pinfo.h"
#include "nightswatch.h"
#include "nonbuiltin.h"
#include "executecmd.h"
#include "parserfunc.h"
#include "redirhandler.h"
#include "handlepipe.h"
#include "misc.h"

char histcmd[20][100];
#define PATH_MAX 4096
int arr[3];
int k=0;

//commonfunc
char *stdfunc(int i)
{
    char *gname = process_list[i].pname;
    strcpy(process_list[i].status,"Killed");
    process_list[i].pid = -1;
    return gname;
}


//UP ARROW HANDLER BONUS
int uparrowhandler(int numup,char *arg,int cnt)
{
    // printf("%d\n",numup);
    FILE *fp = fopen("History.txt","r");
    if(fp==NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    fseek(fp,0,SEEK_END);
    int count =0;
    int n=numup;
    char s[105];
    char *roopath = invoke(arg);
    char cmd[105];
    int status;
    int position = ftell(fp);
    while(position)
    {
        fseek(fp,--position,SEEK_SET);
        if(fgetc(fp)=='\n')
        {
            
            if(count==n+1)
            {
                break;
            }
            if(count==n)
            {
                // printf("%s",s);
                strcpy(cmd,s);
            } 
            count++;    
        }      
        fgets(s, sizeof(s), fp);
    }        
    fclose(fp);
    display(roopath);
    printf("%s",cmd);

    int fd = open("History.txt",O_CREAT | O_RDWR,0644);
    lseek(fd,0,SEEK_END);
    write(fd,cmd,strlen(cmd)+1);
    close(fd);
    char **colonargs = parser(cmd,";");
    int q=0;
    char **arguments;
    while(colonargs[q]!=NULL)
    {
        arguments= parseline(colonargs[q]);
        if(handlepipe(arguments,roopath,histcmd,cnt)==-1)
        {
            if(redirhandler(arguments,roopath,histcmd,cnt)==0)
            {
                status = cmdexec(arguments,roopath,histcmd,cnt);
            }
        }
        q++;
    }
    // free(cmd);
    free(arguments);
    cnt = cnt + 1;
    return cnt;
}




//Ctrl + C handler
void CtrlChandler (int sig_num)
{
    if(getpid() != launchpid)
    {
        return; 
    }

    if(rocketpid != -1)
    {
        kill(rocketpid, SIGINT);
        for (int k = 1; k <= process_count; k++)
        {
            if (process_list[k].pid == rocketpid)
            {
                stdfunc(k);
                break;
            }
        }

    }
}

//Ctrl + Z handler
void CtrlZhandler(int sig_num) 
{ 
    if(getpid() != launchpid)
    {
        return;
    }
    
    if(rocketpid != -1)
    {
	   
        int flag =0;
        kill(rocketpid, SIGTSTP);
        for(int i=1; i<=process_count; i++)
        {
            if(process_list[i].pid == rocketpid)
            {
                flag =1;
                break;
            }
        }
        if ( flag ==0 )
        {
            process_list[process_count].pid = rocketpid;
            strcpy(process_list[process_count].pname, runproc);
            strcpy(process_list[process_count].status, "Stopped");
            process_count++;
        }

    }
    rocketpid=-1; 
    return;
}


void shell_loop(char *arg)
{
    char *commands;
    int status;
    char **colonargs;
    char **arguments;
    char *homepath = invoke(arg);
    int cnt =0;
    do{
        int pid, status;
        while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0)
        {
            char* pname;

            if (WIFEXITED(status))
            {
                for (int i=1; i<=process_count; ++i)
                {
                    if (process_list[i].pid == pid)
                    {
                        pname = stdfunc(i);
                        printf("\n%s with pid %d exited normally.\n", pname, pid);
                        break;
                    }
                }
            }
            if (WIFSIGNALED(status))
            {
                for (int i=1; i<=process_count; ++i)
                {
                    if (process_list[i].pid == pid)
                    {
                        pname = stdfunc(i);
                        printf("\n%s with pid %d was terminated by a signal.\n", pname, pid);
                        break;
                    }
                }
            }
        }
        display(homepath);
        commands = lineread();
        int upArrow = 0;
        for (int ji = 0; ji < strlen(commands); ji++)
        {
            if (commands[ji] == '\33')
            {
                if(commands[ji+2] == 'A')
                {
                    upArrow += 1;
                }
            }
        }
        if(upArrow!=0)
        {
            cnt = uparrowhandler(upArrow,arg,cnt);
        }
        // printf("%s\n",commands);
        else
        {
            int i;
            int fd = open("History.txt",O_CREAT | O_RDWR,0644);
            lseek(fd,0,SEEK_END);
            write(fd,commands,strlen(commands)+1);
            close(fd);
            colonargs = parser(commands,";");
            int q=0;
            while(colonargs[q]!=NULL)
            {
                arguments= parseline(colonargs[q]);
                if(handlepipe(arguments,homepath,histcmd,cnt)==-1)
                {
                    if(redirhandler(arguments,homepath,histcmd,cnt)==0)
                    {
                        status = cmdexec(arguments,homepath,histcmd,cnt);
                    }
                }
                q++;
            }
            free(commands);
            free(arguments);
            cnt++;
        }
    }while(1);
}

int main(int argc, char *argv[])
{
        rocketpid = -1;
        launchpid = getpid();
        process_count = 1;
        signal(SIGINT, CtrlChandler);
        signal(SIGTSTP, CtrlZhandler);
        shell_loop(argv[0]);
        return 0;
}