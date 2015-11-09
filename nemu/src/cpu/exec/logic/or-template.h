#include "cpu/exec/template-start.h"

#define instr or

static void do_execute () {
	DATA_TYPE result = op_dest->val | op_src->val;	

	uint8_t b = result & 0x000000ff;
	cpu.SF = MSB(result);
	cpu.ZF = result ? 0 : 1;
	cpu.OF = 0;
	cpu.PF = ((b&1)^(b>>1&1)^(b>>2&1)^(b>>3&1)^(b>>4&1)^(b>>5&1)^(b>>6&1)^(b>>7&1)) ? 1 : 0;
	cpu.CF = 0;

	OPERAND_W(op_dest, result);

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
