#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  uintptr_t new_brk = brk + increment;assert(0);
  if(new_brk > current->max_brk){
    uintptr_t vaddr = (current->max_brk & 0xfffff000);
    while(vaddr < (uint32_t)(new_brk & 0xfffff000)){
      void* paddr = new_page(1);
      _map(&current->as, (void*)vaddr, paddr, 0);
      vaddr += PGSIZE;
    }
    current->max_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
