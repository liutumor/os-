#include "fs.h"
#include <string.h>
#include <printf.h>

void fs_init()
{
	int i;
		// 清空name_table
	for (i = 0; i < CAPACITY; i++)
		memset(w.name_table[i], 0, 16);

	fs_size = 2;
	strcpy(w.name_table[0], "test");
	strcpy(w.name_table[1], "app.exe");
	struct FileControlBlock fcb;
	fcb.type = 1;
	fcb.acl = 7;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;
	fat_table[0] = fcb;
	fcb.type = 0;
	fcb.acl = 5;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;
	fat_table[1] = fcb;

	// // 初始化name_table,fat_table,fs_size
	// int i;
	// for (i = 0; i < CAPACITY; i++)
	// {
	// 	u32 addr = FAT_NAME_ADDR + i * 16;
	// 	u_ll name = mips_get_dword(addr, NULL);
	// 	if (name != 0)
	// 	{
	// 		name_table[i] = name;
	// 		fat_table[i] = fs_read_fcb(i);
	// 	}
	// 	else
	// 	{
	// 		fs_size = i;
	// 		break;
	// 	}
	// }



}

int fs_add(char* filename, int mode)
{
	struct FileControlBlock fcb;
	fcb.type = mode;
	fcb.acl = 7;
	fcb.date = 1540000000;
	fcb.size = BLOCK_SIZE;

    strcpy(w.name_table[fs_size], filename);
	fat_table[fs_size] = fcb;
    //printf("fs_size is %d\n",fs_size);
	// printf("cur: %s  prev: %s", name_table[fs_size], name_table[fs_size-1]);

	fs_size += 1;

	return 0;
}

int fs_delete(char* filename)
{
	int i, index = -1;
	for (i = 0; i < fs_size; i++)
	{
		if (strcmp(w.name_table[i], filename) == 0)
		{
			index = i;
			break;
		}
	}



	// -1 代表找不到文件
	if (index == -1)
		return 1;
	else
	{
		
		for (i = index; i < fs_size; i++)
		{
			strcpy(w.name_table[i], w.name_table[i + 1]);
			fat_table[i] = fat_table[i + 1];
		}
		fs_size--;
	}
}

struct Wrapper fs_find_all_names()
{
	return w;
}

struct FileControlBlock* fs_find_all_fcbs()
{
	return fat_table;
}

int fs_get_size()
{
	return fs_size;
}

struct FileControlBlock fs_find(char* filename)
{
	int index = -1;
	int i;
	for (i = 0; i < fs_size; i++)
	{
		if (strcmp(w.name_table[i], filename) == 0)
			index = i;
	}

	if (index != -1)
		return fat_table[index];
	else
	{
		struct FileControlBlock ret = {-1, -1, -1, -1, -1};
		return ret;
	}
}


struct FileControlBlock fs_read_fcb(int index)
{
	struct FileControlBlock ret;

	u32 base = FAT_FCB_ADDR + index * 16;
	ret.type = mips_get_byte(base, NULL);
	ret.acl = mips_get_byte(base + 1, NULL);
	ret.size = mips_get_byte(base + 2, NULL);
	ret.pointer = mips_get_byte(base + 3, NULL);
	ret.date = mips_get_word(base + 4, NULL);

	return ret;
}

void fs_write_fcb(int index, struct FileControlBlock fcb)
{
	u32 base = FAT_FCB_ADDR + index * 16;
	mips_put_byte(base, fcb.type);
	mips_put_byte(base, fcb.acl);
	mips_put_byte(base, fcb.size);
	mips_put_byte(base, fcb.pointer);
	mips_put_word(base, fcb.date);
}

//my edit
//给定一个指针block，把block_num块内的内容读进去
void fs_read_block(int block_num, char *block){
	u32 base = FAT_FS_ADDR + block_num * FS_BLOCK_SIZE;
	for(int i=0; i< FS_BLOCK_SIZE; i++){
		*(block + i) = mips_get_byte(base + i, NULL);
	}
}

//给定一个指针block，把这个指针指向的内容写到block_num块内
void fs_write_block(int block_num, char *block){
	u32 base = FAT_FS_ADDR + block_num * FS_BLOCK_SIZE;
	for(int i=0; i<FS_BLOCK_SIZE; i++){
		mips_put_byte(base+i, *(block + i));
	}
	//mips_put_byte(base, 'c');
}
int FT_index = 0;
int file_table[FT_SIZE];
//block 0: Super Block
//block 1: INode Bitmap
//block 2: Data Bitmap
//block 3-20: INode Block
//block 21-99: Data Block
int pwd = 3; //root INode number
int inode_index = 3;
int data_index = 21;

int my_fs_init(){
	for(int i=0; i<FT_SIZE; i++){
		file_table[i] = -1;
	}
	//init root directory
	//root inode preparation
	INODE inode;
	inode.file_type = 1;
	inode.file_length = 0;
	inode.file_ptr[0] = data_index++; //21
	//write root inode
	
	fs_write_block(inode_index++, &inode);

	//root dir preparation(inode pointed to)
	DIR dir;
	dir.count = 1;
	//创建当前目录entry
	DIRENTRY current_dir;
	current_dir.inode = 3;
	strcpy(current_dir.name, ".");
	current_dir.valid = true;
	dir.list[0] = current_dir; //加入DIR list
	//创建一个测试文件test
	// INODE test_inode;
	// test_inode.file_length = 0;
	// test_inode.file_type = 0;
	// test_inode.file_ptr[0] = data_index++;
	// int test_inode_num = inode_index++;
	
	// fs_write_block(test_inode_num, &test_inode);
	// DIRENTRY testfile;
	// testfile.inode = test_inode_num;
	// strcpy(testfile.name, "t1");
	//dir.list[1] = testfile;
	//write root direactory data
	fs_write_block(inode.file_ptr[0], &dir);

}

int my_open (const char * path){
	//pwd为当前目录的inode号，打开inode，取出DIR
	INODE inode_pwd;
	fs_read_block(pwd, &inode_pwd);
	int DIR_dataindex = inode_pwd.file_ptr[0];
	
	DIR DIR_pwd;
	fs_read_block(DIR_dataindex, &DIR_pwd);
	
	//打开dir，遍历list
	DIRENTRY target;
	bool flag_find = false;
	for(int i=0; i<DIR_pwd.count; i++){
		target = DIR_pwd.list[i];
		if( 0 == strcmp(DIR_pwd.list[i].name, path))
			{
				flag_find = true;
				break;
			}
	}
	if(false == flag_find)
		return -1;
	//在filetable中注册，返回file descriptor
	int target_inode = target.inode;
	int fd = FT_index++;
	file_table[fd] = target_inode;
	
	return fd;
}
int my_create (const char * path){
	//通过当前pwd，打开目录所在的DIR
	INODE pwd_inode;
	fs_read_block(pwd, &pwd_inode);
	int dir_index = pwd_inode.file_ptr[0];
	DIR pwd_dir;
	fs_read_block(dir_index, &pwd_dir);
	

	//创建新文件的INODE，其中file_ptr指向新开辟的一块data block
	int newfile_dataindex = data_index++;

	INODE newfile_inode;
	newfile_inode.file_type = 0;
	newfile_inode.file_length = 0;
	newfile_inode.file_ptr[0] = newfile_dataindex;
	int newfile_inode_num = inode_index++;
	fs_write_block(newfile_inode_num, &newfile_inode);

	//DIRENTRY内加一个新文件的INODE
	DIRENTRY newfile_direntry;
	strcpy(newfile_direntry.name, path);
	newfile_direntry.inode = newfile_inode_num;
	newfile_direntry.valid = true;
	pwd_dir.list[pwd_dir.count++] = newfile_direntry;
	
	DIRENTRY tmp = pwd_dir.list[pwd_dir.count -1];
	
	//写回DIRENTRY
	
	fs_write_block(dir_index, &pwd_dir);

	return 1;
}
int my_remove (const char * path){
	//先取出当前DIR
	INODE pwd_inode;
	fs_read_block(pwd, &pwd_inode);
	int dir_index = pwd_inode.file_ptr[0];
	DIR pwd_dir;
	fs_read_block(dir_index, &pwd_dir);
	for(int i=0; i<pwd_dir.count; i++){
		if(0 == strcmp(pwd_dir.list[i].name, path)){
			pwd_dir.list[i].valid = false;
			fs_write_block(dir_index, &pwd_dir);
			return 1;
		}
	}
	fs_write_block(dir_index, &pwd_dir);
	return 0;
}
int my_read (int fd, void * buf, int count){
	//找到fd对应的inode编号
	int inode_num = file_table[fd];
	if(inode_num < 0){
		return -1;
	}
	//根据inode号，找到inode块
	INODE file_inode;
	fs_read_block(inode_num, &file_inode);
	//根据inode块中的file_ptr，找到data块
	int data_num = file_inode.file_ptr[0];
	//从buf写入data块
	fs_read_block(data_num, buf);
	return 1;
}
int my_write (int fd, const void * buf, int count){
	//找到fd对应的inode编号
	int inode_num = file_table[fd];
	if(inode_num < 0){
		return -1;
	}
	//根据inode号，找到inode块
	INODE file_inode;
	fs_read_block(inode_num, &file_inode);
	//根据inode块中的file_ptr，找到data块
	int data_num = file_inode.file_ptr[0];
	//从buf写入data块
	fs_write_block(data_num, buf);
	return 1;
}
int my_mkdir (const char *path){
	//通过pwd打开当前文件夹的inode，再通过inode打开DIR
	INODE pwd_inode;
	fs_read_block(pwd, &pwd_inode);
	int dir_index = pwd_inode.file_ptr[0];
	DIR pwd_dir;
	fs_read_block(dir_index, &pwd_dir);
	
		//构造新文件夹的entry
		DIRENTRY new_direntry;
		strcpy(new_direntry.name, path);
		new_direntry.valid = true;
		int newdir_inode_num = inode_index++;
	
		new_direntry.inode = newdir_inode_num;
		//构造新文件夹的inode，加入新文件夹的DIR，加入., ..两个entry, inode块写回设备
		INODE newdir_inode;
		newdir_inode.file_length = 0;
		newdir_inode.file_type = 1;
		//构造新文件夹的DIR，并且写入设备
		DIR newfile_dir;
		int newfile_dir_index = data_index++;
	
		newdir_inode.file_ptr[0] = newfile_dir_index;
		fs_write_block(newdir_inode_num, &newdir_inode);
		//构造entry
		DIRENTRY current, previous;
		current.valid = previous.valid = true;
		strcpy(current.name, ".");
		strcpy(previous.name, "..");
		current.inode = newdir_inode_num;
		previous.inode = pwd;
		newfile_dir.count = 2;
		newfile_dir.list[0] = previous;
		newfile_dir.list[1] = current;
		fs_write_block(newfile_dir_index, &newfile_dir);
	//pwd的DIR加上这个entry
	pwd_dir.list[(pwd_dir.count)++] = new_direntry;
	//DIR写回硬件
	fs_write_block(dir_index, &pwd_dir);
	return 1;
}
int my_rmdir (const char * path){
	return 1;
}
int my_ls(){
	//通过pwd打开一个inode，通过这个inode打开一个dir
	INODE pwd_inode;
	fs_read_block(pwd, &pwd_inode);
	int dir_index = pwd_inode.file_ptr[0];
	DIR pwd_dir;
	fs_read_block(dir_index, &pwd_dir);
	
	//遍历dir里面的list
	for(int i=0; i<pwd_dir.count; i++){
		DIRENTRY current_entry = pwd_dir.list[i];
		if(current_entry.valid == true)
			printf("%s ", current_entry.name);
	}
	printf("\n");
	
	return 1;
}
int my_cd (const char *path){
	//通过pwd找到INode，再找到DIR
	INODE pwd_inode;
	fs_read_block(pwd, &pwd_inode);
	int dir_index = pwd_inode.file_ptr[0];
	DIR pwd_dir;
	fs_read_block(dir_index, &pwd_dir);
	//遍历DIR中的DIRENTRY，根据路径找到新文件夹的INODE，更改PWD
	for(int i=0; i<pwd_dir.count; i++){
		DIRENTRY entry = pwd_dir.list[i];
		if(strcmp(entry.name, path) == 0){
			pwd = entry.inode;
			return 1;
		}
	}
	return 0;
}

