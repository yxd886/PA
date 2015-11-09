#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE result = op_src->val + 1;

	uint8_t b = result & 0x000000ff;
	cpu.SF = MSB(result);
	cpu.ZF = result ? 0 : 1;
	cpu.OF = (result < op_src->val) ? 1 : 0;
	cpu.PF = ((b&1)^(b>>1&1)^(b>>2&1)^(b>>3&1)^(b>>4&1)^(b>>5&1)^(b>>6&1)^(b>>7&1)) ? 1 : 0;

	OPERAND_W(op_src, result);

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
