#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_opt 3

#define MAXSTRINGS 1024
#define MAXPARAS 4096

#define HASHLEN 200

#include "para.h"
#include "util.h"
#ifndef HEADER_H
#define HEADER_H
#include "opt.h"
#endif

void version(void);
void todo_list(void);
void setop(struct opt* op, int n);
void loadfiles(const char* filename1, const char* filename2, struct opt op);
void print_option(const char* name, int value);
void diff_output_conflict_error(void);
void setoption(const char* arg, const char* s, const char* t, int n, struct opt* op);
void showopt(const char* file1, const char* file2, struct opt op);
void init_opt_files(int argc, const char* argv[], struct opt* op);
void showidentical(para* p, para* q, struct opt op);
void sidebyside(para* p, para* q, struct opt op);
void showunified(para* p, para* q, struct opt op);
void showcontext(para* p, para* q, struct opt op);
void shownormal(para* p, para* q, struct opt op);
