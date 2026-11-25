#include <stdio.h>
#include <string.h>
#include "fs.h"

#define SUPER_BLOCK 0
#define BITMAP_BLOCK 1
#define INODE_START 2
#define INODE_BYTES ((int)sizeof(FsInode))
#define INODE_BLOCKS ((INODE_COUNT * INODE_BYTES + BLOCK_SIZE - 1) / BLOCK_SIZE)
#define DATA_START (INODE_START + INODE_BLOCKS)
#define FS_MAGIC 0x4553494E /* "ESIN" en ASCII */

/* fs.h promet le layout compact : sans lui, l'arithmetique des blocs
 * d'inodes devient fausse. Le compilateur refuse alors de batir. */
_Static_assert(INODE_BYTES == 32, "FsInode doit rester compact : 32 octets");

static int block_read(int b, void *buf) {
    FILE *f = fopen(DISK_FILE, "rb");
    if (f == NULL) { return -1; }
    fseek(f, b * BLOCK_SIZE, SEEK_SET);
    int ok = (fread(buf, 1, BLOCK_SIZE, f) == BLOCK_SIZE);
    fclose(f);
    return ok ? 0 : -1;
}

static int block_write(int b, const void *buf) {
    FILE *f = fopen(DISK_FILE, "r+b");
    if (f == NULL) { return -1; }
    fseek(f, b * BLOCK_SIZE, SEEK_SET);
    int ok = (fwrite(buf, 1, BLOCK_SIZE, f) == BLOCK_SIZE);
    fclose(f);
    return ok ? 0 : -1;
}

/* Deux inodes partagent un bloc (2 x 32 = 64) : lire le bloc, ecraser
 * sa moitie, le reecrire. L'indice i donne l'octet i * 32. */
static int inode_read(int i, FsInode *ino) {
    char blk[BLOCK_SIZE];
    int b = INODE_START + i / 2;
    if (block_read(b, blk) != 0) { return -1; }
    memcpy(ino, blk + (i % 2) * INODE_BYTES, sizeof(FsInode));
    return 0;
}

static int inode_write(int i, const FsInode *ino) {
    char blk[BLOCK_SIZE];
    int b = INODE_START + i / 2;
    if (block_read(b, blk) != 0) { return -1; }
    memcpy(blk + (i % 2) * INODE_BYTES, ino, sizeof(FsInode));
    return block_write(b, blk);
}

static int disk_ok(void) {
    FILE *f = fopen(DISK_FILE, "rb");
    if (f == NULL) {
        printf("disk.img absent : choisissez [1] format d'abord\n");
        return 0;
    }
    int super[2] = { 0, 0 };
    int ok = (fread(super, sizeof super, 1, f) == 1);
    fclose(f);
    if (!ok || super[0] != FS_MAGIC) {
        printf("disk.img non formate : choisissez [1] format d'abord\n");
        return 0;
    }
    return 1;
}

static int find_inode(const char *name) {
    FsInode ino;
    for (int i = 0; i < INODE_COUNT; i++) {
        inode_read(i, &ino);
        if (ino.used && strcmp(ino.name, name) == 0) { return i; }
    }
    return -1;
}

void fs_format(void) {
    FILE *f = fopen(DISK_FILE, "wb");
    if (f == NULL) { printf("disk.img : impossible a creer\n"); return; }
    char zero[BLOCK_SIZE];
    memset(zero, 0, BLOCK_SIZE);
    for (int b = 0; b < DISK_BLOCKS; b++) { fwrite(zero, 1, BLOCK_SIZE, f); }
    fclose(f);

    /* superbloc : la magie + le nombre de blocs de donnees libres */
    char blk[BLOCK_SIZE];
    int super[2] = { FS_MAGIC, DISK_BLOCKS - DATA_START };
    memset(blk, 0, BLOCK_SIZE);
    memcpy(blk, super, sizeof super);
    block_write(SUPER_BLOCK, blk);

    /* bitmap : les blocs systeme 0..5 sont occupes, les 58 autres non */
    memset(blk, 0, BLOCK_SIZE);
    for (int b = 0; b < DATA_START; b++) { blk[b / 8] |= (char)(1 << (b % 8)); }
    block_write(BITMAP_BLOCK, blk);

    printf("disk.img formate : 64 blocs, %d blocs de donnees libres\n",
           DISK_BLOCKS - DATA_START);
}

int fs_create(const char *name) {
    if (!disk_ok()) { return -1; }
    if ((int)strlen(name) > FS_NAME_MAX) {
        printf("nom trop long : %d caracteres maximum\n", FS_NAME_MAX);
        return -1;
    }
    FsInode ino;
    for (int i = 0; i < INODE_COUNT; i++) {
        inode_read(i, &ino);
        if (ino.used && strcmp(ino.name, name) == 0) {
            printf("%s existe deja\n", name);
            return -1;
        }
    }
    for (int i = 0; i < INODE_COUNT; i++) {
        inode_read(i, &ino);
        if (!ino.used) {
            memset(&ino, 0, sizeof ino);
            ino.used = 1;
            strcpy(ino.name, name);
            ino.size = 0;
            for (int b = 0; b < FS_MAX_BLOCKS; b++) { ino.blocks[b] = -1; }
            return inode_write(i, &ino);
        }
    }
    printf("table d'inodes pleine : %d fichiers maximum\n", INODE_COUNT);
    return -1;
}

int fs_write(const char *name, const char *data, int size) {
    if (!disk_ok()) { return -1; }
    int i = find_inode(name);
    if (i < 0) { printf("%s : fichier absent\n", name); return -1; }
    if (size > FS_MAX_FILE_SIZE) {
        printf("%d octets : plus grand que %d\n", size, FS_MAX_FILE_SIZE);
        return -1;
    }
    FsInode ino;
    inode_read(i, &ino);
    char bitmap[BLOCK_SIZE];
    char blk[BLOCK_SIZE];
    if (block_read(BITMAP_BLOCK, bitmap) != 0) { return -1; }

    /* liberer les anciens blocs, puis allouer les nouveaux (first-fit) */
    for (int b = 0; b < FS_MAX_BLOCKS; b++) {
        if (ino.blocks[b] >= 0) {
            bitmap[ino.blocks[b] / 8] &= (char)~(1 << (ino.blocks[b] % 8));
            ino.blocks[b] = -1;
        }
    }
    int need = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int allocated = 0;
    for (int b = DATA_START; b < DISK_BLOCKS && allocated < need; b++) {
        if (!(bitmap[b / 8] & (1 << (b % 8)))) {
            bitmap[b / 8] |= (char)(1 << (b % 8));
            ino.blocks[allocated] = b;
            allocated++;
        }
    }
    if (allocated < need) { printf("disque plein pour %s\n", name); return -1; }
    for (int b = 0; b < need; b++) {
        int chunk = size - b * BLOCK_SIZE;
        if (chunk > BLOCK_SIZE) { chunk = BLOCK_SIZE; }
        memset(blk, 0, BLOCK_SIZE);
        memcpy(blk, data + b * BLOCK_SIZE, chunk);
        if (block_write(ino.blocks[b], blk) != 0) { return -1; }
    }
    ino.size = size;
    if (inode_write(i, &ino) != 0) { return -1; }
    if (block_write(BITMAP_BLOCK, bitmap) != 0) { return -1; }
    return size;
}

int fs_read(const char *name, char *out, int max) {
    if (!disk_ok()) { return -1; }
    int i = find_inode(name);
    if (i < 0) { return -1; }
    FsInode ino;
    inode_read(i, &ino);
    int got = (ino.size < max) ? ino.size : max;
    char blk[BLOCK_SIZE];
    for (int b = 0; b * BLOCK_SIZE < got; b++) {
        if (block_read(ino.blocks[b], blk) != 0) { return -1; }
        int chunk = got - b * BLOCK_SIZE;
        if (chunk > BLOCK_SIZE) { chunk = BLOCK_SIZE; }
        memcpy(out + b * BLOCK_SIZE, blk, chunk);
    }
    return got;
}

int fs_delete(const char *name) {
    if (!disk_ok()) { return -1; }
    int i = find_inode(name);
    if (i < 0) { printf("%s : fichier absent\n", name); return -1; }
    FsInode ino;
    inode_read(i, &ino);
    char bitmap[BLOCK_SIZE];
    if (block_read(BITMAP_BLOCK, bitmap) != 0) { return -1; }
    for (int b = 0; b < FS_MAX_BLOCKS; b++) {
        if (ino.blocks[b] >= 0) {
            bitmap[ino.blocks[b] / 8] &= (char)~(1 << (ino.blocks[b] % 8));
        }
    }
    memset(&ino, 0, sizeof ino);
    if (inode_write(i, &ino) != 0) { return -1; }
    if (block_write(BITMAP_BLOCK, bitmap) != 0) { return -1; }
    return 0;
}

void fs_ls(void) {
    if (!disk_ok()) { return; }
    FsInode ino;
    printf("inode | nom | taille | blocs\n");
    printf("------+--------------+--------+----------------\n");
    for (int i = 0; i < INODE_COUNT; i++) {
        inode_read(i, &ino);
        if (!ino.used) { continue; }
        printf("%6d| %-12s | %6d |", i, ino.name, ino.size);
        for (int b = 0; b < FS_MAX_BLOCKS && ino.blocks[b] >= 0; b++) {
            printf(" %d", ino.blocks[b]);
        }
        printf("\n");
    }
}
