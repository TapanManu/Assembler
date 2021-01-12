#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char pgmname[25]; 
int err_flag=0;
char objcode[7];
char nonloc[100][4];
char nonobj[100][5];
char text[100][7];
int nonx[100];
char* TEXT;
int z=0;
int pos=0;
int lab[100]; //if 1 no modification at that position else modification
int valid = 0,ts=1;
char *sub ;
char zero[2]="0";
char opvalue[5];
char symvalue[20];
char undef[20],lc[20];
int locctr = 0,start = 0,text_start=0,text_len=0;
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

char index_str(char c){
    char hex[] = {'8','9','A','B','C','D','E','F'};
    int d = c - '0';
    return hex[d];
}

char* read_list(char loc[]){
    FILE *fileptr;
    char line[255],op[20],l[20];
    fileptr = fopen("forward_list.txt","r");
    while(fgets(line,255,fileptr)){
        sscanf(line,"%s %s",op,lc);
       // printf("%s ---- %s",op,loc);
        if(!strcmp(op,loc)){
            return lc;
        }
    }
    fclose(fileptr);
    return NULL;
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



void write_header(){
    
    char* HEADER,*pg,*res;
    HEADER = (char*)malloc(16);
    pg = (char*)malloc(4);
    res = (char*)malloc(6);
    strcpy(HEADER,"H^");
    pg = substr(pgmname,0,6);
    strcat(pg,"^");
    strcat(HEADER,pg);
    res = decToHexa(start);
    while(strlen(res)<6){
        strcpy(zero,"0");
        strcat(zero,res);
        strcpy(res,zero);
    }
    strcat(HEADER,res);
    strcat(HEADER,"^");
    res = decToHexa(pgmlen);
    while(strlen(res)<6){
        strcpy(zero,"0");
        strcat(zero,res);
        strcpy(res,zero);
    }
    strcat(HEADER,res);
    printf("%s\n",HEADER);
    
}

void init_text(){
    
    TEXT = (char*)malloc(81);
    strcpy(TEXT,"T^");
    char* st;
    st = (char*)malloc(8);
    st = decToHexa(text_start);
    while(strlen(st)<6){
        strcpy(zero,"0");
        strcat(zero,st);
        strcpy(st,zero);
    }
    strcat(TEXT,st);
    strcat(TEXT,"^");
    strcat(TEXT,"00^");
    text_len = 0;
}

void write_text(){
    char *tl ;
    tl = (char*)malloc(3);
    tl = decToHexa(text_len);
    while(strlen(tl)<2){
            strcpy(zero,"0");
            strcat(zero,tl);
            strcpy(tl,zero);
         }
    TEXT[9]=tl[0];              //updating text length
    TEXT[10]=tl[1];
    printf("%s\n",TEXT);
}

int app_text(char objcode[]){
    if(!strcmp(objcode,"")){
        text_start = text_len + text_start;
        write_text();
        init_text();
    }
    if((text_len + strlen(objcode) <= 30)){
        strcat(TEXT,objcode);
        strcat(TEXT,"^");
        text_len +=strlen(objcode);
    }
    else{
        char *tl ;
        tl = (char*)malloc(3);
        tl = decToHexa(text_len);
        while(strlen(tl)<2){
            strcpy(zero,"0");
            strcat(zero,tl);
            strcpy(tl,zero);
         }
        TEXT[9]=tl[0];              //updating text length
        TEXT[10]=tl[1];
        printf("%s\n",TEXT);
        return 0;       //overflow text record
    }
    return 1;       //continue 
}





void write_end(){
    write_text();
    char* END,*res;
    END = (char*)malloc(10);
    res = (char*)malloc(7);
    strcpy(END,"E^");
    res = decToHexa(start);
    while(strlen(res)<6){
        strcpy(zero,"0");
        strcat(zero,res);
        strcpy(res,zero);
    }
    strcat(END,res);
    printf("%s\n",END);
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
        pos = 0;
        text_start = i;
       // printf("%d\n",locctr);
    }
    else if(!strcmp(opcode,"END")){
        pgmlen = locctr + 3 - start;
        return;
    }
    else if(!strcmp(opcode,"RESW")){
        sscanf(operand,"%d",&i);
        locctr+=3*i;
        strcpy(text[pos],"");
        lab[pos++]=-1;
        
    }
    else if(!strcmp(opcode,"RESB")){
        sscanf(operand,"%d",&i);
        locctr+=i;
        strcpy(text[pos],"");
        lab[pos++]=-1;
        
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
        strcpy(text[pos],obj);
        lab[pos++]=1;
      // printf("objcode:%s\n",obj);
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
        strcpy(text[pos],obj);
        lab[pos++]=1;
        //printf("objcode:%s\n",obj);
        valid = 0;
    }
    else if(!strcmp(opcode,"RSUB")){
        locctr+=3;
        //printf("%d\n",locctr);
        strcpy(obj,"4C0000");
        strcpy(text[pos],obj);
        lab[pos++]=1;
        //printf("objcode:%s\n",obj);
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
            val = -1;
            sscanf(operand,"%d",&val);
            if(valid == 1){
            if(val==-1){
                if(find(operand,",X")!=-1){
                    int len = strlen(operand);
                    operand = substr(operand,0,len-2); 
                    x = 1;
                }
                add_forward_list(operand);
                
               strcpy(nonobj[pos],obj);       //here obj contains opvalue
               strcpy(nonloc[pos],decToHexa(locctr));
                nonx[pos] = x;
                //store the location with undefined operands
                //and store corrsponding obj codes
                strcat(obj,"0000");
                strcpy(text[pos],obj);
                lab[pos++]=0;
            }
            
            
            }
        }
        else{
            char opcode[10];
            int val = -1;
            sscanf(operand,"%d",&val);
            if(val==-1){
             if(find(operand,",X")!=-1){
                    int len = strlen(operand);
                    operand = substr(operand,0,len-2); 
                    x = 1;
                }
            strcpy(opcode,obj);
            while(strlen(operval)<4){
                strcpy(zero,"0");
                strcat(zero,operval);
                strcpy(operval,zero);
            }
            if(x==1){
                char ch = index_str(operval[0]);
                operval[0] = ch;
            }
            strcat(opcode,operval);
            }
            strcpy(text[pos],opcode);
            lab[pos++]=1;
            //printf("objcode:%s\n",opcode);
        }     
    
    }
    else{
        
        printf("yyyyyyyyyyy%s\n",obj);
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
    int br=0;
    write_header();
    init_text();
    for(int i=0;i<pos;i++){
        if(br==1){
            if(lab[i]==-1){
                continue;
            }
            else{
                
                write_text();
                text_start = text_start + text_len;
                init_text();
                br = 0;
            }
        }
        
        if(lab[i]==0){
            
            char* symbol = read_symtab(read_list(nonloc[i]));
            char opcode[10];
            strcpy(opcode,nonobj[i]);
            
        // printf("loc:%s\n",symbol);
        // printf("obj:%s\n",nonobj[i]);
            while(strlen(symbol)<4){
                    strcpy(zero,"0");
                    strcat(zero,symbol);
                    strcpy(symbol,zero);
                }
            if(nonx[i]==1){
                char ch = index_str(symbol[0]);
                symbol[0] = ch;
            }
            strcat(opcode,symbol);
            strcpy(text[i],opcode);
            
        }
        else if(lab[i]==-1){
            if(!strcmp(text[i],"")){
                br = 1;
                
                continue;
            }
            
        }
        if(app_text(text[i])==1){
                continue;
            }
        else{
            text_start = text_start + text_len;
            init_text();
            app_text(text[i]);
        
        }
        
        //printf("objcode:%s\n",text[i]);
    }
    
    write_end();
    fclose(fp);
    return 0;

}