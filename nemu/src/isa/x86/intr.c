#include "rtl/rtl.h"
#include "cpu/exec.h"

#define IRQ_TIMER 32 // for x86

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.eflags.value);
  cpu.eflags.IF = 0;
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  rtl_li(&ir, cpu.idtr.base);
  s0 = vaddr_read(ir + NO * 8, 4)     & 0x0000ffff;
  s1 = vaddr_read(ir + NO * 8 + 4, 4) & 0xffff0000;
  rtl_or(&s0, &s0, &s1);
  rtl_j(s0);
}

bool isa_query_intr(void) {
  if (cpu.INTR & cpu.eflags.IF) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
}
