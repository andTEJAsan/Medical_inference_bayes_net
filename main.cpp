#include "BayesNet.hpp"
#include<iostream>
using namespace std;
int main()
{
	BayesNet bn;
	bn.read_network("alarm.bif");
	return 0;

}

