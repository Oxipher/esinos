#ifndef FS_H
#define FS_H

/* Virtual disk file and block geometry */
#define DISK_FILE        "disk.img"
#define BLOCK_SIZE       64                          /* Bytes per disk block */
#define DISK_BLOCKS      64                          /* Total blocks in disk.img */

/* Filename, inode table, and per-file size limits */
#define FS_NAME_MAX      11                          /* Max characters before '\0' */
#define INODE_COUNT      8                           /* Total inode slots (8 * 32B = 256B = 4 blocks) */
#define FS_MAX_BLOCKS    3                           /* Direct data block pointers per file */
#define FS_MAX_FILE_SIZE (FS_MAX_BLOCKS * BLOCK_SIZE) /* Max file size in bytes (192B) */

/* Compact 32-byte inode metadata describing a single file */
typedef struct {
    int used;                        /* 1 if slot is allocated, 0 if free */
    char name[FS_NAME_MAX + 1];      /* Null-terminated file name (12 bytes) */
    int size;                        /* Logical file size in bytes */
    int blocks[FS_MAX_BLOCKS];       /* Allocated data block indices (-1 if unused) */
} FsInode;

/* Core filesystem operations */
void fs_format(void);                /* Initialize disk.img, superblock, and free-block bitmap */
int  fs_create(const char *name);    /* Allocate an empty file entry with `name` */
int  fs_write(const char *name, const char *data, int size); /* Write `size` bytes to `name` */
int  fs_read(const char *name, char *out, int max);          /* Read up to `max` bytes from `name` */
int  fs_delete(const char *name);    /* Free data blocks and release inode for `name` */
void fs_ls(void);                    /* List active files, sizes, and block mappings */

/* lecture seule pour l'interface demo : carte d'occupation du disque */
int fs_bitmap_get(unsigned char *bits, int nbytes);          /* Copy raw block allocation bitmap */
int fs_stat_get(const char *name, int *size, int blocks[FS_MAX_BLOCKS]); /* Query size and blocks */

#endif
