#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)
size_t get_ramdisk_size();
void ramdisk_read(void *,off_t,size_t);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());

  //int fd = fs_open(filename, 0, 0);
  //int len = fs_filesz(fd);
  //fs_read(fd, DEFAULT_ENTRY, len);
  //fs_close(fd);
  
  int fd = fs_open(filename, 0, 0);
  void* vaddr = DEFAULT_ENTRY;
  uint32_t pagenum= ((fs_filesz(fd) - 1) >> 12) + 1;
  for (; pagenum-- ; vaddr += PGSIZE){
	  void *page = new_page();
	  _map(as, vaddr, page); 
	  fs_read(fd, page, PGSIZE);
	  //Log("Map va to pa :0x%08X to 0x%08X", vaddr, page);
  }
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
