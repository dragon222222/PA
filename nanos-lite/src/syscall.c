#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  //TODO();
  return fs_open((char *)pathname,flags,mode);
}

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  //Log("sys_write");
  return fs_write(fd,(void *)buf,len);
  /*const char *p=(void *)buf;
  int i=0;
  if(fd==1||fd==2)
  {
          for(i=0;i<len;++i)
                  _putc(p[i]);
  }
  else
          return -1;
  return len;*/
}

static inline uintptr_t sys_read(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  return fs_read(fd,(void*)buf,len);
}

static inline uintptr_t sys_lseek(uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  return fs_lseek(fd, offset, whence);
}

static inline uintptr_t sys_close(uintptr_t fd) {
  //TODO();
  return fs_close(fd);
}

static inline uintptr_t sys_brk(uintptr_t new_brk) {
  //TODO();
  return mm_brk(new_brk);
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  switch (a[0]) {
    case SYS_none:
	  SYSCALL_ARG1(r)=1;
	  break;
    case SYS_exit:
	  _halt(SYSCALL_ARG2(r));
	  break;
    case SYS_write:
	  SYSCALL_ARG1(r)=sys_write(a[1],a[2],a[3]);
	  break;
    case SYS_brk:  
	  SYSCALL_ARG1(r)=sys_brk(SYSCALL_ARG2(r));
	  break;
    case SYS_open: 
	  SYSCALL_ARG1(r) = sys_open(a[1], a[2], a[3]) ; 
	  break;
    case SYS_read:
	  SYSCALL_ARG1(r) = sys_read(a[1],a[2],a[3]) ;
	  break;
    case SYS_lseek:
	  SYSCALL_ARG1(r) = sys_lseek(a[1], a[2], a[3]) ;
	  break;
    case SYS_close:
	  SYSCALL_ARG1(r) = sys_close(a[1]) ;
	  break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
