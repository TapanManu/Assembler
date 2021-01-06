#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char pgmname[25]; 
int err_flag=0;
char objcode[7];
char nonloc[100][4];
char nonobj[100][5];
int z=0;
int valid = 0;
char *sub ;
char zero[2]="0";
char opvalue[5];
char symvalue[20];
char undef[20];
int locctr = 0,start = 0;
int pgmlen=0;
char res[10];

void create_symtab(){
    FILE *fileptr;
    fileptr = fopen("symtab.txt","w");
    fputs("",fileptr);
    fclose(fileptr);
}
void create_list(){
    FILE *fileptr;
    fileptr = fopen("forward_list.txt","w");
    fputs("",fileptr);
    fclose(fileptr);
}

// function to convert decimal to hexadecimal 
char* decToHexa(int n) {    
    // char array to store hexadecimal number 
    char hexaDeciNum[10]; 
    
    // counter for hexadecimal number array 
    int i = 0; 
    int k = 0;
    while(n!=0){    
        // temporary variable to store remainder 
        int temp  = 0; 
        // storing remainder in temp variable. 
        temp = n % 16; 
        // check if temp < 10 
        if(temp < 10) { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 
        else{ 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
        n = n/16; 
    } 
    // printing hexadecimal number array in reverse order 
    for(int j=i-1; j>=0; j--) 
        res[k++] = hexaDeciNum[j];
    res[k]='\0';
    return res; 
} 


void insert_symtab(char symbol[]){
    FILE *fileptr;
    fileptr = fopen("symtab.txt","a");
    //printf("%s",symbol);
    strcat(symbol," ");
    char* loc;
    loc = decToHexa(locctr);
    //sprintf(loc, "%d", locctr);
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
    char* temp;
    temp = (char*)malloc(len);
    sub = (char*)malloc(len);
    int i = beg;
    int j=0;
    while(i < end){
       temp[j++] = str[i++];
    }
    temp[j]='\0';
    strcpy(sub,temp);
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

void add_forward_list(char operand[]){
    FILE *fileptr;
    fileptr = fopen("forward_list.txt","a");
    char* loc;
    loc = decToHexa(locctr);
    strcat(loc," ");
    strcat(loc,operand);
    strcat(loc,"\n");
    fputs(loc,fileptr);
    fclose(fileptr);
}

void pass(char label[],char opcode[],char operand[]){
    char value[20];
    char* opvalue,*obj;
    char* symval,*operval;
    obj = (char*)malloc(2);
    int i;
    int x = 0;
    valid = 0;
    if(!strcmp(opcode,"START")){
        sscanf(operand,"%d",&i);
        locctr = i;
        start = i;
       // printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"END")){
        pgmlen = locctr + 3 - start;
        return;
    }
    else if(!strcmp(opcode,"RESW")){
        sscanf(operand,"%d",&i);
        locctr+=3*i;
       // printf("%d\n",locctr);
        
    }
    else if(!strcmp(opcode,"RESB")){
        sscanf(operand,"%d",&i);
        locctr+=i;
        //printf("%d\n",locctr);
        
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
        valid = 0;
        strcpy(obj,operand);
        while(strlen(obj)<6){
            strcpy(zero,"0");
            strcat(zero,obj);
            strcpy(obj,zero);
        }
       printf("obj:%s\n",obj);
    }
    else if(!strcmp(opcode,"WORD")){
        locctr+=3;
        //printf("%d\n",locctr);
        strcpy(obj,operand);
        while(strlen(obj)<6){
            strcpy(zero,"0");
            strcat(zero,obj);
            strcpy(obj,zero);
        }
        //printf("obj:%s\n",obj);
        valid = 0;
    }
    else if(!strcmp(opcode,"RSUB")){
        locctr+=3;
        //printf("%d\n",locctr);
        strcpy(obj,"4F0000");
        //printf("%s",obj);
        return;
    }
    else if((opvalue = read_optab(opcode))!=NULL){
        locctr+=3;
        strcpy(obj,opvalue);
        valid = 1;
    }
    symval = read_symtab(label);
    if(!strcmp(symval,"")){
        if(strcmp(label,"")){ 
            insert_symtab(label);
        }
    }
    if(strcmp(operand,"")){ 
        
        operval = read_symtab(operand);
        if(!strcmp(operval,"")){ 
            int val;
            val = 0;
            sscanf(operand,"%d",&val);
            if(valid == 1){
            if(val==0){
                if(find(operand,",X")!=-1){
                    int len = strlen(operand);
                    operand = substr(operand,0,len-2); 
                    x = 1;
                }
                add_forward_list(operand);
                
               strcpy(nonobj[z],obj);       //here obj contains opvalue
               strcpy(nonloc[z++],decToHexa(locctr));
                //store the location with undefined operands
                //and store corrsponding obj codes
                //strcpy(obj,opvalue);
                strcat(obj,"0000");
            }
            
            printf("%s\n",obj);
            }
        }
        else{
            printf("%s\n",obj);
        }     
    
    }
    else{
        
        printf("%s\n",obj);
    }
   
    
}



int main(int argc, char* argv[]){
    //single pass assembler design
    FILE *fp;
    strcpy(pgmname,argv[1]);
    create_symtab();
    create_list();
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
    for(int i=0;i<z;i++){
        printf("loc:%s\n",nonloc[i]);
        printf("obj:%s\n",nonobj[i]);
    }
    fclose(fp);
    return 0;

}