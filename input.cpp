#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
using namespace std;
struct node{
	char data;
	node* up;
	node* down;
	node* left;
	node* right; 
};
int main()
{
    fstream file; 
    //variables intialized to read the input file
    string word, filename; 
    filename = "a.txt";
    //count - # of rows
    //breadth - # of columns
    //input - array of all chars in the file
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
        	input[count] = char_array[i];
        	count = count+1;
        }
    }
    
    vector<node> inp;
    
    
    
    return 0;
}
