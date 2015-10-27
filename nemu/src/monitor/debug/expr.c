 #include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum {
	NOTYPE = 256, EQ,UEQ,AND,OR,HEX,REG,NO,LEFT,RIGHT,NEG,DEREF,VAR

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"==", EQ},						// equal
	{"-",'-'},
    	{"\\*",'*'},
    	{"!=",UEQ},
    	{"&&",AND},
    	{"\\|\\|",OR},
    	{"!",'!'},
    	{"=",'='},
    	{"^0x[0-9a-fA-F]+",HEX},
   	{"\\$[a-z]{2,3}",REG},
   	{"/",'/'},
   	{"[0-9]+",NO},//certaintype 用于区分×是指针还是称号，或者-是负号还是减号,表示number
   	{"\\(",LEFT},
   	{"\\)",RIGHT},//grpupp
	{"[a-z|0-9|_][a-z|0-9|A-Z|_]+",VAR}
   	
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret != 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;
uint32_t  eval(int p,int q);
bool check_parentheses(int p,int q);
uint32_t find_op(int p,int q);
int judge_yxj(int type,int *yxj,int *pos,int i);


Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
    	int i;
	int flag;

    regmatch_t pmatch;

    nr_token = 0;
	

    while(e[position] != '\0')
    {
        /* Try all rules one by one. */
        for(i = 0,flag=0; i < NR_REGEX; i ++)
        {
            if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
            {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

                tokens[nr_token].type=rules[i].token_type;
                 while(flag<substr_len){
                    tokens[nr_token].str[flag++]=e[position++];
		  
                }
		tokens[nr_token].str[flag]='\0';
               



                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array ``tokens''. For certain
                 * types of tokens, some extra actions should be performed.
                 */

                switch(tokens[nr_token].type)
                {
                case '-':
                    if(nr_token==0||(tokens[nr_token-1].type!=NO&&tokens[nr_token-1].type!=RIGHT)){
                        tokens[nr_token].type=NEG;
			printf("\nthe type has been changed NEG\n");
                          }
			break;
                case '*':
                    if(nr_token==0||(tokens[nr_token-1].type!=NO&&tokens[nr_token-1].type!=RIGHT)){
                        tokens[nr_token].type=DEREF;
			printf("\nthe type has been changed DEREF\n");
		}
			break;
		case NOTYPE:
			nr_token--;
                
                }
		nr_token++;
                

                break;
            }

        }
        
        if(nr_token==32)
        {
            Log("the expression is beyond  the length");
            return false;
        }

        if(i == NR_REGEX)
        {
	    
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }
 

    return true;
}



uint32_t expr(char *e, bool *success) {
	 if(!make_token(e))
    {
        *success = false;
        return 0;
    }else{
	
}
    uint32_t num;
    int p=0,q=nr_token-1;  //nr_token表示分词的个数
    num=eval(p,q);
	*success = true;
    return num;
}
uint32_t  eval(int p,int q)
{
    
    uint32_t op;
    uint32_t val1=0;
    uint32_t val2=0;
    if(p>q)
    {
        printf("the bad expression\n");
        assert(0);

    }
    else if(p==q)
    {
        char *endptr;
        if(tokens[p].type==HEX){
	return  strtol(tokens[p].str,&endptr,16);
         }

        if(tokens[p].type==NO)
            return  atoi(tokens[p].str);
        if(tokens[p].type==REG)
        {
            if(!strcmp(tokens[p].str+1,"eax"))
                return cpu.eax;
            if(!strcmp(tokens[p].str+1,"ecx"))
                return cpu.ecx;
            if(!strcmp(tokens[p].str+1,"edx"))
                return cpu.edx;
            if(!strcmp(tokens[p].str+1,"ebx"))
                return cpu.ebx;
            if(!strcmp(tokens[p].str+1,"esp"))
                return cpu.esp;
            if(!strcmp(tokens[p].str+1,"ebp"))
                return cpu.ebp;
            if(!strcmp(tokens[p].str+1,"esi"))
                return cpu.esi;
            if(!strcmp(tokens[p].str+1,"edi"))
                return cpu.edi;
	   if(!strcmp(tokens[p].str+1,"eip"))
		return cpu.eip;
        }

    }
    else if(check_parentheses(p,q)==true)   /*implement check_parentheses*/
    {
	
        return eval(p+1,q-1);
    }
    else
    {
	
        op= find_op(p,q);    /*implement find_op(p,q)*/
	
        if(tokens[op].type==NEG||tokens[op].type==DEREF||tokens[op].type=='!')
        {
            val2=eval(op+1,q);}else{

        val1=eval(p,op-1);
        val2=eval(op+1,q);}

        switch(tokens[op].type)
        {
        case '+':
            return val1+val2;
		
        case '-':
            return val1-val2;
        case '/':
            return val1/val2;
		
        case '*':
            return val1*val2;
        case OR:
            return val1||val2;
        case AND:
            return val1&&val2;
        case EQ:
            return val1==val2;
        case '=':
            return val1=val2;
        case UEQ:
            return val1!=val2;
        case '!':
            return !val2;
        case NEG:
            return -val2;
        case DEREF:
            return swaddr_read(val2,4);
        default:
           printf("不合法的表达式,在switch case语句中");
		return 0;
        }



    }
	return 0;

}
bool check_parentheses(int p,int q)
{
    if(tokens[p].type!=LEFT||tokens[q].type!=RIGHT)
    {
        return false;
    }
    char s[32];
    int i,flag=0;
    for(i=p+1; i<q; i++)
    {
        if(tokens[i].type==LEFT)
            s[flag++]='(';
        if(tokens[i].type==RIGHT&&s[flag-1]=='(')
        {
            flag=flag-1;
        }
    }
    if(flag>0)
        return false;
    else
        return true;
    return true;
}

uint32_t find_op(int p,int q)
{
    //int fh[32];//运算符
    int yxj=8; //优先级有七级
    //结合性 0表示左结合，1表示右结合
    int pos=p;
    int i,j;
   
    for(i=p; i<q; i++)
    {
        if(tokens[i].type==LEFT)
        {
            j=i+1;
              while(check_parentheses(i,j)!=true)
                  j++;
            i=j+1;
        }
        judge_yxj(tokens[i].type,&yxj,&pos,i);
       



    }
    return pos;

}
int judge_yxj(int type,int *yxj,int *pos,int i)
{
    if((type==NEG||type=='!'||type==DEREF)&&*yxj>7)
    {
        *yxj=7;
        *pos=i;
    }
    else if((type=='*'||type=='/')&&*yxj>=6)
    {
        *yxj=6;
        *pos=i;
    }
    else if((type=='+'||type=='-')&&*yxj>=5)
    {
        *yxj=5;
        *pos=i;
    }
    else if((type==EQ||type==UEQ)&&*yxj>=4)
    {
        *yxj=4;
        *pos=i;
    }
    else if(type==AND&&*yxj>=3)
    {
        *yxj=3;
        *pos=i;
    }
    else if(type==OR&&*yxj>=2)
    {
        *yxj=2;
        *pos=i;
    }
    else if(type=='='&&*yxj>=1)
    {
        *yxj=1;
        *pos=i;
    }
    else return 0;

    return 1;


}




