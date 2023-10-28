#include "BayesNet.hpp"
#include<iostream>
#include<numeric> 
#include<vector>
#include<algorithm> 
#include<sstream>
#include<fstream>
#include<cassert>
using namespace std;

void print_cpt(BayesNet& bn){
	cout << "printing cpt" << endl;
	for(int i = 0; i < bn.total_nodes; i++){
		cout << bn.name[i] << endl;
		for(auto x : bn.cpt[i]) cout << x << " ";
		cout << endl;
	}
	cout << endl;
}

void print_data(vector<vector<int>> data, vector<int> unknown_data, vector<vector<float>> weights){
	cout << "printing data" << endl;
	for(int i = 0; i < data.size(); i++){
		cout << "for node " << i << endl;
		for(auto x : data[i]) cout << x << " "; cout << endl;
		cout << unknown_data[i] << endl;
		for(auto x : weights[i]) cout << x << " "; cout << endl;
	}
	cout << endl;
}

float compute_prob_given_parents(BayesNet& bn, vector<int>& values, int node, int value_of_node){
	int mult = 1;	
	for(int i = 0; i < bn.adj[node].size(); i++){
		mult *= bn.nv[bn.adj[node][i]];
	}

	int index = value_of_node * mult;
	for(int i = 0; i < bn.adj[node].size(); i++){
		mult /= bn.nv[bn.adj[node][i]];
		index += values[bn.adj[node][i]] * mult;
	}

	return bn.cpt[node][index];
}

void compute_dist_gibbs_sampling(BayesNet& bn, vector<int>& values, int node, vector<float>& dist){
	dist.clear();
	int value_of_node = values[node];
	for(int i = 0; i < bn.nv[node]; i++){
		float prob = 1;
		prob *= compute_prob_given_parents(bn, values, node, i);
		values[node] = i;
		vector<int>& children_list = bn.children[node];
		for(int j = 0; j < children_list.size(); j++){
			prob *= compute_prob_given_parents(bn, values, children_list[j], values[children_list[j]]);
		}

		dist.push_back(prob);
	}
	values[node] = value_of_node;

	// cout << endl; 
	// for(auto x : dist) cout << x << " "; cout << endl; 

	float sum = 0;
	for(int i = 0; i < dist.size(); i++){
		sum += dist[i];
	}

	for(int i = 0; i < dist.size(); i++){
		dist[i] = dist[i] / sum;
	}

	// for(auto x : dist) cout << x << " "; cout << endl; 
	// cout << endl;
}


// update record_number'th record in data based on current weights
void recompute_data_weights(BayesNet& bn, vector<vector<int>>& data, vector<int>& unknown_data, vector<vector<float>>& weights, int record_number){
	compute_dist_gibbs_sampling(bn, data[record_number], unknown_data[record_number], weights[record_number]);
}

void compute_data_given_weights(BayesNet& bn, vector<vector<int>>& data, vector<int>& unknown_data, vector<vector<float>>& weights){
	for(int i = 0; i < data.size(); i++){
		if(unknown_data[i] == -1) continue;
		recompute_data_weights(bn, data, unknown_data, weights, i);
	}
}



// compute the cpt values of the given node in the bayes network
void compute_cpt_value(BayesNet& bn, int node, vector<vector<int>>& data, vector<int>& unknown_data, vector<vector<float>>& weights){

	int cpt_size = bn.nv[node];	
	for(int i = 0; i < bn.adj[node].size(); i++){
		cpt_size *= bn.nv[bn.adj[node][i]];
	}
	vector<float> cpt_unnormalized (cpt_size, 0);
	
	for(int i = 0; i < data.size(); i++){
		if(unknown_data[i] == -1){
			int mult = 1;	
			for(int j = 0; j < bn.adj[node].size(); j++){
				mult *= bn.nv[bn.adj[node][j]];
			}

			int index = data[i][node] * mult;
			for(int j = 0; j < bn.adj[node].size(); j++){
				mult /= bn.nv[bn.adj[node][j]];
				index += data[i][bn.adj[node][j]] * mult;
			}

			cpt_unnormalized[index] += 1;
		}

		else{
			int unknown_variable = unknown_data[i];
			for(int j = 0; j < weights[i].size(); j++){
				data[i][unknown_variable] = j;

				int mult = 1;	
				for(int j = 0; j < bn.adj[node].size(); j++){
					mult *= bn.nv[bn.adj[node][j]];
				}

				int index = data[i][node] * mult;
				for(int j = 0; j < bn.adj[node].size(); j++){
					mult /= bn.nv[bn.adj[node][j]];
					index += data[i][bn.adj[node][j]] * mult;
				}

				cpt_unnormalized[index] += weights[i][j];
			}
		}

	}
	
	for(int i = 0; i < cpt_unnormalized.size(); i++){
		cpt_unnormalized[i] /= data.size(); // s. assuming all cpt values are modifiable
	}

	bn.cpt[node] = cpt_unnormalized;
}

void update_full_bn(BayesNet& bn, vector<vector<int>>& data, vector<int>& unknown_data, vector<vector<float>>& weights){
	for(int i = 0; i < bn.total_nodes; i++){
		compute_cpt_value(bn, i, data, unknown_data, weights);
	}
}

int main()
{
	BayesNet bn;
	cout << "hello" << endl;
	bn.read_network("alarm.bif");

	// int numRecords = 11100;

	vector<vector<int>> data;
	vector<int> unknownData; // record number to the unknown field value, the position of the question mark
	vector<vector<float>> dataWeights;

	// take data input
	ifstream dataInput("records.dat");
	string line;
	vector<int> emptyVector;
	vector<float> emptyVectorFloat;
	while(getline(dataInput, line)){
		data.push_back(emptyVector);
		dataWeights.push_back(emptyVectorFloat);
		stringstream ss(line);
		string word;

		int varNumber = -1;
		
		unknownData.push_back(-1);
		while(ss >> word){
			if(word[0] != '"'){
				continue;
			}
			varNumber++;
			word.pop_back();
			word.erase(word.begin());
			if (word == "?"){
				int x = (data.back()).size();
				unknownData[unknownData.size() - 1] = x;
				data.back().push_back(-1);
			}
			else{
				data.back().push_back(bn.variable_and_value_to_integer[{varNumber, word}]);
				// cout << word << bn.dv[varNumber][data.back().back()] << endl;
				// assert(word == bn.dv[varNumber][data.back().back()]); // s. comment this line later
			}
		}
	}

	for(int i = 0; i < data.size(); i++){
		if (unknownData[i] != -1) dataWeights[i] = vector<float> (bn.nv[unknownData[i]], (float) 1/bn.nv[unknownData[i]]);
	}

	// for(auto x : data){
	// 	for(auto y : x) cout << y << " ";
	// 	cout << endl;
	// }

	// print_data(data, unknownData, dataWeights);
	
	cout << unknownData.size() << endl;
	// assert(unknownData.size() == numRecords); // s. comment this line later
	// for(auto x:unknownData) cout << x << endl; 

	// print_cpt(bn);

	for(int i = 0; i < 200; i++){
		update_full_bn(bn, data, unknownData, dataWeights);
		compute_data_given_weights(bn, data, unknownData, dataWeights);
		// print_cpt(bn);
		// print_data(data, unknownData, dataWeights);
	}

	print_cpt(bn);
	print_data(data, unknownData, dataWeights);

	cout << "done" << endl << endl;

	// for(auto x:dataWeights){
	// 	for(auto y:x) cout << y << " ";
	// 	cout << endl;
	// }

	




	// trying gibbs sampling

	vector<int> to_be_sampled;

	// s. assuming everything can be sampled, for now
	for(int i=0; i<to_be_sampled.size(); i++){
		to_be_sampled.push_back(i);
	}

	vector<vector<int>> samples;

	vector<int> current_sample;

	for(int i = 0; i < bn.total_nodes; i++){
		current_sample.push_back(rand() % bn.nv[i]);
	}

	samples.push_back(current_sample);
	for(auto x:current_sample) cout << x << " "; cout << endl;

	int num_samples = 0;
	for(int i = 0; i < num_samples; i++){
		int node_to_sample = rand() % bn.total_nodes;
		vector<float> prob_distribution;
		compute_dist_gibbs_sampling(bn, current_sample, node_to_sample, prob_distribution);
		for(auto x: prob_distribution) cout << x << " "; cout << endl;

		vector<float> cummulative_dist;
		float curr = 0;
		for(int i = 0; i < prob_distribution.size(); i++){
			curr += prob_distribution[i];
			cummulative_dist.push_back(curr);
		}

		int new_value = lower_bound(cummulative_dist.begin(), cummulative_dist.end(), (float) rand()/RAND_MAX) - cummulative_dist.begin();
		current_sample[node_to_sample] = new_value;
		samples.push_back(current_sample);
	}
	
	// s. samples are ready to be processed further. or are they?



	return 0;

}

