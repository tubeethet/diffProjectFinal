//
//  para.c
//  diff
//
//  Created by William McCarthy on 5/9/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include "para.h"
#include "util.h"

#define BUFLEN 256
#define BUFLEN2 1000


para* para_make(char* base[], int filesize, int start, int stop) {
    para* p = (para*) malloc(sizeof(para));
    p->base = base;
    p->filesize = filesize;
    p->start = start;
    p->stop = stop;
    p->firstline = (p == NULL || start < 0) ? NULL : p->base[start];
    p->secondline = (p == NULL || start < 0 || filesize < 2) ? NULL : p->base[start + 1];
    
    return p;
}

para* para_first(char* base[], int size) {
    para* p = para_make(base, size, 0, -1);
    return para_next(p);
}

void para_destroy(para* p) { free(p); }

para* para_next(para* p) {
    if (p == NULL || p->stop == p->filesize) { return NULL; }
    
    int i;
    para* pnew = para_make(p->base, p->filesize, p->stop + 1, p->stop + 1);
    for (i = pnew->start; i < p->filesize && strcmp(p->base[i], "\n") != 0; ++i) { }
    pnew->stop = i;
    
    if (pnew->start >= p->filesize) {
        free(pnew);
        pnew = NULL;
    }
    return pnew;
}
size_t para_filesize(para* p) { return p == NULL ? 0 : p->filesize; }

size_t para_size(para* p) { return p == NULL || p->stop < p->start ? 0 : p->stop - p->start + 1; }

char** para_base(para* p) { return p->base; }

char* para_info(para* p) {
    static char buf[BUFLEN2];   // static for a reason
    snprintf(buf, sizeof(buf), "base: %p, filesize: %d, start: %d, stop: %d\n",
             p->base, p->filesize, p->start, p->stop);
    return buf;  // buf MUST be static
}

int para_equal(para* p, para* q) {
    if (p == NULL || q == NULL) { return 0; }
    if (para_size(p) != para_size(q)) { return 0; }
    if (p->start >= p->filesize || q->start >= q->filesize) { return 0; }
    int i = p->start, j = q->start, equal = 0;
    while ((equal = strcmp(p->base[i], q->base[i])) == 0) { ++i; ++j; }
    return 1;
}
int para_equalc(para* p, para* q){
    if (p == NULL || q == NULL) { return 0; }
    if (para_size(p) != para_size(q)) { return 0; }
    if (p->start >= p->filesize || q->start >= q->filesize) { return 0; }
    int i = p->start, j = q->start, equal = 0;
    while ((equal = strcmp(p->base[i], q->base[i])) == 0) { ++i; ++j; }
    return equal == 0 ? 1 : 0;
}
void para_print(para* p, void (*fp)(const char*, struct opt), struct opt op) {
    if (p == NULL) { return; }
    for (int i = p->start; i <= p->stop && i != p->filesize; ++i) { fp(p->base[i], op); } //passes each line to the fp
}

void para_printinfo(para* p, para* q, struct opt op){
    char buf[BUFLEN2];
    if(p->start<=q->start){
        sprintf(buf,"%da%d,%d",p->start,(q->start+1),(q->stop+1));
        printf("%s\n", buf);
    }
    else{
        sprintf(buf,"%d,%dd%d",(q->start+1),(q->stop+1),p->start);
        printf("%s\n", buf);
    }
}
void para_printboth(para* p, para* q, struct opt op){
    if(p == NULL || q == NULL){return;}
    
    if((p->stop - p->start)-(q->stop- q->start) == 0){
        int i = p->start;
        int j = q->start;
        while(i <= p->stop && i != p->filesize && j <= q->stop && j != q->filesize ){
            if(op.opt[9]){
                diff(p->base[i],q->base[j],op);
            }
            else{
                printboth(p->base[i],q->base[j],op);
            }
            ++i;
            ++j;
        }
    }
}
void para_printfile(char* base[], int count, void (*fp)(const char*, struct opt), struct opt op) {
    para* p = para_first(base, count);
    while (p != NULL) {
        para_print(p, fp, op);
        p = para_next(p);
    }
    printline();
}
