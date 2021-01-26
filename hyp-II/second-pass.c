#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int locctr = 0;
int len = 0;
int start = 0;
int equ = 0;
char OBJ[20];

char OPTAB[][5] = {"ADD","SUB","MUL","DIV","JZ","JNZ","JGTZ","JLTZ","CALL","RET","LD","ST","LDI","PUSH","POP","HLT"};
char REGTAB[][5] = {"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14","R15"};
// INSTRUCTION FORMAT

//ADD,SUB,MUL,DIV INSTRUCTION FORMAT 
// --> opcode (4) reg1(4) reg2(4)  reg3(4)  [bracket indicate bits]

// JZ JNZ JGTZ JLTZ
// --> opcode(4) reg(4) loc(8)

// CALL 
// --> opcode(4) zeros(4) loc(8)

// RET HLT
// --> opcode(4) zeros(4)

//LD ST LDI
// --> opcode(4) reg(4) loc(8)

//PUSH POP
//--> opcode(4)  reg(4)

void clear_file(char file[]){
    FILE *out;
    out = fopen(file,"w");
    fputs("",out);
    fclose(out);
}

int ret_opval(char opc[]){
    for(int i=0;i<16;i++){
        if(!strcmp(OPTAB[i],opc)){
            return i;
        }
    }
    return -1;
}

int ret_regval(char reg[]){
    for(int i=0;i<16;i++){
        if(!strcmp(REGTAB[i],reg)){
            return i;
        }
    }
    return -1;
}

int ret_symbol(char lab[]){
    char lin[255],sym[20];
    int val=0;
    FILE * in;
    in = fopen("symtab.txt","r");
    while(fgets(lin,255,in)){
        sscanf(lin,"%s\t%x",sym,&val);
        if(!strcmp(sym,lab)){
            return val;
        }
    }
    fclose(in);
    return -1;
}

int get_operval(char operand[]){
    int len = strlen(operand);
    int val=0;
    for(int i=0;i<len;i++){
        val += (operand[i]-'0') * pow(10,len-i-1);
    }
    return val;
}

void process_op1(char opcode[],char operand[]){
    int opc = ret_opval(opcode); // in decimal
    char oper[3][5];
    int k=0;
    int j=0;
    for(int i=0;operand[i]!='\0';i++){
        if(operand[i]==','){
            oper[k][j]='\0';
            k++;
            j = 0;
        }
        else{
            oper[k][j++] = operand[i];
        }
    }
    oper[k][j]='\0';
    printf("OBJ:%x%x%x%x\n",opc,ret_regval(oper[0]),ret_regval(oper[1]),ret_regval(oper[2]));
}

void process_op2(int loc,char opcode[],char operand[]){
    int opc = ret_opval(opcode); // in decimal
    char oper[2][25];
    int k=0;
    int j=0;
    for(int i=0;operand[i]!='\0';i++){
        if(operand[i]==','){
            oper[k][j]='\0';
            k++;
            j = 0;
        }
        else{
            oper[k][j++] = operand[i];
        }
    }
    oper[k][j]='\0';
    int x = ret_symbol(oper[1]);
    if(x <  (loc+2)){
        x = 0xff - ((loc+2)-x);
    }
    else{
        x = x - (loc+2);
    }
    printf("OBJ:%x%x%02x\n",opc,ret_regval(oper[0]),x);
}

void process_op3(int loc,char opcode[],char operand[]){
    int opc = ret_opval(opcode);
    printf("OBJ:%x0%02x\n",opc,ret_symbol(operand) - (loc+2));
}

void process_op4(char opcode[],char operand[]){
    int opc = ret_opval(opcode); // in decimal
    printf("OBJ:%x0\n",opc);
}

void process_op5(int loc,char opcode[],char operand[]){
    int opc = ret_opval(opcode); // in decimal
    char oper[2][25];
    int k=0;
    int j=0;
    for(int i=0;operand[i]!='\0';i++){
        if(operand[i]==','){
            oper[k][j]='\0';
            k++;
            j = 0;
        }
        else{
            oper[k][j++] = operand[i];
        }
    }
    oper[k][j]='\0';
    int x = ret_symbol(oper[1]);
    if(x <  (loc+2)){
        x = 0xff - ((loc+2)-x);
    }
    else{
        x = x - (loc+2);
    }
    printf("OBJ:%x%x%02x\n",opc,ret_regval(oper[0]),x);
}

void process_op6(char opcode[],char operand[]){
    int opc = ret_opval(opcode);    
    printf("OBJ:%x%x\n",opc,ret_regval(operand));
}

void second_pass(int loc,char label[],char opcode[],char operand[]){
    int indx=-1;
    if(!strcmp(opcode,"START")){
        
    }
    else if(!strcmp(opcode,"EQU")){
        return;
    }
    else if(!strcmp(opcode,"DB")){
        printf("OBJ:%02x\n",get_operval(operand));
    }
    else if(!strcmp(opcode,"END")){
        return;
    }
    else if((indx = ret_opval(opcode))!=-1){
        //printf("indx:%d\n",indx);
        switch(indx){
            case 0:
            case 1:
            case 2:
            case 3:
                process_op1(opcode,operand);
                break;
            case 4:
            case 5:
            case 6:
            case 7:
                process_op2(loc,opcode,operand);
                break;
            case 8:
                process_op3(loc,opcode,operand);
                break;
            case 9:
            case 15:
                process_op4(opcode,operand);
                break;
            case 10:
            case 11:
            case 12:
                process_op5(loc,opcode,operand);
                break;
            case 13:
            case 14:
                process_op6(opcode,operand);
                break;
            default: printf("error\n");
        }
    }
    else{
        printf("unidentified symbol error");
        exit(1);
    }

}

int main(int argc, char* argv[]){
    FILE *fp;
    int pgmlen = ret_symbol("PGMLEN");
    fp  = fopen("inter.asm","r");
    clear_file("out.obj");
    char line[255];
    char label[20],opcode[20],operand[20];
    int loc = 0;
    while(fgets(line,255,fp)){
        loc = 0;
        label[0]=0; opcode[0]=0; operand[0]=0;
        sscanf(line,"%x\t%s\t%s\t%s",&loc,label,opcode,operand);
        if(!strcmp(opcode,"HLT")){
            ;
        }
        else if(operand[0]==0 && label[0]!=0){
            strcpy(operand,opcode);
            strcpy(opcode,label);
            strcpy(label,"");
        }    
       // printf("locctr:%d\tlabel:%s\topcode:%s\toperand:%s\n",loc,label,opcode,operand);
        second_pass(loc,label,opcode,operand);
        
    }
    
    fclose(fp);
}