// Lab 8 - Starting Code for sorting data in threads using uthash
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthash.h>
#define THREAD_COUNT 3
typedef const char *word_t;

typedef struct {
  word_t word;
  size_t count;
  UT_hash_handle hh;
} word_count_entry_t;

word_count_entry_t *create_entry(word_t, size_t);

typedef word_count_entry_t *count_map_t;

typedef struct {
  count_map_t *map;
  word_t *words;
  size_t num_words;
  pthread_mutex_t *lock;
} count_thread_args_t;

count_thread_args_t *pack_args(count_map_t *map, word_t *words, size_t num_words,
                               pthread_mutex_t *lock);

static void add_word_counts_in_chunk(count_map_t *map, word_t *words, size_t num_words,
                                     pthread_mutex_t *lock) {
  // --------- Task 4 --------- \\
  // Make this function thread-safe by using the lock
  if (lock) {
    pthread_mutex_lock(lock);
  }
  for (size_t i = 0; i < num_words; i++) {
    word_count_entry_t *w = NULL;
    HASH_FIND_STR(*map, words[i], w);

    if (w) {
      w->count++;
    } else {
      w = create_entry(words[i], 1);
      HASH_ADD_STR(*map, word, w);
    }

    if (lock) {
      pthread_mutex_unlock(lock);
    }
  }
}

static void *counter_thread_func(void *);

static count_map_t count_words_parallel(word_t *words, size_t num_words) {
  // --------- Task 2 --------- \\
  // Implement this function
  // Hints:
  // - Use counter_thread_func for pthread_create(..) and modify as needed.
  // - Store the threads and their arguments so you can manage them later
  // - Initialize and pass the mutex to protect the critical sections (Task 4)

  count_map_t map = NULL;
  pthread_mutex_t count_mutex;

  pthread_t threads[THREAD_COUNT];
  count_thread_args_t *threads_args[THREAD_COUNT];

  size_t chunk_size = num_words / THREAD_COUNT;

  // TODO: Perform initialization
  thread_mutex_init(&count_mutex, NULL);
  // Launch threads
  for (size_t i = 0; i < THREAD_COUNT; i++) {
    word_t *thread_arg_words = words + i * chunk_size;
    size_t thread_arg_num_words =
        chunk_size + (i == THREAD_COUNT - 1 ? num_words % THREAD_COUNT : 0);

    // TODO: Prepare the arguments and launch the threads
    threads_args[i] = pack_args(&map, thread_arg_words, thread_arg_num_words, &count_mutex);
    pthread_create(&threads[i], NULL, counter_thread_func, threads_args[i]);
  }

  // TODO: Wait for threads to finish
  for (size_t i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
  }

  // TODO: Cleanup
  for (size_t i = 0; i < THREAD_COUNT; i++) {
    free(threads_args[i]);
  }

  pthread_mutex_destroy(&count_mutex);

  return map;
}

// Takes in an array of words of size num_words and
// returns a hash table where the key is the word
// and the value is the number of occurrences
static word_count_entry_t *count_words_seq(word_t *words, size_t num_words) {
  word_count_entry_t *map = NULL;

  // Pass all the words as a single chunk
  add_word_counts_in_chunk(&map, words, num_words, NULL);

  return map;
}

int sort_func(word_count_entry_t *a, word_count_entry_t *b);

void print_counts(count_map_t);
void delete_table(count_map_t);

int main(void) {
  word_t words_in[13] = {"the",  "quick", "brown", "fox", "jumps", "over", "the",
                         "lazy", "dog",   "the",   "the", "fox",   "brown"};
  const size_t words_in_len = 13;
  count_map_t word_map = NULL;

  // Task 2: Replace this function call with the parallelized version.
  //  word_map = count_words_seq(words_in, words_in_len);
  word_map = count_words_parallel(words_in, words_in_len);

  // Print table
  if (word_map) {
    // --------- Task 1 --------- \\
    // Sort the table by the sort function in uthash using `sort_func`.
    // TODO
    HASH_SORT(word_map, sort_func);
    print_counts(word_map);
  }

  // Cleanup
  if (word_map) {
    delete_table(word_map);
  }

  return 0;
}

word_count_entry_t *create_entry(word_t word, size_t count) {
  word_count_entry_t *ptr = malloc(sizeof(word_count_entry_t));
  ptr->word = word;
  ptr->count = count;
  return ptr;
}

int sort_func(word_count_entry_t *a, word_count_entry_t *b) { return strcmp(a->word, b->word); }

void print_counts(count_map_t word_map) {
  printf("%-32s%-10s\n", "Word", "Count");
  word_count_entry_t *current, *tmp;
  HASH_ITER(hh, word_map, current, tmp) { printf("%-32s%-10zu\n", current->word, current->count); }
}

void delete_table(count_map_t word_map) {
  word_count_entry_t *current, *tmp;
  HASH_ITER(hh, word_map, current, tmp) {
    HASH_DEL(word_map, current);
    free(current);
  }
}

count_thread_args_t *pack_args(count_map_t *map, word_t *words, size_t num_words,
                               pthread_mutex_t *lock) {
  count_thread_args_t *args = malloc(sizeof(count_thread_args_t));
  args->map = map;
  args->words = words;
  args->num_words = num_words;
  args->lock = lock;
  return args;
}

static void *counter_thread_func(void *param) {
  // Call count_words_in_chunk with the appropriate arguments
  count_thread_args_t *args = (count_thread_args_t *)param;
  add_word_counts_in_chunk(args->map, args->words, args->num_words, args->lock);

  return NULL;
}
