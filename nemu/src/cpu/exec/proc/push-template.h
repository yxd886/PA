#include "cpu/exec/template-start.h"

#define instr push

static void do_execute () {

	if (DATA_BYTE == 2){
		cpu.esp -= 2;
		MEM_W(cpu.esp, op_src->val);
	} else{
		cpu.esp -= 4;
		MEM_W(cpu.esp, op_src->val);
	}

	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif
make_instr_helper(i)

#include "cpu/exec/template-end.h"
