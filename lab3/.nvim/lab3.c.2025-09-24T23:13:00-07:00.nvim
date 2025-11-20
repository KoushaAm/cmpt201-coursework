#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 5 // global varaiable for history size default 5

// strips the '/n' at the end of input line
void strip_newline(char *str) {
  size_t len = strlen(str);
  if (len > 0 && str[len - 1] == '\n') {
    str[len - 1] = '\0';
  }
}

// helper function print the lines in history
void print_history(char *history[], int history_index) {
  for (int i = 0; i < HISTORY_SIZE; i++) {
    int idx = (history_index + i) % HISTORY_SIZE;
    if (history[idx] != NULL) {
      printf("%s\n", history[idx]);
    }
  }
}

int main(void) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char *history[HISTORY_SIZE] = {
      NULL}; // our memory that stores pointers to the strings that is inputed

  int history_index = 0; // current writable index in history array

  while (true) {
    printf("Enter input: ");
    read = getline(&line, &len, stdin);

    if (read == -1) {
      break;
    }

    strip_newline(line); // get rid of \n

    // checking if the history index is occupied
    if (history[history_index] != NULL) {
      free(history[history_index]);
    }

    // update the history array with the new line
    history[history_index] = strdup(line);
    // make sure the history slot is updated
    if (history[history_index] == NULL) {
      perror("failed to write into history");
      exit(EXIT_FAILURE);
    }

    // advance to the next writable index within 0 to 4
    history_index = (history_index + 1) % HISTORY_SIZE;

    // if the input was "print" print the history
    if (strcmp(line, "print") == 0) {
      print_history(history, history_index);
    }
  }

  // cleaning up memory
  for (int i = 0; i < HISTORY_SIZE; i++) {
    free(history[i]);
  }

  free(line);
}
