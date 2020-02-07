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
#define population_size 1000

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

struct Ant {
  public:
    vector<int> tour;

    void reset_ant() {
        int tour_size = tour.size();
        for(int i = 0; i < tour_size; ++i) {
            tour.pop_back();
        }
    }
};

struct CX
{
	int data;
	int index;
	CX(){
		this->data = -1;
		this->index = -1;
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
        // current_state.print_soln();
        // cout<< heuristic(current_state) << endl;
    }

    void PerformSearch(char c) {
        switch (c)
        {
        case '0':
            SimulatedAnnealing();
            break;
        case '1':
            Genetic_Algo();
            break;
        case '2':
            AntColonyOpt();
            break;
        default:
            cout<< "Invalid Search Chosen. Please Try Again.\n";
            break;
        }
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

    // Cooling schedule
    double updateTemp(double temp, double temp_max, double temp_min, int iter, int iter_max) {

        // Linear Sched
        double slope = (temp_max - temp_min)/(0 - iter_max);
        return iter*slope + temp_max;

        // Hyperbolic Sched
        return (double) iter_max / (double) (iter + 1);

    }

    // Simulated Annealing
    void SimulatedAnnealing() {
        // Init Temp
        double TEMP_MAX = 50, TEMP_MIN = 0.5;
        double Temperature = TEMP_MAX;
        int max_iters = 100, min_moves = 5;

        // Lower Temperature slowly
        for(int iter = 0; iter < max_iters; ++iter) {
            // cout<< "Temperature: " << Temperature << endl;

            // Get neighboring solutions
            vector<solution> next_states;
            moveGen(next_states);

            // Make a random move depending on its probability
            int moves = 0;
            while (moves < min_moves) {
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
                    moves++;
                    // cout<< "Eval: " << heuristic(current_state) << endl;
                }
            }
            Temperature = updateTemp(Temperature, TEMP_MAX, TEMP_MIN, iter, max_iters);
        }

        cout<< "Best Tour Found (Simulated Annealing): ";
        current_state.print_soln();
        cout<< "Length = " << heuristic(current_state) <<endl;
    }


    vector<float> rank(vector<double> A,int n) {
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


	vector<solution> cycle_crossover(solution parent1,solution parent2) {
		vector<solution> chilren;
		CX child1[n_cities];
		CX child2[n_cities];

		child1[0].data = parent1.nodes[0];
		child2[0].data = parent2.nodes[0];
		child2[0].index = 2;
		child1[0].index = 1;


		int check = 0,check1 = 0,check2 = 0;
		bool same = false;
		bool same1 = false;



		while(same == false || same1 == false) {
		//	cout << "entered" << endl;


		if(child2[check1].data == -1) {
			child2[check1].data = parent2.nodes[check1];
		}
		if(child1[check2].data == -1) {
			child1[check2].data = parent1.nodes[check2];
		}

		//checking if all elements in child1 are there in child2
		int copy[n_cities];
		for (int i = 0; i < n_cities; ++i)
		{
			copy[i] = 0;
		}
		for (int i = 0; i < n_cities; ++i)
		{
			for (int j = 0; j < n_cities; ++j)
			{
				if(child1[i].data == child2[j].data) {
					copy[i] = 1;
					break;
				}
			}
		}
		int handle = 0;
		for (int i = 0; i < n_cities; ++i)
		{
			if(copy[i] == 0){ same = false; handle =1; }
		}
		if(handle == 0){
			same = true;
		}

		//chechking if all elements in child2 are there in child1
		int copy1[n_cities];
		for (int i = 0; i < n_cities; ++i)
		{
			copy1[i] = 0;
		}
		for (int i = 0; i < n_cities; ++i)
		{
			for (int j = 0; j < n_cities; ++j)
			{
				if(child2[i].data == child1[j].data) {
					copy1[i] = 1;
					break;
				}
			}
		}
		int handle1 = 0;
		for (int i = 0; i < n_cities; ++i)
		{
			if(copy1[i] == 0){ same1 = false;  handle1 = 1;}
		}
		if(handle1 == 0){
			same1 = true;
		}

		int search1 = 0;
		for (int i = 0; i < n_cities; ++i)
		{
			if(child1[check2].data == child2[i].data) {
				search1 = 1;
			}
		}

		if(search1 == 0) {
			for (int i = 0; i < n_cities; ++i)
			{
				if(child1[check2].data == parent2.nodes[i]) {
					child2[i].data = parent2.nodes[i];
					child2[i].index = 2;
					check2 = i;
					break;
				}
			}
		}

		int search2 = 0;
		for (int i = 0; i < n_cities; ++i)
		{
			if(child2[check1].data == child1[i].data){
				search2 = 1;
			}
		}

		if(search2 == 0){
			for (int i = 0; i < n_cities; ++i)
			{
				if(child2[check1].data == parent1.nodes[i]) {
					child1[i].data = parent1.nodes[i];
					child1[i].index = 1;
					check1 = i;
					break;
				}
			}
		}

	}

	for (int i = 0; i < n_cities; ++i)
		{
			if(child1[i].data == -1){
				child1[i].data = parent2.nodes[i];
				child2[i].data = parent1.nodes[i];
			}
		}

	solution sequence1,sequence2;



	for (int i = 0; i < n_cities; ++i)
	{
		sequence1.nodes.push_back(child1[i].data);
		sequence2.nodes.push_back(child2[i].data);
	}



	chilren.push_back(sequence1);
	chilren.push_back(sequence2);


	return chilren;

	}

	vector<solution> PMX(solution parent1,solution parent2,int v1,int v2){
		vector<solution> chilren;
		vector<city> sequence1;
		vector<city> sequence2;
		for(int i=0; i<n_cities; i++){
			city a ;
			if(i < v1 || i > v2){
				a.index  = parent1.nodes[i];
			}
			else {
				a.index  = parent1.nodes[i];
				a.next = parent2.nodes[i];
				for(int j=v1; j<v2+1; j++) {
					if(a.next == parent1.nodes[j]) {
						a.next_next = parent2.nodes[j];
					}
				}
			}
			sequence1.push_back(a);
		}

		for(int i=0; i<n_cities; i++){
			city a ;
			if(i < v1 || i > v2){
				a.index  = parent2.nodes[i];
			}
			else {
				a.index  = parent2.nodes[i];
				a.next = parent1.nodes[i];
				for(int j=v1; j<v2+1; j++) {
					if(a.next == parent2.nodes[j]) {
						a.next_next = parent1.nodes[j];
					}
				}
			}
			sequence2.push_back(a);
		}

		solution child1,child2;

		for (int i = 0; i < n_cities; ++i)
		{
			int push = -1;
			if(i < v1 || i > v2){
				for(int j=v1; j<v2+1; j++){
				if(sequence1[i].index == sequence2[j].index){
					push = sequence2[j].next;
					bool ok = true;
					int num=v1;
					while(ok == true){
						int breakloop = 0;
						for(int j=v1; j<v2+1; j++){
							if(push == sequence2[j].index){
								push = sequence2[j].next;
								breakloop = 1;
							}
						} 
						if(breakloop == 0){ break; }
						if(num == v2){ break; }
						num++;
					}
				}
			}
			if(push == -1){
				push = sequence1[i].index;
			}
			}
			else {
				push = sequence2[i].index;
			}
		
			child1.nodes.push_back(push);
		}

		for (int i = 0; i < n_cities; ++i)
		{
			int push = -1;
			if(i < v1 || i > v2){
				for(int j=v1; j<v2+1; j++){
				if(sequence2[i].index == sequence1[j].index){
					push = sequence1[j].next;
					bool ok = true;
					int num=v1;
					while(ok == true){
						int breakloop = 0;
						for(int j=v1; j<v2+1; j++){
							if(push == sequence1[j].index){
								push = sequence1[j].next;
								breakloop = 1;
							}
						} 
						if(breakloop == 0){ break; }
						if(num == v2){ break; }
						num++;
					}
				}
			}
			if(push == -1){
				push = sequence2[i].index;
			}
			}
			else {
				push = sequence1[i].index;
			}
		
			child2.nodes.push_back(push);
		}

		/*cout << "child1" << endl;
		child1.print_soln();
		cout << "child 2" << endl;
		child2.print_soln();*/

		chilren.push_back(child1);
		chilren.push_back(child2);
		return chilren;
	}


	vector<solution> ordinalcrossover(solution parent1,solution parent2,int v1,int v2){
		int sequence1[n_cities];
		int sequence2[n_cities];

		for (int i = 0; i < n_cities; ++i)
		{
			sequence1[i] = -1;
			sequence2[i] = -1;
		}

		int a=0;
		int b=0;
		for (int i = 0; i < n_cities; ++i)
		{
			if(i >v1-1 || i < v2+1){
				sequence1[a] = parent1.nodes[i];
				a++;
				sequence2[b] = parent2.nodes[i];
				b++;
			}
		}

		for (int i = 0; i < n_cities; ++i)
		{
			int ok = 0;
			int ok1 = 0;
			for (int j = 0; j < n_cities; ++j)
			{
				if(parent2.nodes[i] == sequence1[j]){
					ok = 1;
				}
				if(parent1.nodes[i] == sequence2[j]) {
					ok1 = 1;
				}
			}
			if(ok == 0) {
				sequence1[a] = parent2.nodes[i];
				a++;
			}
			if(ok1 == 0) {
				sequence2[b] = parent1.nodes[i];
				b++;
			}
		}

		solution child1,child2;
		for (int i = 0; i < n_cities; ++i)
		{
			child1.nodes.push_back(sequence1[i]);
			child2.nodes.push_back(sequence2[i]);
		}

		std::vector<solution> chilren;
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

    	//Generating population of population_size
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

    	//randomly choosing two points for the crossover
    	srand((unsigned) time(0));
		int v1 = 1 + (rand() % n_cities) ;
		int v2 = 1 + (rand() % n_cities) ;
		if(v1 > v2){
			int temp = v1;
			v1 = v2;
			v2 = temp;
		}


		//loop for iterating
		int count = 0;
		while(count < 100){

		//calculating fitman for all the population
    	vector<double> fitman;
    	for(int i=0; i<population_size; i++){
    		fitman.push_back(heuristic(states[i]));
    		//cout << i << " - " << heuristic(states[i]) << "\n";
    	}


    	//choosing two parents from the population using fitman
    	vector<float> ranks = rank(fitman,population_size); 
    	solution parent1,parent2;
    	int place1,place2;
    	for(int i=0; i<population_size; i++){
    		if(ranks[i] == 1){
    			parent1 = states[i];
    			place1 = i;
    		}
    		else if(ranks[i] == 2){
    			parent2 = states[i];
    			place2 = i;
    		}
    	}


    	//Crossover of the parents and replacing parents with obtained children in the population
    	std::vector<solution> v = PMX(parent1,parent2,v1,v2);
    	states[place1] = v[0];
    	states[place2] = v[1];

		int num = (rand() % 1);
		if(!num) {
			states[place1] = mutation(states[place1]);
		} else {
			states[place2] = mutation(states[place2]);
		}

    	count++;
	    }

	    //choosing the shortest tour path and printing it
	    std::vector<double> value;
	    for (int i = 0; i < population_size; ++i)
	    {
	    	value.push_back(heuristic(states[i]));
	    }

	    std::vector<float> value1 = rank(value,population_size);
	    for (int i = 0; i < population_size; ++i)
	    {
	    	if(value1[i] == 1){
	    		cout << "\nTour Length : " << heuristic(states[i]) << endl;
	    		cout << "Genetic_Algo : " << endl;
	    		states[i].print_soln();
	    	}
	    }
    }

    double tour_len(Ant ant) {
        if(ant.tour.size() != n_cities)
            cout<< "Tour not right\n";

        double total = 0;
        for(int i = 0; i < ant.tour.size() - 1; ++i) {
            total += graph[ant.tour[i]][ant.tour[i + 1]];
        }
        total += graph[ant.tour[ant.tour.size() - 1]][ant.tour[0]];

        return total;
    }

    bool search(vector<int> &cities, int city) {
        for (size_t i = 0; i < cities.size(); i++) {
            if(cities[i] == city)
                return true;
        }
        return false;
    }

    void AntColonyOpt() {
        // Initialise ants
        int num_ants = 100, num_iters = 10;
        double alpha = 2, beta = 0.5, rho = 0.3, Q = 5;
        vector<Ant> ants;
        for(int i=0; i < num_ants; ++i) {
            Ant temp_ant;
            temp_ant.tour.push_back(rand() % n_cities);

            ants.push_back(temp_ant);
        }

        // Initialise phermone on trails
        vector<vector<double>> phermone;
        double rand_init = static_cast <double> (rand()) / static_cast <double> (RAND_MAX);
        for(int i = 0; i < n_cities; ++i) {
            vector<double> temp_pherm;
            for(int j = 0; j < n_cities; ++j) {
                temp_pherm.push_back(rand_init);
            }
            phermone.push_back(temp_pherm);
        }

        // Initialise best tour
        int best_tour[n_cities], best_len = INT_MAX;

        for (int iters = 0; iters < num_iters; iters++) {
            double L[num_ants];
            cout<< "Iter: " << iters << endl;
            for(int l=0; l < num_ants; ++l) {
                int current_city = ants[l].tour[ants[l].tour.size() - 1];
                while(ants[l].tour.size() != n_cities) {
                    // cout<< ants[l].tour.size() <<endl;

                    // Make allowed cities
                    vector<int> allowed;
                    for(int i=0; i < n_cities; ++i) {
                        if(!search(ants[l].tour, i)) {
                            allowed.push_back(i);
                        }
                    }
                    int allowed_len = allowed.size();
                    if(allowed_len == 1) {
                        ants[l].tour.push_back(allowed[0]);
                        continue;
                    }
                    
                    // Create cumulative prob vector
                    double prob[allowed_len];
                    for (size_t i = 0; i < allowed_len; i++) {
                        prob[i] = 0;
                    }
                    
                    double net_prob = 0;
                    for(int j=0; j<allowed_len; ++j) {
                        if(j != current_city)
                            net_prob += pow(phermone[current_city][allowed[j]], alpha) * pow((1 / graph[current_city][allowed[j]]), beta);
                    }

                    for(int j=0; j<allowed_len; ++j) {
                        double temp_prob;
                        temp_prob = (pow(phermone[current_city][allowed[j]], alpha) * pow((1 / graph[current_city][allowed[j]]), beta)) / net_prob;
                        if(j > 0)
                            prob[j] = prob[j - 1] + temp_prob;
                        else
                            prob[j] = temp_prob;
                    }

                    double rand_doub = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/prob[allowed_len - 1]));
                    for (int i = 0; i < allowed_len - 1; i++) {
                        if(rand_doub <= prob[i + 1]) {
                            ants[l].tour.push_back(allowed[i]);
                            break;
                        }
                    }
                    
                }

                L[l] = tour_len(ants[l]);

                // Check with best tour
                if(L[l] < best_len) {
                    for(int i = 0; i < n_cities; ++i) {
                        best_tour[i] = ants[l].tour[i];
                    }
                    best_len = L[l];
                }
            }

            // Update pheromone
            for(int i = 0; i < n_cities; ++i) {
                for(int j = 0; j < n_cities; ++j) {
                    double delta_ij = 0;
                    for(int l=0; l < num_ants; ++l) {
                        double delta_k = 0;
                        bool traversed_ij = false;

                        for(int ant_idx=0; ant_idx < n_cities - 1; ++ant_idx) {
                            if(ants[l].tour[ant_idx] == i && ants[l].tour[ant_idx + 1] == j)
                                traversed_ij = true;
                        }
                        if(ants[l].tour[n_cities - 1] == i && ants[l].tour[0] == j)
                            traversed_ij = true;

                        if(traversed_ij)
                            delta_k = Q/L[l];

                        delta_ij += delta_k;
                    }

                    phermone[i][j] = (rho * phermone[i][j]) + delta_ij;
                }
            }
        }
        
        // Print best tour
        cout<< "Best Tour (Ant Colony Optimisation):";
        for(int i=0; i < n_cities; ++i) {
            cout<< " " << best_tour[i];
        }
        cout<< "\nBest Tour Len: " << best_len <<endl;
    }
};


int main(int argc, char* argv[]) {

    TSP T((string) argv[1]);

    srand(time(NULL));

    // Run Simulated Annealing by Default
    if(argc < 3) {
        T.PerformSearch('0');
    } else {
        T.PerformSearch(argv[2][0]);
    }

    return 0;
}
