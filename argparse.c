#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "argparse.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FALSE (0)
#define TRUE  (1)
#define WHITESPACE ' '

/*
* argCount
*
* Counts the number of arguments in a given input line.
* You may assume only whitespace is used to separate arguments.
* argCount should be able to handle multiple whitespaces between arguments.
*
* Args:
*   line: intput string containing command and arguments separated by whitespaces
*
* Returns:
*   The number of arguments in line.
*   (The command itself counts as the first argument)
*
* Example:
*   argCount("ls -l /home") returns 3
*   argCount("   ls    -l   /home  ") returns 3
*/
static int argCount(char*line)
{
  int count = 0;
  int inArgument = FALSE;

  while (*line != '\0')
  {
    if (*line != WHITESPACE)
    {
      if (inArgument == FALSE)
      {
        inArgument = TRUE;
        count++;
      }
    }
    else
      inArgument = FALSE;

    line++;
  }

  return count;
}



/*
* argparse
*
* Parses an input line into an array of strings.
* *word
*
* You may assume only whitespace is used to separate strings.
* argparse should be able to handle multiple whitespaces between strings.
* The function should dynamically allocate space for the array of strings,
* following the project requirements. 
*
* Args:
*   line: input string containing words separated by whitespace
*   argcp: stores the number of strings in the line
*
* Returns:
*   An array of strings.
*
* Example:
*   argparse("ls -l /home", &argc) --> returns ["ls", "-l", "/home"] and set argc to 3
*   argparse("   ls    -l   /home  ", &argc) --> returns ["ls", "-l", "/home"] and set argc to 3
*/

// void snipChar(char* str, char character)
// {
//   int len = strlen(str);
//   if (str[len - 1] == character && 0 < len)
//         str[len - 1] = '\0';
// }

char** argparse(char* line, int* argcp){
  *argcp = argCount(line);
  char **args = (char **)malloc((sizeof(char*)) * (*argcp + 1));
  if (args == NULL)
  {
    perror("Malloc Error");
    exit(EXIT_FAILURE);
  }

  int i = 0;
  int j = 0;
  while (line[i] != '\n')
  {
    if (line[i] == WHITESPACE)
      i++;
    else if (line[i]!= WHITESPACE && line[i] != '\n')
    {
      int strlen = 0;
      
      while(line[i + strlen] != WHITESPACE && line[i + strlen] != '\n')
        strlen++;

      args[j] = (char*)malloc(strlen+1);
      if (args[j] == NULL)
      {
        perror("Malloc Error");
        for (int k = 0; k < j; k++)
          free(args[k]);
        
        free(args);
        exit(EXIT_FAILURE);
      }
      strncpy(args[j], (line+i), strlen);
      args[j][strlen] = '\0';
      j++;
      i += strlen;
    }
  }
  args[*argcp] = NULL;
  return args;
}



