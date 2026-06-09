#define _DEFAULT_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 256

struct header {
  uint64_t size;
  struct header *next;
};

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data : *(void **)data);

  // if (len < 0) {handle_error("snprintf");
  write(STDOUT_FILENO, buf, len);
}

int main() {
  // increase heap size
  void *ptr = sbrk(265);
  // initialize block
  // initialize block
  struct header *my_block = (struct header *)ptr;
  struct header *my_block2 = (struct header *)ptr + 128;

  my_block->size = 128;
  my_block->next = NULL;

  my_block2->size = 128;
  my_block2->next = my_block;

  size_t data_size = 128 - sizeof(struct header);
  char *data = (char *)my_block + sizeof(struct header);
  char *data2 = (char *)my_block2 + sizeof(struct header);

  memset(data, 0, data_size);
  memset(data2, 1, data_size);

  // print
  print_out("first block: %p\n", &my_block, sizeof(my_block));
  print_out("second block: %p\n", &my_block2, sizeof(my_block2));
  print_out("first block size: %lu\n", &my_block->size, sizeof(uint64_t));
  print_out("first block next: %p\n", &my_block->next, sizeof(my_block->next));
  print_out("second block size: %lu\n", &my_block2->size, sizeof(uint64_t));
  print_out("second block next: %p\n", &my_block2->next, sizeof(my_block2->next));

  for (size_t i = 0; i < data_size; i++) {
    uint64_t val = (uint8_t)data[i];
    print_out("%lu\n", &val, sizeof(uint64_t));
  }

  for (size_t i = 0; i < data_size; i++) {
    uint64_t val = (uint8_t)data2[i];
    print_out("%lu\n", &val, sizeof(uint64_t));
  }
  return 0;
}
