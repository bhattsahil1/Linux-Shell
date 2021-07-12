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
#include "executecmd.h"
int redirhandler(char **arguments,char *homepath,char histcmd[20][100],int cnt)
{
    int existence = 0;
    int yi=0;
    int input_redir = 0;
    int output_redir = 0;
    int output_append = 0;
    int saved_fd_out = dup(1);
    int saved_fd_in = dup(0);

    int pi1,pi2;
    while(arguments[yi]!=NULL)
    {
        if(strcmp(arguments[yi],"<")==0)
        {
            input_redir = 1;
            existence = 1;      
            // pi1 = yi;
            // break;//remove later
        }
        else if(arguments[yi][0]=='>')
        {
            if(arguments[yi][1]=='>')
            {
                output_append = 1;
            }
            else
            {
                output_redir = 1;
            }
            existence = 1;      
            // pi1 = yi;
            // break;//remove later
        }
        yi++;
    } 
    if(existence==0)
    {
        return existence;
    }
    if(output_redir ==1 || output_append ==1 || input_redir ==1)
    {
        char **firstpart = malloc(1000 * sizeof(char*));
        char **secondpart = malloc(1000 * sizeof(char*));
        char **thirdpart = malloc(1000 * sizeof(char*));
        int g=0;
        //case : just <
        if(input_redir == 1 && output_append !=1 && output_redir!=1)
        {
            while(strcmp(arguments[g],"<")!=0)
            {
                firstpart[g] = arguments[g];
                // printf("%s\n",firstpart[g]);
                g++;
            }
            g = g+1;
            int h =0;
            while(arguments[g]!=NULL)
            {
                secondpart[h] = arguments[g];
                // printf("%s\n",secondpart[h]);
                g++;
                h++;
            }
        }
        //case : just > or >>
        if(input_redir != 1 && (output_redir ==1 || output_append==1))
        {
            while(arguments[g][0]!='>')
            {
                firstpart[g] = arguments[g];
                g++;
            }
            g = g+1;
            int h =0;
            while(arguments[g]!=NULL)
            {
                secondpart[h] = arguments[g];
                g++;
                h++;
            }
            //  printf("%s %s\n",firstpart,secondpart);
        }
        //case : < > or < >>
        if(input_redir == 1 && (output_redir ==1 || output_append==1))
        {
            while(strcmp(arguments[g],"<")!=0)
            {
                firstpart[g] = arguments[g];
                g++;
            }
            g = g+1;
            int h =0;
            while(arguments[g][0]!='>')
            {
                secondpart[h] = arguments[g];
                g++;
                h++;
            }
            int hi = 0;
            g = g+1;
            while(arguments[g]!=NULL)
            {
                thirdpart[hi] = arguments[g];
                g++;
                hi++;
            }
        }
        //CASE-1 : SIMPLE OUTPUT REDIRECTION 
        if(output_redir == 1 && input_redir!=1)
        {
            int fd;
            fd = open(secondpart[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd == -1)
            {
                perror("Failed to open the file\n");
            }
            close(1);
            if(dup2(fd,1)!=1)
            {
                perror("dup2 fail");
            }
            close(fd);
        }


        //CASE-2 : SIMPLE OUTPUT APPENDING
        if(output_append == 1 && input_redir!=1)
        {
            int fd;
            fd = open(secondpart[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if(fd == -1)
            {
                perror("Failed to open the file\n");
            }
            close(1);
            if(dup2(fd,1)!=1)
            {
                perror("dup2 fail");
            }
            close(fd);
        }


        //CASE-3 : SIMPLE INPUT REDIRECTION
        if(input_redir == 1 && output_append!=1 && output_redir!=1)
        {
            int fd;
            fd = open(secondpart[0], O_RDONLY);
            if(fd == -1)
            {
                perror("Failed to open the file\n");
            }
            close(0);
            if(dup2(fd,0)!=0)
            {
                perror("dup2 fail");
            }
            close(fd);
        }

        //CASE-4 : INPUT-OUTPUT REDIRECTION
        if(input_redir == 1 && output_redir==1)
        {
            int fd;
            fd = open(secondpart[0],O_RDONLY);
            if(fd == -1)
            {
                perror("Failed to open the file\n");
            }
            close(0);
            if(dup2(fd,0)!=0)
            {
                perror("dup2 fail");
            }
            close(fd);
            int fd2 = open(thirdpart[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(fd2 == -1)
            {
                perror("Failed to open the file\n");
            }
            close(1);
            if(dup2(fd2,1)!=1)
            {
                perror("dup2 fail");
            }
            close(fd2);
        }

        //CASE-5 : INPUT-OUTPUT APPENDING
        if(input_redir == 1 && output_append==1)
        {
            int fd;
            fd = open(secondpart[0],O_RDONLY);
            if(fd == -1)
            {
                perror("Failed to open the file\n");
            }
            close(0);
            if(dup2(fd,0)!=0)
            {
                perror("dup2 fail");
            }
            close(fd);
            int fd2 = open(thirdpart[0], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if(fd2 == -1)
            {
                perror("Failed to open the file\n");
            }
            close(1);
            if(dup2(fd2,1)!=1)
            {
                perror("dup2 fail");
            }
            close(fd2);
        }
        cmdexec(firstpart,homepath,histcmd,cnt);
        dup2(saved_fd_in,0);
        dup2(saved_fd_out,1);
    }
    return existence;
}