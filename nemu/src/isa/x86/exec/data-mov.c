#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&s0);
  operand_write(id_dest, &s0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  if (decinfo.isa.is_operand_size_16) {
    s1 = (uint32_t)reg_w(R_SP);
    s0 = (uint32_t)reg_w(R_AX); rtl_push(&s0);
    s0 = (uint32_t)reg_w(R_CX); rtl_push(&s0);
    s0 = (uint32_t)reg_w(R_DX); rtl_push(&s0);
    s0 = (uint32_t)reg_w(R_BX); rtl_push(&s0); 
    rtl_push(&s1);
    s0 = (uint32_t)reg_w(R_BP); rtl_push(&s0);
    s0 = (uint32_t)reg_w(R_SI); rtl_push(&s0);
    s0 = (uint32_t)reg_w(R_DI); rtl_push(&s0);
  }
  else {
    s0 = cpu.esp;
    rtl_push(&cpu.eax);
    rtl_push(&cpu.ecx);
    rtl_push(&cpu.edx);
    rtl_push(&cpu.ebx);
    rtl_push(&s0);
    rtl_push(&cpu.ebp);
    rtl_push(&cpu.esi);
    rtl_push(&cpu.edi);
  }

  print_asm("pusha");
}

make_EHelper(popa) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_pop(&s0); reg_w(R_DI) = s0;
    rtl_pop(&s0); reg_w(R_SI) = s0;
    rtl_pop(&s0); reg_w(R_BP) = s0;
    rtl_pop(&s0);
    rtl_pop(&s0); reg_w(R_BX) = s0;
    rtl_pop(&s0); reg_w(R_DX) = s0;
    rtl_pop(&s0); reg_w(R_CX) = s0;
    rtl_pop(&s0); reg_w(R_AX) = s0;
  }
  else {
    rtl_pop(&s0); cpu.edi = s0;
    rtl_pop(&s0); cpu.esi = s0;
    rtl_pop(&s0); cpu.ebp = s0;
    rtl_pop(&s0);
    rtl_pop(&s0); cpu.ebx = s0;
    rtl_pop(&s0); cpu.edx = s0;
    rtl_pop(&s0); cpu.ecx = s0;
    rtl_pop(&s0); cpu.eax = s0;
  }

  print_asm("popa");
}

make_EHelper(leave) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_mv(&s0, &cpu.ebp);
    rtl_andi(&cpu.esp, &cpu.esp, 0xffff0000);
    rtl_andi(&s0, &s0, 0xffff);
    rtl_or(&cpu.esp, &cpu.esp, &s0);
    rtl_pop(&s0);
    rtl_andi(&cpu.esp, &cpu.esp, 0xffff0000);
    rtl_andi(&s0, &s0, 0xffff);
    rtl_or(&cpu.ebp, &cpu.ebp, &s0);
  }
  else {
    rtl_mv(&cpu.esp, &cpu.ebp);
    rtl_pop(&cpu.ebp);
  }

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_msb(&s0, &cpu.eax, 2);
    if (s0) rtl_ori(&cpu.edx, &cpu.edx, 0xffff);
    else rtl_andi(&cpu.edx, &cpu.edx, 0xffff0000);
  }
  else {
    rtl_msb(&s0, &cpu.eax, 4);
    if (s0) rtl_li(&cpu.edx, 0xffffffff);
    else rtl_li(&cpu.edx, 0x0);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    rtl_msb(&s0, &cpu.eax, 1);
    if (s0) rtl_ori(&cpu.eax, &cpu.eax, 0xff00);
    else rtl_andi(&cpu.eax, &cpu.eax, 0xffff00ff);
  }
  else {
    rtl_msb(&s0, &cpu.eax, 2);
    if (s0) rtl_ori(&cpu.eax, &cpu.eax, 0xffff0000);
    else rtl_andi(&cpu.eax, &cpu.eax, 0xffff);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

uint32_t pio_read_l(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
