#include "cpu/exec/template-start.h"

#define instr call

static void do_execute () {

	if (DATA_BYTE == 2){
		cpu.esp -= 2;
		MEM_W(cpu.esp, (uint16_t)(cpu.eip & 0x0000ffff));
		cpu.eip = (cpu.eip + op_src->val) & 0x0000ffff;
	} else if (DATA_BYTE == 4){
		cpu.esp -= 4;
		MEM_W(cpu.esp, cpu.eip);
		cpu.eip = cpu.eip + op_src->val;
	}

	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(i)
#endif

#include "cpu/exec/template-end.h"
