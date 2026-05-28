#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
int main(void) {
  char *line = NULL;
  size_t n = 0;

  while (1) {
    printf("Enter programs to run.\n");
    ssize_t len = getline(&line, &n, stdin);
    if (len == -1) {
      break;
    }
    // printf("%s\n", line);
    line[len - 1] = '\0';

    pid_t pid = fork();
    if (pid == 0) // child
    {
      if (execl(line, line, NULL) == -1) {
        perror("Execution of child process has failed");
        free(line);
        exit(-1);
      }
    } else if (pid > 0) // parent
    {
      int status;
      if (waitpid(pid, &status, 0) == -1) {
        perror("Waitpid");
        free(line);
        exit(-1);

        if (WIFEXITED(status)) {
          printf("Child process has exited,\n");
        }
      }
    } else {
      perror("Fork");
      free(line);
      exit(-1);
    }
    free(line);
    return 0;
  }
}
