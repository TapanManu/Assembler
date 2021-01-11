#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
void expand();
void define(FILE*,char[],char[]);
void read_namtab(char[]);

int expanding = 0;
int macro_index = 0;
int defloc = 0;         //location ponter in deftab
int NAMTAB[10][2];
char ARGTAB[10][15];    //max no of args allowed in macro = 10
char* pgmname;
char label[10],opcode[10],operand[10];
int found = 0;
char* defline;

void clear_src(){
    FILE *fp;
    fp = fopen("expanded.asm","w");
    fputs("",fp);
    fclose(fp);
}

void clear_name(){
    FILE *fp;
    fp = fopen("namtab.txt","w");
    fputs("",fp);
    fclose(fp);
}

void write_src(char line[]){
    FILE *fp;
    fp = fopen("expanded.asm","a");
    fputs(line,fp);
    fclose(fp);
}


void write_source(char label[],char opcode[],char operand[]){
    FILE *fp;
    fp = fopen("expanded.asm","a");
    fprintf(fp,"%s\t%s\t%s\n",label,opcode,operand);
    fclose(fp);
}

void get_line(){
    if(expanding == 1){
        //read deftab
        //substitute argtab for positional notation
    }
    return;
}

void processline(FILE* fp,char label[],char opcode[],char operand[]){
    found = 0;
    read_namtab(opcode);        //  search for macro name
    if(found==1){
        printf("\tMACRO\n");
        expand();
    }
    else if(!strcmp(opcode,"MACRO")){
        define(fp,label,operand);
    }
    else{
        write_source(label,opcode,operand);
    }
    
}

void insert_deftab(char defline[]){
    FILE *f;
    f = fopen("deftab.txt","a");
    strcat(defline,"\n");
    fputs(defline,f);
    fclose(f);
}

void read_deftab(){
    
}


void read_namtab(char opcode[]){
    FILE *fp;
    fp = fopen("namtab.txt","r");
    char lin[255],*macro_name;
    int macro_indx,start_indx,end_indx;
    macro_name = (char*)malloc(15);         //max macro name size == 15

    while(fgets(lin,255,fp)){
        sscanf(lin,"%d %s %d %d",&macro_index,macro_name,&start_indx,&end_indx);
       // printf("%s",macro_name);        
        if(!strcmp(opcode,macro_name)){
            found = 1;
        }

    }

    fclose(fp);
}

void create_argtab(char args[]){
    int i=0;
    int argcount=-1;
    int k=0;
    for(i=0;i<strlen(args);i++){
        if(args[i]=='&'){
            argcount++;
            k=0;
        }
        else if(args[i]==','){
            ARGTAB[argcount][k] = '\0';
            continue;
        }
        else if(i==(strlen(args)-1)){
            ARGTAB[argcount][k] = '\0';
        }
        else{
            ARGTAB[argcount][k++] = args[i];
        }
    }
}

void insert_namtab(char name[]){
    FILE *fp;
    fp = fopen("namtab.txt","a");
    fprintf(fp,"%d %s\n",macro_index,name);
    NAMTAB[macro_index][0]=defloc;
    NAMTAB[macro_index][1]=-1;          //end position not determined
    macro_index++;
    fclose(fp);
}

void expand(){
    expanding = 1;      
    FILE *fp;
    fp = fopen("deftab.txt","r");       //read deftab
    char lab[10],opc[10],oper[10];
    defline = (char*)malloc(255);
    fgets(defline,255,fp);
    lab[0]=0; opc[0]=0; oper[0]=0;
    sscanf(defline,"%s %s %s",lab,opc,oper);
    
    //set up argtab , map posiitonal index
    //macro invocation to expanded file as comment
    while(strcmp(opc,"MEND")){
        lab[0]=0; opc[0]=0; oper[0]=0;
        fgets(defline,255,fp);
        sscanf(defline,"%s %s %s",lab,opc,oper);
        if(lab[0]=='.'){
            write_source(lab,opc,oper);
            continue;
        }
        if(oper[0]==0){
            strcpy(oper,opc);
            strcpy(opc,lab);
            strcpy(lab,"");
        }
        get_line();
        processline(fp,lab,opc,oper);
    }
    expanding = 0;
    fclose(fp);
}

void define(FILE *fp,char label[],char oper[]){
    insert_namtab(label);
    char defline[255];
    fgets(defline,255,fp);
    insert_deftab(defline);
    create_argtab(oper);        //creating variable argument list
    int level=1;
    while(level>0){
        fgets(defline,255,fp);
        if(defline[0]=='.'){
            write_src(defline);     //comments
        }
        //search for positional param
        
    }

}


int main(int argc,char* argv[]){
    FILE *input;
    int lineno = 0;
    char line[255];

    clear_name();
    clear_src();
    
    input = fopen(argv[1],"r");

    while(fgets(line,255,input)){
        label[0]=0;
        opcode[0]=0;
        operand[0]=0;
        sscanf(line,"%s %s %s",label,opcode,operand);
        if(label[0]=='.'){
            write_src(line);
            continue;
        }
        if(operand[0]==0){
            strcpy(operand,opcode);
            strcpy(opcode,label);
            strcpy(label,"");
        }
        
        get_line();
        processline(input,label,opcode,operand);
    }

    fclose(input);
    return 0;
}
