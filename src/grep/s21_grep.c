#include "s21_grep.h"

int main(int argc, char **argv) {
  char pattern[MAX_SIZE] = "";
  flags_t flags = {0};

  if (argc < 2) {
    printf(
        "usage: ./s21_grep [-ivclnhso]"
        " [-e pattern] [-f file] [pattern] [file ...]");
    exit(1);
    /* NOT REACHABLE */
  }

  // setting up the pattern string and checking for flags
  flag_check(argc, argv, &flags, pattern);
  if (!flags.e && !flags.f) {
    strncpy(pattern, argv[optind], MAX_SIZE);
  }
  if (pattern[strlen(pattern) - 1] == '|') {
    pattern[strlen(pattern) - 1] = '\0';
  }

  // looking for files to grep
  int i = 1;
  int files = 0;
  if (!flags.f && !flags.e) i = 2;  // since argv[1] is always pattern
  for (; i < argc; i++) {
    if (file_check(argv[i]) == 2) {
      i++;
      continue;
    }
    if (file_check(argv[i]) == 1) {
      files++;
    }
  }
  // pattern should not be counted as file
  int is_pattern;
  if (!flags.e && !flags.f) {
    is_pattern = true;
    if (files > 1) files -= 1;
  }
  i = 1;
  if (files == 0) {
    // calling for grep in stdin mode
    grep(&flags, pattern, NULL, files);
  } else {
    for (; i < argc; i++) {
      if (file_check(argv[i]) == 2) {
        i++;
        continue;
      }
      if (file_check(argv[i]) == 1) {
        if (is_pattern) {
          is_pattern = false;
          continue;
        }
        // calling for grep on each file
        grep(&flags, pattern, argv[i], files);
      }
    }
  }
  return 0;
}

// 0 if not file, 1 if file, 2 if -e||-f
int file_check(char *filename) {
  int res = 1;

  if (strstr(filename, "-e") != NULL || strstr(filename, "-f") != NULL) {
    res = 0;
  }
  if (filename[0] == '-') {
    res = 0;
  }
  if (!strcmp(filename, "-e") || !strcmp(filename, "-f")) {
    res = 2;
  }
  if ((filename[strlen(filename) - 1] == 'f' ||
       filename[strlen(filename) - 1] == 'e') &&
      (filename[0] == '-')) {
    res = 2;
  }

  return res;
}

void guards_clear(guards_t *printed) {
  printed->name = false;
  printed->n = false;
  printed->line = false;
}

void flag_check(int argc, char **argv, flags_t *flags, char pattern[MAX_SIZE]) {
  int flag = 0;
  const char *flags_string = ":e:ivclnhsf:o";

  while ((flag = getopt_long(argc, argv, flags_string, NULL, NULL)) != -1) {
    switch (flag) {
      case 'e': {
        flags->e = 1;
        strncat(pattern, optarg, MAX_SIZE - strlen(pattern));
        strcat(pattern, "|");
        break;
      }
      case 'i': {
        flags->i = 1;
        break;
      }
      case 'v': {
        flags->v = 1;
        break;
      }
      case 'c': {
        flags->c = 1;
        break;
      }
      case 'l': {
        flags->l = 1;
        break;
      }
      case 'n': {
        flags->n = 1;
        break;
      }
      case 'h': {
        flags->h = 1;
        break;
      }
      case 's': {
        flags->s = 1;
        break;
      }
      case 'f': {
        flags->f = 1;
        file_pattern(optarg, pattern);
        break;
      }
      case 'o': {
        flags->o = 1;
        break;
      }
      case '?': {
        printf(
            "grep: invalid option -- %c\n"
            "usage: grep [-eivclnhsfo] [pattern] "
            "[file ...]\n",
            optopt);
        exit(1);
        /* NOT REACHABLE */
      }
    }
  }
}

void file_pattern(char *path, char *pattern) {
  FILE *fp = NULL;
  size_t pattern_len = strlen(pattern);
  if ((fp = fopen(path, "r")) == NULL) {
    printf("grep: %s: No such file or directory\n", path);
    return;
    /* NOT REACHABLE */
  }
  int character;
  size_t i = 0;
  while ((character = fgetc(fp)) != EOF && pattern_len < MAX_SIZE) {
    if ((character == 13 || character == 10) && pattern_len &&
        pattern[pattern_len - 1] != '|') {
      pattern[pattern_len++] = '|';
    }
    if (character != 13 && character != 10) pattern[pattern_len++] = character;
    i++;
  }
  if (pattern[pattern_len - 1] != '|' && pattern_len < MAX_SIZE) {
    pattern[pattern_len] = '|';
  }

  fclose(fp);
}

void grep(flags_t *flags, char pattern[MAX_SIZE], char *filename, int files) {
  FILE *fp = NULL;
  // read from stdin if no file provided
  if (files == 0) {
    fp = stdin;
  } else {
    if ((fp = fopen(filename, "r")) == NULL) {
      if (!flags->l && !flags->s) {
        printf("grep: %s: No such file or directory\n", filename);
      }
      return;
      /* NOT REACHABLE */
    }
  }
  int cflags = REG_EXTENDED, good_lines = 0, line_n = 1;
  if (flags->i) cflags |= REG_ICASE;
  // compile RE
  regex_t reg;
  regmatch_t pmatch[1];
  regcomp(&reg, pattern, cflags);
  // set up wether to print filename
  if (files > 1 && !flags->h) {
    flags->print_filename = true;
  }

  char *line = NULL;
  size_t cap = 0;
  ssize_t len;
  guards_t printed = {0};
  /* ITERATING THROUGH LINES (TILL '\n') */
  while ((len = getline(&line, &cap, fp)) > 0) {
    int i = 0;
    int eflags = 0;
    int exec = 1;
    guards_clear(&printed);
    if (strchr(line, '\n') == NULL) strcat(line, "\n");

    /* ITERATING THROUGH PATTERNS IN A SINGLE LINE (-vcl do not care) */
    while ((((exec = regexec(&reg, &line[i], 1, pmatch, eflags)) == 0) &&
            !flags->v) ||
           (flags->v && exec)) {
      if (flags->c || flags->l) break;
      if (flags->print_filename && !printed.name) {
        printf("%s:", filename);
        printed.name = true;
      }
      if (flags->n && !printed.n) {
        printf("%d:", line_n);
        printed.n = true;
      }
      if (flags->o && !flags->v) {
        for (int index = pmatch[0].rm_so; index < pmatch[0].rm_eo; index++) {
          printf("%c", line[index + i]);
        }
        printf("\n");
        guards_clear(&printed);
      } else if (!printed.line) {
        printf("%s", line);
        printed.line = true;
      }
      if (flags->v) break;

      // to prevent duplicate matches
      i += pmatch[0].rm_eo;

      // let grep know that we're not at the beginning of the line
      eflags |= REG_NOTBOL;
    }
    /* STOP ITERATING THROUGH PATTERNS */

    line_n++;
    if ((!flags->v && !exec) || (flags->v && exec)) good_lines++;
    if (files == 0) fflush(fp);

    if (flags->l && !exec) {
      break;
    }
  }
  /* STOP ITERATING THROUGH LINES */

  if (flags->c && flags->print_filename) {
    printf("%s:%d\n", filename, good_lines);
  } else if (flags->c) {
    printf("%d\n", good_lines);
  }

  if (flags->l && good_lines) {
    if (!files) {
      printf("(standard input)\n");
    } else {
      printf("%s\n", filename);
    }
  }

  regfree(&reg);
  fclose(fp);
}
