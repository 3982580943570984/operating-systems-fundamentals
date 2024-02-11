#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  /* Производим считывание исходного текста */
  FILE * input_file = fopen("input.txt", "w");

  printf("Введите текст (введите \"END\" для завершения):\n");

  int buffer_size = 256;
  char buffer[buffer_size];
  while(fgets(buffer, buffer_size, stdin)) {
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


  /* Записываем результат работы в другой файл */
  FILE * output_file = fopen("output.txt", "w");

  fputs(text, output_file);
  truncate("output.txt", strlen(text));

  free(text);
  fclose(output_file);

  exit(EXIT_SUCCESS);
}

