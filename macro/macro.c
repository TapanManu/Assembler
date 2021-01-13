#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
void expand();
void define(FILE*,char[],char[]);
int read_namtab(char[]);
void insert_deftab(char*);

int expanding = 0;
int arglen=0;               //argtab length
int macro_index = 0;
int defloc = 0;
int exploc=0;         //location ponter in deftab
int NAMTAB[10][2];
char ARGTAB[10][15];    //max no of args allowed in macro = 10
char* pgmname, *sub;
char label[10],opcode[10],operand[30];
int found = 0;
int err_flag;
char *defline=NULL;

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

void clear_def(){
    FILE *fp;
    fp = fopen("deftab.txt","w");
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

void get_line(char label[],char opcode[],char operand[]){
    int j=-1;
    int k=0,l=0;
    int len = -1;
    char *temp;
    temp = (char*)malloc(50);
    char args[10][20];
    if(expanding == 1){
        for(int i=0;i<strlen(operand);i++){
            if(operand[i]=='?'){
                j = i+1;
                strcpy(args[k++],ARGTAB[operand[j]]);
                len = i;
                strcpy(temp,substr(operand,l,len));
                strcat(temp,args[k-1]);
                printf("temp:%s",operand);
                l+=(i+strlen(args[k]));
                //substituting args in argtab into positions
                //TODO
            }
        }
    }
    return;
}

void processline(FILE* fp,char label[],char opcode[],char operand[]){
    found = 0;
    int macronum = read_namtab(opcode);        //  search for macro name
    if(found==1){
       // printf("\tMACRO\n");
        expand(macronum,operand);
    }
    else if(!strcmp(opcode,"MACRO")){
        char* deflin;
        deflin = (char*)malloc(255);
        strcpy(deflin,label);
        strcat(deflin," ");
        strcat(deflin,opcode);
        strcat(deflin," ");
        strcat(deflin,operand);
        insert_deftab(deflin);
        define(fp,label,operand);
    }
    else{
        write_source(label,opcode,operand);
    }
    
}

void insert_deftab(char defline[]){
    FILE *f;
    f = fopen("deftab.txt","a");
    //strcat(defline,"\n");
    fputs(defline,f);
    fclose(f);
}



int read_namtab(char opcode[]){
    FILE *fp;
    fp = fopen("namtab.txt","r");
    char lin[255],*macro_name;
    int macro_indx,start_indx,end_indx;
    macro_name = (char*)malloc(15);         //max macro name size == 15

    while(fgets(lin,255,fp)){
        sscanf(lin,"%d %s",&macro_indx,macro_name);
       // printf("%s",macro_name);        
        if(!strcmp(opcode,macro_name)){
            found = 1;
            return macro_indx;
        }

    }
    return -1;
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
        else if(i==(strlen(args)-1) || args[i]=='\0'){
            ARGTAB[argcount][k++] = args[i];
            //ARGTAB[argcount][k] = '\0';
            printf("ARGS:%s",ARGTAB[argcount]);
        }
        else{
            ARGTAB[argcount][k++] = args[i];
        }
    }
    arglen = argcount;
}

void sub_argtab(char args[]){
    int i=0;
    int argcount=0;
    int k=0;
    for(i=0;i<strlen(args);i++){
        if(args[i]==','){
            ARGTAB[argcount][k] = '\0';
            argcount++;
            k=0;
        }
        else if(i==(strlen(args)-1) || args[i]=='\0'){
            ARGTAB[argcount][k] = '\0';
        }
        else{
            ARGTAB[argcount][k++] = args[i];
        }
    }
    arglen = argcount;
}

int search_argtab(char arg[]){
    for(int i=0;i<=arglen;i++){
        //printf("arg1:%s arg2:%s\n",ARGTAB[i],arg);
        if(!strcmp(ARGTAB[i],arg)){
            return i+1;
        }
    }
    return -1;      //if arg not present
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

void expand(int mac_indx,char args[]){
    expanding = 1;
    int br=0; 
    int length = 0;
    char arg[15];     
    FILE *fp;
    fp = fopen("deftab.txt","r");       //read deftab
    char lab[10],opc[10],oper[30];
    defline = (char*)malloc(255);

     //get the macroname  
    //get macro index
    //using that, insert into namtab, get startloc and endloc
    //which macro is expanding ==> info in lab


    int startloc = NAMTAB[mac_indx][0];
    printf("startloc:%d\n",startloc); 
    int i=0;
    while(i <= startloc){
        fgets(defline,255,fp);
        i++;
    }
    lab[0]=0; opc[0]=0; oper[0]=0;
    printf("defline:%s\n",defline);
    sscanf(defline,"%s %s %s",lab,opc,oper);

   
    //set up argtab , map posiitonal index

    sub_argtab(args);

    //macro invocation to expanded file as comment
    char dot[]=".\t";
    strcat(dot,defline);
    strcpy(defline,dot);
    write_src(defline);
    
    
    while(strcmp(opc,"MEND")){
        lab[0]=0; opc[0]=0; oper[0]=0;
        fgets(defline,255,fp);
        sscanf(defline,"%s %s %s",lab,opc,oper);
        
        if(lab[0]=='.'){
            write_src(defline);
            continue;
        }
        if(oper[0]==0){
            strcpy(oper,opc);
            strcpy(opc,lab);
            strcpy(lab,"");
        }
        if(!strcmp(opc,"MEND")){
            break;
        }
        get_line(lab,opc,oper);
        processline(fp,lab,opc,oper);
    }
    expanding = 0;
    fclose(fp);
}

void define(FILE *fp,char label[],char oper[]){
    insert_namtab(label);
    char* mtab;
    mtab = (char*)malloc(20);
   
    char defline[255],lab[20],opc[20],opr[20];
    fgets(defline,255,fp);
    insert_deftab(defline);
    create_argtab(oper);        //creating variable argument list
    defloc++;
    int level=1;
    while(level>0){
        fgets(defline,255,fp);
        if(!strcmp(defline,"")){
            err_flag = 1;
            return;
        }
        if(defline[0]=='.'){
            write_src(defline);     //comments
            continue;
        }
        lab[0]=0; opc[0]=0; opr[0]=0;
        sscanf(defline,"%s %s %s",lab,opc,opr);
        char arg[15];
        int j=0,cnt=0;
        int replacepos=-1;
        for(int i=0;i<strlen(defline);i++){
            if(cnt == 1){         //start counting fro arguments
                if(isalnum(defline[i])){
                    arg[j] = defline[i];
                    if(j==0){
                        replacepos = i;
                    }
                    else{
                        defline[i]=' ';
                    }
                    j++;
                }
                else if(i==strlen(defline)-1){
                    j = 0;
                    cnt = 0;
                    //compare it with argtab
                    printf("xyzwxx:%s\n",arg);
                    int s = search_argtab(arg);
                    printf("tttttt:%d\n",s);
                    if(s==-1){
                        err_flag = 2;   //argument not found
                    }
                     //replace the position value
                     
                    defline[replacepos] = s + '0';
                }
                else{
                    j = 0;
                    cnt = 0;
                    //compare it with argtab
                    int s = search_argtab(arg);
                    if(s==-1){
                        err_flag = 2;   //argument not found
                    }
                     //replace the position value
                     printf("ssssss:%d\n",s);
                    defline[replacepos] = s + '0';
                }
            }
            if(defline[i]=='&'){
                //search for positional param
                //locate param by begining with &
                cnt = 1;
                defline[i]='?';
            }
        }
        insert_deftab(defline);      //insert into definition table
        defloc++;
        if(opr[0]==0){
            strcpy(opr,opc);
            strcpy(opc,lab);
            strcpy(lab,"");
        }
        if(!strcmp(opc,"MACRO")){
            strcpy(mtab,lab);
            level+=1;
        }
        if(!strcmp(opc,"MEND")){
            
            level-=1;
            if(level==0){
                int index = read_namtab(label);
                NAMTAB[index][1] = defloc;
            }
            else{
                int index = read_namtab(mtab);
                NAMTAB[index][1] = defloc;
            }
            
        }
    }

}


int main(int argc,char* argv[]){
    FILE *input;
    int lineno = 0;
    char line[255];

    clear_name();
    clear_src();
    clear_def();
    
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
        
        get_line(label,opcode,operand);
        processline(input,label,opcode,operand);
    }

    fclose(input);
    return 0;
}
