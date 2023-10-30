#include "BayesNet.hpp"
#include<iostream>
#include<numeric> 
#include<vector>
#include<algorithm> 
#include<sstream>
#include<fstream>
#include<cassert>
using namespace std;

int main(int argc , char * argv[])
{
	BayesNet bn;
	cout << "hello" << endl;
	bn.read_network(argv[1]);
	bn.read_records(argv[2]);
	bn.read_correct(argv[3]);

	bn.EM(stoi(argv[4]));	
	cout << argv[1] << endl;
	bn.create_bif(argv[1],"solved_alarm.bif");

	return 0;

}

