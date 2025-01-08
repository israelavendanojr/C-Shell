#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "builtin.h"
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#define BUFFERSIZE 128
#define DIRBUFFER 256

//Prototypes
static void exitProgram(char** args, int argcp);
static void cd(char** args, int argpcp);
static void pwd(char** args, int argcp);

//Group A
static void cmdA_ls(char** args, int argc);
static void cmdA_cp(char** args, int argc);
static void cmdA_env(char** args, int argc);

/*
* builtIn
*
* Checks if a command is a built-in shell command and executes it if so.
*
* Args:
*   args: array of strings containing command and arguments
*   argcp: number of elements in args array
*
* Returns:
*   1 if the command was a built-in, 0 otherwise.
*
* Built-in commands are executed directly by the shell process rather than
* being forked to a new process. This function compares the given command
* to each of the built-ins (exit, pwd, cd, and ls/cp/env or stat/tail/touch
* depending on group). If a match is found, the corresponding function is called.
*
* Hint: Refer to checklist for group specific examples
*/
int builtIn(char** args, int argcp)
{
    char* command = args[0];

    if (strcmp(command, "pwd") == 0)
    {
      pwd(args, argcp);
      return 1;
    }
    else if (strcmp(command, "cd") == 0)
    {
      cd(args, argcp);
      return 1;
    }
    else if (strcmp(command, "exit") == 0)
    {
      exitProgram(args, argcp);
      return 1;
    }
    else if (strcmp(command, "ls") == 0)
    {
      cmdA_ls(args, argcp);
      return 1;
    }
    else if (strcmp(command, "cp") == 0)
    {
      cmdA_cp(args, argcp);
      return 1;
    }
    else if (strcmp(command, "env") == 0)
    {
      cmdA_env(args, argcp);
      return 1;
    }

    return 0;
}

/*
* exitProgram
*
* Terminates the shell with a given exit status.
* If no exit status is provided, exits with status 0.
* This function should use the exit(3) library call.
*
* Args:
*   args: array of strings containing "exit" and optionally an exit status
*   argcp: number of elements in args array
*/
static void exitProgram(char** args, int argcp)
{
  if (argcp < 2)
  {
    free(args);
    exit(0);
  }

  char* exitStatus = args[1];
  int exitInt = atoi(exitStatus);
  free(args);
  exit(exitInt);

}

/*
* pwd
*
* Prints the current working directory.
*
* Args:
*   args: array of strings containing "pwd"
*   argcp: number of elements in args array, should be 1
*
* Example Usage:
*   Command: $ pwd
*   destFD: /some/path/to/directory
*/
static void pwd(char** args, int argcp)
{
  char *currentDir = malloc(DIRBUFFER);
  getcwd(currentDir, DIRBUFFER);
  printf("%s\n", currentDir);
  free(currentDir);
}

/*  }

* cd
*
* Changes the current working directory.
* When no parameters are provided, changes to the home directory.
* Supports . (current directory) and .. (parent directory).
*
* Args:
*   args: array of strings containing "cd" and optionally a directory path
*   argcp: number of elements in args array
*
* Example Usage:
*   Command: $ pwd
*   destFD: /some/path/to/directory
*   Command: $ cd ..
*   Command: $ pwd
*   destFD: /some/path/to
*
* Hint: Read the man page for chdir(2)
*/
static void cd(char** args, int argcp)
{
  char* path;
  if (argcp == 1)
    path = getenv("HOME");
  else if (argcp == 2)
    path = args[1];
  else
  {
    perror("Bad arguments CD");
    exit(EXIT_FAILURE);
  }

  if (chdir(path) == -1)
  {
    perror("Error changing directory");
    exit(EXIT_FAILURE);
  }
}

static void cmdA_ls(char** args, int argc)
{
  //get directory and directory entries
  struct dirent *directory = NULL;
  char *currentDir = malloc(DIRBUFFER);
  getcwd(currentDir, DIRBUFFER);
  DIR *dirp = opendir(currentDir);
  if (!dirp)
  {
    perror("Couldnt perform opendir()");
    exit(EXIT_FAILURE);
  }

  int longVersion = (1 < argc && strcmp(args[1], "-l") == 0);

  //iterate directory and print names
  while ((directory = readdir(dirp)) != NULL)
  {
    if (longVersion)
    {
      if (directory->d_name[0] != '.')
      {
        struct stat* fileStat = malloc(sizeof(struct stat));
        struct passwd *pwd = NULL;
        struct group *grp = NULL;

        if (stat(directory->d_name, fileStat) == -1)
        {
          perror("Couldnt get stat");
          free(fileStat);
          continue;
        }

        printf( (S_ISDIR(fileStat->st_mode)) ? "d" : "-");
        //user
        printf( (fileStat->st_mode & S_IRUSR) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWUSR) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXUSR) ? "x" : "-");
        //groups
        printf( (fileStat->st_mode & S_IRGRP) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWGRP) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXGRP) ? "x" : "-");
        //world
        printf( (fileStat->st_mode & S_IROTH) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWOTH) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXOTH) ? "x" : "-");

        pwd = getpwuid(fileStat->st_uid);
        grp = getgrgid(fileStat->st_gid);

        char timeBuffer[20];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y %b %d %H:%M", 
          localtime(&fileStat->st_mtime));

        printf("%lu %s %5s %ld %s %s\n", fileStat->st_nlink, pwd->pw_name, grp->gr_name
          , fileStat->st_size, timeBuffer, directory->d_name);
        
        
        free(fileStat);
      }
    }
    else
    {
      if (directory->d_name[0] == '.')
        continue;
      printf("%s ", directory->d_name);
    }
  
  }
  

  closedir(dirp);
  free(currentDir);
}

static void cmdA_cp(char** args, int argc)
{
  //arguments passed
  char* source = args[1];
  char* destination = args[2];

  //source and dest paths
  int sourceFD = open(source, O_RDONLY);
  int destFD = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  char buffer[BUFFERSIZE];
  int bytesRead = 0;

  //if sourcePathValid && destinationPathValid
  if ((0 < sourceFD) && (0 < destFD))
  {

    //iterate through source
    while ((bytesRead = read(sourceFD, buffer, BUFFERSIZE)) != 0)
    {

      if (destFD < 0)
      {
        perror("Open Error\n");
        exit(EXIT_FAILURE);
      }
      else
      {
        //write into new file
        int sourceCopy = write(destFD, buffer, bytesRead);
        if (sourceCopy < 0)
        {
          perror("Write Error\n");
          exit(EXIT_FAILURE);
        }
      }
    }

    if (close(sourceFD) < 0 || close(destFD) < 0)
    {
      perror("Close Error\n");
      exit(EXIT_FAILURE);
    }

    close(sourceFD);
    close(destFD);
  }


}


static void cmdA_env(char** args, int argc)
{ 
  extern char** environ;
  if (argc == 1)
  {
    int i = 0;
    while (environ[i] != NULL)
    {
        printf("%s\n", environ[i]);
        i++;
    }
  }
  else if(argc == 2)
  {
    char *nameValue = args[1];
    char *equalChar = strchr(nameValue, '=');

    if (equalChar == NULL)
    {
      perror("EqualChar not found");
      exit(EXIT_FAILURE);

    }
    *equalChar = '\0';
    if (setenv(nameValue, equalChar + 1, 1) == -1)
    {
      perror("SetEnv() Error");
      *equalChar = '=';
      exit(EXIT_FAILURE);
    }
    *equalChar = '=';

    int i = 0;
    while (environ[i] != NULL)
    {
        printf("%s\n", environ[i]);
        i++;
    }
  }
}
