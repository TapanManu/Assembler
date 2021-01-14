#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;

char val[20],valprev[20];
char zero[2] ="0";

stack <int> s;
string pgmname="source";
int prevctr=0;
int locctr=0;
int err_flag=0;
int lineno=0;

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
    symstream.open(pgmname+"_symtab.txt",ios::in);
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

int search_symtab(string label)
{
    fstream symstream;
    string line, comp;
    symstream.open(pgmname+"_symtab.txt", ios::in);
    while (getline(symstream, line))
    {
        stringstream syms(line);
        syms >> comp;
        if (!strcmp(comp.c_str(), label.c_str()))
        {
            cout << comp << label;
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
    return NULL;
}


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
        if(valid_opcode(opcode.substr(1,opcode.length()-1))){
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
    else if(!strcmp(opcode.c_str(),"EQU")){
        stringstream cc(operand);
        int value=0;
        if(cc >> value)
            locctr+=value;
    }
    else{
        err_flag=1;
    }
   
}
void clear_file(string filename){
    fstream symtab;
    symtab.open(filename,ios::out);
    symtab<<"";
    symtab.close();
}

void insert_symtab(string label,int abs=0,string op=" "){
    fstream symtab;
    char field = 'R';
    symtab.open(pgmname+"_symtab.txt",ios::app);
    decToHex(locctr);
    
    if(abs==1){
        field = 'A';
        strcpy(val,op.c_str());
    }
    
    while(strlen(val)<=3){
        strcpy(zero,"0");
        strcat(zero,val);
        strcpy(val,zero);
    }
    
    symtab<<label<<" "<<val<<" "<<field<<"\n";
    symtab.close();
}
stack<int> st;
int handle_expression(string operand){
    cout<<"EXPRESSION HANDLING BEGINS\n";
    int prev = 0;
    int loc = 0;
    int count=0;
    st.push(-1);        //bottom marker
    while((loc = operand.find('-'))!=string::npos){
       //cout<<"xxoperand:"<<operand<<"\n";
       // cout<<"prev:"<<prev<<" loc:"<<loc<<"\n";
        string op = operand.substr(0,loc);
       // cout<<"op"<<op<<"\n";
        st.push(read_symtab(op));
        if(st.top()==-1)
            break;
        cout<<"loc:"<<loc<<" "<< st.top()<<"\n";
        count+=1;
        prev = loc+1;
        operand = operand.substr(prev,operand.length()+1);    
    }
    st.push(read_symtab(operand));
    cout<<"xx"<<st.top()<<"\n";
    count+=1;
    cout<<"EXPRESSION HANDLING ENDS\n";
    while(count>=0){
        int x = st.top();
        st.pop();
        int y = st.top();
        if(y==-1){
            return x;
        }
        st.pop();
        cout<<"xyz"<<x<<"|"<<y;
        y = y - x;
        //st.pop();
        st.push(y);
        count--;
    }
    return st.top();
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
        clear_file(pgmname+"_symtab.txt");
        clear_file(pgmname+"_intermediate.txt"); 
    }
    else if(!strcmp(opc,"BASE")){
        return;
    }
    else if(!strcmp(opc,"USE")){
        return;
    }
    else if(!strcmp(opc,"EQU")){
        if(operand.compare("*")==0){
            decToHex(locctr);
            insert_symtab(label,1,val);
        }
        else{
            int v;
            if(operand.find('+')!=string::npos || operand.find('-')!=string::npos){
                //handle expressions
                v = handle_expression(operand);
                cout<<v;
            }
            else{
                v = read_symtab(operand);
            }
             decToHex(v);
             insert_symtab(label,1,val);
            //insert_symtab(label,1,operand);
        }
        return;
    }
    if(!strcmp(opc,"END")){
        return;
    }
    if(strcmp(label.c_str(),"")){
        if(!search_symtab(label)){
            insert_symtab(label);
        }
        else {
            err_flag = 2;
        }
        
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
            if(label.compare("")==0){
                label = "***";
            }
            write_intermediate(label+" "+opcode+" "+operand);
        }
    }
    else{
        cout<<"unable to open source file";
    }
    source.close();
    return 0;
}





