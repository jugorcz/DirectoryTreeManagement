#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int lstat(const char *restrict path, struct stat *restrict buf);

char *realpath(const char *restrict file_name,
       char *restrict resolved_name);

typedef struct
{
    int day;
    int month;
    int year;
} date;

date* modificationDate;
int operand;
char directoryPath[256];

int checkOperand(char* operand);
date* checkDate(char* dateToCheck);

int properArguments(int argc, char* argv[])
{
    if(argc < 4)
    {
        printf("\nWrong arguments amount, should be four: \n"
                       "[1] file name \n"
                       "[2] directory path \n"
                       "[3] operand like: < > =\n"
                       "[4] modification date like 01-02-2018\n");
        return 0;
    }

    /* set path */
    realpath(argv[1],directoryPath);

    operand = checkOperand(argv[2]);
    if(operand == 0)
        return 0;

    modificationDate = checkDate(argv[3]);
    if(modificationDate == NULL)
        return 0;

    printf("\n[0]%s",argv[0]);
    printf("\n[1]%s",argv[1]);
    printf("\n[2]%s",argv[2]);
    printf("\n[3]%s",argv[3]);

    printf("\n\nnew path: %s",directoryPath);
    printf("\noperand: %d",operand);
    printf("\ndate: %d-%d-%d",modificationDate->day,modificationDate->month,modificationDate->year);

    return 1;
}

int checkOperand(char* operand)
{
    if(strcmp(operand,"less")== 0) return  1; /* < date */
    if(strcmp(operand,"more")== 0) return  2; /* > date */
    if(strcmp(operand,"equal")== 0) return  3; /* = date */

    printf("\nWrong operand, should be < > or =.\n");
    return 0;
}


date* checkDate(char* dateToCheck)
{
    if(strlen(dateToCheck) != 10){
        printf("\nWrong date format, should be dd-mm-yyyy\n");
        return NULL;
    }

    modificationDate = malloc(sizeof(date));
    long int tmp;


    char* separatedDate = strtok(dateToCheck,"-");
    if(separatedDate != NULL)
    {
        tmp = strtol(separatedDate, NULL, 10);
        if(tmp > 0 && tmp <= 31)
            modificationDate -> day = (int)tmp;
        else{
            free(modificationDate);
            return NULL;
        }
    }

    separatedDate = strtok(NULL,"-");
    if(separatedDate != NULL)
    {
        tmp = strtol(separatedDate, NULL, 10);
        if(tmp > 0 && tmp <= 12)
            modificationDate -> month = (int)tmp;
        else{
            free(modificationDate);
            return NULL;
        }
    }

    separatedDate = strtok(NULL,"-");
    if(separatedDate != NULL)
    {
        tmp = strtol(separatedDate, NULL, 10);
        if(tmp > 0 && tmp <= 2018)
            modificationDate -> year = (int)tmp;
        else{
            free(modificationDate);
            return NULL;
        }
    }

    return modificationDate;
}

int properDate(time_t time)
{
    struct tm* timeInfo;
    timeInfo = localtime (&(time));
    int day = timeInfo->tm_mday;
    int month = timeInfo->tm_mon + 1;
    int year = timeInfo->tm_year + 1900;

    if(operand == 1) /* fileDate < date */
    {
        if(year < modificationDate->year) return 1;
        if(year == modificationDate->year && month < modificationDate->month) return 1;
        if(year == modificationDate->year && month == modificationDate->month
           && day < modificationDate->day) return 1;
        return 0;
    }

    if(operand == 2) /* fileDate > date */
    {
        if(year > modificationDate->year) return 1;
        if(year == modificationDate->year && month > modificationDate->month) return 1;
        if(year == modificationDate->year && month == modificationDate->month
           && day > modificationDate->day) return 1;
        return 0;
    }

    if(operand == 3) /* fileDate = date */
    {
        if(year == modificationDate->year && month == modificationDate->month
           && day == modificationDate->day) return 1;
        return 0;
    }

    return 0;
}

void displayContent(char* path)
{
    chdir(path);

    DIR* directory = opendir(path);
    if(directory == NULL) {
        printf("\nError - cannot open directory.");
        return;
    }

    struct dirent* dirContent;
    struct stat fileStatus;

    struct tm* timeInfo;
    char timeBuff[20];
    char filePath[256];
	pid_t childPid;
    while ((dirContent = readdir(directory)) != NULL)
    {
        if(strcmp(dirContent->d_name,".") == 0) continue;
        if(strcmp(dirContent->d_name,"..") == 0) continue;

        realpath(dirContent->d_name,filePath);
        lstat(filePath,&fileStatus);
		printf("\n\n\t\t\tproces: %d czyta: %s\n",(int)getpid(),dirContent -> d_name);
		
        if(S_ISDIR(fileStatus.st_mode) && !S_ISLNK(fileStatus.st_mode))
        {
        	int res;
        	printf("\n");
        	childPid = fork();
			if(childPid == 0) //proces dziecka
			{
            	displayContent(filePath);
            	chdir(path);
            	return;
            }
            else
    		{
    			wait(&res);
				if(WEXITSTATUS(res) != 0) return;
    		}
        }
        else if(!S_ISLNK(fileStatus.st_mode) && S_ISREG(fileStatus.st_mode) && properDate(fileStatus.st_mtime) == 1)
        {
            printf("\n\nFile name: \t\t%s\n", dirContent -> d_name);
            printf("File Size: \t\t%d bytes\n",(int)fileStatus.st_size);

            printf("File Permissions: \t");
            printf( (S_ISDIR(fileStatus.st_mode)) ? "d" : "-");
            printf( (fileStatus.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileStatus.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileStatus.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileStatus.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileStatus.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileStatus.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileStatus.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileStatus.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileStatus.st_mode & S_IXOTH) ? "x" : "-");

            timeInfo = localtime (&(fileStatus.st_mtime));
            strftime(timeBuff,20,"%b %d %H:%M",timeInfo);
            printf("\nLast modification date:\t%s\t",timeBuff);
        }
    }
}

int main(int argc, char* argv[])
{
    if(properArguments(argc,argv) == 0) return 0;
    printf("\n");

    displayContent(directoryPath);

    return 0;
}
