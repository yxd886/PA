#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE result = op_src->val - 1;

	/* TODO: Update EFLAGS. */
	//panic("please implement me");

	//set EFLAGS's value
	cpu.ZF = result ? 0 : 1;
	
	if((uint8_t)(op_dest->val & 0x0f) == 0)
		cpu.AF = 1;
	else
		cpu.AF = 0;
	
	uint8_t low_b = result & 0xff;
	cpu.PF = !((low_b & 0x01)^(low_b>>1 & 0x01)^(low_b>>2 & 0x01)^(low_b>>3 & 0x01)^(low_b>>4 & 0x01)^(low_b>>5 & 0x01)^(low_b>>6 & 0x01)^(low_b>>7 & 0x01));

	cpu.SF = MSB(result);
	
	cpu.OF = MSB(op_src->val) ? !cpu.SF : 0;
	
	OPERAND_W(op_src, result);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
