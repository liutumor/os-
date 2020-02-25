#pragma once

#include <types.h>

struct FileControlBlock {
	u8 type;
	u8 acl;
	u8 size;
	u8 pointer;
	u_int date;
};
