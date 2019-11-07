#include "rtl/rtl.h"
#include "cpu/exec.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.value);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  rtl_li(&ir, cpu.idtr.base);
  s0 = vaddr_read(ir + NO * 8, 4)     & 0x0000ffff;
  s1 = vaddr_read(ir + NO * 8 + 4, 4) & 0xffff0000;
  rtl_or(&s0, &s0, &s1);
  decinfo.jmp_pc = s0;
  rtl_j(decinfo.jmp_pc);
}

bool isa_query_intr(void) {
  return false;
}
