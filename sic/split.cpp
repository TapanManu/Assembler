
// C++ program to words in a sentence. 
#include <bits/stdc++.h> 
using namespace std; 
  
void removeDupWord(char str[]) 
{ 
    int count=0;
    // Returns first token  
    char words[3][10];
    char *token = strtok(str, " "); 
   
    // Keep printing tokens while one of the 
    // delimiters present in str[]. 
    while (token != NULL) 
    { 
        strcpy(words[count++],token);
        cout<<words[count-1]<<endl; 
        token = strtok(NULL, " "); 
    } 
} 
  
// Driver function 
int main() 
{ 
    char str[] = "Geeks for Geeks"; 
    removeDupWord(str); 
    return 0; 
}
