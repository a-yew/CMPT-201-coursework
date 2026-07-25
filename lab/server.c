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

#define handle_error(msg)                                                                          \
  do {                                                                                             \
    perror(msg);                                                                                   \
    exit(EXIT_FAILURE);                                                                            \
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
  int cfd = client->cfd;
  int client_id = client->client_id;
  free(client);

  char buf[BUF_SIZE];
  ssize_t num_read;
  // TODO: print the message received from client
  while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
    buf[num_read] = '\0';
    // TODO: increase total_message_count per message
    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    printf("Msg #%d: Client ID %d: %s", total_message_count, client_id, buf);
  }
  close(cfd);
  printf("Ending thread for client %d\n", client_id);
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
    int cfd = accept(sfd, NULL, NULL);
    pthread_mutex_lock(&client_id_mutex);
    int client_id = client_id_counter;
    client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    printf("New client created! ID %d on socker FD %d\n", client_id, cfd);

    // TODO: call handle_client() when launching a new thread, and provide
    // client_info
    struct client_info *info = malloc(sizeof(struct client_info));
    info->cfd = cfd;
    info->client_id = client_id;

    pthread_t tid;
    pthread_create(&tid, NULL, handle_client, info);
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}
