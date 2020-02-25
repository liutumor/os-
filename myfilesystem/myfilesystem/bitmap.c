#include <stdio.h>
#include "fs.h"

void init_bitmap(){
    //bitmap中，第n个字节为0，代表BLOCK n空闲，为1代表被占用
    char *all0 = malloc(BLOCKSIZE);
    for(int i=0; i<BLOCKSIZE; i++){
        all0[i] = 0;
    }
    write_block(1, all0);//init INode bitmap
    write_block(2, all0);//init Data bitmap
}

//return the first free inode offset
int take_inode(){
    char *inode_bitmap = malloc(BLOCKSIZE);
    int result = read_block(1, inode_bitmap);
    for(int i=0; i<BLOCKSIZE; i++){
        if(inode_bitmap[i] == 0){
            inode_bitmap[i] = 1; //mark as taken
            write_block(1, inode_bitmap); // write back
            return i + INODE_START;
        }
    }
}

//return the first free data block offset
int take_data(){
    char *data_bitmap = malloc(BLOCKSIZE);
    int result = read_block(2, data_bitmap);
    for(int i=0; i<BLOCKSIZE; i++){
        if(data_bitmap[i] == 0){
            data_bitmap[i] = 1; //mark as taken
            write_block(2, data_bitmap); //write back
            return i+ DATA_START;
        }
    }
}

//TODO: implement delete...
