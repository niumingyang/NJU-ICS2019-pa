#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void *entry);
void context_uload(PCB *pcb, const char *filename);

static int fg_pcb = 1;

void change_pcb(int k){
  if(k >= 2 && k <= 4)
    fg_pcb = k - 1;
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);

    j ++;
    _yield();
  }
}

void init_proc() {
  Log("Initializing processes...");

  // load program here
  //context_kload(&pcb[0], (void *)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  //naive_uload(NULL, "/bin/dummy");

  switch_boot_pcb();
}

int cnt = 0;
_Context* schedule(_Context *prev) {
  // save the context pointer
  current->cp = prev;

  //current = &pcb[0];

  if (cnt++ < 99) current = &pcb[fg_pcb];
  else {
    current = &pcb[0];
    cnt = 0;
  }
  return current->cp;
}
