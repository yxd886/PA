#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	//cpu.esp -= 4;
	//MEM_W(cpu.esp,cpu.eip);
	//cpu.eip +=(int32_t)op_src->val;
	//#if DATA_BYTE==2
	///     cpu.eip &= 0x0000ffff;
	//#endif
	if(op_src->type == OP_TYPE_IMM )
		{
		 cpu.eip += op_src->val;
		 #if DATA_BYTE == 2
		 cpu.eip &= 0x0000ffff;
		 #endif
		}
	if(op_src->type == OP_TYPE_REG||OP_TYPE_MEM )
		{
		#if DATA_BYTE == 2
		 cpu.eip = op_src->val&0x0000ffff;
		#endif
		#if DATA_BYTE == 4
		 cpu.eip = op_src->val;
		#endif
		
		}
	
	print_asm_template1();
}

make_instr_helper(rm)
make_instr_helper(i)

#include "cpu/exec/template-end.h"
