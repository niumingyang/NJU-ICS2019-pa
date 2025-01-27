#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {	
	printf("$eax: 0x%08x %d\n", cpu.eax, cpu.eax);
	printf("$ecx: 0x%08x %d\n", cpu.ecx, cpu.ecx);
	printf("$edx: 0x%08x %d\n", cpu.edx, cpu.edx);
	printf("$ebx: 0x%08x %d\n", cpu.ebx, cpu.ebx);
	printf("$esp: 0x%08x %d\n", cpu.esp, cpu.esp);
	printf("$ebp: 0x%08x %d\n", cpu.ebp, cpu.ebp);
	printf("$esi: 0x%08x %d\n", cpu.esi, cpu.esi);
	printf("$edi: 0x%08x %d\n", cpu.edi, cpu.edi);
	printf("$pc:  0x%08x %d\n", cpu.pc,  cpu.pc);
  printf("$eflags: CF: %01x  OF: %01x  ZF: %01x  SF: %01x\n", cpu.eflags.CF, cpu.eflags.OF, cpu.eflags.ZF, cpu.eflags.SF);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  if (strcmp(s, "eax")==0) return cpu.eax;
  else if (strcmp(s, "ecx")==0) return cpu.ecx;
  else if (strcmp(s, "edx")==0) return cpu.edx;
  else if (strcmp(s, "ebx")==0) return cpu.ebx;
  else if (strcmp(s, "esp")==0) return cpu.esp;
  else if (strcmp(s, "ebp")==0) return cpu.ebp;
  else if (strcmp(s, "esi")==0) return cpu.esi;
  else if (strcmp(s, "edi")==0) return cpu.edi;
  else if (strcmp(s, "pc")==0)  return cpu.pc;
  else *success = 0;
  return 0;
}
