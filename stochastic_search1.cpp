#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <random>  
#include <algorithm>  
#include <iterator>
#include <bits/stdc++.h> 
#define population_size 10

using namespace std;

// Vertex data structure
struct vertex {
  private:
    double x;
    double y;
  
  public:
    vertex(double x, double y) {
        this->x = x;
        this->y = y;
    }
};

// Candidate solution
struct solution {
  public:
    vector<int> nodes;

    void print_soln() {
        cout<< nodes[0];
        for(vector<int>::const_iterator idx = nodes.begin() + 1; idx != nodes.end(); ++idx) {
            cout<< " " << *idx;
        }
        cout<< endl;
    }
};


//struct for crossover
struct city
{
	int index;
	int next;
	int next_next;
	city(){
		this->index = 0;
		this->next = 0;
		this->next_next = 0;
	}
};

// Main class
class TSP {
  private:
    bool euc;
    int n_cities;
    vector<vector<double>> graph;
    solution current_state;

  public:
    TSP(string file_name) {

        ifstream input(file_name);
        double val;
        string euc;

        // Input euc
        input >> euc;
        if(euc == "euclidean") {
            euc = true;
        } else {
            euc = false;
        }

        // Input num cities
        input >> n_cities;

        // Input vertices
        int inp_counter = 0;
        for(inp_counter = 0; inp_counter < n_cities; ++inp_counter) {
            input >> val;

            double y;
            input >> y;

            vertex v(val, y);
        }

        // Input edges
        for(int city = 0; city < n_cities; ++city) {
            vector<double> edges;
            for (inp_counter = 0; inp_counter < n_cities; ++inp_counter) {
                input >> val;
                edges.push_back(val);
            }
            graph.push_back(edges);
        }

        // Init solution
        for(int i=0; i<n_cities; ++i) {
            current_state.nodes.push_back(i);
        }
        current_state.print_soln();
        cout<< heuristic(current_state) << endl;

        // cout<< euc << endl;
        // cout<< n_cities << endl;
        // for(vector<vector<double>>::const_iterator vtx = graph.begin(); vtx != graph.end(); ++vtx) {
        //     for(vector<double>::const_iterator edge = vtx->begin(); edge != vtx->end(); ++edge) {
        //         printf(" %.10f", *edge);
        //     }
        //     cout<< endl;
        // }
    }

    double heuristic(solution state) {
        double cost;
        for(int i=0; i < n_cities - 1; ++i) {
            cost += graph[state.nodes[i]][state.nodes[i+1]];
        }

        cost += graph[state.nodes[n_cities - 1]][state.nodes[0]];

        return cost;
    }

    // 2-City swapping
    void moveGen(vector<solution> &next_states) {
        solution temp_soln;
        for(int c1 = 0; c1 < n_cities - 1; ++c1) {
            for(int c2 = c1 + 1; c2 < n_cities; ++c2) {
                temp_soln = current_state;

                temp_soln.nodes[c1] = current_state.nodes[c2];
                temp_soln.nodes[c2] = current_state.nodes[c1];

                next_states.push_back(temp_soln);
            }
        }

        // cout<< next_states.size() << endl;
    }

    // Sigmoid function
    double sigmoid(double val, double temperature) {
        return 1 / (1 + exp(-val/temperature));
    }

    // Simulated Annealing
    void sim_anneal() {
        // Init Temp
        double Temperature = 100, epsilon = 0.01;

        // Lower Temperature slowly
        while(Temperature > epsilon) {
            cout<< "Temperature: " << Temperature << endl;

            // Get neighboring solutions
            vector<solution> next_states;
            moveGen(next_states);

            // Make a random move depending on its probability
            bool made_move = false;
            while (!made_move) {
                // Generate random move
                int check_move = rand() % n_cities;
                double diff_eval, next_eval;

                // Get eval
                next_eval = heuristic(next_states[check_move]);
                diff_eval = next_eval - heuristic(current_state);

                // Compute probability of move
                double prob = sigmoid(diff_eval, Temperature);
                double rand_float = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);

                // Make move if prob less than rand val
                if(prob <= rand_float) {
                    current_state = next_states[check_move];
                    made_move = true;
                }
            }
            
            cout<< "Eval: " << heuristic(current_state) << endl;
            Temperature -= epsilon;
        }
    }


    vector<float> rank(vector<double> A,int n){
    	vector<float> R;
    	// Sweep through all elements in A for each 
    	// element count the number of less than and 
    	// equal elements separately in r and s. 
    	for (int i = 0; i < n; i++) { 
        int r = 1, s = 1; 
        float result;
        for (int j = 0; j < n; j++) { 
            if (j != i && A[j] < A[i]) 
                r += 1; 
                  
            if (j != i && A[j] == A[i]) 
                s += 1;      
        } 	
          
        // Use formula to obtain rank 
        result = r + (float)(s - 1) / (float) 2;
        R.push_back(result);
        //cout << R[i] << " \n";
   		}
    	return R;
	}

	vector<solution> crossover(solution parent1,solution parent2,int n,int v1,int v2){
		vector<solution> chilren;
		vector<city> sequence1;
		vector<city> sequence2;
		for(int i=0; i<n_cities; i++){
			city a ;
			if(i > v1 && i < v2){
				a.index  = parent1.nodes[i];
			}
			else {
				a.index  = parent1.nodes[i];
				a.next = parent2.nodes[i];
			}
			for(int j=v1; j<v2+1; j++) {
				if(a.next == parent1.nodes[j]) {
					a.next_next = parent2.nodes[j];
				}
			}
			sequence1.push_back(a);
		}

		for(int i=0; i<n_cities; i++){
			city a ;
			if(i > v1 && i < v2){
				a.index  = parent2.nodes[i];
			}
			else {
				a.index  = parent2.nodes[i];
				a.next = parent1.nodes[i];
			}
			for(int j=v1; j<v2+1; j++) {
				if(a.next == parent2.nodes[j]) {
					a.next_next = parent1.nodes[j];
				}
			}
			sequence2.push_back(a);
		}

		solution child1,child2;

		for (int i = 0; i < n_cities; ++i)
		{
			int push;
			for(int j=v1; j<v2+1; j++) {
				if(sequence1[i].index == sequence2[j].index) {
					for(int h=v1; h<v2+1; h++){
						if(sequence2[j].index == sequence1[h].index){
							push = sequence1[i].next_next;
						}
					}
					if(sequence1[i].next_next != 0){
						push = sequence1[i].next;
					}
				}
			}
			if(sequence1[i].next_next != 0 && sequence1[i].next != 0){
				push = sequence1[i].index;
			}
			child1.nodes.push_back(push);
		}

		for (int i = 0; i < n_cities; ++i)
		{
			int push;
			for(int j=v1; j<v2+1; j++) {
				if(sequence2[i].index == sequence1[j].index) {
					for(int h=v1; h<v2+1; h++){
						if(sequence1[j].index == sequence2[h].index){
							push = sequence2[i].next_next;
						}
					}
					if(sequence1[i].next_next != 0){
						push = sequence2[i].next;
					}
				}
			}
			if(sequence1[i].next_next != 0 && sequence1[i].next != 0){
				push = sequence2[i].index;
			}
			child2.nodes.push_back(push);
		}

		chilren.push_back(child1);
		chilren.push_back(child2);
		return chilren;
	}

	solution mutation(solution node){
		random_device rd;  
        mt19937 g(rd());
        shuffle(node.nodes.begin(), node.nodes.end(), g);
        return node;
	}

    void Genetic_Algo(){
    	vector<solution> states;
    	for(int i=0; i<population_size; i++){
    		solution node;
    		for(int j=0; j<n_cities; j++){
    			node.nodes.push_back(j);
    		}
    		random_device rd;  
        	mt19937 g(rd());
        	shuffle(node.nodes.begin(), node.nodes.end(), g);
    		states.push_back(node);
    		//states[i].print_soln();
    	}


    	vector<double> fitman;
    	for(int i=0; i<population_size; i++){
    		fitman.push_back(heuristic(states[i]));
    		//cout << i << " - " << heuristic(states[i]) << "\n";
    	}


    	vector<float> ranks = rank(fitman,population_size); 
    	solution parent1,parent2;
    	int place1,place2;
    	for(int i=0; i<population_size; i++){
    		if(ranks[i] == population_size){
    			parent1 = states[i];
    			place1 = i;
    		}
    		else if(ranks[i] == population_size-1){
    			parent2 = states[i];
    			place2 = i;
    		}
    	}

    	srand((unsigned) time(0));
		int v1 = 1 + (rand() % n_cities) ;
		int v2 = 1 + (rand() % n_cities) ;
		if(v1 > v2){
			int temp = v1;
			v1 = v2;
			v2 = temp;
		}

    	std::vector<solution> v = crossover(parent1,parent2,population_size,v1,v2);
    	states[place1] = v[0];
    	states[place2] = v[1]; 

    	cout << "Genetic_Algo" << endl;
    	states[0].print_soln();
    	cout << "eval :" << heuristic(states[0]) << endl;;
    }

};


int main(int argc, char* argv[]) {

    TSP T((string) argv[1]);

    srand(time(NULL));

    //T.sim_anneal();

    T.Genetic_Algo();

    return 0;
}
