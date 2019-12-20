#include "cpu/exec.h"

make_EHelper(lidt) {
  cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  if (decinfo.isa.is_operand_size_16)
    cpu.idtr.base = vaddr_read(id_dest->addr+2, 4) & 0xffffff;
  else cpu.idtr.base = vaddr_read(id_dest->addr+2, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  switch (id_dest->reg) {
		case 0: cpu.cr0.val = id_src->val; break;
		case 3: cpu.cr3.val = id_src->val; break;
		default: assert(0);
  }

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  switch (id_src->reg) {
		case 0: s0 = cpu.cr0.val; break;
		case 3: s0 = cpu.cr3.val; break;
		default: assert(0);
	}

  operand_write(id_dest, &s0);

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  raise_intr(id_dest->val, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&s0);
  rtl_j(s0);
  rtl_pop(&s0);
  cpu.cs = s0;
  rtl_pop(&s0);
  cpu.eflags.value = s0;

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  switch (id_dest->width) {
    case 1: s0 = pio_read_b(id_src->val); break;
    case 2: s0 = pio_read_w(id_src->val); break;
    case 4: s0 = pio_read_l(id_src->val); break;
    default: assert(0);
  }
  operand_write(id_dest, &s0);

  print_asm_template2(in);
}

make_EHelper(out) {
  rtl_li(&ir, id_dest->val);
  switch (id_dest->width) {
    case 1: pio_write_b(ir, id_src->val); break;
    case 2: pio_write_w(ir, id_src->val); break;
    case 4: pio_write_l(ir, id_src->val); break;
    default: assert(0);
  }

  print_asm_template2(out);
}
