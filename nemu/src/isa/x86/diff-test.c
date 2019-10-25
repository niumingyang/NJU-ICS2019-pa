#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  //if (memcmp(ref_r, &cpu, DIFFTEST_REG_SIZE) == 0)
  //  return true;
  //else return false;
  return false;
}

void isa_difftest_attach(void) {
}
