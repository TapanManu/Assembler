
// C++ program to words in a sentence. 
#include <bits/stdc++.h> 
using namespace std; 

stack <int> s;
void decToHex(int ctr){
    char val[20];
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
    cout<<val<<"\n";
}
  
// Driver function 
int main() 
{ 
    int ctr = 256;
    decToHex(ctr);
    return 0; 
}
