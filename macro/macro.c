#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

int expanding = 0;
int macro_index = 0;
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

void insert_deftab(){

}

void read_deftab(){
    
    defline = (char*)malloc(30);



    
}


void read_namtab(char opcode[]){
    FILE *fp;
    fp = fopen("namtab.txt","r");
    char lin[255],*macro_name;
    int macro_indx,start_indx,end_indx;

    macro_name = (char*)malloc(15);         //max macro name size == 15

    while(fgets(lin,255,fp)){
        sscanf(lin,"%d %s %d %d",&macro_index,macro_name,&start_indx,&end_indx);

        if(!strcmp(opcode,macro_name)){
            found = 1;
        }

    }

    fclose(fp);
}

void insert_namtab(){
    FILE *fp;
    fp = fopen("namtab.txt","a");

    fclose(fp);
}

void expand(){
    expanding = 1;
    FILE *fp;
    fp = fopen("deftab.txt","r");
    //read deftab
    //set up argtab , map posiitonal index
    //macro invocation to expanded file as comment
    while(!strcmp(opcode,"MEND")){
        get_line();
        processline(label,opcode,operand);
    }
    expanding = 0;
    fclose(fp);
}

void define(){

}

void get_line(){
    if(expanding == 1){
        //read deftab
        //substitute argtab for positional notation
    }
    return;
}

void processline(char label[],char opcode[],char operand[]){
    found = 0;
    read_namtab(opcode);        //  search for macro name
    if(found==1){
        expand();
    }
    else if(!strcmp(opcode,"MACRO")){
        define();
    }
    else{
        write_source(label,opcode,operand);
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
        printf("label:%s opcode:%s\n",label,opcode);
        get_line();
        processline(label,opcode,operand);
    }

    fclose(input);
    return 0;
}
