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

void clear_symtab(){
    ofstream out;
    out.open("symtab.txt",ios::out);
    out<<"\n";
    out.close();
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
    else
        locctr=0;
    start = locctr;
    cout<<start;
}

void writeToIntermediate(int locctr,char opcode[],char operand[],int clear){
    ofstream fout;
    if(clear==1)
        fout.open("intermediate_file.txt",ios::out);
    else
        fout.open("intermediate_file.txt",ios::app);
    fout<<locctr<<"\t"<<opcode<<"\t"<<operand<<"\n";
    fout.close();
}

void writeToIntermediate(string err){
    ofstream fout;
    fout.open("intermediate_file.txt",ios::app);
    fout<<err<<"\n";
    fout.close();
}

void write_symtab(char label[],int locctr){
    ofstream out;
    out.open("symtab.txt",ios::app);
    out<<label<<"\t"<<locctr<<"\n";
    out.close();
}

int search_symtab(char label[],int locctr){
    ifstream f;
    string line;
    char l[10],value[10];
    char str[20];
    int found=0;
    f.open("symtab.txt",ios::in);
    while(getline(f,line)){
        strcpy(str,line.c_str());
        sscanf(str,"%s\t%s\t",l,value);
        if(!(strcmp(label,l))){
            found = 1;              //same label found ==>error
            break;
        }
    }
    f.close();
    return found;
}

int search_optab(char opcode[]){
    ifstream ip;
    ip.open("optab.txt",ios::in);
    string line;
    char l[10],value[10];
    char str[20];
    int found=0;

    while(getline(ip,line)){
        strcpy(str,line.c_str());
        sscanf(str,"%s\t%s\t",l,value);
        if(!(strcmp(opcode,value))){
            found = 1;              //valid
            break;
        }
    }
    ip.close();
    return found;
}

void read_from_assembly(){
    static int linecount=1;
    int prevloc=0;
    char label[20],opcode[20],operand[20];
    int dup_error=0,op_error=0;
    
    strcpy(operand," ");    //default value to avoid garbage
    getline(fin,line);      //read each line
    prevloc=locctr;
    strcpy(temp,line.c_str());
    sscanf(temp,"%s\t%s\t%s",label,opcode,operand);

    if(!strcmp(opcode,"END")) {         //return if opcode=END
        writeToIntermediate(prevloc,opcode,operand,0);
        return;
    }

    if(strstr(label,".")){
        //comment line ignore writing to intermediate
        read_from_assembly();
        return;
    }

    if(strcmp(label,"**")){            //if there is label
        int i = search_symtab(label,locctr);
        if(i==0){
            write_symtab(label,locctr);
        }
        else{
            dup_error=1;        //duplication symbol error
        }
    }

    //opcode validation
    if (search_optab(opcode))
        locctr = locctr+3;
    else if(!strcmp(opcode,"WORD"))
        locctr = locctr+3;
    else if(!strcmp(opcode,"BYTE"))
        locctr = locctr+1;
    else if(!strcmp(opcode,"RESW")){
        len=strlen(operand);
        locctr = locctr+3*charToInt(operand,strlen(operand)-1);
    }
    else if(!strcmp(opcode,"RESB")){
        len=strlen(operand);
        locctr = locctr+1*charToInt(operand,strlen(operand)-1);
    }
    else
        op_error=1;
    

    if(linecount==1){
        init(opcode,operand);
        writeToIntermediate(0,opcode,operand,1);
    }
    else{
        if(dup_error==1)
            writeToIntermediate("duplication error");
        else if(op_error==1)
            writeToIntermediate("invalid opcode error");
        else{
            if(!strcmp(operand,""))
                strcpy(operand,"0");       //avoid garbage
            writeToIntermediate(prevloc,opcode,operand,0);
        }
    }
    linecount++;
    read_from_assembly();
}



int main(){
    clear_symtab();                     //clear symtab at beginning
    fin.open("assembly_pg.txt",ios::in);
    read_from_assembly();
    fin.close(); 
    return 0;
}