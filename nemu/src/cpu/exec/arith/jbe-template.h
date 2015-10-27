#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	//DATA_TYPE result = ~op_src->val;
	//OPERAND_W(op_src, result);
	//cpu.esp -= 4;
	//MEM_W(cpu.esp,cpu.eip);
	//cpu.eip +=(int32_t)op_src->val;
	//#if DATA_BYTE==2
	  //   cpu.eip &= 0x0000ffff;
	//#endif
	if(cpu.ZF==1||cpu.CF==1)
		{
		  cpu.eip +=(int32_t)op_src->val;
		  #if DATA_BYTE == 2
		     cpu.eip &= 0x0000ffff;
		  #endif
		  
		}
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
