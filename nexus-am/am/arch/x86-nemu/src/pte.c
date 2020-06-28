#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  PDE *pt = (PDE*)p->ptr;
  PDE *pde = &pt[PDX(va)];
  if (!(*pde & PTE_P)) {
    *pde = PTE_P | PTE_W | PTE_U | (uint32_t)palloc_f();
  }
  PTE *pte = &((PTE*)PTE_ADDR(*pde))[PTX(va)];
  if (!(*pte & PTE_P)) {
    *pte = PTE_P | PTE_W | PTE_U | (uint32_t)pa;
  }
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  /*uintptr_t pi = (uintptr_t) ustack.end;
  for(int i=0;i<3;i++){
	  pi -=4;
	  *(uint32_t *)pi=0;
  }

  pi -=4;*(uint32_t *)pi = 0x00000002;
  pi -=4;*(uint32_t *)pi = 0x00000008;
  pi -=4;*(uint32_t *)pi = (uint32_t )entry;
  pi -=4;*(uint32_t *)pi = 0;
  pi -=4;*(uint32_t *)pi = 0x81;

  for(int i=0;i<8;i++){
	  pi -=4;
	  *(uint32_t *)pi =0;
  }
  return (_RegSet *)pi;*/
  uintptr_t pi = (uintptr_t) ustack.end;
     // _start()
     pi -= 4; *(uintptr_t *)pi = 0; 
     pi -= 4; *(uintptr_t *)pi = 0; 
     pi -= 4; *(uintptr_t *)pi = 0; 
     pi -= 4; *(uintptr_t *)pi = 0; 
     // trap()
     pi = pi - sizeof(_RegSet);
     _RegSet regs;
     regs.eflags = 0x00000202;
     regs.cs = 0x8;
     regs.eip = 0x8048000;
     regs.error_code = 0;
     regs.irq = 0;
     regs.eax = 0;
     regs.ecx = 0;
     regs.edx = 0;
     regs.ebx = 0;
     regs.esp = 0;
     regs.ebp = 0;
     regs.esi = 0;
     regs.edi = 0;
     *(_RegSet *)pi = regs;
     return (_RegSet *)pi;	
}
