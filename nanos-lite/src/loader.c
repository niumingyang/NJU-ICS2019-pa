#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Phdr Phdr_info;
  ramdisk_read(&Phdr_info, 0, sizeof(Elf_Phdr));
  printf("\n\n\n1\n\n\n\n\n\n%x", Phdr_info.p_vaddr);
  ramdisk_read((uint32_t *)Phdr_info.p_vaddr, Phdr_info.p_offset, Phdr_info.p_filesz);
  memset((uint32_t *)(Phdr_info.p_vaddr + Phdr_info.p_filesz), 0, Phdr_info.p_memsz - Phdr_info.p_filesz);
  return Phdr_info.p_vaddr;
}

void naive_uload(PCB *pcb, const char *filename) { 
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
