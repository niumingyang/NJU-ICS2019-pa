#include "fs.h"
#include <unistd.h>

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

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/events", 0, 0, 0, events_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *path, int flags, int mode) {
  for (int i = 0; i < NR_FILES; ++i)
    if (strcmp(file_table[i].name, path) == 0) return i;
  // should not reach here
  panic("File not found: %s\n", path);
  return -1;
}

ssize_t fs_read(int fd, void *buf, size_t count) {
  size_t start_oft = file_table[fd].disk_offset + file_table[fd].open_offset;
  if(file_table[fd].open_offset + count > file_table[fd].size)
    count = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].read != NULL)
    file_table[fd].read(buf, start_oft, count);
  else ramdisk_read(buf, start_oft, count);    
  file_table[fd].open_offset += count;
  return count;
}

ssize_t fs_write(int fd, const void *buf, size_t count) {
  size_t start_oft = file_table[fd].disk_offset + file_table[fd].open_offset;
  if(file_table[fd].open_offset + count > file_table[fd].size)
    count = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].write != NULL)
    file_table[fd].write(buf, start_oft, count);
  else ramdisk_write(buf, start_oft, count);
  file_table[fd].open_offset += count;
  return count;
  //Log();
}

int fs_close(int fd) {
  return 0;
}

off_t fs_lseek(int fd, off_t offset, int whence) {
  size_t oft;
  switch(whence){
    case SEEK_SET: oft = offset; break;
    case SEEK_CUR: oft = offset + file_table[fd].open_offset; break;
    case SEEK_END: oft = offset + file_table[fd].size; break;
    default: return -1;
  }
  file_table[fd].open_offset = oft;
  return oft;
}