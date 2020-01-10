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
       // cout << word << endl;
        int n = word.length();
        breadth = n;
        char char_array[n+1];
        strcpy(char_array,word.c_str());
        for(int i=0; i<n; i++)
        {
		cout << char_array[i] << '\n';
        	input[count] = char_array[i];
        	count = count+1;
        }
    }
    
    vector<node*> inp;
    // inserting data into the nodes
    for(int i=0; i<count; i++)
        {
		node* x = new node;
		x->data = input[i];
		inp.push_back(x);
        }
   //assigning left up rigth down nodes
   int k=0;
   int a=0;
   while(k < count/breadth)
   {
	
   	for(int i=a; i<a+breadth; i++)
  	{
		if(i == breadth-1)
		{  inp[i]->right = NULL;  }
		else
		{  inp[i]->right = inp[i+1];  }
		if(i == a)
		{  inp[i]->left = NULL;  }
		else
		{  inp[i]->left = inp[i-1];  }
 	}
	a = a+breadth;
	k++;
   }
   int h=0;
   int j=breadth;
   int t=0;
   while(h < count/breadth-1)
   {
	int i=j;
	while(i<j+breadth)
	{
		inp[i]->up = inp[i-breadth];
		i++;
	}
	int g=t;
	while(g < count-breadth)
	{
		inp[g]->down = inp[g+breadth];
		g++;
	}
	t=t+breadth;
	j=j+breadth;
	h++;
   }

    return 0;
}
