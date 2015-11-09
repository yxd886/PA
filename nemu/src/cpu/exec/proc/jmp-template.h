#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute () {

	switch (ops_decoded.opcode & 0xff){
		case 0xe9:
		case 0xeb:
			cpu.eip += op_src->val;
			if (DATA_BYTE == 2){
				cpu.eip &= 0x0000ffff;
			}
			break;
		case 0xff:
			cpu.eip = op_src->val;
			if (DATA_BYTE == 2){
				cpu.eip &= 0x0000ffff;
			}
			// printf("%u\n", DATA_BYTE);
			cpu.eip -= 2;
			break;
	}

	print_asm_template2();
}

make_instr_helper(i)

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"
