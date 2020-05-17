//
//  diff_03.c
//  diff
//
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//
#include "differ03.h"

void version(void) {
    printf("\n\n\ndiff (CSUF diffutils) 0.1.0\n");
    printf("Copyright (C) 2014 CSUF\n");
    printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
    printf("You may redistribute copies of this program\n");
    printf("under the terms of the GNU General Public License.\n");
    printf("For more information about these matters, see the file named COPYING.\n");
    printf("Written by Jimmy Pham with starter code provided by William McCarthy\n");
}

void todo_list(void) {
    printf("\nTODO: Context mode \n");
    printf("\nTODO: Unified mode \n");
}

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
typedef struct opt opt;
char filename1[BUFLEN], filename2[BUFLEN];

int count1 = 0, count2 = 0;

void setop(opt* op, int n){
    op->opt[n] = 1;
}

void loadfiles(const char* filename1, const char* filename2, struct opt op) {
    memset(buf, 0, sizeof(buf));
    memset(strings1, 0, sizeof(strings1));
    memset(strings2, 0, sizeof(strings2));
    
    FILE *fin1 = openfile(filename1, "r");
    FILE *fin2 = openfile(filename2, "r");
    
    while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
    while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
    if(op.opt[6] || op.opt[7]){
        printf("---%s \t%s\n",filename1,diff_last_modified(filename1));
        printf("---%s \t%s\n",filename2,diff_last_modified(filename2));
    }
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
    fprintf(stderr, "diff: conflicting output style opt\n");
    fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
    exit(CONFLICTING_OUTPUT_opt);
}

void setoption(const char* arg, const char* s, const char* t, int n, struct opt* op) {//arg used in command line, s first alias, t second alias
    if ((strcmp(arg, s) == 0) || (t != NULL && strcmp(arg, t) == 0)) {
        setop(op, n);
    }
}

void showopt(const char* file1, const char* file2, struct opt op) {
    printf("diff opt...\n");
    print_option("show_version", op.opt[0]);
    print_option("show_brief", op.opt[1]);
    print_option("ignorecase", op.opt[2]);
    print_option("report_identical", op.opt[3]);
    print_option("show_sidebyside", op.opt[4]);
    print_option("show_leftcolumn", op.opt[5]);
    print_option("show_unified", op.opt[6]);
    print_option("showcontext", op.opt[7]);
    print_option("suppresscommon", op.opt[8]);
    print_option("diffnormal", op.opt[9]);
    
    printf("file1: %s,  file2: %s\n\n\n", file1, file2);
    
    printline();
}


void init_opt_files(int argc, const char* argv[], struct opt* op) {
    int cnt = 0;
    const char* files[2] = { NULL, NULL };
    for(int i = 0;i <10; i++){
        op->opt[i] = 0;
    }
    while (argc-- > 0) {
        const char* arg = *argv;
        setoption(arg, "-v",       "--version",               0, op);
        setoption(arg, "-q",       "--brief",                 1, op);
        setoption(arg, "-i",       "--ignore-case",           2, op);
        setoption(arg, "-s",       "--report-identical-files",3, op);
        setoption(arg, "-y",       "--side-by-side",          4, op);
        setoption(arg, "--left-column", NULL,                 5, op);
        setoption(arg, "-u",       "showunified",             6, op);
        setoption(arg, "-c",       "--context",               7, op);
        setoption(arg, "--suppress-common-lines", NULL,       8, op);
        setoption(arg, "--normal", NULL,                      9, op);
        if (arg[0] != '-') {
            if (cnt == 2) {
                fprintf(stderr, "apologies, this version of diff only handles two files\n");
                fprintf(stderr, "Usage: ./diff [opt] file1 file2\n");
                exit(TOOMANYFILES_ERROR);
            } else { files[cnt++] = arg; }
        }
        ++argv;
    }
    if (!op->opt[7] && !op->opt[6] && !op->opt[4] && !op->opt[5]) {
        op->opt[9] = 1;
    }
    if (op->opt[0]) { version();  exit(0); }
    if (((op->opt[4] || op->opt[5]) && (op->opt[9] || op->opt[7] || op->opt[6])) ||
        (op->opt[7] && op->opt[6]) || (op->opt[9] && (op->opt[7] || op->opt[6]))) {
        
        diff_output_conflict_error();
    }
    
    if(files[0]!=NULL || files[1]!=NULL){
        loadfiles(files[0], files[1], *op);
        strcpy(filename1,files[0]);
        strcpy(filename2,files[1]);
    }
}

void showidentical(para* p, para* q, struct opt op){}
void sidebyside(para* p, para* q, struct opt op){
    int foundmatch = 0;
    para* qlast = q;
    while (p != NULL) {
        qlast = q;
        foundmatch = 0;
        while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
            q = para_next(q);
        }
        q = qlast;
        if (foundmatch) {
            while ((foundmatch = para_equal(p, q)) == 0) {
                para_print(q, printright, op);
                q = para_next(q);
                qlast = q;
            }
            if(para_equal(p,q) == 1){
                para_printboth(p,q,op);
            }
            p = para_next(p);
            q = para_next(q);
        } else {
            para_print(p, printleft, op);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        para_print(q, printright, op);
        q = para_next(q);
    }
}
void showunified(para* p, para* q, struct opt op){
    int foundmatch = 0;
    para* qlast = q;
    while (p != NULL) {
        qlast = q;
        foundmatch = 0;
        while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
            q = para_next(q);
        }
        q = qlast;
        if (foundmatch) {
            while ((foundmatch = para_equal(p, q)) == 0) {
                para_print(q, printright, op);
                q = para_next(q);
                qlast = q;
            }
            if(para_equal(p,q) == 1){
                para_printboth(p,q,op);
            }
            p = para_next(p);
            q = para_next(q);
        } else {
            para_print(p, printleft, op);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        para_print(q, printright, op);
        q = para_next(q);
    }
}
void showcontext(para* p, para* q, struct opt op){
    int foundmatch = 0;
    para* qlast = q;
    while (p != NULL) {
        qlast = q;
        foundmatch = 0;
        while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
            q = para_next(q);
        }
        q = qlast;
        if (foundmatch) {
            while ((foundmatch = para_equal(p, q)) == 0) {
                para_print(q, printright, op);
                q = para_next(q);
                qlast = q;
            }
            if(para_equal(p,q) == 1){
                para_printboth(p,q,op);
            }
            p = para_next(p);
            q = para_next(q);
        } else {
            para_print(p, printleft, op);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        para_print(q, printright, op);
        q = para_next(q);
    }
}
void shownormal(para* p, para* q, struct opt op){
    int foundmatch = 0;
    para* qlast = q;
    int plast;
    while (p != NULL) {
        qlast = q;
        plast = p->stop;
        foundmatch = 0;
        while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
            q = para_next(q);
        }
        q = qlast;
        if (foundmatch) {
            while ((foundmatch = para_equal(p, q)) == 0) {
                para_printinfo(p,q,op);
                para_print(q, printright, op);
                q = para_next(q);
                qlast = q;
            }
            if(para_equal(p,q) == 1){
                if(para_equalc(p,q)==1){
                    para_printinfo(p,q,op);
                }
                para_printboth(p,q,op);
            }
            p = para_next(p);
            q = para_next(q);
        } else {
            printf("%dd%d,%d\n",(p->start+1),(q->start-1),(q->start));
            para_print(p, printleft, op);
            p = para_next(p);
        }
    }
    while (q != NULL) {
        printf("%da%d,%d\n",plast,q->start,q->stop);
        para_print(q, printright, op);
        q = para_next(q);
    }
}
int main(int argc, const char * argv[]) {
    struct opt op;
    if(argc == 1){
        printf("diff: missing operand after 'diff'\ndiff: Try 'diff --help' for more information.\n");
        exit(1);
    }
    init_opt_files(--argc, ++argv, &op);
    para* p = para_first(strings1, count1);
    para* q = para_first(strings2, count2);
    if(op.opt[1] && op.opt[3] == 0){
        int different = 0;
        if (para_size(p) != para_size(q)) { different = 1; }
        if((p->stop - p->start)-(q->stop- q->start) == 0){
            int i = p->start;
            int j = q->start;
            while(i <= p->stop && i != p->filesize && j <= q->stop && j != q->filesize ){
                if (strcmp(p->base[i],q->base[j])!=0){different = 1;}
                ++i;
                ++j;
            }
        }
        if(different){
            printf("Files %s and %s differ\n", filename1, filename2);
        }
    }
    if(op.opt[3]){
        int same = 1;
        if (para_size(p) != para_size(q)) { same = 0; }
        if((p->stop - p->start)-(q->stop- q->start) == 0){
            int i = p->start;
            int j = q->start;
            while(i <= p->stop && i != p->filesize && j <= q->stop && j != q->filesize ){
                if (strcmp(p->base[i],q->base[j])!=0){same = 0;}
                ++i;
                ++j;
            }
        }
        if(same){
            printf("Files %s and %s are identical\n", filename1, filename2);
        }
        else{
            if(op.opt[1]){
                printf("Files %s and %s differ\n", filename1, filename2);
            }
            else{
                shownormal(p,q,op);
            }
        }
    }
    else if(op.opt[4]){
        sidebyside(p,q,op);
    }
    else if(op.opt[6]){
        showunified(p,q,op);
    }
    else if(op.opt[7]){
        showcontext(p,q,op);
    }
    else if(op.opt[9]){
        shownormal(p,q,op);
    }
    return 0;
}
