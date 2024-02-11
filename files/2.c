#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
  char target_dir[256];

  printf("Введите имя каталога, в который будет выполнено копирование: ");
  scanf("%255s", target_dir);

  DIR * dir = opendir(".");
  if (dir == NULL) {
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  struct dirent * entry;
  struct stat entry_stat;

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == 'a' || entry->d_name[0] == 'z') {
      stat(entry->d_name, &entry_stat);

      // https://stackoverflow.com/questions/40163270/what-is-s-isreg-and-what-does-it-do
      if (S_ISREG(entry_stat.st_mode)) {
        char target_path[512];
        
        snprintf(target_path, sizeof(target_path), "%s/%s", target_dir, entry->d_name);

        int source_fd = open(entry->d_name, O_RDONLY);
        int destin_fd = open(target_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

        int buffer_size = 256;
        char buffer[buffer_size];
        ssize_t bytes_read;

        while ((bytes_read = read(source_fd, buffer, buffer_size)) > 0) {
          write(destin_fd, buffer, bytes_read);
        }

        close(source_fd);
        close(destin_fd);
      }
    }
  }

  closedir(dir);

  exit(EXIT_SUCCESS);
}



