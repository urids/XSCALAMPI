#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

const char service_interp[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";

void entrypnt(void) //entry is the entry point instead of main
 {
  int A;
  A=3*2+10;
  fprintf(stderr,"oks %d ",dummy(A));
   _exit(0);
 }

