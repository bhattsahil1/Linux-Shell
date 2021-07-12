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
#include<signal.h>
#include<grp.h>
#include<pwd.h>
#include<stdbool.h>
#include "nightswatch.h"
#include <sys/select.h> 
#include <termios.h>
#if HAVE_STROPTS_H
#include <stropts.h>
#endif
// #include <stropts.h>



bool foundchar = false;

void nightswatch(char **arguments)
{
    int duration  =1;
    char *command = arguments[1];
    if(strcmp(arguments[1],"-n")==0)
    {
        duration = atoi(arguments[2]);
        command = arguments[3];
    }
    if(strcmp(command,"dirty")==0)
    {
        // signal('q',charinterrupt);
        while(1)
        {
            FILE *file = fopen("/proc/meminfo","r");
            char line[100];
            int i=0;
            // char *c = read(1,&buf,1);
            while(fgets(line,sizeof(line),file))
            {
                i++;
                if(i==17)
                {
                    char *ptr = line;
                    ptr = strrchr(line,':');
                    char* newptr = ptr +14;
                    printf("%s\n",newptr);
                    break;
                }
            }
            fclose(file);
            sleep(duration);
        }
    }
    else
    {
        printf("Invalid command\n");
    }

}
    