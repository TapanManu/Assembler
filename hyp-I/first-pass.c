// develop an assembler hypothetiocal
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int locctr =0,len=0,equ=0;
char line[255];

char optab[][15] = {"ADD","NEG","LOAD","STORE","JE","JL","JG","HLT"};

void clear_file(char file[]){
    FILE *out;
    out = fopen(file,"w");
    fputs("",out);
    fclose(out);
}

void write_inter(int locctr,char lin[]){
    FILE *out;
    out = fopen("int.asm","a");
    fprintf(out,"%d\t%s",locctr,lin);
    fclose(out);
}

void ins_symtab(char label[]){
    FILE *out;
    out = fopen("symtab.txt","a");
    fprintf(out,"%s\t%d\n",label,locctr);
    fclose(out);
}

void ins_sym(char label[],int oper){
    FILE *out;
    out = fopen("symtab.txt","a");
    fprintf(out,"%s\t%d\n",label,oper);
    fclose(out);
}

int ret_opval(char *val){
    for(int i=0;i<7;i++){
        if(!strcmp(optab[i],val)){
            return i;
        }
    }
    return -1;
}

int get_intval(char operand[]){
    int len = strlen(operand);
    int val = 0;
    for(int i=0;i<len;i++){
        int op = operand[i]-'0';
        printf("%d",op);
        val += op*(int)(pow(10,len-i-1));
    }
    //printf("%d",val);
    return val;
}

void first_pass(char label[],char opcode[], char operand[]){
    if(!strcmp(opcode,"START")){
        locctr = get_intval(operand);
        printf("%d",locctr);
    }
    else if(!strcmp(opcode,"DB")){
        len=1;
        
    }
    else if(!strcmp(opcode,"EQU")){
        len = 0;
        equ = 1;
    }
    else if(!strcmp(opcode,"END")){
        return;
    }
    else if(!strcmp(opcode,"HLT")){
        len=1;
       
    }
    else if(ret_opval(opcode)!=-1){
        if(!strcmp(opcode,"HLT") || !strcmp(opcode,"ADD") || ! strcmp(opcode,"NEG")){
            len=1;
            
        }
        else{
            len=2;
            
        }
        
    }
    else{
        printf("unidentified symbol error");
    }
}


int main(int argc, char* argv[]){
    FILE *fp;
    char label[20],opcode[20],operand[20];
    fp = fopen("src.asm","r");
    
    clear_file("symtab.txt");
    clear_file("int.asm");
    
    while(fgets(line,255,fp)){
        label[0]=0;opcode[0]=0;operand[0]=0;
        sscanf(line,"%s\t%s\t%s",label,opcode,operand);
        equ=0;
        if(operand[0]==0){
            if(strcmp(opcode,"HLT")){
                strcpy(operand,opcode);
                strcpy(opcode,label);
                strcpy(label,"");
            }
        }
        printf("label:%s\topcode:%s\toperand:%s\n",label,opcode,operand);
        first_pass(label,opcode,operand);
        if(label[0]!=0)
            if(equ==0)
                ins_symtab(label);
            else
                ins_sym(label,get_intval(operand));
        write_inter(locctr,line);                    
        locctr+=len;
        
    }
    fclose(fp);
    return 0;
}
