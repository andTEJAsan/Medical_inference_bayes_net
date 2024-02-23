#include "BayesNet.hpp"
#include<cmath>
#include<iostream>
#include<numeric> 
#include<vector>
#include<algorithm> 
#include<sstream>
#include<fstream>
#include<ctime>
#include<cassert>
using namespace std;

int main(int argc , char * argv[])
{
	time_t start_time = time(NULL);
	BayesNet bn;
	// cout << "hello" << endl;
	bn.read_network(argv[1]);
	bn.read_records(argv[2]);
	// bn.read_correct(argv[3]);
	// cout << (60*(stod(argv[4]))) - 5- start_time << endl;

	bn.EM((double) (110) - ((double) time(NULL)- (double) start_time));	
	// cout << argv[1] << endl;
	// bn.create_bif(argv[1],"solved_alarm.bif");

	return 0;

}

