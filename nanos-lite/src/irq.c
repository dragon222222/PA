#include "common.h"
_RegSet* do_syscall(_RegSet *r);

static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    case _EVENT_SYSCALL: 
	    do_syscall(r);
	    break;
    case _EVENT_TRAP: Log("_EVENT_TRAP");return NULL;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
