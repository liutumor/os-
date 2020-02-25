#ifndef FS
#define FS
#define MAX_FILE_NAME_LENGTH 30
#define MAX_OPEN_FILES 10
#define INODE_ENTRIES 8
#define MAX_NAME_LENGTH 30
#define MAX_DIR_ENTRY 10
#define INODE_START 3
#define DATA_START 101
#define FILETABLE_SIZE 10
char PATH[10][MAX_FILE_NAME_LENGTH];
/* data structures */
typedef struct FileTable{
    int inode_ptr[FILETABLE_SIZE];
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

//size: 36B
typedef struct DirEntry{
    char name[MAX_NAME_LENGTH];
    int inode;
} DIRENTRY;

//size: 364B
typedef struct Dir{
    int count; //entry count in this directory
    DIRENTRY list[MAX_DIR_ENTRY];
} DIR;

/* tools */
int parse_path(char *path);
void display_filetable();
/* file API */
extern int my_open (const char * path);
extern int my_creat (const char * path);
extern int my_read (int fd, void * buf, int count);
extern int my_write (int fd, const void * buf, int count);
extern int my_close (int fd);

extern int my_remove (const char * path);
extern int my_rename (const char * old, const char * new);
extern int my_mkdir (const char * path);
extern int my_rmdir (const char * path);

extern void my_mkfs ();

/* provided by the lower layer */

#define BLOCKSIZE 512
/* not used in any declaration, just a reminder that each block is 1KB */
/* and may be useful inside the code. */
typedef char block [BLOCKSIZE];

extern int dev_open ();
extern int read_block (int block_num, char * block);
extern int write_block (int block_num, char * block);

#endif

