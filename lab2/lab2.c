#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  char *input = NULL;
  size_t len = 0; // current allocated size for line
  ssize_t nread;  // size for getline

  while (1) {
    printf("Enter programs to run.\n> ");

    nread = getline(&input, &len, stdin);
    
    // input by default conatinas "\n..." we need to get rid of "\n" part
    if (nread > 0 && input[nread - 1] == '\n') {
      input[nread - 1] = '\0'; // remove new line
    }

    if (strlen(input) == 0) {
      printf("Exiting program.\n");
      break;
    } else {
      
      // create a child process
      pid_t pid = fork(); 
      if (pid == -1) { // failed to fork? 
        perror("forking failed"); 
        exit(1); 
      }
        
      // if child process successfully made
      if (pid == 0) {
        // executing the command 
        execl(input, input, (char *)NULL); 
        perror("Exec failure"); 
      } else {
        // waiting for the command 
        int status; 
        waitpid(pid, &status, 0); 
      }
    }
  }

  free(input);
  return 1;
}
