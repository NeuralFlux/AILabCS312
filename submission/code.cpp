#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <random>

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
        // return iter*slope + temp_max;

        // Geometric Cooling Schedule
        return 0.999 * temp;

        // Hyperbolic Sched
        return (double) iter_max / (double) (iter + 1);

    }

    // Simulated Annealing
    void SimulatedAnnealing(clock_t start) {
        // Init Temp
        double TEMP_MAX = 100, TEMP_MIN = 0.5;
        double Temperature = TEMP_MAX;
        int max_iters = 10000, min_moves = 5;

        // Lower Temperature slowly
        for(int iter = 0; iter < max_iters; ++iter) {
            clock_t current;
            double cpu_time_used;
            current = clock();
            cpu_time_used = ((double) (current - start)) / CLOCKS_PER_SEC;

            if(Temperature <= 1.1 || cpu_time_used >= 280) {
                break;
            }
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

        current_state.print_soln();
        // cout << heuristic(current_state) <<endl;
    }
};


int main(int argc, char* argv[]) {

    clock_t start;
    start = clock();

    TSP T((string) argv[1]);

    srand(time(NULL));

    T.SimulatedAnnealing(start);

    return 0;
}
