#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {

  // for get line
  char *buff = NULL;
  size_t buff_size = 0;
  ssize_t nread;

  printf("please enter some number: \n");
  nread = getline(&buff, &buff_size, stdin);

  // error handling for input
  if (nread == -1) {
    perror("getline failed");
    free(buff);
    exit(EXIT_FAILURE);
  }

  printf("Tokens: \n");

  // tokenizing single spaced " "
  char *saveptr = NULL;
  char *tok = strtok_r(buff, " ", &saveptr);

  while (tok != NULL) {
    printf(" %s\n", tok);
    tok = strtok_r(NULL, " ", &saveptr);
  }

  free(buff);
  return EXIT_SUCCESS;
}
