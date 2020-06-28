#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if (is_mmio(addr)!= -1)
  	return mmio_read(addr,len,is_mmio(addr));
  else
  	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  if (is_mmio(addr)!= -1)
	  mmio_write(addr,len,data,is_mmio(addr));
  else
  	memcpy(guest_to_host(addr), &data, len);
}

paddr_t page_translate(vaddr_t vaddr,bool write);

uint32_t vaddr_read(vaddr_t addr, int len) {
  if(cpu.cr0.paging) {
        if (((addr&0xfff)+len)>0x1000) {
      int l1, l2; //分别表示在两页的长度
      l1 = 0x1000 - (addr &0xfff);
      l2 = len - l1;

      uint32_t a1 = page_translate(addr, 0); //读取第一个页面
      uint32_t m1 = paddr_read(a1, l1);

      uint32_t a2 = page_translate(addr + l1, 0);
      uint32_t m2 = paddr_read(a2, l2);

      return m1 + (m2 << (l1 << 3));  //结合
            assert(0);
        }
        else {
            paddr_t paddr = page_translate(addr,false);
            return paddr_read(paddr, len);
        }
    }
    else  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  if(cpu.cr0.paging) {
        if (((addr&0xfff)+len)>0x1000) {
            int l1, l2; //分别表示在两页的长度
            l1 = 0x1000 - (addr &0xfff);
            l2 = len - l1;

           uint32_t a1 = page_translate(addr, 1); //读取第一个页面
           paddr_write(a1, l1, data);

           uint32_t high_data = data >> (l1 << 3);
           uint32_t a2 = page_translate(addr + l1, 1);
           return paddr_write(a2, l2, high_data);
        }
        else {
            paddr_t paddr = page_translate(addr,true);
            paddr_write(paddr, len,data);
        }
    }
   else paddr_write(addr, len,data);
}

paddr_t page_translate(vaddr_t vaddr,bool write){
 PDE pd;
 PTE pt;
 uint32_t p1=(vaddr>>22);
 uint32_t p2=(cpu.cr3.page_directory_base<<12)+(p1<<2);
 pd.val=paddr_read(p2,4);
 assert(pd.present);

 uint32_t p3=((vaddr>>12)&0x3ff);
 uint32_t p4=(pd.val&0xfffff000)+(p3<<2);
 pt.val=paddr_read(p4,4);
 assert(pt.present);

 uint32_t p_addr=(pt.val&0xfffff000)+(vaddr&0xfff);

 pd.accessed=1;
 paddr_write(p2,4,pd.val);
 if(pt.accessed==0||(pt.dirty==0&&write)){
 pt.accessed=1;
 pt.dirty=1;
 }
 paddr_write(p4,4,pt.val);
 return p_addr;
}
