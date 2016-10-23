#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
extern "C"{
const char service_interp[] __attribute__((section(".interp"))) = "/lib64/ld-linux-x86-64.so.2";
int dummy(int a);

void entryp(void)  //entry is the entry point instead of main
 {
  int A;
  A=3*2;
  fprintf(stderr,"ok %d ", dummy(A));
   _exit(0);
 }
}
