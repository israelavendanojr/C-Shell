/* CS 347 -- Mini Shell!
 * Original author Phil Nelson 2000
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include "argparse.h"
#include "builtin.h"

/* PROTOTYPES */

void processline (char *line);
ssize_t getinput(char** line, size_t* size);

/*
* main
*
* Main entry point of the myshell program.
* This is essentially the primary read-eval-print loop of the command interpreter.
*
* Runs the shell in an endless loop until an exit command is issued.
*
* Hint: Use getinput and processline as appropriate.
*/

int main () {
    char* input = NULL;
    size_t size;
    while (1)
    {
      printf("(づ￣ ³￣)づ : ");
      ssize_t len = getinput(&input, &size);

      if (len == -1) {
          perror("Couldn't read input");
          break;
      }

      if (strcmp(input, "exit\n") == 0)
          break;

      if (1 < len)
        processline(input);    
    }

    free(input);
    return EXIT_SUCCESS;
}

/*
* getinput
*
* Prompts the user for a line of input (e.g. %myshell%) and stores it in a dynamically
* allocated buffer (pointed to by *line).
* If input fits in the buffer, it is stored in *line.
* If the buffer is too small, *line is freed and a larger buffer is allocated.
* The size of the buffer is stored in *size.
*
* Args:
*   line: pointer to a char* that will be set to the address of the input buffer
*   size: pointer to a size_t that will be set to the size of the buffer *line or 0 if *line is NULL.
*
* Returns:
*   The length of the the string stored in *line.
*
* Hint: There is a standard i/o function that can make getinput easier than it sounds.
*/
ssize_t getinput(char** line, size_t* size) {
  
  ssize_t len = getline(line, size, stdin);

  if (len == -1){
    perror("Len error");
    return 0;
  }
  
  return len;
}


/*
* processline
*
* Interprets the input line as a command and either executes it as a built-in
* or forks a child process to execute an external program.
* Built-in commands are executed immediately.
* External commands are parsed then forked to be executed.
*
* Args:
*   line: string containing a shell command and arguments
*
* Note: There are three cases to consider when forking a child process:
*   1. Fork fails
*   2. Fork succeeds and this is the child process
*   3. Fork succeeds and this is the parent process
*
* Hint: See the man page for fork(2) for more information.
* Hint: The process should only fork when the line is not empty and not trying to
*       run a built-in command.
*/
void processline (char *line)
{
  pid_t cpid;
  int status;
  int argCount;
  char** arguments = argparse(line, &argCount);

  int built = builtIn(arguments, argCount);
  if (built == 0 && line != NULL)
  {
    cpid = fork();
    if (0 < cpid)
    {
      wait(&status);
    }
    else if (0 == cpid)
    {
      if (execvp(arguments[0], arguments) == -1)
      {
        perror("Execvp failed");
        exit(EXIT_FAILURE);
      }
    }
    else
      perror("Fork failed");
  }
  
  for (int i = 0; i < argCount; i++)
    free(arguments[i]);

  free(arguments);

}


