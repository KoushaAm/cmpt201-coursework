/*
Questions to answer at top of client.c:
(You should not need to change the code in client.c)
1. What is the address of the server it is trying to connect to (IP address and
port number). answer: 127.0.0.1 local host (IP) and port number 8000
2. Is it UDP or TCP? How do you know?
answer: TCP because the socket is called with SOCK_STREAM ,  which is a TCP
socket
3. The client is going to send some data to the server. Where does it get this
data from? How can you tell in the code? answer: data comes from our standard
input , the code reads form STDIN_FILENO with read and write it into the buffer
data in socket
4. How does the client program end? How can you tell that in the code?
answer: it ends and when read from standard input returns 0 (an EOF)
    then it closes the connection with close() and exit(EXIT_SUCCESS)
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}





#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;

  // TODO: print the message received from client
  int cfd = client->cfd;
  int client_id = client->client_id;

  printf("New client created! ID %d on socket FD %d\n", client_id, cfd);
  char buf[BUF_SIZE];
  ssize_t n;

  while ((n = read(cfd, buf, BUF_SIZE - 1)) > 0) {
    buf[n] = '\0'; // tulll termination for the string
    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    int current_count = total_message_count;
    pthread_mutex_unlock(&count_mutex);
    printf("Msg # %4d; Client ID %d: %s", current_count, client_id, buf);
    if (buf[n - 1] != '\n')
      printf("\n");
  }
  // TODO: increase total_message_count per message
  // done above
  if (n == 0) {
    printf("Ending thread for client %d\n", client_id);
  } else if (n < 0) {
    perror("read");
  }
  close(cfd);
  free(client);
  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }

  for (;;) {
    // TODO: create a new thread when a new connection is encountered
    // TODO: call handle_client() when launching a new thread, and provide
    // client_info
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int cfd = accept(sfd, (struct sockaddr *)&client_addr, &client_len);
    if (cfd == -1) {
      perror("accept");
      continue;
    }
    pthread_mutex_lock(&client_id_mutex);
    int cid = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    struct client_info *info = malloc(sizeof(struct client_info));
    info->cfd = cfd;
    info->client_id = cid;

    pthread_t tid;
    if (pthread_create(&tid, NULL, handle_client, info) != 0) {
      perror("pthread_create");
      close(cfd);
      free(info);
      continue;
    }

    pthread_detach(tid);
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}
