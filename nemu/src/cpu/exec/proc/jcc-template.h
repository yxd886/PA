#include "cpu/exec/template-start.h"

#define instr jcc

static void do_execute () {

	int32_t rel;
#if DATA_BYTE == 1
	rel = (int32_t)(int8_t)op_src->val;
#elif DATA_BYTE == 2
	rel = (int32_t)(int16_t)op_src->val;
#elif DATA_BYTE == 4
	rel = (int32_t)op_src->val;
#endif

	switch (ops_decoded.opcode & 0xff){
		case 0x70:
		case 0x80:
			if (cpu.OF == 1)
				cpu.eip += rel;
			break;
		case 0x71:
		case 0x81:
			if (cpu.OF == 0)
				cpu.eip += rel;
			break;
		case 0x72:
		case 0x82:
			if (cpu.CF == 1)
				cpu.eip += rel;
			break;
		case 0x73:
		case 0x83:
			if (cpu.CF == 0)
				cpu.eip += rel;
			break;
		case 0x74:
		case 0x84:
			if (cpu.ZF == 1)
				cpu.eip += rel;
			break;
		case 0x75:
		case 0x85:
			if (cpu.ZF == 0)
				cpu.eip += rel;
			break;
		case 0x76:
		case 0x86:
			if (cpu.CF == 1 || cpu.ZF == 1)
				cpu.eip += rel;
			break;
		case 0x77:
		case 0x87:
			if (cpu.CF == 0 && cpu.ZF == 0)
				cpu.eip += rel;
			break;
		case 0x78:
		case 0x88:
			if (cpu.SF == 1)
				cpu.eip += rel;
			break;
		case 0x79:
		case 0x89:
			if (cpu.SF == 0)
				cpu.eip += rel;
			break;
		case 0x7a:
		case 0x8a:
			if (cpu.PF == 1)
				cpu.eip += rel;
			break;
		case 0x7b:
		case 0x8b:
			if (cpu.PF == 0)
				cpu.eip += rel;
			break;
		case 0x7c:
		case 0x8c:
			if (cpu.SF != cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7d:
		case 0x8d:
			if (cpu.SF == cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7e:
		case 0x8e:
			if (cpu.ZF == 1 || cpu.SF != cpu.OF)
				cpu.eip += rel;
			break;
		case 0x7f:
		case 0x8f:
			if (cpu.ZF == 0 && cpu.SF == cpu.OF)
				cpu.eip += rel;
			break;
		case 0xe3:
			if (reg_l(R_ECX) == 0 || reg_w(R_CX) == 0)
				cpu.eip += rel;
			break;
	}

#if DATA_BYTE == 2
	cpu.eip &= 0x0000ffff;
#endif

	print_asm_template2();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
