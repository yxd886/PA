#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"

make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(leave){
	cpu.esp = cpu.ebp;
	reg_l(R_EBP) = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	print_asm("leave");
	return 1;
}

make_helper(ret){
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += 4;
	//int len = decode_i_l(cpu.eip);
	print_asm("ret");
	//return 1 + len;
	return 0;
}

make_helper(ret_i_w){
	cpu.eip = swaddr_read(cpu.esp, 4);
	cpu.esp += (4 + op_src->val);

	//int len = decode_i_l(cpu.eip);
	print_asm("reti");
	//return 1 + len;
	return 0;
}

make_helper(cld)
{
	cpu.DF = 0;
	print_asm("cld");
	return 1;
}

make_helper(clc)
{
	cpu.CF = 0;
	print_asm("clc");
	return 1;
}

make_helper(cwd)
{
	if(!ops_decoded.is_data_size_16)
	{
		if((cpu.eax>>31) == 1)
			cpu.edx = 0xffffffff;
		else
			cpu.edx = 0x00000000;
	}
	else
	{
		if((cpu.gpr[0]._16 >> 15) == 1)
			cpu.gpr[2]._16 = 0xffff;
		else
			cpu.gpr[2]._16 = 0x0000;
	}
	print_asm("cwd");
	return 1;
}

/*
make_helper(cltd){
	if(cpu.eax >> 31)
		cpu.edx = 0x00000000;
	else
		cpu.eax = 0xffffffff;

	print_asm("cltd");
	return 1;
}
*/
/*
make_helper(ctd_v){
	if(ops_decoded.is_data_size_16){
		if( (reg_w(R_AX) & 0x8000 ) == 0) reg_w(R_DX) = 0x0;
		else reg_w(R_DX) = 0xffff;
	}else{
		if( (reg_l(R_EAX) & 0x80000000 ) == 0) reg_l(R_EDX) = 0x0;
		else reg_l(R_EDX) = 0xffffffff;
	}
	return 1;
}
*/