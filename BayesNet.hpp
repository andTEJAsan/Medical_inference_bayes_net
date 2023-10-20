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
class BayesNet {
	typedef int node;
	int total_nodes = 0;
	int num_edges = 0;
	unordered_map<string,int> id; // id of each Node
	// vector<vector<double>> cpt
	vector<string> name;
	vector<vector<int>> adj;
	vector<vector<float>> cpt; 
	// domain size of ith variable : n[i]
	vector<int> domain_size;
	// number of parents of ith variable : c[i]
	// node_to_parent int,int -> int
	vector<int> nv; // num_domain values of node i
	vector<vector<string>> dv; // dv[i][j] -> jth domain value of node i

	unordered_map<node,string> node_to_str;
	unordered_map<string,node> str_to_node;
	vector<node> top_order_to_node;
	vector<int> node_to_top_order; // node is also int
	void init_graph(); // Initialize the graph
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
	public:
	void read_network(string filename);	


};
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
			string varname = second.substr(1,second.size()-2);
			name.push_back(varname);
			id[varname] = total_nodes; 
			getline(fin,line);
			ss.clear();
			ss.str(line);
			while(ss.get() != '[');
			int domain_size;
			ss >> domain_size;
			#ifdef DEBUG
			cout << "domain size" << domain_size << "\n";
			#endif
			nv.push_back(domain_size);
			vector<string> domain(domain_size);
			for(int i = 0 ; i < domain_size; i++)
			{
				while(ss.get() != '\"');
				stringstream ss2;
				while(true){
					char ch = ss.get();
					if(ch == '\"') break;
					ss2 << ch;
				}
				domain[i] = ss2.str();
			}
			dv.push_back(domain);
			vector<int> _v;
			adj.push_back(_v);
			vector<float> _v1;
			cpt.push_back(_v1); // make space for cpt of node i
			total_nodes++;
		}
		else if(first == "probability"){
			string child;
			ss >> child>> child;
			child = child.substr(1,child.size()-2);	
			int child_node = id[child];
			cout << child << "\n";
			cout << "getting parents of " << child  << "\n";
			while(true)	
			{
				bool eos_found=false;
				while(true)
				{
					char ch = ss.get();
					if(ch == ')') {
						eos_found = true;
						break;
					}
					if(ch == '\"')
					{
						break;
					}
				}
				if(eos_found) break;
				string parent;
				ss >> parent;
				parent.pop_back();
				cout << "parent name " << parent << "\n";
				int parent_node = id[parent];
				cout << "parent_node " << parent_node << "\n";
				// add child to the out-adj list of 
				adj[child_node].push_back(parent_node);
			}
			// computing size of cpt[child_node] = Pi
			int cpt_size = nv[child_node];
			for(auto parent : adj[child_node]) {
				cout << "in the adj list " << name[parent];
				cpt_size *= nv[parent];
			}
			cpt[child_node].resize(cpt_size);
			getline(fin,line);
			ss.clear();
			ss.str(line);
			string dummy;
			ss >> dummy;
			cout << "cpt for " << child << "\n";
			cout << "cpt size " << cpt_size << "\n";
			for(int i = 0 ; i < cpt_size; i++)
			{
				string cpt_string;
				ss >> cpt_string;
				cout << "cpt_string " << cpt_string << "\n";
				float cpt_entry = stof(cpt_string);
				cpt[child_node][i] = cpt_entry;
				cout << " cpt entry " << cpt_entry << "\n";
			}

			
		}

	}
	
};


#endif
