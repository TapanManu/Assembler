#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int locctr = 0;
int len = 0;
int start = 0;
int equ = 0;

char OPTAB[][15] = {"ADD","SUB","MUL","DIV","JZ","JNZ","JGTZ","JLTZ","CALL","RET","LD","ST","LDI","PUSH","POP","HLT"};

void write_inter(char line[]){
    FILE *out;
    out = fopen("inter.asm","a");
    fprintf(out,"%02x\t%s",locctr,line);
    fclose(out);
}

void ins_symtab(char label[],int loc){
    FILE *out;
    out = fopen("symtab.txt","a");
    fprintf(out,"%s\t%02x\n",label,loc);
    fclose(out);
}

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

int get_operval(char operand[]){
    int len = strlen(operand);
    int val=0;
    for(int i=0;i<len;i++){
        val += (operand[i]-'0') * pow(10,len-i-1);
    }
    return val;
}

void first_pass(char label[],char opcode[],char operand[]){
    int indx=-1;
    if(!strcmp(opcode,"START")){
        locctr = get_operval(operand);
        start = locctr;
    }
    else if(!strcmp(opcode,"EQU")){
        len = 0;
        equ = 1;
    }
    else if(!strcmp(opcode,"DB")){
        len = 1;
    }
    else if(!strcmp(opcode,"END")){
        return;
    }
    else if((indx = ret_opval(opcode))!=-1){
        //printf("indx:%d\n",indx);
        switch(indx){
            case 9:
            case 13:
            case 14:
            case 15:
                len = 1;
                break;
            default: len = 2;
        }
    }
    else{
        printf("unidentified symbol error");
        exit(1);
    }

}

int main(int argc, char* argv[]){
    FILE *fp;
    fp  = fopen("in.asm","r");
    clear_file("inter.asm");
    clear_file("symtab.txt");
    char line[255];
    char label[20],opcode[20],operand[20];
    while(fgets(line,255,fp)){
        equ = 0;
        label[0]=0; opcode[0]=0; operand[0]=0;
        sscanf(line,"%s\t%s\t%s",label,opcode,operand);
        if(!strcmp(opcode,"HLT")){
            ;
        }
        else if(operand[0]==0 && label[0]!=0){
            strcpy(operand,opcode);
            strcpy(opcode,label);
            strcpy(label,"");
        }    
        printf("label:%s\topcode:%s\toperand:%s\n",label,opcode,operand);
        first_pass(label,opcode,operand);
        if(label[0]!=0){
            if(equ == 1){           //if opcode is EQU
                ins_symtab(label,get_operval(operand));
            }
            else{
                ins_symtab(label,locctr);
            }
        }
        write_inter(line);
        locctr+=len;
    }
    int pgmlen = locctr - start - len;
    ins_symtab("PGMLEN",pgmlen);
    fclose(fp);
}