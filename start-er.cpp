#include<vector>
#include<map>
using namespace std;
class BayesNet {
	typedef int node;
	int total_nodes;
	int num_edges;
	// vector<vector<double>> cpt
	vector<vector<int>> adj;
	vector<vector<float>> cpt;
	vector<int> domain_size;
	unordered_map<node,string> node_to_str;
	unordered_map<string,node> str_to_node;
	vector<node> top_order_to_node;
	vector<int> node_to_top_order; // node is also int
	void init_graph(); // Initialize the graph
	void compute_top_order();
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



};

