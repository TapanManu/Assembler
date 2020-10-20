#include <iostream>
#include <bits/stdc++.h>
#include <fstream>

using namespace std;

char obj[10],opc[10],addr[10],operand[10],val[20];
char err[] = "error";

stack <int> s;

char* substring(char* dest,char* source,int min,int max){
    int j=0;
    for(int i=min;i<max;i++){
        dest[j++]=source[i];
    }
    dest[j++]='\0';
    return dest;
}

void decToHex(int ctr){
    while(ctr>0){
        char ch=65;
        int d = ctr%16;
        if(d>9 && d<16){
            ch = int(ch) + d - 10;
        }
        else{
            ch = d;
        }
        s.push(int(ch));
        ctr/=16;
    }
    int i=0;
    while(!s.empty()){
        if(s.top()>=65 && s.top()<=90){
            
            val[i]=(char)s.top();
            i++;
        }
        else{
            val[i]=s.top()+48;      //char equivalent of num
            i++;
        }
        s.pop();
    }
    val[i++]='\0';
}

int charToInt(char str[],int x){
    static int len = x+1;
    if(x<0)
        return 0;                   
    return ((str[x])-'0')*pow(10,len-x-1) + charToInt(str,--x);
}
char* translate_opcode(char* op){
    ifstream fin;
    fin.open("optab.txt",ios::in);
    string line;
    char code[20];
    while(getline(fin,line)){
        strcpy(code,line.c_str());
        sscanf(code,"%s\t%s\t",obj,opc);
        
        if(!strcmp(op,opc)){
            return obj;
        }
    }
    fin.close();
    return err;
}

char index_addr(char ch){
    int c = int(ch)-'0';
    char hex[]={'8','9','A','B','C','D','E','F'};
    return hex[c];
}

char* translate_oper(char* op,int x){
    ifstream fin;
    char zero[2] ="0";
    fin.open("symtab.txt",ios::in);
    string line;
    char code[20];
    while(getline(fin,line)){
        strcpy(code,line.c_str());
        sscanf(code,"%s\t%s\t",operand,addr);
        if(!strcmp(op,operand)){
            while(strlen(addr)<=3){
                strcpy(zero,"0");
                strcat(zero,addr);
                strcpy(addr,zero);
            }
            if(x==1){
                addr[0]=index_addr(addr[0]);
            }
            return addr;
        }
    }
    fin.close();
    return err;
}

void objcode(char* line){
    char opcode[20],locctr[10],operand[10],opr[10];
    int x = 0;
    char zero[2]="0";
    sscanf(line,"%s\t%s\t%s\t",locctr,opcode,operand);
    if(!strcmp(opcode,"END"))
        return;
    int len = strlen(operand);
    if(operand[len-2]==','){            //whether index addressing used
        if(charToInt(operand,len-1)>1){
            x = 1;
            substring(opr,operand,0,strlen(operand)-2);
        }
        strcpy(operand,opr);
    }
    char obj[25],oper[25];
    strcpy(obj,translate_opcode(opcode));
    if(!strcmp(obj,"error")){
        if(!strcmp(opcode,"RESW")||!strcmp(opcode,"RESB")){
            strcpy(obj,"  ");
        }
        if(!strcmp(opcode,"WORD")||!strcmp(opcode,"BYTE")){
            strcpy(obj,"00");
        }
    }
    cout<<obj;
    if(!strcmp(obj,"4C"))
        cout<<"0000"<<"\n";
    else{
        strcpy(oper,translate_oper(operand,x));   
        if(!strcmp(oper,"error")){
            if(!strcmp(opcode,"RESW")||!strcmp(opcode,"RESB")){
                strcpy(oper,"    ");
            }
            if(!strcmp(opcode,"WORD")||!strcmp(opcode,"BYTE")){
                if(!strcmp(opcode,"BYTE")){
                    if(operand[0]=='C' && operand[1]=='`'){
                        int i=2;
                        strcpy(oper,"");
                        while(operand[i]!='\0'){
                            int c = operand[i];
                            strcpy(val,"");
                            decToHex(c);
                            strcat(oper,val);
                            i++;
                        }
                    }
                    if(operand[0]=='X' && operand[1]=='`'){
                       strcpy(oper,substring(oper,operand,2,strlen(operand)));
                    }
                    while(strlen(oper)<4){
                        strcpy(zero,"0");
                        strcat(zero,oper);
                        strcpy(oper,zero);
                    }
                    strcpy(oper,operand);
                }
                else{
                while(strlen(operand)<4){
                    strcpy(zero,"0");
                    strcat(zero,operand);
                    strcpy(operand,zero);
                }
                strcpy(oper,operand);
                }
            }
        }
        cout<<oper<<"\n";
    }
}

int main(){
    string s;
    ifstream f;
    f.open("intermediate_file.txt",ios::in);
    getline(f,s);
    while(getline(f,s)){
        char line[25];
        strcpy(line,s.c_str());
        objcode(line);
        /* obj code perfect correction now create object file*/
    }
    return 0;
}