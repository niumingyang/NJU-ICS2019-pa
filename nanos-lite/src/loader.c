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

/*static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr Ehdr_info;
  Elf_Phdr Phdr_info;
  size_t rd_offset = sizeof(Elf_Ehdr);
  fs_read(fd, &Ehdr_info, sizeof(Elf_Ehdr));
  for (int i = 0; i < Ehdr_info.e_phnum; ++i) {
    fs_read(fd, &Phdr_info, Ehdr_info.e_phentsize);
    fs_lseek(fd, Phdr_info.p_offset, SEEK_SET);
    size_t size = Phdr_info.p_filesz;
    size_t page_cnt = (size + PGSIZE - 1) / PGSIZE;
    void* pa;
    void* va = (uintptr_t *)Phdr_info.p_vaddr;
    for(int i = 0; i < page_cnt; ++i){
      pa = new_page(1);
      _map(&pcb->as, va, pa, 0);
      fs_read(fd, pa, (((size - i * PGSIZE) < PGSIZE) ? (size - i * PGSIZE) : PGSIZE));
      va += PGSIZE;
    }
    //memset((uintptr_t *)(Phdr_info.p_vaddr + Phdr_info.p_filesz), 0, Phdr_info.p_memsz - Phdr_info.p_filesz);
    rd_offset += Ehdr_info.e_phentsize;
    fs_lseek(fd, rd_offset, SEEK_SET);
  }
  fs_close(fd);
  return Ehdr_info.e_entry;
}*/
#define PAGE_SIZE 4096
static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr Elf_Header;
  fs_read(fd, (void*)&Elf_Header, sizeof(Elf_Ehdr));

  Elf_Phdr Elf_Program_Header;
  for (int i=0;i<Elf_Header.e_phnum;i++) {
    fs_lseek(fd, Elf_Header.e_phoff + i*Elf_Header.e_phentsize, SEEK_SET);
    fs_read(fd, (void*)&Elf_Program_Header, Elf_Header.e_phentsize);
    
    
    if (Elf_Program_Header.p_type == PT_LOAD) 
    {
      
      uint32_t un_size = Elf_Program_Header.p_filesz; //需要写入的字节数
      uint32_t v_addr  = Elf_Program_Header.p_vaddr;
      void* p_addr;

      fs_lseek(fd, Elf_Program_Header.p_offset, SEEK_SET);
      while (1) {
        p_addr = new_page(1); //申请一页空闲的物理页
        _map(&(pcb->as), (void*)v_addr, p_addr, 0);

        if (un_size > PAGE_SIZE)
          fs_read(fd, p_addr, PAGE_SIZE);
        else{
          fs_read(fd, p_addr, un_size);
          // pcb->max_brk = v_addr + un_size;
          break;
        }

        un_size -= PAGE_SIZE;
        v_addr  += PAGE_SIZE;
      }

  
      pcb->max_brk = v_addr+PAGE_SIZE;
    }
  }
  fs_close(fd);
  return Elf_Header.e_entry;
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
