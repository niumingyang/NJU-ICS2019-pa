#include "proc.h"
#include <elf.h>
#include <unistd.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

int fs_open(const char *path, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t count);
ssize_t fs_write(int fd, const void *buf, size_t count);
int fs_close(int fd);
off_t fs_lseek(int fd, off_t offset, int whence);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr Ehdr_info;
  Elf_Phdr Phdr_info;
  fs_read(fd, &Ehdr_info, sizeof(Elf_Ehdr));
  for (int i = 0; i < Ehdr_info.e_phnum; ++i) {
    fs_lseek(fd, Ehdr_info.e_phoff + i*Ehdr_info.e_phentsize, SEEK_SET);
    fs_read(fd, &Phdr_info, Ehdr_info.e_phentsize);
    if (Phdr_info.p_type == PT_LOAD) {
      uint32_t fsz = Phdr_info.p_filesz;
      void *va = (void *)Phdr_info.p_vaddr;
      void *pa;

      fs_lseek(fd, Phdr_info.p_offset, SEEK_SET);
      while (1) {
        pa = new_page(1);
        _map(&(pcb->as), va, pa, 0);
        if (fsz > PGSIZE)
          fs_read(fd, pa, PGSIZE);
        else{
          fs_read(fd, pa, fsz);
          break;
        }
        fsz -= PGSIZE;
        va += PGSIZE;
      }
/*
      if (Phdr_info.p_memsz > Phdr_info.p_filesz) {
        size_t zero_size = Phdr_info.p_memsz - Phdr_info.p_filesz;
        
        if (zero_size <= PGSIZE - fsz) {
          memset((void*)((uint32_t)pa + fsz*4), 0, zero_size);
        } else {
          memset((void*)((uint32_t)pa + fsz*4), 0, PGSIZE - fsz);
          zero_size -= (PGSIZE - fsz);
          va    += PGSIZE;

          while (1) {
            pa = new_page(1);
            _map(&(pcb->as), va, pa, 0);

            if (zero_size>PGSIZE) 
              memset(pa, 0, PGSIZE);
            else {
              memset(pa, 0, zero_size);
              break;
            }

            zero_size -= PGSIZE;
            va    += PGSIZE;
          }
        }
      }
      pcb->max_brk = (uintptr_t)va + PGSIZE;*/
    }
  }
  fs_close(fd);
  return Ehdr_info.e_entry;
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
  _protect(&(pcb->as));
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
