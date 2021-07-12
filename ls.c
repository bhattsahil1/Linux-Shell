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
#include "ls.h"
#define PATH_MAX 4096
int getMode(const char* path)
{
    struct stat stats;

    stat(path, &stats);

    return stats.st_mode;
}
int getBLOCKSZ(const char* path)
{
    struct stat stats;

    stat(path, &stats);

    return stats.st_blocks;
}
void lsdetails(const char *fname)
{
    struct stat stats;
    stat(fname,&stats);
    int hard_links = stats.st_nlink;
    int x = stats.st_size;
    // int y = stats.st_blksize;
    struct group *grp;
    struct passwd *pwd;
    grp = getgrgid(stats.st_gid);
    pwd = getpwuid(stats.st_uid);
    char buffer[50]; 
    int j = snprintf(buffer, 30, "%s\n", ctime(&stats.st_mtime));
    char* ptr = strrchr(buffer, ':');
    char *fptr = buffer;
    fptr = fptr + 4;
    printf("%d %s %s %8d ",hard_links,pwd->pw_name,grp->gr_name,x);
    while(fptr != ptr)
    {
        printf("%c",*fptr);
        fptr++;
    }
}

void lspermissions(const char *fname)
{
    char array[10];
    int i;
    for(i=0;i<10;i++)
    {
        array[i] = '-';
    }
    int p =getMode(fname);
    if(S_ISDIR(p))
    {
        array[0]='d';
    }
    if(p & S_IRUSR)
    {
        array[1]='r';
    }
    if(p & S_IWUSR)
    {
        array[2]='w';
    }
    if(p & S_IXUSR)
    {
        array[3]='x';
    }
    if(p & S_IRGRP)
    {
        array[4]='r';
    }
    if(p & S_IWGRP)
    {
        array[5]='w';
    }
    if(p & S_IXUSR)
    {
        array[6]='x';
    }
    if(p & S_IROTH)
    {
        array[7]='r';
    }
    if(p & S_IWOTH)
    {
        array[8]='w';
    }
    if(p & S_IXOTH)
    {
        array[9]='x';
    }
    printf("%s ",array);
}


void ls(char **arguments)
{
    struct dirent *dircon;
    char pwd[PATH_MAX];
    getcwd(pwd,sizeof(pwd));
    int is_a=0;
    int is_l=0;

    int j=1;
    char *path=NULL;
    while(arguments[j]!=NULL)
    {
        if(strcmp(arguments[j],"-a")==0)
            is_a =1;
        else if(strcmp(arguments[j],"-l")==0)
            is_l=1;
        else if((strcmp(arguments[j],"-al")==0) || (strcmp(arguments[j],"-la")==0))
        {
            is_a=1;
            is_l=1;
        }
        else
        {
            if(strcmp(arguments[j],"~")==0)
            {
                path = NULL;
            }
            else
            {            
                path = arguments[j];
            }
        }
        j++;
    }
    
    //ls
    if((is_a==0) && (is_l==0))
    {
        if(path == NULL)
        {
            DIR *folder = opendir(pwd);
            while((dircon = readdir(folder))!=NULL)
            {
                if(strcmp(dircon->d_name,"..")!=0 && strcmp(dircon->d_name,".")!=0)
                {
                    printf("%s ",dircon->d_name);
                }
                // free(dircon);
            }
            printf("\n");
            closedir(folder);
        }
        else
        {

                if (S_ISDIR(getMode(path)))
                {
                    DIR *folder = opendir(path);
                    while((dircon = readdir(folder))!=NULL)
                    {
                        if(strcmp(dircon->d_name,"..")!=0 && strcmp(dircon->d_name,".")!=0)
                        {
                            printf("%s ",dircon->d_name);
                        }
                    }
                    printf("\n");
                    closedir(folder);
                }
                else
                {
                    int fd = open(path,O_RDONLY);
                    if(fd==-1)
                    {
                        printf("The file %s doesn't exist\n",path);
                    }
                    else
                    {
                         printf(" %s\n",path);
                    }
                    close(fd);
                }        
        }
    }
        //ls -a
        if((is_a==1) && (is_l==0))
        {
            if(path==NULL)
            {
                DIR *folder = opendir(pwd);
                while((dircon = readdir(folder))!=NULL)
                {
                    printf("%s ",dircon->d_name);
                }
                printf("\n");
                closedir(folder);
            }
            else
            {
                if (S_ISDIR(getMode(path)))
                {
                    DIR *folder = opendir(path);
                    while((dircon = readdir(folder))!=NULL)
                    {
                        printf("%s ",dircon->d_name);
                    }
                    printf("\n");
                    closedir(folder);
                }
                else
                {
                    int fd = open(path,O_RDONLY);
                    if(fd==-1)
                    {
                        printf("The file %s doesn't exist\n",path);
                    }
                    else
                    {
                         printf(" %s\n",path);
                    }
                    close(fd);
                }       
            }
        }
        //ls -l
        if((is_l == 1) && (is_a == 0))
        {
            if(path==NULL)
            {
                DIR *folder = opendir(pwd);
                // if((folder = opendir(pwd)) == NULL)
                // {
                // printf("ERROR: Couldn't open directory.\n");
                // exit(1);
                // }
                // printf("total : %d\n",getBLOCKSZ(pwd));
                while((dircon = readdir(folder))!=NULL)
                {
                    if(strcmp(dircon->d_name,"..")!=0 && strcmp(dircon->d_name,".")!=0)
                    {
                        lspermissions(dircon->d_name);
                        lsdetails(dircon->d_name);
                        printf(" %s\n",dircon->d_name);
                    }
                    // free(dircon);
                }
                closedir(folder);
            }
            else
            {
                if(S_ISDIR(getMode(path)))
                {
                    DIR *folder = opendir(path);
                    // if((folder = opendir(path)) == NULL)
                    // {
                    // perror("direrror");
                    // exit(1);
                    // }
                    printf("total : %d\n",getBLOCKSZ(path));
                    while((dircon = readdir(folder))!=NULL)
                    {
                        if(strcmp(dircon->d_name,"..")!=0 && strcmp(dircon->d_name,".")!=0)
                        {
                            char *temp_path = (char*)calloc(PATH_MAX,sizeof(char));
                            temp_path = strcpy(temp_path,path);
                            temp_path = strcat(temp_path,"/");
                            temp_path = strcat(temp_path,dircon->d_name);
                            lspermissions(temp_path);
                            lsdetails(temp_path);
                            printf(" %s\n",dircon->d_name);
                        }
                    }
                    closedir(folder);
                }
                else
                {
                    int fd = open(path,O_RDONLY);
                        if(fd ==-1)
                        {
                            printf("The file %s doesn't exist\n",path);
                        }
                        else
                        {
                            lspermissions(path);
                            lsdetails(path);
                            printf(" %s\n",path);
                        }
                        close(fd);
                }
            }
        }
        //ls -al 
        if((is_l == 1) && (is_a == 1))
        {
            if(path==NULL)
            {
                DIR *folder = opendir(pwd);
                printf("total : %d\n",getBLOCKSZ(pwd));
                while((dircon = readdir(folder))!=NULL)
                {
                    
                    lspermissions(dircon->d_name);
                    lsdetails(dircon->d_name);
                    printf(" %s\n",dircon->d_name);
                    
                }
                closedir(folder);
            }
            else
            {
                if(S_ISDIR(getMode(path)))
                {
                    DIR *folder = opendir(path);
                    // printf("total : %d\n",getBLOCKSZ(path));
                    while((dircon = readdir(folder))!=NULL)
                    {
                        char *temp_path = (char*)calloc(PATH_MAX,sizeof(char));
                        temp_path = strcpy(temp_path,path);
                        temp_path = strcat(temp_path,"/");
                        temp_path = strcat(temp_path,dircon->d_name);
                        lspermissions(temp_path);
                        lsdetails(temp_path);
                        printf(" %s\n",dircon->d_name);
                    }
                    closedir(folder);
                }
                else
                {
                    int fd = open(path,O_RDONLY);
                        if(fd ==-1)
                        {
                            printf("The file %s doesn't exist\n",path);
                        }
                        else
                        {
                            lspermissions(path);
                            lsdetails(path);
                            printf(" %s\n",path);
                        }
                        close(fd);
                }
            }
        }
}