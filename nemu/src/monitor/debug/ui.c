#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}



static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_help(char *args);
static int cmd_p(char *args);
static int cmd_x(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "single step si for 1 step and si n for n step", cmd_si },
	{ "info", "info r print register information, info w print watchpoint information", cmd_info },
	{ "p", "evaluation of expression, just like:p $eax+1", cmd_p},
	{ "x", "scan memory, x N EXPR", cmd_x },
	{ "w", "set watchpoint just like: w *0x2000", cmd_w },
	{ "d", "delete watchpoint just like: d N", cmd_d }
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_si(char *args){
	int number = 0;
	if(NULL == args)
		cpu_exec(1);
	else{
		number = strtol(args, NULL, 10);	
		if(number > 0)
			cpu_exec(number);
		else
			printf("parameter error!\n si for 1 step and si n for n step\n");
	}
	return 0;
}

static int cmd_info(char *args) 
	{
	/* extract the first argument */

	if(args == NULL) 
		{
		/* no argument given */
		printf("Unknown command '%s'\n",args );	
	    return 0;
		}
	
	else 
		{
		if(args[0]=='r') 
			{
	    printf(" eax=0x%x\n ",cpu.eax);
		printf("ecx=0x%x\n ",cpu.ecx);
		printf("edx=0x%x\n ",cpu.edx);
		printf("ebx=0x%x\n ",cpu.ebx);
		printf("esp=0x%x\n ",cpu.esp);
		printf("ebp=0x%x\n ",cpu.ebp);
		printf("esi=0x%x\n ",cpu.esi);
		printf("edi=0x%x\n ",cpu.edi);
		printf("eip=0x%x\n ",cpu.eip);
		printf("ax=0x%x\n ",(cpu.eax&0x0000ffff));
		printf("cx=0x%x\n ",(cpu.ecx&0x0000ffff));
		printf("dx=0x%x\n ",(cpu.edx&0x0000ffff));
		printf("bx=0x%x\n ",(cpu.ebx&0x0000ffff));
		printf("sp=0x%x\n ",(cpu.esp&0x0000ffff));
		printf("bp=0x%x\n ",(cpu.ebp&0x0000ffff));
		printf("si=0x%x\n ",(cpu.esi&0x0000ffff));
		printf("di=0x%x\n ",(cpu.edi&0x0000ffff));
		return 0;
			}
		if(args[0]=='w')
			{
              for(free_=head;free_->address!=0;free_=free_->next)
				printf("%d,%08x:%08x\n",free_->NO,free_->address,free_->value);
			  return 0;
			
			}
			
		}
		printf("Unknown command '%s'\n",args );
	
	return 0;
     }


static int cmd_p(char *args){
	bool success;
	int result;
	if(NULL==args){
		printf("p EXPR,just like:p 2+4");
		return 0;
	}
	result = expr(args,&success);
	if(false==success)
		printf("Expression is wrong");
	else
		printf("%x\n",result);
	return 0;
}

static int cmd_x(char *args){
	char *csize = strtok(args, " ");
	char *caddr = csize+strlen(csize)+1;
	int size = atoi(csize);
	int addr;
	int i;
	bool success;
	if(NULL == csize||NULL == caddr)
		printf("x N EXPR,just like: x 10 $eax");
	else{
		printf("%s\n",caddr);
		addr = expr(caddr,&success);
		if(false==success)
			printf("Expression is wrong\n");
		else
			for(i=0;i<size;i++)
			printf("0x%08x\t0x%08x\n",addr+i*4,swaddr_read(addr+i*4,4));
	}
	return 0;
}

static int cmd_w(char *args){
	bool success;
	int addr;
	if(NULL==args){
		printf("w EXPR,just like w 5+2");
		return 0;
	}
	addr = expr(args,&success);
	if(false==success)
		printf("Expression is wrong");
	else{
		free_=head;
		while(free_->address!=0)free_=free_->next;
		free_->address=addr;
		free_->value=swaddr_read(addr,4);
		printf("watchpoint set successfully!\n");
	}	
	return 0;
}

static int cmd_d(char *args){
	int n;
	if(NULL==args){
		printf("d N, just like 2");
		return 0;
	}
	n = atoi(args);
	for(free_=head;free_->NO != n;free_=free_->next)
		;
	while(free_->next!=NULL){
		free_->address=free_->next->address;
		free_->value=free_->next->value;
		free_=free_->next;
	}
	printf("watchpoint deleted successfully!\n");
	return 0;
}

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
