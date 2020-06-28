#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  //TODO();
  rtl_lm(&t0, &id_dest->addr, 2);
  cpu.idtr.limit = t0;
  t1 = id_dest->addr + 2;
  rtl_lm(&t0, &t1, 4);
  cpu.idtr.base = (t0 & (decoding.is_operand_size_16 ? 0x00FFFFFF : 0xFFFFFFFF));

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();
  /*switch(id_dest->reg){
	  case 0:cpu.cr0.val = id_src->val;
		 break;
	  case 3:cpu.cr3.val= id_src->val;
		 break;
	  default:assert(0);
		  break;
	  }
*/
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();
  /*switch(id_dest->reg){
          case 0:id_src->val = cpu.cr0.val;
                 break;
          case 3:id_src->val = cpu.cr3.val;
                 break;
          default:assert(0);
                  break;
          }
*/
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val,*eip);//直接调用

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(eip);
  rtl_pop(&t0);
  cpu.cs=t0;
  rtl_pop(&cpu.eflags.val);

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  reg_l(R_EAX)=pio_read(reg_w(R_EDX),id_dest->width);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  pio_write(reg_w(R_EDX),id_dest->width,reg_l(R_EAX));

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
