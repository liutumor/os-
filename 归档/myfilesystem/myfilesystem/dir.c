#include "fs.h"
#include<stdio.h>


int create_inode(int ifDir){
  int inode_block_num = take_inode();
  INODE inode;
  inode.file_length = 0;
  inode.file_type = ifDir;
  for(int i=0; i< INODE_ENTRIES; i++){
    inode.file_ptr[i] = -1;
  }
  write_block(inode_block_num, &inode);
  return inode_block_num;
}

int make_file(){
  int root_inode = take_inode() ;//get block# for inode
  INODE i;//prepare the inode for root dir
  i.file_type = 0; //dir
  i.file_length = 0;
  for(int it=1; it<INODE_ENTRIES; it++){
    i.file_ptr[it] = -1;
  }
  write_block(root_inode, &i); //write into inode
  return root_inode;
}

//返回dir的inode num
int make_dir(int father_inode){
  int root_data = take_data(); //get block# of data
  int root_inode = take_inode() ;//get block# for inode
  INODE i;//prepare the inode for root dir
  i.file_type = 1; //dir
  i.file_length = 364;
  i.file_ptr[0] = root_data;
  for(int it=1; it<INODE_ENTRIES; it++){
    i.file_ptr[it] = -1;
  }
  write_block(root_inode, &i); //write into inode

  //init data
  DIR d;
  DIRENTRY current, father;
  current.inode = root_inode; //root的上一级目录也是root
  father.inode = father_inode;
  strcpy(current.name, ".");
  strcpy(father.name, "..");
  d.count = 2;
  d.list[0] = current;
  d.list[1] = father;
  write_block(root_data, &d);

  return root_inode;
}




//succeed return 1, otherwise -1
int add_entry(DIR *dir, DIRENTRY entry){
    if(MAX_DIR_ENTRY ==dir->count)
    {
        return -1;
    }
    else
    {
        dir->list[dir->count++] = entry;
        return 1;
    }
}

int add_entry_inode(int father_inode_num, DIRENTRY entry)
{
  char *buf = malloc(BLOCKSIZE);
  read_block(father_inode_num, buf);
  INODE *father_inode = (INODE*)buf;
  int dir_block = father_inode->file_ptr[0];
  read_block(dir_block, buf);
  DIR *newdir = (DIR*)buf;
  add_entry(newdir, entry);
  write_block(dir_block, newdir);
}