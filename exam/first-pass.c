#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char OPTAB[][6] = {"ADD","SUB","MUL","DIV","JZ","JNZ","JGTZ","JLTZ","CALL","RET","LD","ST","LDI","PUSH","POP","HLT"};
int equ = 0;
int locctr=0;
int start = 0;
int len = 0;


void write_inter(char line[]){
    FILE *out;
    out = fopen("inter.asm","a");
    fprintf(out,"%03x\t%s",locctr,line);
    fclose(out);
}

void ins_symtab(char label[],int loc,int flag){
    FILE *out;
    out = fopen("symtab.txt","a");
    //flag = 1 absolute else relative
    fprintf(out,"%s\t%03x\t%d\n",label,loc,flag);
    fclose(out);
}

void clear_file(char file[]){
    FILE *out;
    out = fopen(file,"w");
    fputs("",out);
    fclose(out);
}

int readoptab(char opc[]){
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


void firstpass(char label[],char opc[],char operand[]){
    int indx = -1;
    if(!strcmp(opc,"START")){
        locctr = get_operval(operand);
        start = get_operval(operand);
    }
    else if(!strcmp(opc,"EQU")){
        len = 0;
        equ = 1;
    }
    else if(!strcmp(opc,"DB")){
        len = 1;
    }
    else if(!strcmp(opc,"END")){
        return;
    }
    else if((indx = readoptab(opc))!=-1){
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
        printf("unidentified symbol error\n");
        exit(1);
    }

}


int main(){
    FILE *fp;
    fp = fopen("in.asm","r");
    char line[255];
    int skip =0;
    char label[20],opcode[20],operand[20],comment[20];
    clear_file("symtab.txt");
    clear_file("inter.asm");
    while(fgets(line,255,fp)){
        skip = 0;
        label[0]=0; opcode[0]=0; operand[0] = 0; comment[0] = 0;
        equ = 0;
        sscanf(line,"%s\t%s\t%s\t%s",label,opcode,operand,comment);
        if(label[0]==';'){
            skip  = 1;
        }
        else if(!strcmp(opcode,"HLT")){
            ;
        }
        else if(comment[0]==0 && operand[0]==';' && label[0]!=0){
            strcpy(comment,operand);
            strcpy(operand,opcode);
            strcpy(opcode,label);
            strcpy(label,"");
        }
        else if((operand[0]==0 || operand[0]==';') && label[0]!=0){
            strcpy(operand,opcode);
            strcpy(opcode,label);
            strcpy(label,"");
        }
        if(!strcmp(label,"")){
            skip = 1;
        }
        printf("label:%s\topcode:%s\toperand:%s\tcomment:%s\n",label,opcode,operand,comment);
        if(label[0]!=';')
            firstpass(label,opcode,operand);
        if(skip == 0){
            
            if(equ==1){
                ins_symtab(label,get_operval(operand),1);
            }
            else{
                ins_symtab(label,locctr,0);
            }
        }
        write_inter(line);
        locctr+=len;
    }
    int PGMLEN = locctr - start - len;
    ins_symtab("PGMLEN",PGMLEN,1);
    fclose(fp);
    return 0;
}