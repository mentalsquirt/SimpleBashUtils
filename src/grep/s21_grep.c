#include "s21_grep.h"

#define MAX_SIZE 4096
#define true 1
#define false 0

int main(int argc, char **argv) {
  int error = true;  // default for OK
  char **patterns;
  if ((patterns = (char **)malloc(MAX_SIZE * MAX_SIZE * sizeof(char) +
                                  MAX_SIZE * sizeof(char *))) == NULL) {
    exit(1);
  }
  char *ptr = (char *)(patterns + MAX_SIZE);
  for (int i = 0; i < MAX_SIZE; i++) patterns[i] = ptr + MAX_SIZE * i;
  for (int i = 0; i < MAX_SIZE; i++) {
    patterns[i][0] = 7;  // using bell as an "empty" value
  }
  flags_t flags = {0};
  //  '-' at the patterns[i][0] means that this is a file and it should be
  //  opened

  if (argc < 3) {
    printf("grep: argc < 3\n");
    error = false;
  }

  if (error) {
    setup(argc, argv, &flags, &error, patterns);
  }

  // -e not -e and -e int
  // "not|and|int"

  char *pattern_str;
  if ((pattern_str = (char *)malloc(MAX_SIZE * sizeof(char))) == NULL) {
    exit(1);
  }

  if (error && patterns[0][0] != 7) {
    int i = 0;  // counter for final pattern string
    strncpy(pattern_str, patterns[0], MAX_SIZE - i);
    strncat(pattern_str, "|", MAX_SIZE - i++);
    i += strlen(patterns[0]);
    for (int element = 1; patterns[element][0] != 7; element++) {
      strncat(pattern_str, patterns[element], MAX_SIZE - i);
      strncat(pattern_str, "|", MAX_SIZE - i++);
      i += strlen(patterns[element]);
    }
    if (pattern_str[i - 1] == '|') pattern_str[i - 1] = '\0';
  }

  int files = argc;
  for (int i = optind; i < argc; i++) {
    if (argv[i][0] == '-') {
      files = argc - i - 1;
      break;
    } else {
      files = (argc - optind);
    }
  }
  if (files == 1) flags.single_file = true;

  if (error) {
    for (int i = 0; i < files; i++) {
      grep(&flags, pattern_str, argv[argc - files + i], &error);
    }
  }

  free(pattern_str);
  free(patterns);
  return 0;
}

int setup(int argc, char **argv, flags_t *flags, int *error, char **patterns) {
  int result = 0;
  int pattern_counter = 0;
  int files_pattern_counter = 0;
  const char *short_flags = "+:e:ivclnhsf:o";

  while ((result = getopt(argc, argv, short_flags)) != -1) {
    switch (result) {
      case 'e': {
        flags->e++;
        strncpy(patterns[pattern_counter], optarg, strlen(optarg));
        pattern_counter++;
        break;
      }
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f': {
        flags->f++;
        patterns[pattern_counter][0] = '-';
        strncat(patterns[pattern_counter], optarg,
                MAX_SIZE - strlen(patterns[pattern_counter]));
        pattern_counter++;
        files_pattern_counter++;
        break;
      }
      case 'o':
        flags->o = 1;
        break;
      case '?': {
        printf(
            "grep: invalid option -- %c\nusage: grep [-eivclnhsfo] [pattern] "
            "[file ...]\n",
            optopt);
        exit(1);
      }
    }
  }

  if (*error) {
    while (files_pattern_counter > 0) {
      for (int i = 0; i < pattern_counter; i++) {
        if (patterns[i][0] == '-') {
          patterns[i] = &patterns[i][1];
          read_file_pattern(patterns, i, error);
        }
      }
      if (*error) {
        files_pattern_counter--;
      } else {
        files_pattern_counter = 0;
      }
    }
  }

  if (*error && !flags->e && !flags->f) {
    strncpy(patterns[pattern_counter++], argv[optind++], MAX_SIZE);
  }

  return *error;
}

void read_file_pattern(char **patterns, int element, int *error) {
  FILE *fp = NULL;
  int i = 0;
  if ((fp = fopen(patterns[element], "r")) != NULL) {
    int c;
    while ((c = fgetc(fp)) != EOF) {
      if (c == 13 || c == 10) patterns[element][i++] = '|';
      if (c != 13 && c != 10) patterns[element][i++] = c;
    }
  } else {
    *error = false;
    printf("grep: %s: No such file or directory\n", patterns[element]);
  }
  if (patterns[element][i - 1] == '|') patterns[element][i - 1] = '\0';
  fclose(fp);
}

void grep(flags_t *flags, char *pattern_str, char *filename, int *error) {
  FILE *fp;

  if ((fp = fopen(filename, "r")) == NULL) {
    *error = false;  // file did not open error
    if (!flags->l && !flags->s)
      printf("grep: %s: No such file or directory\n", filename);
    return;
  }

  char *temp;
  if ((temp = (char *)malloc(MAX_SIZE * sizeof(char))) == NULL) {
    exit(1);
  }
  char *line;
  if ((line = (char *)malloc(MAX_SIZE * sizeof(char))) == NULL) {
    exit(1);
  }
  int cflags = REG_EXTENDED;
  int lines_with_matches = 0, line_n = 1;

  if (flags->i) cflags = REG_ICASE;  // ignore uppercase/lowercase

  regex_t reg;
  regmatch_t pmatch[1];
  regcomp(&reg, pattern_str, cflags);

  while (fgets(line, MAX_SIZE, fp)) {  // going through LINES in file
    int exec = 1;
    int line_printed = false;
    int n_printed = false;
    int name_printed = false;

    strncpy(temp, line,
            MAX_SIZE);  // to prevent duplicates, matched region gets replaced
                        // with ascii 127 in temp, hence its existence
    if (strchr(line, '\n') == NULL) strcat(line, "\n");

    // going through PATTERNS IN A SINGLE LINE
    while ((exec = regexec(&reg, temp, 1, pmatch, 0)) == 0) {
      if (flags->v) break;
      if (flags->c) break;
      if (flags->l) break;
      if (!flags->single_file && !flags->l && !flags->c && !name_printed) {
        printf("%s:", filename);
        name_printed = true;
      }
      if (strcmp(line, "\n") == 0) line_printed = true;
      if (flags->n && !n_printed) {
        printf("%d:", line_n);
        n_printed = true;
      }
      for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
        if (flags->o) printf("%c", line[i]);
        temp[i] = 127;
      }
      if (flags->o) {
        printf("\n");
        n_printed = false;
      } else if (!line_printed) {
        printf("%s", line);
        line_printed = true;
      }
    }
    if (flags->v && !flags->h && !flags->single_file && !flags->l &&
        !flags->c && !name_printed) {
      printf("%s:", filename);
    }
    if (exec && flags->v && flags->n && !n_printed) {
      printf("%d:", line_n);
      n_printed = true;
    }
    if (exec == REG_NOMATCH && flags->v && !line_printed) printf("%s", line);
    if (!exec) lines_with_matches++;
    line_n++;
    if (flags->l && !(exec)) break;
  }
  if (flags->c && !flags->single_file) {
    printf("%s:%d\n", filename, lines_with_matches);
  } else if (flags->c) {
    printf("%d\n", lines_with_matches);
  }
  if (flags->l && lines_with_matches > 0) printf("%s\n", filename);
  free(line);
  free(temp);
  regfree(&reg);
  fclose(fp);
}
