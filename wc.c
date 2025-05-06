#include <stdio.h>
#include <stdlib.h>

typedef struct {
  unsigned long int lines;
  unsigned long int words;
  unsigned long int bytes;
} Counts;

Counts do_wc(FILE *stream) {
  Counts c = {0, 0, 0};
  int ch;

  while ((ch = fgetc(stream)) != EOF) {
    c.bytes++;
    if (ch == '\n')
      c.lines++;

    _Bool in_word = 0;
    if (ch == ' ' || ch == '\n' || ch == '\t')
      in_word = 0;
    else if (!in_word) {
      in_word = 1;
      c.words++;
    }
  }

  return c;
}

void update_totals(Counts *totals, Counts file_counts) {
  totals->bytes += file_counts.bytes;
  totals->lines += file_counts.lines;
  totals->words += file_counts.words;
}

int main(int argc, char *argv[]) {
  Counts totals = {0, 0, 0};

  if (argc == 1) {
    fprintf(stderr, "Missing parameters");
    return 1;
  } else {

    for (int i = 1; i < argc; i++) {
      Counts c = {0, 0, 0};
      FILE *file = fopen(argv[i], "r");
      if (file == NULL) {
        fprintf(stderr, "Unable to load the file %s", argv[i]);
        return 1;
      } else {
        c = do_wc(file);
        update_totals(&totals, c);
        printf("%7lu %7lu %7lu %s\n", c.lines, c.words, c.bytes, argv[i]);
        fclose(file);
      }
    }
  }

  if (argc > 2)
    printf("%7lu %7lu %7lu total\n", totals.lines, totals.words, totals.bytes);

  return EXIT_SUCCESS;
}
