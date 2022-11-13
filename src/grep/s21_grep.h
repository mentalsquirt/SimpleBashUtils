#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

#define MAX_SIZE 1024
#define true 1
#define false 0

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
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
  int print_filename;
} flags_t;

typedef struct {
  int line;
  int n;
  int name;
} guards_t;

int file_check(char *filename);
void guards_clear(guards_t *printed);
void flag_check(int argc, char **argv, flags_t *flags, char pattern[MAX_SIZE]);
void file_pattern(char *path, char pattern[MAX_SIZE]);
void grep(flags_t *flags, char pattern[MAX_SIZE], char *filename, int files);

#endif  // SRC_GREP_S21_GREP_H_
