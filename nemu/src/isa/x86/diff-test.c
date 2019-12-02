#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (memcmp(ref_r, &cpu, DIFFTEST_REG_SIZE_CHECK) == 0)
    return true;
  else return false;
}

void isa_difftest_attach(void) {
  // set memory
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  ref_difftest_memcpy_from_dut(0x100000, guest_to_host(0x100000), PMEM_SIZE-0x100000);
  // set the value of idtr
  ref_difftest_memcpy_from_dut(0x7e00, &cpu.idtr.limit, 2);
  ref_difftest_memcpy_from_dut(0x7e02, &cpu.idtr.base, 4);
  // set instruction 'lidt 0x7e00)'
  char set_lidt[]={0x0f, 0x01, 0x18};
  ref_difftest_memcpy_from_dut(0x7e40, set_lidt, sizeof(set_lidt));
  // change eax and eip of QEMU to execute 'lidt'
  uint32_t old_pc = cpu.pc, old_eax = cpu.eax;
  cpu.pc = 0x7e40; 
  cpu.eax = 0x7e00;
  ref_difftest_setregs(&cpu);
  ref_difftest_exec(1);
  cpu.pc = old_pc; cpu.eax = old_eax;
  // set registers
  ref_difftest_setregs(&cpu);
}
