#include "cpu/exec/template-start.h"

#define instr setcc

static void do_execute () {


	// printf("before setcc: %u\n", reg_l(op_src->reg));

	switch (ops_decoded.opcode & 0xff){
		case 0x90:
			if (cpu.OF == 1)	
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x91:
			if (cpu.OF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x92:
			if (cpu.CF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x93:
			if (cpu.CF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x94:
			if (cpu.ZF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x95:
			if (cpu.ZF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x96:
			if (cpu.CF == 1 || cpu.ZF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x97:
			if (cpu.CF == 0 && cpu.ZF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x98:
			if (cpu.SF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x99:
			if (cpu.SF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9a:
			if (cpu.PF == 1)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9b:
			if (cpu.PF == 0)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9c:
			if (cpu.SF != cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9d:
			if (cpu.SF == cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9e:
			if (cpu.ZF == 1 || cpu.SF != cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
		case 0x9f:
			if (cpu.ZF == 0 && cpu.SF == cpu.OF)
				OPERAND_W(op_src, 1);
			else
				OPERAND_W(op_src, 0);
			break;
	}

	// printf("after setcc: %u\n", reg_l(op_src->reg));

	print_asm_template2();
}

make_instr_helper(rm)

#include "cpu/exec/template-end.h"
