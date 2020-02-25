#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "fs.h"
#include "bitmap.c"
#include "dir.c"

FILETABLE file_table;


/* input: inode number; return: fd */
int register_filetable(int inode){
  for(int i=0; i<FILETABLE_SIZE; i++){
    //ft entry is not in use or used by same file
    if(file_table.inode_ptr[i] == -1 || (inode == file_table.inode_ptr[i])){
      (file_table.inode_ptr)[i] = inode;
      return i;
    }
  }
  return -1;
}

void display_filetable(){
  for(int i=0; i<FILETABLE_SIZE; i++){
    printf("%d, %d\n", i, file_table.inode_ptr[i]);
  }
}

int parse_path(char *file_path){
  char tmp[MAX_FILE_NAME_LENGTH];
  memcpy(tmp, file_path, MAX_FILE_NAME_LENGTH);
  int count = 0;
  char *delim = "/";
    char *p;
    strcpy(PATH[count++], strtok(tmp, delim));
    while((p = strtok(NULL, delim)))
      strcpy(PATH[count++], p);
    return count;
}

//return block number of directory
int navigate_path(int level){
  //???????????? DIR
  int dir_block = -1;
  char *result_buf = malloc(BLOCKSIZE);
  int result  = read_block(101, result_buf);
  DIR *dir = (DIR*)(result_buf); 
  //????path???????
  for(int i=0; i<level-1; i++){
    char *current_dir_name = PATH[i];
    for(int j = 0; j<MAX_DIR_ENTRY; j++){
      //????????entry
      DIRENTRY entry = (dir->list)[i];
      //???entry name???????¡¤???????, ????????????
      if(entry.inode != -1 && entry.name == current_dir_name){
        int new_inode = entry.inode;
        char *buf = malloc(BLOCKSIZE);
        read_block(new_inode, buf);
        INODE *current_node = (INODE*)buf;
        if(current_node->file_type == 1){
          //hack: ???????????§Ø??block??dir
          dir_block = (current_node->file_ptr)[0];
          read_block(dir_block, buf);
          dir = (DIR*)buf;
        }
      }
    }
  }
  return dir_block;
}
/* open an exisiting file for reading or writing */
//??????????fd, ????-1
int my_open (const char * path)
{
  int level = parse_path(path);
  int current_inode_num = 3;
  char *buf = malloc(BLOCKSIZE);
  for(int i=0; i<level - 1; i++){
    read_block(current_inode_num, buf);//buf?§Ø??????Inode
    INODE *current_inode = (INODE*)buf;
    int dir_block = current_inode->file_ptr[0];
    read_block(dir_block, buf);
    DIR *current_dir = (DIR*)buf;
    DIRENTRY *entry_lst = current_dir->list;
    for(int j=0; j<MAX_DIR_ENTRY; j++){
      if(strcmp(entry_lst[i].name, PATH[i]) == 0){
        current_inode_num = entry_lst[i].inode;
        break;
      }
    }
  }
  read_block(current_inode_num, buf);
  INODE *current_inode = (INODE*)buf;
  int current_dir_block =   current_inode->file_ptr[0];
  char *filename = PATH[level-1];
  read_block(current_dir_block, buf);
  DIR *current_dir = (DIR*)buf; //current_dir is root
  int entry_count = current_dir->count;
  DIRENTRY *entry_lst = current_dir->list;
  //????????????entry, ?????????????
  for(int i=0; i<entry_count; i++){
    char *n = entry_lst[i].name;
    if(strcmp(n, filename) == 0){
      int file_inode = entry_lst[i].inode;
      return register_filetable(file_inode);
    }
  }
  return -1;
}

/* open a new file for writing only */
int my_creat (const char * path)
{
  int level = parse_path(path);
  int current_inode_num = 3;
  for(int i=0; i<level - 1; i++){
    char *buf = malloc(BLOCKSIZE);
    read_block(current_inode_num, buf);//buf?§Ø??????Inode
    INODE *current_inode = (INODE*)buf;
    int dir_block = current_inode->file_ptr[0];
    read_block(dir_block, buf);
    DIR *current_dir = (DIR*)buf;
    DIRENTRY *entry_lst = current_dir->list;
    for(int j=0; j<MAX_DIR_ENTRY; j++){
      if(strcmp(entry_lst[i].name, PATH[i]) == 0){
        current_inode_num = entry_lst[i].inode;
        break;
      }
    }
  }
  //????root
  //create inode
  char *name = PATH[level -1 ];
  
  DIRENTRY newfile;
  strcpy(newfile.name, name);
  newfile.inode = make_file();
  //connect to root
  add_entry_inode(current_inode_num, newfile);
  return 1;

}

/* sequentially read from a file */
int my_read (int fd, void * buf, int count)
{
  int block_num = ceil((float)count/BLOCKSIZE);//????????????block
  int inode_num = file_table.inode_ptr[fd];//???fd???inode??
  char *tmpbuf = malloc(BLOCKSIZE);
  read_block(inode_num, tmpbuf);
  INODE *file_inode = (INODE*)tmpbuf;
  //????block??????
  for(int i=0; i<block_num; i++){
    int current_data_block = file_inode->file_ptr[i];
    char *tmpblock = malloc(BLOCKSIZE);
    read_block(current_data_block, tmpblock);
    memcpy(buf + i*BLOCKSIZE, tmpblock, BLOCKSIZE);
  }
  return -1;
}

/* sequentially write to a file */
int my_write (int fd, const void * buf, int count)
{
  int block_num = ceil((float)count/BLOCKSIZE);//????????????block
  int inode_num = file_table.inode_ptr[fd];//???fd???inode??
  char *tmpbuf = malloc(BLOCKSIZE);
  read_block(inode_num, tmpbuf);
  INODE *file_inode = (INODE*)tmpbuf;
  //????block???§Õ??
  for(int i=0; i<block_num; i++){
    int new_data_block = take_data();
    file_inode->file_ptr[i] = new_data_block;
    char *tmpblock = malloc(BLOCKSIZE);
    memcpy(tmpblock, buf + i*BLOCKSIZE, BLOCKSIZE);
    write_block(new_data_block, tmpblock);
  }
  write_block(inode_num, file_inode);
}

int my_close (int fd)
{
  printf ("my_close (%d) not implemented\n", fd);
  return -1;
}

int my_remove (const char * path)
{
  printf ("my_remove (%s) not implemented\n", path);
  return -1;
}

int my_rename (const char * old, const char * new)
{
  printf ("my_remove (%s, %s) not implemented\n", old, new);
  return -1;
}

/* only works if all but the last component of the path already exists */
int my_mkdir (const char * path)
{
  //hack: ?????????
  char *dirname = path;
  int father_inode = 3;
  int dir_inode = make_dir(father_inode);
  DIRENTRY entry;
  entry.inode = dir_inode;
  strcpy(entry.name, dirname);
  add_entry_inode(father_inode, entry);
}

int my_rmdir (const char * path)
{

}

/* check to see if the device already has a file system on it,
 * and if not, create one. */
void init_device(){
  /* initialize device */
  //dd if=/dev/zero of=simulated_device bs=512 count=4096
  int devsize = dev_open();
  printf("Device initialized, size = %d\n", devsize);
}

void init_superblock(){
  //init superblock
  SUPERBLOCK init_super_block = {.inode_loc = 3, .data_loc = 101, .free_space = 3000, .student_num = "09016314"};
  write_block(0, (char*)(&init_super_block)); //write into block 0#
  /*
  //some test
  char* result = malloc(BLOCKSIZE);
  read_block(0, result);
  SUPERBLOCK *sb = (SUPERBLOCK*) result;
  printf("student num233: %s\n", sb->student_num);
  */
}

void init_root_dir(){
  int root_inode = 3;
  make_dir(3);
  int file_inode = make_file();

  char *buf = malloc(BLOCKSIZE);
  read_block(root_inode, buf);
  INODE *root = (INODE*)buf;
  int root_block = root->file_ptr[0];
  read_block(root_block, buf);
  DIR *rootdir = (DIR*)buf;


  //make testfile
  DIRENTRY testfile;
  strcpy(testfile.name,  "testfile");
  testfile.inode = make_file();
  add_entry(rootdir, testfile);
  write_block(root_block, rootdir);


  //make testfolder
  DIRENTRY testfolder;
  strcpy(testfolder.name, "testfolder");
  testfolder.inode = make_dir(root_inode);
  add_entry(rootdir, testfolder);
  write_block(root_block, rootdir);


  //make myfile
  DIRENTRY myfile;
  strcpy(myfile.name, "myfile");
  myfile.inode = make_file();
  //???testfolder??inode?????testfolder??DIR
  read_block(testfolder.inode, buf);
  INODE *testfolder_inode = (INODE*)buf;
  int testfolder_data = testfolder_inode->file_ptr[0];
  read_block(testfolder_data, buf);
  DIR *testfolder_dir = (DIR*)buf;
  add_entry(testfolder_dir, myfile);
  write_block(testfolder_data, testfolder_dir );

  
}

void init_filetable(){
  for(int i=0; i<FILETABLE_SIZE; i++){
    file_table.inode_ptr[i] = -1;
  }
}
void my_mkfs ()
{
  init_device();
  init_superblock();
  init_bitmap();
  init_root_dir();
  init_filetable();
  
  printf ("my_mkfs finished\n");
}

