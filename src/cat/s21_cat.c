#include "s21_cat.h"

int main(int argc, char **argv) {
  flags_t flags = {0, 0, 0, 0, 0, 0};
  int error = 1;  // default for OK

  if (argc < 2) {
    printf("cat: argc < 2");
    error = 0;
  }
  if (error) {
    setup(argc, argv, &flags, &error);
  }
  return 0;
}

void setup(int argc, char **argv, flags_t *flags, int *error) {
  int result = 0;
  const char *short_flags =
      "+beEnstTv";  // '+' at index 0 to make it skip flags past files
  const struct option long_flags[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  while ((result = getopt_long(argc, argv, short_flags, long_flags, NULL)) !=
         -1) {
    switch (result) {
      case 'b':
        flags->b = 1;
        break;  // numbers only non-empty lines
      case 'e':
        flags->e = 1;
        flags->v = 1;
        break;  // v + end-of-line characters as $
      case 'E':
        flags->e = 1;
        break;
      case 'n':
        flags->n = 1;
        break;  // number all output lines
      case 's':
        flags->s = 1;
        break;  // squeeze multiple adjacent blank lines
      case 't':
        flags->t = 1;
        flags->v = 1;
        break;  // v + tabs as ^I
      case 'T':
        flags->t = 1;
        break;
      case 'v':
        flags->v = 1;
        break;  // display non-printing characters
      default: {
        printf("cat: illegal option -- %c\nusage: cat [-benstv] [file ...]\n",
               result);
        *error = 0;
        break;
      }
    }
  }
  if (*error) {
    while (optind < argc) {
      read_file(argv, flags, error);
      optind++;
    }
  }
}

void read_file(char **argv, flags_t *flags, int *error) {
  FILE *fp = NULL;
  if (flags->b && flags->n) flags->n = 0;
  if ((fp = fopen(argv[optind], "r+")) == NULL) {
    printf("cat: %s: %s\n", argv[optind], strerror(errno));
    *error = 0;
  } else {
    cat(fp, flags);
    fclose(fp);
  }
}

void cat(FILE *fp, flags_t *flags) {
  int current = 0, previous = 1, newlines_in_a_row = 0, count = 1;
  bool first = true;
  while ((current = fgetc(fp)) != EOF) {
    if (flags->s) {
      if (current == '\n') newlines_in_a_row++;
      if (current != '\n') newlines_in_a_row = 0;
    }
    if (current == '\n' && (!flags->s || newlines_in_a_row < 3)) {
      if (flags->n && (previous == '\n' || first)) printf("%6d\t", count++);
      if (flags->e) {
        printf("$");
      }
      printf("%c", current);
    }
    if (current != '\n') {
      if ((previous == '\n' || first) && (flags->n || flags->b))
        printf("%6d\t", count++);
      if (current < 32 && current != 9 && current != 10 && flags->v)
        printf("^%c", current + 64);
      else if (current > 127 && current < 160 && flags->v)
        printf("M-^%c", current - 64);
      else if (current == 127 && flags->v)
        printf("^%c", current - 64);
      else if (current == '\t' && flags->t)
        printf("^I");
      else
        printf("%c", current);
    }
    previous = current;
    first = false;
  }
}
