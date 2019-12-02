#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (memcmp(ref_r, &cpu, DIFFTEST_REG_SIZE) == 0)
    return true;
  else return false;
}

void isa_difftest_attach(void) {
  ref_difftest_memcpy_from_dut(0, guest_to_host(0), 0x7c00);
  ref_difftest_memcpy_from_dut(0x100000, guest_to_host(0x100000), PMEM_SIZE-0x100000);
  ref_difftest_setregs(&cpu);
}
