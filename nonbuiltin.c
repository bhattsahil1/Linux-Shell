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
#include "nonbuiltin.h"
#include "misc.h"
int arr[3];
void nonbuiltin(char **args)
{
  int ampflag =0;
  int i=0;
  while(args[i]!=NULL)
  {
      i++;
  }
  if(strcmp(args[i-1],"&")==0)
  {
      ampflag=1;
      args[i-1]=NULL;
  }
  int status;
  pid_t pid, wpid;
  pid = fork();

  if (pid < 0)
  {
    perror("Error while forking");
  } 

  else if (pid == 0)
  {
    if(ampflag)
    {
      setpgid(0,0);
    }
    // pid_t cpid = fork();
    // if(cpid ==0)
    // {
    //   if (execvp(args[0], args) == -1)
    //   {
    //     perror("Error");
    //   }
    //   exit(EXIT_FAILURE);
    // }
    // waitpid(cpid,&status,0);
    // if(ampflag)
    // {
    //   if(WIFEXITED(status))
    //     {
    //       fprintf(stderr,"Process %s with pid %d terminated normally.\n",args[0],cpid);
    //       if(WCOREDUMP(status))
    //       {
    //         fprintf(stderr,"Process %s with pid %d produced a core dump\n",args[0],cpid);
    //       }
    //     }
    //     else if(WIFSIGNALED(status))
    //     {
    //       fprintf(stderr,"Process %s with pid %d was terminated by a signal\n",args[0],cpid);
    //     }
    // }
  }

  else if(pid>0)
  {
    if(!ampflag){
        // do 
        // {
        // rocketpid = pid;
        wpid = waitpid(pid, &status, WUNTRACED);
        // } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else
    {
            process_list[process_count].pid = pid; //Could be potential source of trouble
            strcpy(process_list[process_count].pname, args[0]);
            strcpy(process_list[process_count++].status, "running");
        // do {
        // wpid = waitpid(pid, &status, WNOHANG | __W_CONTINUED);
      // } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
  }
}