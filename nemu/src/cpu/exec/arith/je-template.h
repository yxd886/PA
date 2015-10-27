#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	//DATA_TYPE result = ~op_src->val;
	//OPERAND_W(op_src, result);
	//cpu.esp -= 4;
	//MEM_W(cpu.esp,cpu.eip);
	//cpu.eip +=(int32_t)op_src->val;
	//#if DATA_BYTE==2
	  //   cpu.eip &= 0x0000ffff;
	//#endif
	if(cpu.ZF==1)
		{
		   #if DATA_BYTE==1
        uint8_t val= (uint8_t)(cpu.eip&0xFF);
        val+=(uint8_t)op_src->val;
        cpu.eip=(cpu.eip&0xFFFFFF00)+val;
    #elif   DATA_BYTE==2
        uint16_t val= (uint16_t)(cpu.eip&0xFFFF);
        val+=(uint16_t)op_src->val;
        cpu.eip=(cpu.eip&0xFFFF0000)+val;
    #else

        cpu.eip=cpu.eip+op_src->val;
    #endif // DATA_BYTE
		  
		}
	print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
