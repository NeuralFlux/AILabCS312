#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
using namespace std;
int main()
{
    fstream file; 
    string word, t, q, filename; 
    filename = "a.txt"; 
    char input[255];
    int count = 0;
    int breadth =0;
    file.open(filename.c_str()); 
    while (getline(file,word)) 
    { 
        cout << word << endl;
        int n = word.length();
        breadth = n;
        char char_array[n+1];
        strcpy(char_array,word.c_str());
        for(int i=0; i<n; i++)
        {
        	cout<<char_array[i];
        	cout<<"\n";
        	input[count] = char_array[i];
        	count = count+1;
        } 
    }
    
    return 0;
}
