#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
size_t get_ramdisk_size();
void ramdisk_read(void *,off_t,size_t);

uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  //ramdisk_read(DEFAULT_ENTRY,0,get_ramdisk_size());

  int fd = fs_open(filename, 0, 0);
  int len = fs_filesz(fd);
  fs_read(fd, DEFAULT_ENTRY, len);
  fs_close(fd);

  return (uintptr_t)DEFAULT_ENTRY;
}
