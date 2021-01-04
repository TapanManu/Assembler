#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char pgmname[25]; 
int err_flag=0;
char objcode[7];
char *OBJ;
char opvalue[5];
char symvalue[20];
char undef[20];
int locctr = 0;

void create_symtab(){
    FILE *fileptr;
    fileptr = fopen("symtab.txt","w");
    fputs("",fileptr);
    fclose(fileptr);
}


void insert_symtab(char symbol[]){
    FILE *fileptr;
    fileptr = fopen("symtab.txt","a");
    printf("%s",symbol);
    strcat(symbol," ");
    char loc[10];
    sprintf(loc, "%d", locctr);
    strcat(symbol,loc);
    strcat(symbol,"\n");
    fputs(symbol,fileptr);
    fclose(fileptr);
}

char* read_symtab(char search[]){
    FILE *fileptr;
    char lin1[255],sym[20];
    fileptr = fopen("symtab.txt","r");
    if(!strcmp(search,"")){
        return "";
    }
   
   
    while(fgets(lin1,255,fileptr)){
        sscanf(lin1,"%s %s",sym,symvalue);
        if(!strcmp(sym,"0")){
            continue;
        }
        
        if(!strcmp(sym,search)){
           return symvalue;
        }
    }
    
    fclose(fileptr);
    return "";
    
}

char* read_optab(char search[]){
    FILE *fileptr;
    char line[255],op[20],l[20];
    fileptr = fopen("optab.txt","r");
    while(fgets(line,255,fileptr)){
        sscanf(line,"%s %s %s",op,l,opvalue);
       // printf("%s ---- %s",op,search);
        if(!strcmp(op,search)){
            return opvalue;
        }
    }
    fclose(fileptr);
    return NULL;
}

char* substr(char* str,int beg,int end){
    int len = end - beg + 1;
    char sub[len];
    int i = beg;
    int j=0;
    while(beg < end){
        sub[j++] = str[i++];
    }
    sub[j]='\0';
    return sub;
}

int find(char* str,char * substr){
    int i=0;
    int j=0;
    int index = -1;
    for(i=0;i<strlen(str);i++){
        if(substr[j]==str[i]){
            if(j==0)
                index = i;
            j++;
        }
        else{
            j = 0;
        }
    }
    if(j == strlen(substr)){
        return index;
    }
    return -1;
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
    char* opvalue;
    int i;
    
    if(!strcmp(opcode,"START")){
        sscanf(operand,"%d",&i);
        locctr = i;
        printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"END")){
        return;
    }
    else if(!strcmp(opcode,"RESW")){
        sscanf(operand,"%d",&i);
        locctr+=3*i;
        printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"RESB")){
        sscanf(operand,"%d",&i);
        locctr+=i;
        printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"BYTE")){
        if(find(operand,"X'")!=-1){
            int len = strlen(operand);
            operand = substr(operand,2,len);
            locctr+= (len - 3)/2;
        }
        else if(find(operand,"C'")!=-1){
            int len = strlen(operand);
            operand = substr(operand,2,len);
            locctr+= (len - 3);
        }
        printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"WORD") || !strcmp(opcode,"RSUB")){
        locctr+=3;
        printf("%d\n",locctr);
    }
    else if((opvalue = read_optab(opcode))!=NULL){
        locctr+=3;
        printf("%d\n",locctr);
        
    }
}



int main(int argc, char* argv[]){
    //single pass assembler design
    FILE *fp;
    strcpy(pgmname,argv[1]);
    create_symtab();
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
        //printf("label:%s\n opcode:%s\n operand:%s\n",label,opcode,operand);
    }
    fclose(fp);
    return 0;

}