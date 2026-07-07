#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT 100

typedef struct {
  int line_number;
  int value;
} Input;

typedef struct {
  int line_number;
  int doubled_value;
} IntermediateInput;

typedef struct {
  int doubled_value;
  int line_numbers[MAX_INPUT];
  int count;
} Output;

void map(Input *input, IntermediateInput *intermediate_input);
void groupByKey(IntermediateInput *input, Output *output, int *result_count);
void reduce(Output *output);

int main() {
  Input input_data[MAX_INPUT];
  int input_size = 0;
  int value;

  // Read input values until "end" is encountered
  printf("Enter values (one per line). Type 'end' to finish:\n");
  while (input_size < MAX_INPUT) {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
      break;
    }
    if (sscanf(buffer, "%d", &value) == 1) {
      input_data[input_size].line_number = input_size + 1;
      input_data[input_size].value = value;
      input_size++;
    } else if (sscanf(buffer, "end") == 0) {
      break;
    } else {
      printf("Invalid input. Please enter an integer or 'end' to finish.\n");
    }
  }

  // Step 1: Map phase

  IntermediateInput mapped_results[MAX_INPUT] = {0};

  for (int i = 0; i < input_size; i++) {
    map(&input_data[i], &mapped_results[i]);
  }

  // Step 2: Grouping phase

  Output output_results[MAX_INPUT] = {0};
  int result_count = 0;

  for (int i = 0; i < input_size; i++) {
    groupByKey(&mapped_results[i], output_results, &result_count);
  }

  // Step 3: Reduce phase
  for (int i = 0; i < result_count; i++) {
    if (output_results[i].count > 0) {
      reduce(&output_results[i]);
    }
  }

  return 0;
}

void map(Input *input, IntermediateInput *intermediate_input) {
  (*intermediate_input).line_number = (*input).line_number;
  (*intermediate_input).doubled_value = (*input).value * 2;
}

void groupByKey(IntermediateInput *input, Output *output, int *result_count) {
  int doubled_value = (*input).doubled_value;
  int line_number = (*input).line_number;
  for (int i = 0; i < *result_count; i++) {
    if (output[i].doubled_value == doubled_value) {
      output[i].line_numbers[output[i].count] = line_number;
      output[i].count++;
      return;
    }
  }
  output[*result_count].doubled_value = doubled_value;
  output[*result_count].line_numbers[0] = line_number;
  output[*result_count].count = 1;
  (*result_count)++;
}

void reduce(Output *output) {
  int doubled_value = (*output).doubled_value;
  int count = (*output).count;
  printf("%d, [", doubled_value);
  for (int i = 0; i < count; i++) {
    printf("%d", (*output).line_numbers[i]);
    if (i < count - 1) {
      printf(", ");
    }
  }
  printf("])\n");
}
