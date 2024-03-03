#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
  /* Запись текста в файл */
  int input_fd = open("input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);

  printf("Введите текст (введите \"END\" для завершения):\n");

  int buffer_size = 256;
  char buffer[buffer_size];
  while(fgets(buffer, buffer_size, stdin) != NULL) {
    if (strncmp(buffer, "END\n", 4) == 0) {
      break;
    }

    write(input_fd, buffer, strlen(buffer));
  }

  close(input_fd);

  /* Чтение из файла и обработка */
  input_fd = open("input.txt", O_RDONLY);

  /* Определяем размер файла и выделяем память для его содержимого */
  int file_size = lseek(input_fd, 0, SEEK_END);
  char *text = malloc(file_size + 1);
  lseek(input_fd, 0, SEEK_SET);

  /* Записываем содержимое файла в буфер */
  ssize_t bytes_read = read(input_fd, text, file_size);
  if (bytes_read < file_size) {
    perror("Ошибка чтения файла");
    close(input_fd);
    free(text);
    exit(EXIT_FAILURE);
  }
  text[file_size] = '\0';
  close(input_fd);

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
  int output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0777);
  ssize_t bytes_written = write(output_fd, text, strlen(text));
  if (bytes_written < 0) {
    perror("Ошибка записи в файл");
  }

  free(text);
  close(output_fd);

  exit(EXIT_SUCCESS);
}


