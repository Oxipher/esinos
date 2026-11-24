#ifndef FS_H
#define FS_H

#define DISK_FILE        "disk.img"
#define BLOCK_SIZE       64
#define DISK_BLOCKS      64
#define FS_NAME_MAX      11
#define INODE_COUNT      8
#define FS_MAX_BLOCKS    3
#define FS_MAX_FILE_SIZE (FS_MAX_BLOCKS * BLOCK_SIZE)

typedef struct {
    int used;
    char name[FS_NAME_MAX + 1];
    int size;
    int blocks[FS_MAX_BLOCKS];
} FsInode;

void fs_format(void);
int  fs_create(const char *name);
int  fs_write(const char *name, const char *data, int size);
int  fs_read(const char *name, char *out, int max);
int  fs_delete(const char *name);
void fs_ls(void);

#endif