#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O://0
	    if(cpu.eflags.OF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_B://2
	    if(cpu.eflags.CF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_E://4
	    if(cpu.eflags.ZF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_BE://6
            if(cpu.eflags.CF||cpu.eflags.ZF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_S://8
            if(cpu.eflags.SF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_L://12
            if(cpu.eflags.OF!=cpu.eflags.SF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
    case CC_LE://14
            if(cpu.eflags.OF!=cpu.eflags.SF||cpu.eflags.ZF)
		    *dest=1;
	    else
		    *dest=0;
	    break;
      //TODO();
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
