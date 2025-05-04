#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    for (;;)
      printf("yes\n");
  } else {
    long len, i, j;
    char *ss, *out;

    for (len = i = 0; argv[++i];)
      len += strlen(argv[i]) + 1;
    out = ss = (char *)malloc(len);

    for (i = 1; argv[i]; i++)
      ss += sprintf(ss, " %s" + !(i - 1), argv[i]);
    for (;;)
      printf("%s\n", out);
  }
}
