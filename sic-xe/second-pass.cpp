#include <iostream>
#include<bits/stdc++.h>
#include <fstream>
#include "sicxe.h"
using namespace std;

stack <int> s;

int base=0;     //value stored in base register
//loaded only in ldb instruction and base directive
char zero[2]="0";
int err_flag=0;
char val[20];
string obj_code;



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

int hexToDec(string loc){
    int value=0;
    for(int i=loc.length()-1;i>=0;i--){
        value+=loc.at(i)*pow(16,loc.length()-i-1);
    }
    return value;
}

int read_symtab(string operand){
    fstream symstream;
    string line,comp;
    symstream.open("symtab.txt",ios::in);
    while(getline(symstream,line)){
        stringstream syms(line);
        syms >> comp;
        if(!strcmp(comp.c_str(),operand.c_str())){
            syms >> comp; //hex address is obtained
            return hexToDec(comp);
        }
    }
    return -1;       //if not present
}

string read_register(char reg){
    fstream fin;
    fin.open("register.txt",ios::in);
    string line;
    string code;
    while(getline(fin,line)){
        stringstream rs(line);
        rs >> code;
        if(code.at(0)==reg){
            rs >> code;
            return code;
        }
    }
    err_flag = 4;   //invalid register error
    return NULL;
}

string register_operands(string operand){
    stringstream ro(operand);
    string registers;
    char r[2];
    int i=0;
    while(getline(ro,registers,',')){
        r[i++] = registers.at(0);
    }
    return read_register(r[0]) +  read_register(r[1]);

}

string xbpe(int x,int b,int p,int e){
    string xbpe = to_string(x) + to_string(b) + to_string(p) + to_string(e);
    bitset<8> bits(xbpe);
    int dec = bits.to_ulong();  //binary to decimal
    decToHex(dec);
    string result(val);
    return result;
}

void opcode_processor(string loc,string opcode,string operand){
        int x=0,b=0,p=1,e=0;
        string value;
        int add = 0;        //value to be added to opcode
        int format=0;
        if(opcode.at(0)=='+'){
            value = get_opcode_value(opcode.substr(1,opcode.length()+1));
            format = 4;
            e = 1;
        }
        else{
            value = get_opcode_value(opcode);
            format = get_opcode_length(opcode);
        }
        int locvalue=0;
        stringstream s(to_string(hexToDec(loc)));
        stringstream res;
        string r;
        s >> locvalue;
        int nextlocvalue = locvalue + format;
        if(format==1){
            //format 1 has only opcode no operand
            obj_code = value;
            return;
        }
        if(!search_symtab(operand)){
            //process for x(array indexing)
            if(format==2){
                obj_code = value + register_operands(operand);
                return;
            }
            int operand_loc=0;
            if(operand.at(0)=='#'){
                add = 1;
                operand = operand.substr(1,operand.length()-1);
            }
            else if(operand.at(0)=='@'){
                add = 2;
                operand = operand.substr(1,operand.length()-1);
            }
            else{
                add = 3; 
            }
            operand_loc = read_symtab(operand);
            if(operand_loc==-1){
                err_flag = 2;
                return;
            }
            int disp;
            if(operand_loc > nextlocvalue){
                disp = operand_loc - nextlocvalue;
                res << setfill('0') << setw(3) << hex << (disp&0xfff);
                //oxfff is hex value of 4095
                //simple way to convert to hexadecimal code
                if(disp<-2048){
                    b = 1;
                    //process for base register
                    p = 0;
                }
                else{
                    r = res.str();
                }

                
            }
            else{
                disp = nextlocvalue - operand_loc;
                res << setfill('0') << setw(3) << hex << (disp&0xfff);
                if(disp > 2047){
                    b = 1;
                    //process for base register
                    p = 0;
                }
                else{
                    r = res.str();
                }

            }
            string resval = xbpe(x,b,p,e);
            if(format==3 || format==4){
                int v = hexToDec(value);
                v = v+add;        //add value to opcode
                decToHex(v);
            }
            string valop(val);
            obj_code = valop + resval + r;
        }
        else{
            err_flag=1;
            return;
        }
}



void second_pass(string loc,string label,string opcode,string operand){
    
    if(valid_opcode(opcode)){
        opcode_processor(loc,opcode,operand);
        //now obj_code contains the object code
    }
    else if(!strcmp(opcode.c_str(),"END")){
        return; //end of pass 2
    }
    else if(!strcmp(opcode.c_str(),"BYTE")){
        if(operand.at(0)=='C'){
            string constant = operand.substr(operand.at(2),operand.length()-3);
            decToHex(stoi(constant));
            //now val contains the hex constant
            cout<<val;
        }
        else if(operand.at(0)=='X'){
            string constant = operand.substr(operand.at(2),operand.length()-3);
            
            cout<<constant;
        }
    }
    else if(!strcmp(opcode.c_str(),"WORD")){
        decToHex(stoi(operand));
        while(strlen(val)<=5){
                strcpy(zero,"0");
                strcat(zero,val);
                strcpy(val,zero);
            }
        cout<<val;
    }
    else if(!strcmp(opcode.c_str(),"RESW")|| !strcmp(opcode.c_str(),"RESB")){
            return;
            //no object code created
    }
    //it is assumed intermediate file is free of syntax errors after first pass 
}





int main(int argc,char* argv[]){
    fstream source;
    string line,loc,label,opcode,operand;
    source.open(argv[1],ios::in);
    if(source.is_open()){
        while(getline(source,line)){
            stringstream ss(line);
            if(ss>>loc>>label>>opcode>>operand){
                ;
            }
            else{
                source.seekg(-1*line.length(),ios::cur);
                getline(source,line);
                stringstream s(line);
                loc="";
                label="";opcode="";operand="";
                s>>loc>>opcode>>operand;
            }
            cout<<"loc:"<<loc<<" label:"<<label<<" opcode:"<<opcode<<" operand:"<<operand;
            cout<<"\n";
        }
    }
    else{
        cout<<"source file cannot be located";
    }
    return 0;
}