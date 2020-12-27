int search_symtab(string label){
    fstream symstream;
    string line,comp;
    symstream.open("symtab.txt",ios::in);
    while(getline(symstream,line)){
        stringstream syms(line);
        syms >> comp;
        if(!strcmp(comp.c_str(),label.c_str())){
            cout<<comp<<label;
            return 0;  //if element present
        }
    }
    return 1;       //if not present
}

int valid_opcode(string opcode){
    fstream opstream;
    
    string line,comp;
    opstream.open("optab.txt",ios::in);
    while(getline(opstream,line)){
        stringstream ops(line);
        ops >> comp;
        if(!strcmp(comp.c_str(),opcode.c_str())){
           // if opcode is present
            return 1;
        }
    }
    return 0; //if not
}

int get_opcode_length(string opcode){
    fstream opstream;
    int length=0;
    string line,comp;
    opstream.open("optab.txt",ios::in);
    while(getline(opstream,line)){
        stringstream ops(line);
        ops >> comp;
        if(!strcmp(comp.c_str(),opcode.c_str())){
            ops >> length;
            return length;
        }
    }
    return 0;
}

string get_opcode_value(string opcode){
    fstream opstream;
    int length=0;
    string value;
    string line,comp;
    opstream.open("optab.txt",ios::in);
    while(getline(opstream,line)){
        stringstream ops(line);
        ops >> comp;
        if(!strcmp(comp.c_str(),opcode.c_str())){
            ops >> length;
            ops >> value;
            return value;
        }
    }
    return null; 
}