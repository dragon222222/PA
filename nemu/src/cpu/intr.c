#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  //t0=cpu.eflags;
  //rtl_push(&t0);
  cpu.eflags.IF=0;
  //t0=cpu.cs;
  //rtl_push(&t0);
  //rtl_push(&ret_addr);

  rtl_push(&cpu.eflags.val);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);

  uint32_t tmp1,tmp2;
  tmp1 = vaddr_read(cpu.idtr.base+8*NO,2);
  tmp2 = vaddr_read(cpu.idtr.base+8*NO+6,2);

  tmp1=tmp1+(tmp2<<16);
  decoding.jmp_eip=tmp1;
  decoding.is_jmp=1;
}

void dev_raise_intr() {
	cpu.INTR = 0x1;
}
