#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  rtl_and(&s0, &id_src->val, &id_dest->val);
  
  rtl_update_ZFSF(&s0, id_dest->width);
  
  // update CF and OF
  s0 = 0;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&s0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  
  // update CF and OF
  s0 = 0;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&s0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  
  // update CF and OF
  s0 = 0;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&s0, &id_src->val, &id_dest->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  
  // update CF and OF
  s0 = 0;
  rtl_set_CF(&s0);
  rtl_set_OF(&s0);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(rol) {
  for (s0 = 0; s0 < id_src->val; s0++) {
    rtl_shri(&s1, &id_dest->val, id_dest->width * 8 - 1);
    rtl_shli(&id_dest->val, &id_dest->val, 1);
    rtl_xori(&id_dest->val, &id_dest->val, s1);
  }
  rtl_li(&s0, id_dest->val);
  rtl_set_CF(&s1);
  operand_write(id_dest, &s0);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);

  print_asm_template1(not);
}
