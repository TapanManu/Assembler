#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include "sicxe.h"
using namespace std;

char val[20],valprev[20];
char zero[2] ="0";

stack <int> s;
string pgmname="source";
int prevctr=0;
int locctr=0;
int err_flag=0;
int lineno=0;

int find_length(string operand){
    if(operand.at(0)=='X'){
        return strlen((operand.c_str())-3)/2+1;
    }
    if(operand.at(0)=='C'){
        return strlen(operand.c_str())-3;      
                                    //A C char along with two ' ' are there
    }
    return 0;
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

void update_locctr(string opcode,string operand){
    
    if(!strcmp(opcode.c_str(),"START")){
        return;         //no need of update
    }
    if(opcode.at(0)=='+'){
        if(valid_opcode(opcode.substr(1,opcode.length()+1))){
            int oplen = get_opcode_length(opcode.substr(1,opcode.length()+1));
            if(oplen==3)
                locctr+=4;
            else err_flag=3;
        }
        else err_flag=1;  
    }
    else if(valid_opcode(opcode)==1){
        locctr+=get_opcode_length(opcode);
    }
    else if(!strcmp(opcode.c_str(),"BYTE")){
        locctr+=find_length(operand);
    }
    else if(!strcmp(opcode.c_str(),"WORD")){
        locctr+=3;
    }
    else if(!strcmp(opcode.c_str(),"RESW")){
        stringstream cc(operand);
        int value=0;
        if(cc >> value)
            locctr+=value*3;
        else err_flag=3;
    }
    else if(!strcmp(opcode.c_str(),"RESB")){
        stringstream cc(operand);
        int value=0;
        if(cc >> value)
            locctr+=value;
        else err_flag=3;
    }
    else{
        err_flag=1;
    }
   
}
void clear_file(string filename){
    fstream symtab;
    symtab.open(filename,ios::out);
    symtab<<"\n";
    symtab.close();
}

void insert_symtab(string label){
    fstream symtab;
    symtab.open("symtab.txt",ios::app);
    decToHex(locctr);
    
    while(strlen(val)<=3){
        strcpy(zero,"0");
        strcat(zero,val);
        strcpy(val,zero);
    }
    symtab<<label<<" "<<val<<"\n";
    symtab.close();
}

void write_intermediate(string line){
    fstream fout;
    fout.open(pgmname+"_intermediate.txt",ios::app);
    decToHex(prevctr);
    while(strlen(val)<=3){
        strcpy(zero,"0");
        strcat(zero,val);
        strcpy(val,zero);
    }
    fout<<val<<" "<<line<<"\n";
    fout.close();
}

void first_pass(string label,string opcode,string operand){
    char opc[20];
    strcpy(opc,opcode.c_str());
    prevctr=locctr;
    if(!strcmp(opc,"START")){
        if(strcmp(label.c_str(),"")){
            pgmname=label;             //storing program name
        }
        stringstream ss(operand);
        if(ss >> locctr); else locctr=0;
        clear_file(pgmname+"_intermediate.txt"); 
    }
    if(!strcmp(opc,"END")){
        return;
    }
    if(strcmp(label.c_str(),"")){
        if(search_symtab(label)){
            insert_symtab(label);
        }
        else err_flag = 2;
    }
    
    update_locctr(opcode,operand);
    if(err_flag==1){
        cout<<"unidentified symbol error in code in line:"<<lineno;
        exit(1);        //close the program
    }
    if(err_flag==2){
        cout<<"duplication symbol error in line:"<<lineno;
        exit(1);
    }
    if(err_flag==3){
        cout<<"syntax error in line:"<<lineno;
        exit(1);
    }

}

    


int main(int argc,char* argv[]){
    fstream source;
    string label,opcode,operand;
    string line;
    source.open(argv[1],ios::in);
    clear_file("symtab.txt");
    if(source.is_open()){
        while(getline(source,line)){
            lineno++;
            stringstream s(line);
            if(line.at(0)=='.'){
                continue;
            }
            if(s>>label>>opcode>>operand){
                ; 
            }
            else{
                source.seekg(-1*line.length(),ios::cur);
                getline(source,line);
                stringstream ss(line);
                opcode="";operand="";
                ss>>opcode>>operand;
                label="";
                
            }
            cout<<"label:"<<label<<"opcode:"<<opcode<<"operand:"<<operand<<endl;
            
            first_pass(label,opcode,operand);
            write_intermediate(line);
        }
    }
    else{
        cout<<"unable to open source file";
    }
    source.close();
    return 0;
}





