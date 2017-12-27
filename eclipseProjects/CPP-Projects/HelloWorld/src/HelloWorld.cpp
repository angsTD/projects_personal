//============================================================================
// Name        : HelloWorld.cpp
// Author      : AngsTD
// Version     :
// Copyright   : Dont you Dare :P
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include<stdio.h>
#include<string.h>
#include "testheader.h"

using namespace std;

class ABBA
{
    public:
    string canObtain(string initial,string target);
    private:
    void tryValidMoves(string initial, string* target);
};

void ABBA::tryValidMoves(string initial, string* target)
{
	string targ = *target;
    string init = initial;
    //targLen = targ.length();

    char last = targ.back();

    cout<<"last character: "<<last<<endl;

    if(last == 'A')
    {
        targ.pop_back();
        cout<<"targ is  :"<<targ<<endl;
    }
    else
    {
        targ.pop_back();
        reverse(targ.begin(),targ.end());
        cout<<"targ is  :"<<targ<<endl;
    }
    *target = targ;

}
string ABBA::canObtain(string initial,string target)
{
    string init = initial;
    string targ = target;

    string possible = "Possible";
    string impossible = "Impossible";

    uint initLen = initial.length();
    uint tarLen = target.length();

    bool isStringEq = false;

    while(initLen!=tarLen)
    {

    	cout<<"Before Move: init length: "<<initLen<<" tar len: "<<tarLen<<endl;

    	 tryValidMoves(init,&targ);
    	tarLen = targ.length();

    	cout<<"init length: "<<initLen<<" tar len: "<<tarLen<<endl;
    }

    if(!strcmp(init.c_str(),targ.c_str()))
    {
    	return possible;
    }
    else
    {
    	return impossible;
    }

}


int main() {

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	ABBA test;
	string init = "BBBBABABBBBBBA";
	string target = "BBBBABABBABBBBBBABABBBBBBBBABAABBBAA";
	string s = test.canObtain(init,target);

	cout<<s<<endl;

	return 0;
}

/*class ABBA
{
    public:
    string canObtain(string initial,string target);
    private:
    bool tryValidMoves(string* initial, string target);
};

bool ABBA::tryValidMoves(string* initial, string target)
{
	string init = *initial;

	bool isSubStrEq = false;
	init.append("A");
	isSubStrEq = (bool)init.compare(0,init.length(),target,0,init.length());
	cout<<"is Substr: "<<isSubStrEq<<" init :"<<init<<"initial"<<*initial<<" Target"<<target<<endl;
			//strncmp(init.c_str(),target.c_str(),init.length());

	if(isSubStrEq)
	{
		init = *initial;
		reverse(init.begin(),init.end());
		init.append("B");
		isSubStrEq = (bool)init.compare(0,init.length(),target,0,init.length());

		cout<<"is Substr: "<<isSubStrEq<<" init :"<<init<<"initial"<<*initial<<" Target"<<target<<endl;
				//strncmp(init.c_str(),target.c_str(),init.length());
	}

	if(!isSubStrEq)
	{
		*initial = init;
	}

	cout<<"returning str eq: "<<isSubStrEq<<endl;
	return isSubStrEq;

}
string ABBA::canObtain(string initial,string target)
{
    string init = initial;
    string targ = target;

    string possible = "possible";
    string impossible = "impossible";

    uint initLen = initial.length();
    uint tarLen = target.length();

    cout<< "initLen : "<<initLen<<endl;
    cout<<"tarLen : "<<tarLen<<endl;

    bool isStringEq = false;

    while(initLen!=tarLen)
    {
    	static int loopNum = 1;
    	cout<<"Number of Loops :"<<loopNum++<<endl;
    	cout<<"inside loop, init len : "<<initLen<<", tar Len:  "<<tarLen<<endl;

    	isStringEq = tryValidMoves(&init,target);

    	if(isStringEq)
    	{
    		return impossible;
    	}
    	initLen = init.length();
    	cout<<"init "<<init<<" init length : "<<initLen<<endl;
    }

    if(!isStringEq)
    {
    	return possible;
    }
    else
    {
    	return impossible;
    }

}

int main() {

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	ABBA test;
	string init = "BBBBABABBBBBBA";
	string target = "BBBBABABBABBBBBBABABBBBBBBBABAABBBAA";
	string s = test.canObtain(init,target);

	cout<<s<<endl;


	cout<<"--------------------------------------"<<endl;

	uint32_t val = 512;
	uint32_t* addr;
	addr = &val;
	uint32_t echo;
	echo = sd_ble_gap_address_get(addr);
	cout<<"echo : "<<echo<<endl;

	return 0;
}
*/


