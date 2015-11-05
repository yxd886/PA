#include "cpu/exec/template-start.h"

#define instr or

static void do_execute () {
	DATA_TYPE result = op_dest->val | op_src->val;
	OPERAND_W(op_dest, result);

	/* TODO: Update EFLAGS. */
	//panic("please implement me");
	int PF_flag=0;
	cpu.CF=0;
	cpu.OF=0;
	if (result == 0)
		 cpu.ZF=1;
	else cpu.ZF=0;
	
	cpu.SF=MSB(result);
	if(result&0x01)
        PF_flag++;
    if(result&0x02)
        PF_flag++;
    if(result&0x04)
        PF_flag++;
    if(result&0x08)
        PF_flag++;
    if(result&0x10)
        PF_flag++;
    if(result&0x20)
        PF_flag++;
    if(result&0x40)
        PF_flag++;
    if(result&0x80)
        PF_flag++;
    if(PF_flag%2==0)
       cpu.PF=1;
       else cpu.PF=0;

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
