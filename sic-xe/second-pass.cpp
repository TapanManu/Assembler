#include <iostream>
#include<bits/stdc++.h>
#include <fstream>

using namespace std;

stack <int> s;

int base=1000;     //value stored in base register
//loaded only in ldb instruction and base directive
char zero[2]="0";
int err_flag=0;
char val[20];
string obj_code;

int lineno=0;

int search_symtab(string label)
{
    fstream symstream;
    string line, comp;
    symstream.open("symtab.txt", ios::in);
    while (getline(symstream, line))
    {
        stringstream syms(line);
        syms >> comp;
        if (!strcmp(comp.c_str(), label.c_str()))
        {
            //cout << comp << label;
            return 1; //if element present
        }
    }
    return 0; //if not present
}

int valid_opcode(string opcode)
{
    fstream opstream;

    string line, comp;
    opstream.open("optab.txt", ios::in);
    while (getline(opstream, line))
    {
        stringstream ops(line);
        ops >> comp;
        if(opcode.at(0)=='+'){
            opcode = opcode.substr(1,opcode.length()-1);
        }
        if (!strcmp(comp.c_str(), opcode.c_str()))
        {
            // if opcode is present
            return 1;
        }
    }
    return 0; //if not
}

int get_opcode_length(string opcode)
{
    fstream opstream;
    int length = 0;
    string line, comp;
    opstream.open("optab.txt", ios::in);
    while (getline(opstream, line))
    {
        stringstream ops(line);
        ops >> comp;
        if (!strcmp(comp.c_str(), opcode.c_str()))
        {
            ops >> length;
            return length;
        }
    }
    return 0;
}

string get_opcode_value(string opcode)
{
    fstream opstream;
    int length = 0;
    string value;
    string line, comp;
    opstream.open("optab.txt", ios::in);
    while (getline(opstream, line))
    {
        stringstream ops(line);
        ops >> comp;
        if (!strcmp(comp.c_str(), opcode.c_str()))
        {
            ops >> length;
            ops >> value;
            return value;
        }
    }
    return "";
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

int hexToDec(string hexVal) 
{    
    int len = hexVal.length(); 
    int base = 1; 
      
    int dec_val = 0; 
    
    for (int i=len-1; i>=0; i--) 
    {    
        if (hexVal[i]>='0' && hexVal[i]<='9') 
        { 
            dec_val += (hexVal[i] - 48)*base; 
            base = base * 16; 
        } 
           
        else if (hexVal[i]>='A' && hexVal[i]<='F') 
        { 
            dec_val += (hexVal[i] - 55)*base; 
            base = base*16; 
        } 
    } 
      
    return dec_val; 
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
    return "";
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

string process_base(int operand_loc){
    int disp = operand_loc - base;
    stringstream res;
    string r;
    res << setfill('0') << setw(3) << hex << (disp&0xfff);  
    if(disp > 4095 || disp < -4095){
        err_flag = 5;   //beyond limit
        return NULL;
    }
    else{
        r = res.str();
    }
    return r;
}

string xbpe(int x,int b,int p,int e){
    string xbpe = to_string(x) + to_string(b) + to_string(p) + to_string(e);
    bitset<8> bits(xbpe);
    int dec = bits.to_ulong();  //binary to decimal
    decToHex(dec);
    string result(val);
    return result;
}

string to_up(string ch){
    string res="";
    for(int i=0;i<ch.length();i++){
        res = res + (char)toupper(ch.at(i));
    }
    return res;
}

void opcode_processor(string loc,string opcode,string operand){
        int x=0,b=0,p=1,e=0;
        string value;
        int add = 0;        //value to be added to opcode
        int format=0;
        if(opcode.at(0)=='+'){
            value = get_opcode_value(opcode.substr(1,opcode.length()-1));
            format = 4;
            e = 1;
            p = 0;
            b = 0;
            //modification record needed
        }
        else{
            value = get_opcode_value(opcode);
            format = get_opcode_length(opcode);
        }
        int locvalue=0;
               
        stringstream s;
        s <<  hex << (stoi(loc)&0xfff);
        stringstream res;
        string r;
        s >> locvalue;
        // cout << locvalue;
        int nextlocvalue = hexToDec(to_string(locvalue)) + format;
        if(format==1){
            //format 1 has only opcode no operand
            obj_code = value;
            return;
        }
        if(format==2){
            obj_code = value + register_operands(operand);
            return;
        }
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

        if(search_symtab(operand)|| add==1 ){ //add=1 indicates constant
            if(operand.find(",X")!=string::npos){
                x = 1;
                b = 0;
                p = 1;
            }
            int operand_loc=0;
            if(add==1){
                res << setfill('0') << setw(3) << hex << (stoi(operand)&0xfff);
                r = to_up(res.str());
                //cout<<r;
            }
            else{
            operand_loc = read_symtab(operand);
            if(operand_loc==-1){
                //cout<<"error-operand";
                err_flag = 2;
                return;
            }
            int disp;
            if(format==4){
                res << setfill('0') << setw(5) << hex << (operand_loc);
                r = to_up(res.str());
            }
            else if(format==3){
                cout<<"next:"<<nextlocvalue;
                cout<<"operand_loc:"<<operand_loc;
            if(operand_loc > nextlocvalue){
                
                disp = nextlocvalue - operand_loc;
                res << setfill('0') << setw(3) << hex << (disp&0xfff);
                //oxfff is hex value of 4095
                //simple way to convert to hexadecimal code
                if(disp<-2048){
                    b = 1;
                    p = 0;
                    r = to_up(process_base(operand_loc));
                }
                else{
                    r = to_up(res.str());
                }

                
            }
            else{
                disp = nextlocvalue - operand_loc;
                res << setfill('0') << setw(3) << hex << (disp&0xfff);
                if(disp > 2047){
                    b = 1;
                    p = 0;
                    r = to_up(process_base(operand_loc));
                }
                else{
                    r = to_up(res.str());
                }

            }
            }
            }
            string resval = xbpe(x,b,p,e);
            if(format==3 || format==4){
                int v=0;
                stringstream sss;
                v = hexToDec(value);
                //cout<<"value:"<<hexToDec(value);
                v = v+add;        //add value to opcode
                //cout<<"value:"<<v;
                decToHex(v);
            }
            string valop(val);
            //cout<<"valop:"<<valop;
            obj_code = valop + resval + r;
        }
        else{
            err_flag=1;
            return;
        }
}



void second_pass(string loc,string label,string opcode,string operand){
    if(!strcmp(opcode.c_str(),"END") || !strcmp(opcode.c_str(),"START") || opcode.compare(" ")==0){
        return; 
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
    }
    else if(valid_opcode(opcode)){   
        cout<<"\t";
        opcode_processor(loc,opcode,operand);
        cout<<"objcode:"<<obj_code;
        return;
    }
    else{
        err_flag=1;
    }
    if(err_flag==1 || err_flag==2){
       cout<<"unidentified symbol error at line "<<lineno<<"\n";
       exit(1);
    }

    if(err_flag==4){
        cout<<"unindentified register at line "<<lineno<<"\n";
        exit(1);
    }
    if(err_flag==5){
        cout<<"displacement beyond range , error at line "<<lineno<<"\n";
        exit(1);
    }
    //it is assumed intermediate file is free of syntax errors after first pass 
}





int main(int argc,char* argv[]){
    fstream source;
    int pgmstart = 0;
    string line,loc,label,opcode,operand;
    source.open(argv[1],ios::in);
    if(source.is_open()){
        while(getline(source,line)){
            lineno++;
            stringstream ss(line);
            cout<<"line:"<<line;
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
           // cout<<"loc:"<<loc<<" label:"<<label<<" opcode:"<<opcode<<" operand:"<<operand;
            second_pass(loc,label,opcode,operand);
            cout<<"\n";
        }
    }
    else{
        cout<<"source file cannot be located";
    }
    return 0;
}