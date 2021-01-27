#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

char OPTAB[][6] = {"ADD","SUB","MUL","DIV","JZ","JNZ","JGTZ","JLTZ","CALL","RET","LD","ST","LDI","PUSH","POP","HLT"};
char REGTAB[][6] = {"R0","R1","R2","R3","R4","R5","R6","R7","R8","R9","R10","R11","R12","R13","R14","R15"};
int equ = 0;
int locctr=0;
int start = 0;
int len = 0;
int pgmlen =0;
char OBJ[20];


void write_inter(char line[]){
    FILE *out;
    out = fopen("inter.asm","a");
    fprintf(out,"%03x\t%s",locctr,line);
    fclose(out);
}

int read_symtab(char operand[]){
   FILE *in;
   in = fopen("symtab.txt","r");
   char l[255];
   char sym[20];
   int hexval=0;
   int flag = 0;
   while(fgets(l,255,in)){
       sscanf(l,"%s\t%x\t%d",sym,&hexval,&flag);
       while(!strcmp(sym,operand)){
           return hexval;
       }
   }
   return -1;
}

int get_flag(char operand[]){
   FILE *in;
   in = fopen("symtab.txt","r");
   char l[255];
   char sym[20];
   int hexval=0;
   int flag = 0;
   while(fgets(l,255,in)){
       sscanf(l,"%s\t%x\t%d",sym,&hexval,&flag);
       while(!strcmp(sym,operand)){
           return flag;
       }
   }
   return -1;
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

int readregtab(char reg[]){
    for(int i=0;i<16;i++){
        if(!strcmp(REGTAB[i],reg)){
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

void format1(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    snprintf(OBJ,sizeof(OBJ),"%x0",op);
    printf("obj:%s\n",OBJ);
}

void format2(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    snprintf(OBJ,sizeof(OBJ),"%x%x",op,readregtab(oper));
    printf("obj:%s\n",OBJ);
}

void format3(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    int len = strlen(oper);
    char OPERS[3][10];
    int k=0;
    int j=0;
    for(int i=0;oper[i]!='\0';i++){
        if(oper[i]==','){
            OPERS[k][j] = '\0';
            k++;
            j=0;
        }
        else{
            OPERS[k][j] = oper[i];
            j++;
        }
    }
    OPERS[k][j]='\0';

    snprintf(OBJ,sizeof(OBJ),"%x%x%x%x",op,readregtab(OPERS[0]),readregtab(OPERS[1]),readregtab(OPERS[2]));
    printf("obj:%s\n",OBJ);
}

void format4(int loc,char opc[],char oper[]){
    
}

void format5(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    snprintf(OBJ,sizeof(OBJ),"%x%03x",op,read_symtab(oper));
    printf("obj:%s\n",OBJ);
}
void secondpass(int loc,char label[],char opc[],char operand[]){
    int indx = -1;
    if(!strcmp(opc,"START")){
        //initialise header record
    }
    else if(!strcmp(opc,"EQU")){
        return;
        //skip
    }
    else if(!strcmp(opc,"DB")){
        return;
    }
    else if(!strcmp(opc,"END")){
        //get end record
        
        return;
    }
    else if((indx = readoptab(opc))!=-1){
        switch(indx){
            case 9:
            case 15:
                format1(loc,opc,operand);
                break;
            case 13:
            case 14:
                format2(loc,opc,operand);
                break;
            case 0:
            case 1:
            case 2:
            case 3:
                format3(loc,opc,operand);
                break;
            case 8:
                format5(loc,opc,operand);
                break;
            default: format4(loc,opc,operand);
        }
    }
    else{
        printf("unidentified symbol error\n");
        exit(1);
    }

}


int main(){
    FILE *fp;
    fp = fopen("inter.asm","r");
    char line[255];
    int skip =0;
    pgmlen = read_symtab("PGMLEN");
    int loc = 0;
    char label[20],opcode[20],operand[20],comment[20];
    clear_file("out.obj");
    while(fgets(line,255,fp)){
        skip = 0;
        label[0]=0; opcode[0]=0; operand[0] = 0; comment[0] = 0;
        equ = 0;
        loc = 0;
        sscanf(line,"%x\t%s\t%s\t%s\t%s",&loc,label,opcode,operand,comment);
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
        printf("loc:%d\tlabel:%s\topcode:%s\toperand:%s\tcomment:%s\n",loc,label,opcode,operand,comment);
        if(label[0]!=';')
            secondpass(loc,label,opcode,operand);
        
    }
    printf("%d",pgmlen);
    fclose(fp);
    return 0;
}