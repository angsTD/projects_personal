//============================================================================
// Name        : ABC.cpp
// Author      : AngsTD
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

string addChar(string a,string str,int K,int N, int k, int numA, int numB,int numC)
{

	char ch = a[0];
	if(ch == 'A')
	{
		k = k + numB + numC;
		numA++;
	}
	else if(ch == 'B')
	{
		k = k + numC;
		numB++;
	}
	else
	{
		numC++;
	}
	str.append(a);

	if(str.length()==N && k == K)
	{
		return str;
	}

	for(int i=0;i<3;i++)
	{
		cout<<"Blah Blah"<<endl;

	}


}
int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
