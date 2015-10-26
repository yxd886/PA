#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
	//OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	//panic("please implement me");
	cpu.CF = 0;
	cpu.OF = 0;
	if(result < 0)
		cpu.SF = 1;
	else
		cpu.SF = 0;
	if(result == 0)
		cpu.ZF = 0;
	int bit[8];
	bit[0]=result&0x1;
	bit[1]=(result>>1)&0x1;
	bit[2]=(result>>2)&0x1;
	bit[3]=(result>>3)&0x1;
	bit[4]=(result>>4)&0x1;
	bit[5]=(result>>5)&0x1;
	bit[6]=(result>>6)&0x1;
	bit[7]=(result>>7)&0x1;
	cpu.PF = ~(bit[0]^bit[1]^bit[2]^bit[3]^bit[4]^bit[5]^bit[6]^bit[7]);
	

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
