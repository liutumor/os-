#include <mips/regdef.h>
#include <mips/m32c0.h>
#include <mips/asm.h>
#include <inc/trap.h>
			.data
			.global	KERNEL_SP;
KERNEL_SP:
			.word		0



	.text
	.set noat
LEAF(env_pop_tf)
	move	k0, a0			# k0 = &curenv->env_tf
	mtc0	a1, C0_ENTRYHI	# C0_ENTRYHI = ASID

	#	mfc0	t0,C0_STATUS
	#	ori		t0,0x3
	#	xori	t0,0x3
	#	mtc0	t0,C0_STATUS
	lw 		k1, TF_CAUSE(k0)
	mtc0	k1, C0_CAUSE
	lw		v1,	TF_LO(k0)
	lw		v0,	TF_HI(k0)
	mtlo	v1	
	lw		v1,	TF_EPC(k0)
	mthi	v0	
	mtc0	v1,	C0_EPC
	lw		$31,TF_REG31(k0)
	lw		$30,TF_REG30(k0)
	lw		$29,TF_REG29(k0)
	lw		$28,TF_REG28(k0)
	lw		$25,TF_REG25(k0)
	lw		$24,TF_REG24(k0)
	lw		$23,TF_REG23(k0)
	lw		$22,TF_REG22(k0)
	lw		$21,TF_REG21(k0)
	lw		$20,TF_REG20(k0)
	lw		$19,TF_REG19(k0)
	lw		$18,TF_REG18(k0)
	lw		$17,TF_REG17(k0)
	lw		$16,TF_REG16(k0)
	lw		$15,TF_REG15(k0)
	lw		$14,TF_REG14(k0)
	lw		$13,TF_REG13(k0)
	lw		$12,TF_REG12(k0)
	lw		$11,TF_REG11(k0)
	lw		$10,TF_REG10(k0)
	lw		$9,TF_REG9(k0)
	lw		$8,TF_REG8(k0)
	lw		$7,TF_REG7(k0)
	lw		$6,TF_REG6(k0)
	lw		$5,TF_REG5(k0)
	lw		$4,TF_REG4(k0)
	lw		$3,TF_REG3(k0)
	lw		$2,TF_REG2(k0)
	lw		$1,TF_REG1(k0)
	lw		k0,TF_STATUS(k0)
	mtc0	k0,C0_STATUS
	// j 0x80019550
	// eret
	nop
END(env_pop_tf)
	.set at

LEAF(lcontext)
	.extern	mCONTEXT
	sw		a0,mCONTEXT
	jr	ra
	nop
END(lcontext)



