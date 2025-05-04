#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    while (1) {
      printf("yes\n");
    }
  } else {

    long len, i, j;
    char *out;

    for (len = i = 0; argv[++i];)
      len += strlen(argv[i]) + 1;

    out = (char *)malloc(len);

    if (out == NULL) {
      fprintf(stderr, "Memory allocation failed!\n");
      return 1;
    }

    for (i = 1; argv[i]; i++) {
      if (i > 1)
        strcat(out, " ");
      strcat(out, argv[i]);
    }

    while (1) {
      printf("%s\n", out);
    }
  }
}
