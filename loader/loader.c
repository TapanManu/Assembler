#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

char* sub;
int startaddr=0;
char* hexval;

void clear_map(){
    FILE *fp;
    fp = fopen("loadmap.txt","w");
    fputs("",fp);
    fclose(fp);
}

char hexcode(int c){
    char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    return hex[c];
}

char* reversestr(char* str){
    char temp[strlen(str)];
    int pos=0;
    for(int i=strlen(str)-1;i>=0;i--){
        temp[pos++] = str[i];
    }
    temp[pos]='\0';
    strcpy(str,temp);
    return str;
}

char* decToHex(int val){
    hexval = (char*)malloc(15);
    int pos=0;
    strcpy(hexval,"");
    while(val!=0){
        int d = val%16;
        hexval[pos++] = hexcode(d);
        val = val/16;
    }
    hexval[pos]='\0';
    return reversestr(hexval);  
    //return "abc";
}



int hexToDec(char addr[]){
    int len = strlen(addr);
    int num=0;
    for(int i=0;i<len;i++){
        int ch = addr[i]-'0';
        if(ch>10){
            ch = ch - 7;
        }
        num+=(int)(ch*pow(16,len-i-1));
    }
    return num;
}

char* substring(char str[],int beg,int end){
    int len = end-beg+1;
    char temp[len];
    int j=0;
    for(int i=beg;i<end;i++){
        temp[j++] = str[i];
    }
    temp[j]='\0';
    sub = (char*)malloc(len);
    strcpy(sub,temp);
    return sub;
}

void write_loader(int loc,char obj[]){
    //file to demonstrate loadmap
    FILE *fp;
    fp = fopen("loadmap.txt","a");
    char* res;
    res = (char*)malloc(2);
    res = substring(obj,0,2);
    fprintf(fp,"%s  %s  ",decToHex(loc),res);
    res = substring(obj,2,4);
    fprintf(fp,"%s  %s  ",decToHex(loc+1),res);
    res = substring(obj,4,6);
    fprintf(fp,"%s  %s  \n",decToHex(loc+2),res);
    fclose(fp);
}

int find(char str[],int pos,char search){
    for(int i=pos;i<strlen(str);i++){
        if(str[i]==search){
            return i;
        }
    }
    return -1;
}

void scan_header(char head[]){
    //count delimiter ^
    int startloc=0;
    char* addr;
    int endloc=0;
    int count=0;
    for(int i=0;i<strlen(head);i++){
        
        if(head[i]=='^')
            count++;
        if(count==2 && head[i]=='^'){
            startloc=i+1;
            //printf("s:%d\n",startloc);
        }
        if(count==3){
            endloc=i;
            //printf("d:%d\n",endloc);
            break;
        }
    }
    addr = (char*)malloc(endloc-startloc+1);
    strcpy(addr,substring(head,startloc,endloc));
    startaddr = hexToDec(addr);
}

void scan_text(char line[]){
    int start=12;       //first objcode position
    for(int i=start;i<strlen(line);i+=7){
        write_loader(startaddr,substring(line,i,i+7));
        startaddr = startaddr+3;
    }
}

int main(int argc,char argv[]){
    FILE* input;
    char line[255];
    clear_map();
    input = fopen("input.obj","r");

    fgets(line,255,input);
    scan_header(line);

    while(fgets(line,255,input)){
       scan_text(line); 
    }
    fclose(input);
    return 0;
}