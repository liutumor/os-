# Main makefile
#
# Copyright (C) 2007 Beihang University
# Written by Zhu Like ( zlike@cse.buaa.edu.cn )
#

drivers_dir	  := drivers
boot_dir	  := boot
user_dir	  := user
init_dir	  := init
lib_dir		  := lib
#fs_dir		  := fs
mm_dir		  := mm
tools_dir	  := tools
#link_script   := $(tools_dir)/scse0_3.lds
link_script   := scse0_3.lds

modules		  := boot drivers init lib mm user fs
objects		  := $(boot_dir)/*.o			  \
			 	 $(lib_dir)/*.o				  \
				 $(drivers_dir)/*.o	 		  \
				 $(init_dir)/*.o			  \
				 $(mm_dir)/*.o				  \
				 $(user_dir)/*.o			  \
				 tool/*.o   			      \
				 fs/*.o


.PHONY: all $(modules) clean run

all: $(modules) vmlinux

vmlinux: $(modules)
	$(LD) -EL -nostartfiles -N -T scse0_3.lds -G0 -o vmlinux.elf $(objects) 
	$(OC) --remove-section .MIPS.abiflags --remove-section .reginfo vmlinux.elf
	$(SZ) vmlinux.elf
	$(OD) -D -l -t vmlinux.elf > vmlinux.dis
	$(OD) -D vmlinux.elf > vmlinux.txt
	$(OC) vmlinux.elf -O srec vmlinux.rec

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf)  $(user_disk)
run:
	/OSLAB/gxemul -E testmips -C R3000 -M 64 -d gxemul/fs.img gxemul/vmlinux

include include.mk
