#include "nemu.h"

paddr_t page_translate(vaddr_t addr){
  if(cpu.cr0.paging){
    paddr_t dir = (addr >> 22) & 0x3ff;
    paddr_t page = (addr >> 12) & 0x3ff;
    paddr_t offset = addr & 0xfff;
    uint32_t pdb = cpu.cr3.page_directory_base;

    uint32_t pt = paddr_read((pdb << 12) + (dir << 2), 4);
    assert(pt & 1);

    uint32_t pf = paddr_read((pt & 0xfffff000) + (page << 2), 4);
    //assert(pf & 1);

    return (pf & 0xfffff000) + offset;
  }
  return addr;
}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if((addr & 0xfff) + len > 0x1000){
    paddr_t paddr = page_translate(addr);
    uint8_t tmp[8];
    uint32_t offset = addr & 3;
    
    *(uint32_t *)(tmp + offset) = paddr_read(paddr, 4 - offset);

    paddr = page_translate((addr & ~0xfff) + 0x1000);
    *(uint32_t *)(tmp + 4) = paddr_read(paddr, len + offset - 4);

    return (*(uint32_t *)(tmp + offset)) & (~0u >> ((4 - len) << 3));
  }
  else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if((addr & 0xfff) + len > 0x1000){
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data, len);
  }
}
