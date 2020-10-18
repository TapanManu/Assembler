#include<iostream>
#include<fstream>
#include<math.h>
#include<bits/stdc++.h>

using namespace std;

static int locctr=0,start=0;
static int len=0,num=0;

string line;

char codes[3][5];
char temp[30];

ifstream fin;


int charToInt(char str[],int x){
    if(x<0)
        return num;
    return ((str[x])-'0')*pow(10,len-x-1) + charToInt(str,--x);
}

void init(char opcode[],char operand[]){
    locctr=0;
    if(!strcmp(opcode,"START")){
        if(!strcmp(operand," ")){      
            locctr=0;
        }
        else{
            num=0;
            len=strlen(operand);
            locctr=charToInt(operand,len-1);
        }
    }
    start = locctr;
    cout<<locctr;
}

void read_from_assembly(){
    static int linecount=1;
    int count=0;
    char label[20],opcode[20],operand[20];
    if(!getline(fin,line))
        return;
    strcpy(temp,line.c_str());
    sscanf(temp,"%s\t%s\t%s",label,opcode,operand);
    if(linecount==1){
        init(opcode,operand);
    }
    linecount++;
    read_from_assembly();
}



int main(){
    fin.open("assembly_pg.txt",ios::in);
    read_from_assembly();
    fin.close(); 
    return 0;
}