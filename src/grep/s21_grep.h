#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags_t {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int single_file;
} flags_t;

int setup(int argc, char **argv, flags_t *flags, int *error, char **patterns);
void read_file_pattern(char **patterns, int element, int *error);
void grep(flags_t *flags, char *pattern_str, char *filename, int *error);

#endif  // SRC_GREP_S21_GREP_H_
