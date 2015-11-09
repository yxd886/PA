#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute () {

	if (DATA_BYTE == 2){
		REG(op_src->reg) = MEM_R(cpu.esp);
		cpu.esp += 2;
	} else{
		REG(op_src->reg) = MEM_R(cpu.esp);
		cpu.esp += 4;
	}
	
	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
