//============================================================================
// Name        : ABoardGame.cpp
// Author      : AngsTD
// Version     :
// Copyright   : Dont you Dare :P
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "stdlib.h"
#include <vector>

using namespace std;

class ABoardGame
{
public:
string whoWins(vector<string> board);
int8_t checkRegionN(int8_t region,vector<string>board, uint8_t N);
};

int8_t ABoardGame::checkRegionN(int8_t region,vector<string>board, uint8_t N)
{
	int bobCount = 0;
	int aliceCount = 0;

	for(int row = N-region;row<= (N +region -1);row++)
	{
		if((row == N-region)||(row == N + region -1) )
		{
			cout<<"row is "<< N-region<<" or "<< N+region-1<<endl;
			string rowStr = board[row];
			for(int column = N-region; column <= (N + region -1);column++)
			{
				cout<<"Row :"<< row<< "column: "<<column<<endl;
				char element = rowStr[column];
				if(element == 'A')
				{
					aliceCount++;
				}
				else if(element == 'B')
				{
					bobCount++;
				}
			}
		}
		else
		{
			cout<<" row is "<< row<<endl;
			for(int column = N-region; column <= (N +region -1);column+=(2*region-1))
			{
				cout<<"Row :"<< row<< "column: "<<column<<endl;

				string rowStr = board[row];
				char element = rowStr[column];
				if(element == 'A')
				{
					aliceCount++;
				}
				else if(element == 'B')
				{
					bobCount++;
				}
			}
		}
	}

	if(aliceCount>bobCount)
	{
		return 1;
	}
	else if (bobCount > aliceCount)
	{
		return 2;
	}
	else
	{
		return 0;
	}

	}

string ABoardGame::whoWins(vector<string> board){
	int N = board.size()/2;
	string alice = "Alice";
	string bob = "Bob";
	string draw = "Draw";
	string resultStr;
	int8_t result = 0;

	for(int region = 1;region<=N;region++)
	{
		cout<<"calculating region: "<<region<<endl;
		result = checkRegionN(region,board,N);

		if(result ==1)
		{
			resultStr = alice;
			break;
		}
		else if (result ==2)
		{
			resultStr = bob;
			break;
		}
	}

	if (result == 0)
	{
		resultStr = draw;
	}
	return resultStr;
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!


	ABoardGame test;
	const char* args[] = {".....A", "......", "..A...", "...B..", "......", "......"};
	vector<string> vec(args,end(args));
	string s = test.whoWins(vec);

	cout<<"winner is: "<<s<<endl;


	return 0;
}
