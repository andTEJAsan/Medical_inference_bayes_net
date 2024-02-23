#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bits/stdc++.h>


// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
bool conv = false;
double gamma = 1e-4;
string solved_file_name="solved_alarm.bif";
class Graph_Node{

private:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<double> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
public:
	// Constructor- a node is initialised with its name and its categories
    vector<double> temp_counts;
    vector<double> temp_paretns_count;
    vector<int> counts; // counts[i] stores the number of times the ith combination of variable values has been observed.
    vector<int> parents_count; // parents_count[i] stores the number of times the ith combination of parent variable values has been observed.
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
		

	}
    vector<int> get_counts()
    {
        return counts;
    }
    vector<int> get_parents_count()
    {
        return parents_count;
    }
    vector<double> get_temp_counts()
    {
        return temp_counts;
    }
    vector<double> get_temp_parents_count()
    {
        return temp_paretns_count;
    }
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<double> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
    void set_counts(vector<int> new_counts)
    {
        counts.clear();
        counts=new_counts;
    }
    void increment_counts(int index,double val=1.0)
    {
        temp_counts[index]+=val;
    }
    void increment_parents_count(int index,double val=1.0)
    {
        temp_paretns_count[index]+=val;
    }
    void set_parents_count(vector<int> new_parents_count)
    {
        parents_count.clear();
        parents_count=new_parents_count;
    }
	void set_CPT(vector<double> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }



};


 // The whole network represted as a list of nodes
class network{

	list <Graph_Node> Pres_Graph;

public:
    string str;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            //std:://cout<<"node not found\n";
        return listIt;
    }
	

};

network read_network(string file_name)
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile(file_name.c_str()); 
  	string temp;
  	string name;
  	vector<string> values;
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<double> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
     					curr_CPT.push_back(atof(temp.c_str()));
     					ss2>>temp;
    				}
                    
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                
            }
     		
     		

    		
    		
    	}
    	
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}
int get_ind(string val_name,vector<string> values)
{
    for(int i=0;i<values.size();i++)
    {
        if(values[i].compare(val_name)==0)
            return i;
    }
    return -1;
}
int get_index(network &alarm, map<string,string> &data, string name){
    auto listIt = alarm.search_node(name);
    int prod = 1;
    int index = get_ind(data[name],listIt->get_values());
    bool flag = false;
    auto parents = listIt->get_Parents();
    int ni = parents.size();
    for(int k = 0; k<parents.size();k++){
        auto parent = alarm.search_node(parents[k]);
        int ind = get_ind(data[parents[k]],parent->get_values());
        // cout << index << "\n";
        index = index*parent->get_nvalues()+ind;
    }
    // cout << index << "\n";
    prod *= listIt->get_nvalues();
    // index = index*(listIt->get_nvalues())+get_ind(data[listIt->get_name()],listIt->get_values());
    // if(listIt->temp_counts.size() == 0){
    //     listIt->counts = vector<int>(prod,0);
    //     listIt->parents_count = vector<int>(prod/listIt->get_nvalues(),0);
    //     listIt->temp_counts = vector<double>(prod,gamma);
    //     listIt->temp_paretns_count = vector<double>(prod/listIt->get_nvalues(),gamma*listIt->get_nvalues());
    //     vector<double> cpt(prod,1.0/listIt->get_nvalues());             // can initialize with random values
    //     listIt->set_CPT(cpt);
    // }
    return index;
}
double prob(network &alarm, map<string,string> &data, string name){ // returns probability of name given its parents
    auto listIt = alarm.search_node(name);
    auto cpt = listIt->get_CPT();
    int index = get_index(alarm,data,name);
    return cpt[index];
}

void read_data(string filename,map<string,vector<string>> &data,vector<string> &names){
    ifstream myfile(filename.c_str());
    string line;
    if(myfile.is_open())
    {
        while(!myfile.eof())
        {
            stringstream ss;
            getline(myfile,line);
            ss.str(line);
            for(int i=0;i<37;i++){
                ss>>line;
                // //cout << line << "\n";
                data[names[i]].push_back(line);
                // //cout << names[i] << "\n";
                if(names[i].compare("\"LVFailure\"")==0){
                    //cout << line << "\n";
                }
            }
        }
    }
    else
    {
        //std:://cout<<"file not found\n";
    }
}

void extract_incomplete(map<string,vector<string>> &data,map<string,vector<string>> &incomplete_data,vector<string> &names){
    for(int i = 0; i<data["\"LVFailure\""].size();i++){
        bool flag = false;
        for(int j = 0; j<names.size();j++){
            if(data[names[j]][i].compare("\"?\"")==0){
                flag = true;
                break;
            }
        }
        if(flag){
            for(int j = 0; j<names.size();j++){
                incomplete_data[names[j]].push_back(data[names[j]][i]);
            }
        }
    }
}


// void change_data(map<string,vector<string>> &data,network &alarm,vector<string> &names){
//     for(int i = 0; i<data["\"LVFailure\""].size();i++){
//         for(int j=0; j<names.size();j++){
//             if(data[names[j]][i].compare("\"?\"") == 0){

//             }
//         }
//     }
// }

double get_prob(network &alarm, map<string,string> &data, string name){
    auto listIt = alarm.search_node(name);
    auto children = listIt->get_children();
    double prod = 1.0;
    for(int i = 0; i<children.size();i++){
        prod *= prob(alarm,data,alarm.get_nth_node(children[i])->get_name());
    }
    prod *= prob(alarm,data,name);
    return prod;
}

vector<double> get_probs(network &alarm, map<string,string> &data, string name){
    auto listIt = alarm.search_node(name);
    auto values = listIt->get_values();
    auto nvalues = listIt->get_nvalues();
    vector<double> probs(nvalues);
    double sum=0.0;
    for(int i = 0; i<values.size();i++){
        data[name] = values[i];
        probs[i] = get_prob(alarm,data,name);
        sum += probs[i];
    }
    // data[name] = "\"?\"";
    for(int i = 0; i<values.size();i++){
        probs[i] /= sum;
    }
    return probs;
}



pair<map<string,string>,int> get_dataline(map<string,vector<string>> &data, int index,vector<string> &names){
    map<string,string> dataline;
    int num = -1;
    for(int i =0; i<names.size();i++){
        dataline[names[i]] = data[names[i]][index];
        // cout << data[names[i]][index] << "hh\n";
        if(data[names[i]][index].compare("\"?\"")==0){
            // cout << data[names[i]][index] << "hh\n";
            num = i;
        }
    }
    return {dataline,num};
}

bool check_convergence(vector<double> old_cpt, vector<double> new_cpt){
    double norm = 0.0;
    // cout << old_cpt.size() << " " << new_cpt.size() << "\n";
    for(int i = 0; i<old_cpt.size();i++){
        norm += (old_cpt[i] - new_cpt[i])*(old_cpt[i] - new_cpt[i]);
        // cout << norm << "\n";
        // cout << old_cpt[i] << " " << new_cpt[i] << "\n";
    }
    cout << "norm " << sqrt(norm) << " " << norm << "\n";
    if(sqrt(norm/old_cpt.size()) < 0.0001)
        return true;
    else
        return false;
}

void update_cpt(map<string,vector<string>> &data, network &alarm,vector<string> &names){
    conv = true;
    for(int i=0;i<data["\"LVFailure\""].size();i++){
        int index;
        map<string,string> dataline;
        tie(dataline,index) = get_dataline(data,i,names);
        auto probs = get_probs(alarm,dataline,names[index]);
        auto values = alarm.search_node(names[index])->get_values();
        for(int j = 0; j<probs.size();j++){
            for(int k = 0; k<alarm.netSize();k++){
                auto listIt = alarm.get_nth_node(k);
                dataline[names[index]] = values[j];
                int num = get_index(alarm,dataline,listIt->get_name());
                listIt->increment_counts(num,probs[j]);
                listIt->increment_parents_count(num/listIt->get_nvalues(),probs[j]);
            }
        }
    }
    // cout << "here\n";
    for(int i = 0; i<alarm.netSize();i++){
        vector<double> cpt;
        auto listIt = alarm.get_nth_node(i);
        auto counts = listIt->get_counts();
        auto parents_count = listIt->get_parents_count();
        auto temp_counts = listIt->get_temp_counts();
        auto temp_parents_count = listIt->get_temp_parents_count();
        int nvalues = listIt->get_nvalues();
        // cout << "counts size " << counts.size() << "\n";
        for(int j = 0; j<counts.size();j++){
            if(temp_parents_count[j/nvalues] ==0){
                cout << "Danger\n";
            }
            cpt.push_back((temp_counts[j])/(temp_parents_count[j/nvalues]));
        }
        listIt->temp_counts=(vector<double>(counts.size(),gamma));
        listIt->temp_paretns_count=(vector<double>(parents_count.size(),gamma*listIt->get_nvalues()));
        // cout << "cpt size " << cpt.size() << "\n";
        bool c = check_convergence(listIt->get_CPT(),cpt);
        if(c == false){
            conv = false;
        }
        listIt->set_CPT(cpt);
    }
}
void print_p(network &Alarm){
    for(int i = 0; i<Alarm.netSize();i++){
        auto listIt = Alarm.get_nth_node(i);
        cout << "variable: " << listIt->get_name() << "\n";
        auto cpt = listIt->get_CPT();
        cout << "CPT: ";
        int nvalues = listIt->get_nvalues();
        for(int j = 0; j<nvalues;j++){
            int tmp = j;
            while(tmp<cpt.size()){
                cout << cpt[tmp] << " ";
                tmp +=nvalues;
            }
        }
        cout << "\n";
    }
}

void accuracy(network &Alarm){
    auto correct_alarm = read_network("gold_alarm.bif");
    for(int i = 0; i<Alarm.netSize();i++){
        auto listIt = Alarm.get_nth_node(i);
        auto cpt = listIt->get_CPT();
        auto listIt1 = correct_alarm.get_nth_node(i);
        auto cpt1 = listIt1->get_CPT();
        double norm = 0.0;
        int temp =0;
        int nvalues = listIt->get_nvalues();
        for(int j = 0; j<nvalues;j++){
            int tmp = j;
            while(tmp<cpt.size()){
                norm += (cpt[tmp] - cpt1[temp])*(cpt[tmp] - cpt1[temp]);
                temp++;
                tmp +=nvalues;
            }
        }
        cout << "norm " << sqrt(norm/cpt.size())<< "\n";
        cout << "\n";
    }
}

void writeFile(network &Alarm){
    ofstream outfile(solved_file_name);
    ifstream infile("alarm.bif");
    int num = 0;
    if(outfile && infile){
        while(!infile.eof()){
            string line;
            getline(infile,line);
            if(line.compare("\n") == 0)
                continue;
            stringstream ss;
            ss = stringstream(line);
            string temp;
            ss >> temp;
            // cout << temp << "\n";
            if(temp.compare("table") == 0){
                string line1 = "    table ";
                auto listIt = Alarm.get_nth_node(num);
                num++;
                int i = 0;
                ss >> temp;
                stringstream ss1;
                while(temp.compare(";") != 0){
                    auto cpt = listIt->get_CPT();
                    int p = listIt->temp_paretns_count.size();
                    string tempLine = to_string(cpt[i/p + (i%p)*listIt->get_nvalues()]);
                    // line1 += tempLine.substr(0,tempLine.find('.')+5) + " ";
                    line1 += to_string(cpt[i/p + (i%p)*listIt->get_nvalues()]) + " ";
                    // ss1 << cpt[i/p + (i%p)*listIt->get_nvalues()] << setprecision(2) << " ";
                    i++;
                    ss >> temp;
                }
                line1 += ss1.str();
                line1 += ";\n";
                outfile << line1;
            }else{
                outfile << line << "\n";
            }
        }
    }
    infile.close();
    outfile.close();
}

int main()
{
	network Alarm;
	Alarm = read_network("gold_alarm.bif");
    string topo_order[] = {"ErrLowOutput","Hypovolemia","LVFailure","LVEDVolume","StrokeVolume","History","CVP","PCWP","Anaphylaxis","TPR","PulmEmbolus","Intubation","KinkedTube","MinVolSet","Disconnect","VentMach","VentTube","Press","VentLung","PAP","Shunt","MinVol","FiO2","VentAlv","ArtCO2","ExpCO2","PVSat","SaO2","InsuffAnesth","Catechol","HR","ErrCauter","HRSat","HREKG","HRBP","CO","BP"};
    map<string,string> data_line;
    int N = 10000;
    ofstream outfile("sample.dat");
    srand( (unsigned)time( NULL ) );
    int M = 100;
    while(M--){
    while(N--){
        for(int i = 0; i<37;i++){
            auto listIt = Alarm.get_nth_node(i);
            data_line[listIt->get_name()] = listIt->get_values()[0];
            // cout << listIt->get_name() << "\n";
        }
        for(int i = 0; i<37;i++){
            // cout << "\"" + topo_order[i]+"\"" << "\n";
            auto listIt = Alarm.search_node("\"" + topo_order[i]+"\"");
            // cout << "hello\n";
            vector<double> probs;
            for(int j =0; j<listIt->get_values().size();j++){
                data_line[listIt->get_name()] = listIt->get_values()[j];
                // cout << listIt->get_name() << " " << listIt->get_values()[j] << "\n";
                // cout << get_index(Alarm,data_line,listIt->get_name()) << "\n";
                probs.push_back(prob(Alarm,data_line,listIt->get_name()));
            }
            double r = (double)rand() / RAND_MAX;
            // cout << r << "\n";
            double sum = 0.0;
            int j = 0;
            // for(int k = 0; k<probs.size();k++){
            //     cout << probs[k] << ",";
            // }
            while(sum <= r && j<probs.size()){
                sum += probs[j];
                j++;
            }
            // cout << "\n";
            // break;
            // cout << j << " " << r << "\n";
            data_line[listIt->get_name()] = listIt->get_values()[j-1];
            // cout << "hello\n";
        }
        double r = (double)rand() / RAND_MAX;
        int ques = -1;
        if(r <= -1){
            ques = rand()%37;
        }
        for(int i = 0; i<37;i++){
            auto listIt = Alarm.get_nth_node(i);
            if(i == ques){
                data_line[listIt->get_name()] = "\"?\"";
            }
            outfile << data_line[listIt->get_name()] << " ";
        }
        outfile << "\n";
    }
    }
    outfile.close();
    
}




