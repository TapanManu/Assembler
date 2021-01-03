#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char pgmname[25]; 
int err_flag=0;
char objcode[7];
char *OBJ;
char *FORWARD_LIST;
char undef[20];
int locctr = 0;

void create_symtab(char name[]){
    FILE *fileptr;
    fileptr = fopen(strcat(name,"_symtab.txt"),"w");
    fclose(fileptr);
}


void insert_symtab(char name[],char symbol[]){
    FILE *fileptr;
    fileptr = fopen(strcat(name,"_symtab.txt"),"a");
    strcat(symbol," ");
    char loc[10];
    sprintf(loc, "%d", locctr);
    strcat(symbol,loc);
    strcat(symbol,"\n");
    fputs(symbol,fileptr);
    fclose(fileptr);
}

char* read_symtab(char name[],char search[]){
    FILE *fileptr;
    char line[255],sym[20],value[20];
    if(!strcmp(search,"")){
        return "x";
    }
    fileptr = fopen(strcat(name,"_symtab.txt"),"r");
    while(fgets(line,255,fileptr)){
        sscanf(line,"%s %s",sym,value);
        printf("xxx%s",sym);
        if(!strcmp(sym,search)){
           return value;
        }
    }
    fclose(fileptr);
    return "x";
}

char* read_optab(char search[]){
    FILE *fileptr;
    char line[255],op[20],value[5];
    fileptr = fopen("optab.txt","r");
    while(fgets(line,255,fileptr)){
        sscanf(line,"%s %s",op,value);
        if(!strcmp(op,search)){
            return value;
        }
    }
    fclose(fileptr);
    return NULL;
}

void add_forward_list(char name[],char operand[]){
    FILE *fileptr;
    fileptr = fopen(strcat(name,"_forward_list.txt"),"a");
    strcat(operand," ");
    char loc[10];
    sprintf(loc, "%d", locctr+1);
    strcat(operand,loc);
    strcat(operand,"\n");
    fputs(operand,fileptr);
    fclose(fileptr);
}

void pass(char label[],char opcode[],char operand[]){
    char value[20];
    char subop[30];
    int x = 0;
    int i;
    
    if(strcmp(label,"")){
       // read_symtab(pgmname,label);  ==> segmentation fault error
       // strcpy(value,read_symtab(pgmname,label));
        printf("%s",label);
    }
    if(!strcmp(value,"")){
        //no symbol present
        insert_symtab(pgmname,label);
    }
    
    else if(!strcmp(value,operand)){
        char opvalue[5];
        strcpy(opvalue,read_optab(opcode));
        if(!strcmp(opvalue,"")){
            if(!strcmp(opcode,"START")){
                sscanf(operand,"%d",&i);
                locctr=i;
                return;
            }
            else if(!strcmp(opcode,"END")){
                //check forward list empty or not

                return;
            }
            else if(!strcmp(opcode,"RESW")){
                sscanf(operand,"%d",&i);
                locctr+=3*i;
            }
            else if(!strcmp(opcode,"RESB")){
                sscanf(operand,"%d",&i);
                locctr+=i;
            }
            else if(!strcmp(opcode,"BYTE")){
                if(operand[0]=='C'){
                    memcpy(subop,&operand,2);
                    int len = strlen(operand) - 2;
                    subop[len] = '\0';
                    locctr +=len;
                }
                else if (operand[0]=='X'){
                    memcpy(subop,&operand,2);
                    int len = strlen(operand) - 2;
                    subop[len] = '\0';
                    locctr +=(len/2);
                }
                
            }
            else if(!strcmp(opcode,"WORD")){
                locctr+=3;
            }
            else{
                err_flag = 2;
            }
        }
        else{
            strcpy(objcode,opvalue);
            
            if(strstr(operand,",X")!=NULL){
                int len = strlen(operand) - 2;
                memcpy(subop,&operand[len],0);
                subop[len] = '\0';
                strcpy(operand,subop);
                locctr +=3;
                x = 1;
            }
            strcpy(opvalue,read_symtab(pgmname,operand));
            if(!strcmp(opvalue,"")){
                add_forward_list(pgmname,operand);
            }
            else{
                if(x==1){
                    if(isdigit(operand[0])){
                        int v = operand[0] - '0';
                        v = v + 8;
                        operand[0] = (char)v + '0';
                    }
                    else{
                        int v = operand[0] - 'a';
                        v = v + 8;
                        operand[0] = (char)v + 'a';
                    }
                    strcpy(objcode,operand);
                }
                else{
                    //convert to hex
                strcpy(objcode,operand);
                }
            }
            
        }
    

    }
    else if(!strcmp(value,label)){
        //duplication
        err_flag = 1;
    }



    if(err_flag == 1){
        printf("duplication symbol error");
        exit(1);
    }
    else if(err_flag == 2){
        printf("unknown object code error");
        exit(1);
    }
    
}



int main(int argc, char* argv[]){
    //single pass assembler design
    FILE *fp;

    

    strcpy(pgmname,argv[1]);
    create_symtab(pgmname);
    fp = fopen(argv[1],"r");
    char line[255],label[20],opcode[20],operand[20];
    
    
    while(fgets(line, 255, fp)) {
        //printf("%s\n",line);
        label[0]=0; opcode[0]=0; operand[0]=0;    //clear string
        sscanf(line,"%s %s %s",label,opcode,operand);
        if(operand[0]==0){
            strcpy(operand,opcode);
            strcpy(opcode,label);
            label[0]=0;
        }
        pass(label,opcode,operand);
        printf("label:%s\n opcode:%s\n operand:%s\n",label,opcode,operand);
    }
    fclose(fp);
    return 0;

}