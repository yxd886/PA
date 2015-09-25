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

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);




static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Single Step", cmd_si },
	{ "info", "Print the State of the Program ", cmd_info },
	{ "x", "Scan memory ", cmd_x },

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

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
static int cmd_si(char *args) 
	{
	/* extract the first argument */
	char* arg = strtok(NULL, " ");

	if(arg == NULL) 
		{
		/* no argument given */
		cpu_exec(1);
	    return 0;
		}
	
	else 
		{
		cpu_exec(atoi(arg));
	    return 0;
			
		}
		printf("Unknown command '%s'\n",args );
	
	return 0;
     }

static int cmd_info(char *args) 
	{
	/* extract the first argument */
	char* arg = strtok(NULL, " ");

	if(arg == NULL) 
		{
		/* no argument given */
		printf("Unknown command '%s'\n",args );	
	    return 0;
		}
	
	else 
		{
		if(arg[0]=='r') 
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
		printf("Unknown command '%s'\n",args );
	
	return 0;
     }
static int cmd_x(char *args)
{
    int i;
	int j;
	char* arg1 = strtok(NULL, " ");
	char* arg2 = strtok(NULL, " ");
	
		if(arg1 == NULL|| arg2==NULL) 
			{
			/* no argument given */
			printf("Unknown command '%s'\n",args ); 
			return 0;
			}
		else
			{
			 i=strtol(arg2,NULL,16);
			 for(j=0;j<atoi(arg1);j++)
			 	{

				 printf("%d\n",swaddr_read(i,4));
				 i=i+4;
				 	
			 	}
			 	
			return 0;
			}




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
