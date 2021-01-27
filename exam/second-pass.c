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
char *TEXT;
int textlen=0;

void write_head(){
    FILE *out;
    out = fopen("out.obj","a");
    //max address space is 4k, hence start is of only 3 digits
    snprintf(OBJ,sizeof(OBJ),"H^%03x^%06x\n",start,pgmlen);
    fputs(OBJ,out);
    fclose(out);
}

void write_end(){
    FILE *out;
    out = fopen("out.obj","a");
    //max address space is 4k, hence start is of only 3 digits
    snprintf(OBJ,sizeof(OBJ),"E^%03x\n",start);
    fputs(OBJ,out);
    fclose(out);
}

void init_text(int loc){
    FILE *out;
    out = fopen("out.obj","a");
    //max address space is 4k, hence start is of only 3 digits
    TEXT = (char*)malloc(80);
    char TS[5];
    snprintf(TS,sizeof(TS),"%03x",loc);
    strcpy(TEXT,"T^");
    strcat(TEXT,TS);
    strcat(TEXT,"^");
    strcat(TEXT,"00^");
    textlen = 0;
    fclose(out);
}

void write_text(int textlen){
    FILE *out;
    out = fopen("out.obj","a");
    char TL[5];
    snprintf(TL,sizeof(TL),"%02x",(textlen)/2);
    TEXT[6] = TL[0];
    TEXT[7] = TL[1];
    fprintf(out,"%s\n",TEXT);
    fclose(out);
}

void app_text(char OBJ[]){
    textlen+=strlen(OBJ);
    strcat(TEXT,OBJ);
    strcat(TEXT,"^");
}

//dummy obj file
void write_out(char OBJ[]){
    FILE *out;
    out = fopen("out.obj","a");
    
    fputs(OBJ,out);
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
    if(textlen + strlen(OBJ)>=60){
        write_text(textlen);
        init_text(loc);
    }
    app_text(OBJ);
   // write_out(OBJ);
}

void format2(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    snprintf(OBJ,sizeof(OBJ),"%x%x",op,readregtab(oper));
    printf("obj:%s\n",OBJ);
    if(textlen + strlen(OBJ)>=60){
        write_text(textlen);
        init_text(loc);
    }
    app_text(OBJ);
   // write_out(OBJ);
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
    if(textlen + strlen(OBJ)>=60){
        write_text(textlen);
        init_text(loc);
    }
    app_text(OBJ);
    // write_out(OBJ);
}

void format4(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    int len = strlen(oper);
    char OPERS[3][20];
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
    
    int getflag = get_flag(OPERS[1]);
    int symval = read_symtab(OPERS[1]);
    int y;

    if(getflag==1){     //for absolute value
        y = symval;
    }
    else{
        if(symval<(loc+2)){
            y = (loc+2) - symval;
            y = 0xff - y;
        }
        else{
            y = symval - (loc+2);
        }
    }

    snprintf(OBJ,sizeof(OBJ),"%x%x%02x",op,readregtab(OPERS[0]),y);
    printf("obj:%s\n",OBJ);
    if(textlen + strlen(OBJ)>=60){
        write_text(textlen);
        init_text(loc);
    }
    app_text(OBJ);
    // write_out(OBJ);
}

void format5(int loc,char opc[],char oper[]){
    int op = readoptab(opc);
    snprintf(OBJ,sizeof(OBJ),"%x%03x",op,read_symtab(oper));
    printf("obj:%s\n",OBJ);
    if(textlen + strlen(OBJ)>=60){
        write_text(textlen);
        init_text(loc);
    }
    app_text(OBJ);
    // write_out(OBJ);
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
        snprintf(OBJ,sizeof(OBJ),"%04x",get_operval(operand));
        printf("obj:%s\n",OBJ);
       // write_out(OBJ);
        if(textlen + strlen(OBJ)>=60){
            write_text(textlen);
            init_text(loc);
            }
        app_text(OBJ);
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
    write_head();
    init_text(loc);
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
    write_text(textlen);
    write_end();
    fclose(fp);
    return 0;
}