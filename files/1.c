#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
  /* Производим считывание исходного текста */
  FILE * input_file = fopen("input.txt", "w");

  printf("Введите текст (введите \"END\" для завершения):\n");

  int buffer_size = 256;
  char buffer[buffer_size];
  while(fgets(buffer, buffer_size, stdin) != NULL) {
    if (strncmp(buffer, "END\n", 4) == 0) {
      break;
    }

    fputs(buffer, input_file);
  }

  fclose(input_file);

  /* Открываем файл на чтение */
  input_file = fopen("input.txt", "r");

  /* Определяем размер файла и выделяем память для его содержимого */
  fseek(input_file, 0, SEEK_END);
  int file_size = ftell(input_file);
  char * text = malloc(file_size + 1);

  /* Записываем содержимое файла в буфер */
  rewind(input_file);
  fread(text, 1, file_size, input_file);
  text[file_size] = '\0';

  fclose(input_file);

  /* Производим обработку текста */
  char * first_word = NULL, * last_word = NULL;
  char * delimeters = " \n\t";
  char * text_duplicate = strdup(text);
  char * token = strtok(text_duplicate, delimeters);

  if (token != NULL) {
    first_word = token;
    while ((token = strtok(NULL, delimeters)) != NULL) {
      last_word = token;
    }
  }

  if (first_word && last_word && strcmp(first_word, last_word) != 0) {
    char * first_pos = strstr(text, first_word);
    char * last_pos = strstr(text, last_word);

    size_t first_len = strlen(first_word);
    size_t last_len = strlen(last_word);

    char * temp_first_word = strndup(first_word, first_len);
    char * temp_last_word = strndup(last_word, last_len);

    memcpy(first_pos, temp_last_word, last_len);
    memcpy(last_pos, temp_first_word, first_len);

    free(temp_first_word);
    free(temp_last_word);
  }

  /* Записываем результат работы в другой файл */
  FILE * output_file = fopen("output.txt", "w");

  fputs(text, output_file);
  truncate("output.txt", strlen(text));

  free(text);
  fclose(output_file);

  exit(EXIT_SUCCESS);
}


