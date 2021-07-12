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
#include<signal.h>
#include<limits.h>
#include<dirent.h>
#include<sys/wait.h>
#include<grp.h>
#include<pwd.h>
#include "display.h"
#include "ls.h"
#include "pinfo.h"
#include "nightswatch.h"
#include "nonbuiltin.h"
#include "misc.h"
#define PATH_MAX 4096

int cmdexec(char **arguments,char* homepath,char histcmd[20][100],int cnt)
{
            int x;
            //CHECKS IF NO ARGUMENT WAS ENTERED
            if(arguments[0]==NULL)
            {
                return 1;
            }
            strcpy(runproc,arguments[0]);
            //CD
            if(strcmp(arguments[0],"cd")==0)
            {
                char *openfolder = (char*)calloc(PATH_MAX,sizeof(char));
                if(arguments[1]==NULL)
                {
                    perror("invalid");
                    exit(EXIT_FAILURE);
                }
                if(arguments[1][0] == '~')
                {   
                    char *ptr = arguments[1];
                    ptr = ptr+1;
                    openfolder = strcpy(openfolder,homepath);
                    openfolder = strcat(openfolder,ptr);
                }
                else
                {
                    openfolder = strcpy(openfolder,arguments[1]);   
                }
                x = chdir(openfolder);
                if(x!=0)
                {
                    printf("Directory %s doesn't exist\n",openfolder);
                }
            }
            //PWD
            else if(strcmp(arguments[0],"pwd")==0)
            {
                char pwd[PATH_MAX];
                getcwd(pwd,sizeof(pwd));
                printf("%s\n",linkformatter(homepath,pwd));
            }
            //ECHO
            else if(strcmp(arguments[0],"echo")==0)
            {
                if(arguments[1]==NULL)
                {
                    printf("No argument given for echo\n");
                }
                else if(arguments[1][0]=='$')
                {
                    char *s = malloc(sizeof(char) * 100);
                    int ptn=1;
                    for(ptn=1;ptn<strlen(arguments[1]);ptn++)
                    {   
                        s[ptn-1] = arguments[1][ptn];
                    }
                    s[ptn-1] = '\0';
                    char *env = getenv(s);
                    if(env==NULL)
                    {
                        printf("Error: Variable %s does not exist\n",s);
                    }
                    else
                    {
                        printf("%s\n",env);
                    }
                }
                else
                {
                    int i=1;
                    while(arguments[i]!=NULL)
                    {
                        printf("%s ",arguments[i]); //NEEDS TO BE CORRECTED FOR HANDLING MULTIPLE arguments.
                        i++;
                    }
                    printf("\n");
                }
            }
    
            //LS
            else if(strcmp(arguments[0],"ls")==0 && arguments[1]==NULL)
            {
                ls(arguments);
            }
            else if(strcmp(arguments[0],"ls")==0 && strcmp(arguments[1],"&")!=0)
            {
                ls(arguments);
            }
            
            else if(strcmp(arguments[0],"ls")==0 && strcmp(arguments[1],"&")==0)
            {
                int status;
                pid_t pid, wpid;
                pid = fork();
                arguments[1]=NULL;
                if (pid == 0)
                {
                    execvp(arguments[0], arguments);
                    fprintf(stderr, "Error: command not found\n");
                    exit(0);
                }
                else
                {
                    
                    strcpy(process_list[process_count].pname, arguments[0]);
                    strcpy(process_list[process_count].status,"Running");
                    process_list[process_count].pid = pid;
                    process_count++;
                    do 
                    {
                        // tcsetpgrp(0,pid);
                        wpid = waitpid(pid, &status, WNOHANG | __W_CONTINUED);
                        // signal(SIGTTOU,SIG_IGN);
                        // tcsetpgrp(0,getpid());
                        // signal(SIGTTOU,SIG_DFL);
                    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
                }
            }
            //PINFO
            else if(strcmp(arguments[0],"pinfo")==0)
            {
                p_info(arguments,homepath);
            }

            //HISTORY BONUS
            else if(strcmp(arguments[0],"history")==0)
            {
                FILE *fp = fopen("History.txt","r");
                if(fp==NULL)
                {
                    perror("fopen");
                    exit(EXIT_FAILURE);
                }
                fseek(fp,0,SEEK_END);
                int count =0;
                int n=10;
                if(arguments[1]!=NULL)
                {
                    n = atoi(arguments[1]);
                }
                char s[105];
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
                        if(count!=0)
                        {
                            printf("%s",s);
                        } 
                        count++;    
                    }      
                    fgets(s, sizeof(s), fp);
                }        
                fclose(fp);

            } 

            //SET ENVIRONMENT VARIABLE
            else if(strcmp(arguments[0],"setenv")==0)
            {
                if(arguments[1]==NULL)
                {
                    printf("Please enter an environment variable\n");
                }
                if(arguments[2]==NULL)
                {
                    arguments[2] = strcpy(arguments[2]," ");
                }
                if(arguments[3]!=NULL)
                {   
                    printf("Error\n");
                    return x;
                }
                if(arguments[1]!=NULL)
                {
                    int returnval = setenv(arguments[1],arguments[2],1);
                    if(returnval == -1)
                    {
                        printf("Error\n");
                    }
                } 
            }

            //UNSET AN ENVIRONMENT VARIABLE
            else if(strcmp(arguments[0],"unsetenv")==0)
            {
                int returnval = unsetenv(arguments[1]);
                if(returnval == -1)
                {
                    printf("Error: Entered argument is not an environment variable\n");
                }
            }

            //LIST OF JOBS
            else if(strcmp(arguments[0],"jobs")==0)
            {
                printf("\n");
                for(int i=1; i<process_count; i++)
                {
                    if(strcmp(process_list[i].status, "Killed")==0)
                    {
                        continue;
                    }
                    printf("[%d]  %s  %s  [%d] \n", i, process_list[i].status,process_list[i].pname,process_list[i].pid);
                }
            }

            //KJOB
            else if(strcmp(arguments[0],"kjob")==0)
            {
                int count =0;
                int i,job_id;
                if(arguments[1] == NULL)
                {
                    printf("Error\n");
                }
                else
                {
                    job_id = atoi(arguments[1]);
                }
                if(arguments[2]==NULL)
                {
                    printf("Error2\n");
                }
                else
                {
                    int bgid = process_list[job_id].pid;
                    strcpy(process_list[job_id].status,"Killed");
                    int sig = atoi(arguments[2]);
                    int ham = kill(bgid,sig);
                }
            }

            //BRING TO FOREGROUND
            else if(strcmp(arguments[0],"fg")==0)
            {
                if(arguments[1]==NULL)
                {
                    printf("Error\n");
                }
                else
                {                
                    int job_id = atoi(arguments[1]);
                    int pid = process_list[job_id].pid;
                    strcpy(process_list[job_id].status,"Running");
                    kill(pid,SIGSTOP);
                    rocketpid = pid;
                    int status;
                    kill(pid,SIGCONT);
                    waitpid(pid,&status,WUNTRACED);
                }
            }

            //SEND TO BACKGROUND
            else if(strcmp(arguments[0],"bg")==0)
            {
                if(arguments[1]==NULL)
                {
                    printf("Error\n");
                }
                else
                {
                    int job_id = atoi(arguments[1]);
                    int pid = process_list[job_id].pid;
                    strcpy(process_list[job_id].status,"Running");
                    kill(pid,SIGCONT);
                }
            }

            //KILL ALL BACKGROUND PROCESSES
            else if(strcmp(arguments[0],"overkill")==0)
            {
                for(int i=1;i<=process_count;i++)
                {
                    if(strcmp(process_list[i].status,"Killed")==0)
                    {
                        continue;
                    }
                    int killit = process_list[i].pid;
                    strcpy(process_list[i].status,"Killed");
                    kill(killit,SIGINT);
                }
            }

            //NIGHTSWATCH BONUS
            else if(strcmp(arguments[0],"nightswatch")==0)
            {
                nightswatch(arguments);
            } 

            //QUIT COMMAND
            else if(strcmp(arguments[0],"quit")==0)
            {
                printf("Exiting Shell...\n");
                exit(EXIT_SUCCESS);
            }

            //CRONJOB BONUS
            else if(strcmp(arguments[0],"cronjob")==0)
            {
                if(strcmp(arguments[1],"-c")!=0)
                {   
                    printf("Error\n");
                }
                char cmd[100];
                strcpy(cmd,arguments[2]);
                char *args[] = {cmd,NULL};
                int till_time = atoi(arguments[6]);
                int time_interval = atoi(arguments[4]);
                int count_time = 0;
                while(count_time<=till_time)
                {
                    sleep(1);
                    count_time++;
                    if((count_time % time_interval )==0)
                    {
                        display(homepath);
                        cmdexec(args,homepath,histcmd,cnt);
                    }
                }
            }

            //FOR ALL OTHER CASES
            else
            {
                    int ampflag =0;
                    int i=0;
                    while(arguments[i]!=NULL)
                    {
                    i++;
                    }
                    if(strcmp(arguments[i-1],"&")==0)
                    {
                    ampflag=1;
                    arguments[i-1]=NULL;
                    }
                    int status;
                    pid_t pid, wpid;
                    pid = fork();
                    if (pid == 0)
                    {
                        if(ampflag) 
                        {
                            setpgid(0, 0);
                        }
                        execvp(arguments[0], arguments);
                        fprintf(stderr, "Error: command not found\n");
                        exit(0);
                    }
                    else
                    {
                        if (!ampflag)
                        {
                            rocketpid = pid;
                            waitpid(pid, &status, WUNTRACED);
                        }
                        else 
                        {
                            strcpy(process_list[process_count].pname, arguments[0]);
                            strcpy(process_list[process_count].status,"Running");
                            process_list[process_count].pid = pid;
                            process_count++;
                            do 
                            {
                                // tcsetpgrp(0,pid);
                                wpid = waitpid(pid, &status, WNOHANG | __W_CONTINUED);
                                // signal(SIGTTOU,SIG_IGN);
                                // tcsetpgrp(0,getpid());
                                // signal(SIGTTOU,SIG_DFL);
                            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
                        }
                    }
            }
    
    return x;
}
