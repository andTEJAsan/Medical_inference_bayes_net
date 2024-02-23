#include<iomanip>
#include<ctime>
#include<cassert>
#include<sstream>
#include<fstream>
#include<string>
#include<vector>
#include<unordered_map>
#include<iostream>
#include<map>
using namespace std;
#ifndef __BAYESNET_HPP__
#define __BAYESNET_HPP__	
struct hash_pair_vector_int {
    size_t operator()(const pair<vector<int>, int>& p) const {
        size_t h = 0;
        for (int i : p.first) {
            h ^= hash<int>{}(i) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        h ^= hash<int>{}(p.second);
        return h;
    }

};
class BayesNet {
	public:
	typedef int node;
	int hits = 0;
	int miss = 0;
	int total_nodes = 0;
	int num_edges = 0;
	unordered_map<string,int> id; // id of each Node
	// vector<vector<long double>> cpt
	vector<string> name;
	vector<vector<int>> adj;
	vector<vector<int>> children; // s. initialze this 
	// map<pair<int,string>, int> variable_and_value_to_integer; // s. initialize this
	vector<vector<long double>> cpt; 
	vector<vector<long double>> unnormalized;
	vector<vector<long double>> sum_weights; // sum_weights[i][j] stores the sum of counts when ith node takes its jth domain value

	// // domain size of ith variable : n[i]
	// vector<int> domain_size;

	// number of parents of ith variable : c[i]
	// node_to_parent int,int -> int
	vector<int> nv; // num_domain values of node i
	vector<vector<string>> dv; // dv[i][j] -> jth domain value of node i
	vector<unordered_map<string,int>> dv_node; // dv_node[i][s] -> index of string s in dv[i]

	unordered_map<node,string> node_to_str;
	unordered_map<string,node> str_to_node;

	unordered_map<pair<vector<int>,int>,pair<int,int>, hash_pair_vector_int> index_table;
	vector<node> top_order_to_node;
	vector<int> node_to_top_order; // node is also int

	// void init_graph(); // Initialize the graph

	void dfs(int s, vector<vector<int>>& adj, vector<bool>& visited, vector<int> & topsort) // returns the leaving time of s	
	{
		for(auto v : adj[s]){
			if(visited[v]) continue;
			visited[v] = true;
			dfs(v,adj,visited,topsort);
		}
		topsort.push_back(s);
	}
	void compute_top_order(){
		vector<bool> visited(total_nodes);
		vector<int> post_order(total_nodes);
		for(node u = 0 ; u < total_nodes; u++)
		{
			if(!visited[u]) dfs(u,adj,visited,top_order_to_node);
		};
		// now I have the post_order value for all vertices
		node_to_top_order.resize(total_nodes);
		for(int i = 0 ; i < total_nodes; i++)
		{
			node_to_top_order[top_order_to_node[i]] = i;
		}
	}

	
	vector<vector<int>> samples;
	vector<vector<long double>> sample_weight; // sample_weight[i][j] -> weight of ith sample with the unkown_index taking the jth domain values
	// size of sample_weight[i] is nv[unkown_index[i]]
	// if no unkown value is there weight is 1.
	vector<int> unknown_index;
	void read_network(string filename);	
	void read_records(string filename);
	void read_correct(string filename);
	void create_bif(string f1, string filename);
	
	vector<vector<long double>> tcpt;
	int count_sample(vector<int>& sample, long double weight)
	{
		for(node i = 0 ; i < total_nodes; i++)
		{
			int cpt_index = 0;
			int base = 1;
			for(auto it = adj[i].rbegin(); it != adj[i].rend(); ++it)
			{
				int cur_parent = *it;
				cpt_index += sample[cur_parent]*base;
				base *= nv[cur_parent];
			}
			int z  = cpt_index;
			sum_weights[i][cpt_index] += weight;
			
			cpt_index += sample[i]*base;
			unnormalized[i][cpt_index] += weight;
		}
		return 0;
	};

	inline pair<int,int> cpt_index(vector<int>& sample, int node)
	{
		// if(index_table.find({sample,node}) != index_table.end())
		// {
		// 	hits++;
		// 	return index_table[{sample,node}];
		// }
		miss++;
		int cpt_index = 0;
		int base = 1;
		for(auto it = adj[node].rbegin(); it != adj[node].rend(); ++it)
		{
			cpt_index += sample[*it]*base;
			base*= nv[*it];
		}
		// index_table[{sample,node}] = {cpt_index,cpt_index+sample[node]*base};
		return {cpt_index,cpt_index + sample[node]*base};
	};
	inline int weight_of_sample(int sample_index, int unkown_index)
	{
		// assuming that this will be called only on incomplete samples
		// compute the probabilities based on unnormalized cpt
		// possible optimizationm use cpt tables
		vector<int>& incomplete_sample = samples[sample_index];
		long double sum = 0;
		vector<long double> temp;
		for(int u = 0; u < nv[unkown_index]; u++)
		{
			// compute P(X = uth domain value| par(X)) *
			incomplete_sample[unkown_index] = u;
			auto [sum_ind,cpt_ind] = cpt_index(incomplete_sample,unkown_index);

			long double num = unnormalized[unkown_index][cpt_ind];
			long double den = sum_weights[unkown_index][sum_ind];
			for(auto v : children[unkown_index]){
				auto [sum_ind,cpt_ind] = cpt_index(incomplete_sample,v);
				int y = unnormalized[v].size();
				num *= unnormalized[v][cpt_ind];
				den *= sum_weights[v][sum_ind];
			}
			long double sw = num/den;
			temp.push_back(sw);
			sum += sw;

		}
		
		incomplete_sample[unkown_index] = -1;
		for(int u = 0 ; u <  nv[unkown_index]; u++)
		{
			sample_weight[sample_index][u] = (temp[u]/sum);
		}
		
		return 0;
	};
	long double lmda = 0.001;
	inline void init_cpt(long double lmbda )
	{

		for(int i = 0 ; i < unnormalized.size(); i++)
		{
			fill(begin(unnormalized[i]),end(unnormalized[i]),lmbda);
			fill(sum_weights[i].begin(),sum_weights[i].end(), lmbda*nv[i]);
		}
	};
	int normalize_cpt(){
		for(int i = 0; i < total_nodes; i++)
		{
			int cpt_size = cpt[i].size();
			int mod = cpt_size / nv[i];
			for(int j = 0; j < cpt_size; j++)
			{
				cpt[i][j] = unnormalized[i][j] / sum_weights[i][j % mod];
			}
		}
		return 0;
	}
	void compute_error(){
		long double sum = 0;
		for(int i = 0 ; i < total_nodes; i++)
		{
			for(int j = 0 ; j < cpt[i].size(); j++)
			{
				sum += abs(cpt[i][j]-tcpt[i][j]);
			}
		}
		cout << sum << "\n";
	}
	void EM(double timer)
	{
		double start_time = (double) time(NULL);
		init_cpt(lmda);
		normalize_cpt();
		create_bif("alarm.bif","solved_alarm.bif");
		while((double) time(NULL) - start_time < timer)
		{
			// cout << "epoch " << ++j << "\n";  
			for(int s = 0 ; s < samples.size(); s++)	
			{
				if(unknown_index[s] == -1) continue;
				weight_of_sample(s,unknown_index[s]);
			}// computing the weights for every sample

			init_cpt(lmda);
			for(int s = 0; s < samples.size(); s++)
			{
				if(unknown_index[s] == -1)
				{
					count_sample(samples[s],1);
				}
				else
				{

					int node = unknown_index[s];
					for(int i = 0; i < nv[node]; i++ )
					{
						samples[s][node] = i;
						
						count_sample(samples[s],sample_weight[s][i]);
						
					}
					samples[s][node] = -1;
				}
			}

		if((double) time(NULL) - start_time >= timer/2){
			normalize_cpt();
			create_bif("alarm.bif","solved_alarm.bif");
		}
		// cout << "Epoch : Error = ";
		// compute_error();
		// cout << "\n";
		}
	}
	void print_cpt(){
	cout << "printing cpt" << endl;
	for(int i = 0; i < total_nodes; i++){
		cout << name[i] << endl;
		for(auto x : cpt[i]) cout << x << " ";
		cout << endl;
	}
	cout << endl;

	}
};



void BayesNet::read_records(string filename){
	ifstream fin(filename);
	string line;
	while(getline(fin,line))
	{
		stringstream ss(line);
		vector<int> sample;
		unknown_index.push_back(-1);
		int sz = 1;
		for(int i = 0 ; i < total_nodes; i++)
		{
			string current;
			ss >> current;
			if(current == "\"?\"") {
				unknown_index.back() = i;
				sz = nv[i];
				sample.push_back(-1);
			}
			else{
				
				sample.push_back(dv_node[i][current]);
			}
		}
		samples.push_back(sample);
		sample_weight.push_back(vector<long double>(sz));
	}
	
};

void BayesNet::read_correct(string filename)
{
	ifstream fin(filename);
	string line;
	while(getline(fin,line))
	{
		stringstream ss(line);
		string first;
		ss >> first;
		if(first == "variable")	{
			getline(fin,line);
			tcpt.push_back(vector<long double> ());
		}
		else if(first == "probability"){
			string child;
			ss >> child>> child;
			int child_node = id[child];
			int cpt_size = cpt[child_node].size();
			tcpt[child_node].resize(cpt_size);
			getline(fin,line);

			ss.clear();
			ss.str(line);
			string dummy;
			ss >> dummy;
			for(int i = 0 ; i < cpt_size; i++)
			{
				string cpt_string;
				ss >> cpt_string;
				long double cpt_entry = stold(cpt_string);
				tcpt[child_node][i] = cpt_entry;
			}

			
		}
	}

	
}
void BayesNet::read_network(string filename){
	ifstream fin(filename);
	string line;
	while(getline(fin,line))
	{
		stringstream ss(line);
		string first;
		ss >> first;
		if(first == "variable")	{
			string second;
			ss >> second;
			// string varname = second.substr(1,second.size()-2);
			name.push_back(second);
			id[second] = total_nodes; 
			getline(fin,line);
			ss.clear();
			ss.str(line);
			while(ss.get() != '[');
			int domain_size;
			ss >> domain_size;

			nv.push_back(domain_size);
			vector<string> domain(domain_size);
			unordered_map<string,int> inv_domain;
			while(ss.get() != '{');
			for(int i = 0 ; i < domain_size; i++)
			{
				string x;
				ss >> x;
				domain[i] = x;
				inv_domain[x] = i;
			}
			dv.push_back(domain);
			dv_node.push_back(inv_domain);
			adj.push_back(vector<int>());
			children.push_back(vector<int>());
			cpt.push_back(vector<long double> ()); // make space for cpt of node i
			unnormalized.push_back(vector<long double> ());
			sum_weights.push_back(vector<long double>());
			total_nodes++;
		}
		else if(first == "probability"){
			string child;
			ss >> child>> child;
			int child_node = id[child];
			while(true)	
			{
				string parent;
				ss >> parent;
				if(parent[0] != '\"') break;
				int parent_node = id[parent];
				// add child to the out-adj list of 
				adj[child_node].push_back(parent_node);
				children[parent_node].push_back(child_node);
			}
			// computing size of cpt[child_node] = Pi
			int cpt_size =1;
			for(auto parent : adj[child_node]) {
				cpt_size *= nv[parent];
			}
			sum_weights[child_node].resize(cpt_size);
			cpt_size *= nv[child_node];
			cpt[child_node].resize(cpt_size);
			unnormalized[child_node].resize(cpt_size);
			getline(fin,line);
			ss.clear();
			ss.str(line);
			string dummy;
			ss >> dummy;
			for(int i = 0 ; i < cpt_size; i++)
			{
				string cpt_string;
				ss >> cpt_string;
				long double cpt_entry = stold(cpt_string);
				cpt[child_node][i] = cpt_entry;
			}

			
		}

	}

};
void BayesNet::create_bif(string f1, string filename)
	{
		ifstream fin(f1);
		ofstream fout(filename);
		string line;
		normalize_cpt();
		while(getline(fin,line))	
		{
			stringstream ss(line);
			string first;
			ss >> first;
			fout << line << "\n";
			if(first == "probability")
			{
				getline(fin,line);
				string paren;
				ss >> paren >> paren;
				fout << "\t"<<"table ";
				for(auto x : cpt[id[paren]]) fout << fixed << setprecision(4)<<  x << " ";
				fout << ";\n";
			}
		}
	}
	

#endif
