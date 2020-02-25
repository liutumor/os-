#pragma once

#ifndef _FS_H_
#define _FS_H_ 1

#include <mfp_io.h>
#include <types.h>
#include "fcb.h"
#include <printf.h>
#include <mips/cpu.h>

#define BLOCK_SIZE 4
#define CAPACITY 64

// 封装二维数组以进行return操作
struct Wrapper
{
	char name_table[CAPACITY][16];					// 文件名列表
};
struct Wrapper w;
struct FileControlBlock fat_table[CAPACITY];		// FCB列表
int fs_size;										// 文件数量

void fs_init();
struct FileControlBlock fs_find(char*);
int fs_add(char*, int);
int fs_delete(char*); 

struct Wrapper fs_find_all_names();
struct FileControlBlock* fs_find_all_fcbs();
int fs_get_size();
struct FileControlBlock fs_read_fcb(int);
void fs_write_fcb(int, struct FileControlBlock);



// See COPYRIGHT for copyright information.



// File nodes (both in-memory and on-disk)

// Bytes per file system block - same as page size
#define BY2BLK		BY2PG
#define BIT2BLK		(BY2BLK*8)

// Maximum size of a filename (a single path component), including null
#define MAXNAMELEN	128

// Maximum size of a complete pathname, including null
#define MAXPATHLEN	1024

// Number of (direct) block pointers in a File descriptor
#define NDIRECT		10
#define NINDIRECT	(BY2BLK/4)

#define MAXFILESIZE	(NINDIRECT*BY2BLK)

#define BY2FILE     256

struct File {
	u_char f_name[MAXNAMELEN];	// filename
	u_int f_size;			// file size in bytes
	u_int f_type;			// file type
	u_int f_direct[NDIRECT];
	u_int f_indirect;

	struct File *f_dir;		// valid only in memory
	u_char f_pad[256-MAXNAMELEN-4-4-NDIRECT*4-4-4];
};

#define FILE2BLK	(BY2BLK/sizeof(struct File))

// File types
#define FTYPE_REG		0	// Regular file
#define FTYPE_DIR		1	// Directory


// File system super-block (both in-memory and on-disk)

#define FS_MAGIC	0x68286097	// Everyone's favorite OS class

struct Super {
	u_int s_magic;		// Magic number: FS_MAGIC
	u_int s_nblocks;	// Total number of blocks on disk
	struct File s_root;	// Root directory node
};

// Definitions for requests from clients to file system

#define FSREQ_OPEN	1
#define FSREQ_MAP	2
#define FSREQ_SET_SIZE	3
#define FSREQ_CLOSE	4
#define FSREQ_DIRTY	5
#define FSREQ_REMOVE	6
#define FSREQ_SYNC	7

struct Fsreq_open {
	char req_path[MAXPATHLEN];
	u_int req_omode;
};

struct Fsreq_map {
	int req_fileid;
	u_int req_offset;
};

struct Fsreq_set_size {
	int req_fileid;
	u_int req_size;
};

struct Fsreq_close {
	int req_fileid;
};

struct Fsreq_dirty {
	int req_fileid;
	u_int req_offset;
};

struct Fsreq_remove {
	u_char req_path[MAXPATHLEN];
};

//my edit
//device operations
void fs_read_block(int block_num, char *block);
void fs_write_block(int block_num, char *block);

//application interfaces
extern int my_fs_init();
extern int my_open (const char * path);
extern int my_create (const char * path);
extern int my_remove (const char * path);
extern int my_read (int fd, void * buf, int count);
extern int my_write (int fd, const void * buf, int count);
extern int my_mkdir (const char *path);
extern int my_rmdir (const char * path);
extern int my_cd (const char *path);
extern int my_ls();

//Data structures
#define FT_SIZE 10
#define INODE_ENTRIES 8
#define MAX_NAME_LENGTH 3
#define MAX_DIR_ENTRY 5

extern int FT_index;
extern int file_table[FT_SIZE];

typedef struct FileTable{
    int inode_ptr[FT_SIZE];
} FILETABLE;

typedef struct SuperBlock{
    int inode_loc; //starting block of I-nodes, 1st inode -> root dir
    int data_loc; //starting block of data region
    int free_space; //how many remaining blocks are empty
    char student_num[9];    
} SUPERBLOCK;

typedef struct INode{
    int file_type; //0: file, 1: Dir, other: unvalid
    int file_length; //how many bytes
    int file_ptr[INODE_ENTRIES]; //block number of the file(at most 8)
} INODE;

//size: 5B
typedef struct DIRENTRY{
	int inode;
    char name[MAX_NAME_LENGTH];
	bool valid;

} DIRENTRY;

//size: 34B
typedef struct DIR{
    int count; //entry count in this directory
    DIRENTRY list[MAX_DIR_ENTRY];
} DIR;

#endif // _FS_H_
