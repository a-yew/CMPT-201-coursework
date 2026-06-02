#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LEN 5

typedef struct HistoryNode {
  char *input;
  struct HistoryNode *next;
} HistoryNode;

HistoryNode *head = NULL;
HistoryNode *tail = NULL;
int history_count = 0;

char *get_input(void);
void add_to_history(char *input);
void remove_oldest_history(void);
void print_history(void);
void free_history(void);

int main(void) {
  while (1) {
    char *input = get_input();
    if (input == NULL) {
      break;
    }
    if (strcmp(input, "print") == 0) {
      print_history();
    }
  }
  free_history();
  return 0;
}

char *get_input(void) {
  char *line = NULL;
  size_t n = 0;
  printf("Enter input: ");
  ssize_t len = getline(&line, &n, stdin);
  if (len == -1) {
    free(line);
    return NULL;
  }
  if (len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0';
  }
  return line;
}

void remove_oldest_record(void) {
  if (head == NULL) {
    return;
  }
  HistoryNode *old_head = head;
  head = head->next;
  if (head == NULL) {
    tail = NULL;
  }
  free(old_head->input);
  free(old_head);
  history_count--;
}

void add_to_history(char *input) {
  HistoryNode *new_node = malloc(sizeof(HistoryNode));
  if (new_node == NULL) {
    perror("malloc error");
    free(input);
    exit(-1);
  }
  new_node->input = input;
  new_node->next = NULL;
  if (tail == NULL) {
    head = new_node;
    tail = new_node;
  } else {
    tail->next = new_node;
    tail = new_node;
  }
  history_count++;
  if (history_count > MAX_LEN) {
    remove_oldest_record();
  }
}
void print_history(void) {
  HistoryNode *current = head;
  while (current != NULL) {
    printf("%s\n", current->input);
    current = current->next;
  }
}
