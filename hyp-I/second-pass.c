// develop an assembler hypothetiocal
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include<stdlib.h>

int locctr =0,len=0,equ=0;
char line[255];
char OBJ[20];
char decval[5];
char optab[][15] = {"ADD","NEG","LOAD","STORE","JE","JL","JG","HLT"};
char negval[][5] = {"20","28","30","38"};
char zero[2] = "0";
char REGTAB[][7] = {"R0","R1","R2","R3"};
char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void clear_file(char file[]){
    FILE *out;
    out = fopen(file,"w");
    fputs("",out);
    fclose(out);
}

void write_obj(){
    FILE *out;
    out = fopen("hyp1.obj","a");
    //fprintf(out,"%d\t%s",locctr,lin);
    fclose(out);
}

/*void ins_symtab(char label[]){
    FILE *out;
    out = fopen("symtab.txt","a");
    fprintf(out,"%s\t%d\n",label,locctr);
    fclose(out);
}*/

int read_sym(char oper[]){
    FILE *in;
    char l[255],lab[20];
    int val=0;
    in = fopen("symtab.txt","r");
    while(fgets(l,255,in)){
        sscanf(l,"%s\t%d",lab,&val);
        if(!strcmp(lab,oper)){
            return val;
        }
    }
    return -1;
    fclose(in);
}

int ret_opval(char *val){
    for(int i=0;i<8;i++){
        if(!strcmp(optab[i],val)){
            return i;
        }
    }
    return -1;
}

int ret_reg(char *reg){
    for(int i=0;i<4;i++){
        if(!strcmp(REGTAB[i],reg)){
            return i;
        }
    }
    return -1;
}

char* decToHex(int dec){
    strcpy(decval,"");
    char revdecval[5];
    int x = 0;
    while(dec!=0){
        int dc = dec%16;
        revdecval[x++] = hex[dc];
        dec = dec/16;
    }
    revdecval[x]='\0';
    for(int i=0;i<x;i++){
        decval[i] = revdecval[x-i-1]; 
    }
    decval[x]='\0';
    return decval;
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

void process_single(char opcode[],char operand[]){
    int opc = ret_opval(opcode);  
    opc = opc*2;
   // printf("opc:%d\n",opc);
    OBJ[0] = hex[opc];
    OBJ[1] = '0';
    OBJ[2] = '\0';
}

void process_op(char opcode[],char operand[]){
    int opc = ret_opval(opcode);
    char oper1[4],oper2[4];
    oper1[0] = operand[0];
    oper1[1] = operand[1];
    oper1[2] = '\0';
    oper2[0] = operand[3];
    oper2[1] = operand[4];
    oper2[2] = '\0';
    int r = ret_reg(oper1);
    int s = ret_reg(oper2);
   // printf("r:%d\ts:%d\n",r,s);
    opc = opc*2;
    if(r>=2){
        opc = opc+1; 
    }
    if(r%2==0){
        s = s*2;
    }
    else {
        s = 8 + s*2;
    }
  //  printf("opc:%d\ts:%d\n",opc,s);
    OBJ[0] = hex[opc];
    OBJ[1] = hex[s];
    OBJ[2] = '\0';
}

void second_pass(int loc,char label[],char opcode[], char operand[]){
    int format = 0;
    if(!strcmp(opcode,"START")){
        locctr = get_intval(operand);
        printf("%d",locctr);
    }
    else if(!strcmp(opcode,"DB")){
        while(strlen(operand)<4){
            strcpy(zero,"0");
            strcat(zero,operand);
            strcpy(operand,zero);
        }
        strcpy(OBJ,operand);
    }
    else if(!strcmp(opcode,"EQU")){
        // break text record
        return;
    }
    else if(!strcmp(opcode,"END")){
        // end record
        return;
    }
    else if(!strcmp(opcode,"HLT")){
        format = 1;
        strcpy(OBJ,"E0");
    }
    else if(ret_opval(opcode)!=-1){
            if(!strcmp(opcode,"NEG")){
                format = 1;
                int r = ret_reg(operand);
                strcpy(OBJ,negval[r]);
            }
            else if(!strcmp(opcode,"ADD")){
                format = 1;
                process_op(opcode,operand);
               
            }
            else if(!strcmp(opcode,"LOAD") || !strcmp(opcode,"STORE")){
                format = 2;
                char * r;
                r = (char*)malloc(3);
                process_op(opcode,operand);
                
                //printf("obj:%s\n",OBJ);
                int d =0;
                char disp[10];
                int k=0;
                for(int j=6;j<strlen(operand);j++)
                    disp[k++] = operand[j];
                disp[k] = '\0';
                int res;
                res = read_sym(disp);
                int next = loc + format;
                if(next > res){
                    d = next - res;
                    d = 256 - d;
                    
                    r = decToHex(d);
                }
                else{
                    d = res - next;
                    r = decToHex(d);
                }
                strcat(OBJ,r);
            }
            else if(!strcmp(opcode,"JL") || !strcmp(opcode,"JG") || !strcmp(opcode,"JE")){
                format = 2;
                char * r;
                
                process_single(opcode,operand);
                
            //    printf("obj:%s\n",OBJ);
                int d =0;
                char disp[10];
                int k=0;
                for(int j=0;j<strlen(operand);j++)
                    disp[k++] = operand[j];
                disp[k] = '\0';
                int res;
                //printf("disp:%s\n",disp);
                res = read_sym(disp);
                //printf("%d\n",res);
                int next = loc + format;
                //printf("%d\n",next);
                if(next > res){
                    r = (char*)malloc(3);
                    d = next - res;
                 //   printf("d:%d\n",d);
                    d = 256 - d;
                    
                    r = decToHex(d);
                }
                else{
                    r = (char*)malloc(3);
                    d = res - next;
                  //  printf("d:%d\n",d);
                    r = decToHex(d);
                    while(strlen(r)<2){
                        strcpy(zero,"0");
                        strcat(zero,r);
                        strcpy(r,zero);
                        }
                    //printf("r:%s\n",r);
                }
                strcat(OBJ,r);
            }
        
    }
    else{
        printf("unidentified symbol error");
    }
}


int main(int argc, char* argv[]){
    FILE *fp;
    char label[20],opcode[20],operand[20];
    fp = fopen("int.asm","r");
    
    
    clear_file("hyp1.obj");
    
    while(fgets(line,255,fp)){
        int loc = 0;
        label[0]=0;opcode[0]=0;operand[0]=0;
        sscanf(line,"%d\t%s\t%s\t%s",&loc,label,opcode,operand);
        if(operand[0]==0){
            if(strcmp(opcode,"HLT")){
                strcpy(operand,opcode);
                strcpy(opcode,label);
                strcpy(label,"");
            }
        }
        second_pass(loc,label,opcode,operand);
        printf("%s-->%s\n",opcode,OBJ);
        strcpy(OBJ,"");
    }
    fclose(fp);
    return 0;
}
