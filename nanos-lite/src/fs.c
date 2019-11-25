#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

intptr_t fs_open(const char *path, int flags, int mode) {
  for (int i = 0; i < NR_FILES; ++i)
    if (strcmp(file_table[i].name, path) == 0) return i;
  // should not reach here
  panic("File not found: %s\n", path);
  return -1;
}

intptr_t fs_write(int fd, const char *buf, size_t count) {
  if(fd == FD_STDOUT || fd == FD_STDERR) {
    for (int i = 0; i < count; ++i)
      _putc(buf[i]);
    return count;
  }
  else if(fd == FD_STDIN) return count;
  else {
    size_t start_oft = file_table[fd].disk_offset + file_table[fd].open_offset;
    if(file_table[fd].open_offset + count > file_table[fd].size)
      count = file_table[fd].size - file_table[fd].open_offset;
    if (file_table[fd].write != NULL)
      file_table[fd].write(buf, start_oft, count);
    else ramdisk_write(buf, start_oft, count);
    file_table[fd].open_offset += count;
    return count;
  }
  return -1;
  //Log();
}

intptr_t fs_close(int fd) {
  return 0;
}