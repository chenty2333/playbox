#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1001
char buffer[BUFFER_SIZE];
unsigned int len = 0;

void do_cat(FILE *file) {

  while ((len = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
    fwrite(buffer, 1, len, stdout);
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    fprintf(stderr, "Missing parameters!");
    return 1;
  }

  int i = 1;
  do {
    FILE *file = fopen(argv[i], "rb");
    if (file == NULL) {
      fprintf(stderr, "The file does not exit!\n");
      return 1;
    }

    do_cat(file);

    fclose(file);
  } while (++i < argc);

  return 0;
}
