#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int unescape2(char **str) {
  char *s = *str;
  if (*s != '\\')
    return (unsigned char)*s++;

  s++;

  switch (*s) {
  case 'n':
    *str = s + 1;
    return '\n';
  case 't':
    *str = s + 1;
    return '\t';
  case 'r':
    *str = s + 1;
    return '\r';
  case 'a':
    *str = s + 1;
    return '\a';
  case 'b':
    *str = s + 1;
    return '\b';
  case 'f':
    *str = s + 1;
    return '\f';
  case 'v':
    *str = s + 1;
    return '\v';
  case '\\':
    *str = s + 1;
    return '\\';
  case 'x': {
    int val = 0;
    s++;
    for (int i = 0; i < 2 && isxdigit(s[i]); i++) {
      val *= 16;
      val += isdigit(s[i]) ? s[i] - '0' : tolower(s[i]) - 'a' + 10;
    }
    *str = s + 2;
    return val;
  }
  case '0': {
    int val = 0;
    s++;
    for (int i = 0; i < 3 && s[i] >= '0' && s[i] <= '7'; i++) {
      val = val * 8 + (s[i] - '0');
    }
    *str = s + 3;
    return val;
  }
  default:
    *str = s + 1;
    return *s;
  }
}

int main(int argc, char *argv[]) {
  int i = 1, escape = 0;
  char *out;
  if (argc == 1) {
    fprintf(stderr, "Missing parameters!\n");
    return 1;
  }

  if (strcmp(argv[i], "-e") == 0) {
    i++;
    escape = 1;
  }

  for (; i < argc; i++) {
    if (i != (escape ? 2 : 1))
      putchar(' ');

    char *p = argv[i];

    if (escape) {
      while (*p) {
        int ch = (*p == '\\') ? unescape2(&p) : (unsigned char)*p++;
        if (ch == -1)
          return 0;
        putchar(ch);
      }
    } else {
      fputs(p, stdout);
    }
  }

  putchar('\n');
  return 0;
}
