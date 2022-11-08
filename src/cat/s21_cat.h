#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct flags_t {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} flags_t;

void setup(int argc, char **argv, flags_t *flags, int *error);
void read_file(char **argv, flags_t *flags, int *error);
void cat(FILE *fp, flags_t *flags);

#endif  // SRC_CAT_S21_CAT_H_