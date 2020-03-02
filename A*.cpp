#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <set>
#include <queue>
#include<algorithm>
#define tabu_tenure 5

using namespace std;

class Node {
  public:
  // Found means it's added to the stack
  // Visited means it's been traversed
    pair<int, int> coord;
    char data;
    bool visited;
    bool inopen;
    bool found;
    int depth;
    float heuristic;
    float g;
    float f;
    Node* parent;

    Node(int row, int col, char data) {
        this->coord.first = row;
        this->coord.second = col;
        this->data = data;
        this->visited = false;
        this->inopen = false;
        this->found = false;
        this->depth = INT32_MAX - 1;
        this->heuristic = 0;
        this->g = 0;
        this->f = 0;
        this->parent = NULL;
    }

    Node(int row, int col, char data, bool vis, bool found, int depth, int heu) {
        this->coord.first = row;
        this->coord.second = col;
        this->data = data;
        this->visited = vis;
        this->found = found;
        this->depth = depth;
        this->heuristic = heu;
    }

    bool operator<(const Node &rhs) {
        return heuristic < rhs.heuristic;
    }
};

struct LessThanByHeu
{
  bool operator()(const Node* lhs, const Node* rhs) const {
    return lhs->f < rhs->f;
  }
};

void moveGen(pair<int, int> current_coord, pair<int, int> neighbors[], vector<vector<Node>>& graph) {
    // Initialise potential neighbors (DOWN, UP, RIGHT, LEFT)
    pair<int, int> potential_nbrs[4];
    potential_nbrs[0].first = current_coord.first + 1;
    potential_nbrs[0].second = current_coord.second;

    potential_nbrs[1].first = current_coord.first - 1;
    potential_nbrs[1].second = current_coord.second;

    potential_nbrs[2].first = current_coord.first;
    potential_nbrs[2].second = current_coord.second + 1;

    potential_nbrs[3].first = current_coord.first;
    potential_nbrs[3].second = current_coord.second - 1;

    // Check and update neighbors which are empty and undiscovered
    for(int i = 0; i < 4; i++) {
        if(potential_nbrs[i].first >= 0 && potential_nbrs[i].second >= 0) {  // Possible nodes only
            // Check if node is visited/found already
            if((graph[potential_nbrs[i].first][potential_nbrs[i].second].visited == true ||
               graph[potential_nbrs[i].first][potential_nbrs[i].second].found == true) &&
               graph[current_coord.first][current_coord.second].depth + 1 >= graph[potential_nbrs[i].first][potential_nbrs[i].second].depth) {
                continue;
            }

            // Possible to move only if free or goal
            char curr_char = graph[potential_nbrs[i].first][potential_nbrs[i].second].data;

            if(curr_char == ' ' || curr_char == '*') {
                neighbors[i].first = potential_nbrs[i].first;
                neighbors[i].second = potential_nbrs[i].second;
            }
        }
    }
    return;
}


void PropogateImprovement(pair<int,int> neighbor, vector<vector<Node>>& graph)
{
	pair<int, int> neighbors1[4];
	for(int i = 0; i < 4; i++) {
		neighbors1[i].first = -1;
		neighbors1[i].second = -1;
	}
	moveGen(neighbor, neighbors1, graph);
	for(int i=0; i<4; i++) {
		if(neighbor.first != -1 && neighbor.second != -1) {
			float new_g = graph[neighbor.first][neighbor.second].g + 1;
			float g_s = graph[neighbor.first][neighbor.second].g + 1;
			if(new_g < g_s) {
				graph[neighbors1[i].first][neighbors1[i].second].parent = &graph[neighbor.first][neighbor.second];
				graph[neighbors1[i].first][neighbors1[i].second].g = new_g;
				if(graph[neighbors1[i].first][neighbors1[i].second].visited == true)
				{
					PropogateImprovement(neighbors1[i],graph);
				}
			}
		}
	}
}


// Compare the current state to goal state
bool goalTest(pair<int, int> current_coord, pair<int, int> dest_coord) {
    return current_coord == dest_coord;
}

// A Maze class to simulate the maze
class Maze {
private:
    /*
     graph: Contains all the data and nodes
     current_coord: The current co-ordinates
     dest_coord: The destination co-ordinates
     line_len: The length of a row (no. of chars)
     mode: Search type
     graph_bck: Backup of current maze
    */
    vector<vector<Node>> graph;
    pair<int, int> current_coord;
    pair<int, int> dest_coord;
    int line_len;
    char mode;

    vector<vector<Node>> graph_bck;
public:
    Maze(string file_name) {
        // Read and store input
        string line;
        int row = 0;
        bool first_line = true;

        ifstream infile(file_name);

        while (getline(infile, line)) {
            if(first_line) {
                mode = line[0];
                first_line = false;
                continue;
            }

            // Get max columns in the maze
            line_len = line.length();

            // Fill and insert nodes into the graph
            vector<Node> nodes;
            for(int col=0; col < line_len; col++) {
                nodes.push_back(Node (row, col, line[col]));
            }
            graph.push_back(nodes);

            // Extract the destination co-ordinates
            size_t found = line.find("*");
            if(found != string::npos) {
                dest_coord.first = row;
                dest_coord.second = (int) found;
            }

            // Advance to next row
            row++;
        }

        // Initialise current coord
        current_coord = make_pair(0, 0);
        graph[0][0].data = '0';

        // Create backup of current graph
        create_bck();
    }

     // Manhattan Distance
    int heuristic_1(pair<int, int> x) {
        // printf("Heu(%d, %d): %d\n", x.first, x.second, abs(x.first - dest_coord.first) + abs(x.second - dest_coord.second));
        return -(abs(x.first - dest_coord.first) + abs(x.second - dest_coord.second));
    }

    // Chebyshev Distance
    int heuristic_2(pair<int, int> x) {
        return -max(abs(x.first - dest_coord.first), abs(x.second - dest_coord.second));
    }

    void performSearch() {

        // Initialise variables to measure time
        clock_t start, end;
        double cpu_time_used;

        start = clock();

        // Perform the required search
        switch (mode) {
        case '0':
            A_star();
            break;

        default:
            cout<< "Please enter a valid search type" << endl;
            break;
        }

        // Calculate time taken
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        cout<< "Time taken: "<< cpu_time_used<<endl;
    }

    void printMaze() {
        // Print the maze character by character
        for (vector<vector<Node>>::const_iterator row = graph.begin(); row != graph.end(); ++row) {
            for(vector<Node>::const_iterator col = row->begin(); col != row->end(); ++col) {
                cout<< col->data;
            }
            cout<< endl;
        }
    }

    int countClosed() {
        // Count the number of visited nodes
        int states = 0;
        for(int i=0; i < graph.size(); i++) {
            for(int j=0; j < line_len; j++) {
                if(graph[i][j].visited == true) {
                    states++;
                }
            }
        }

        return states;
    }

    // Make a backup of current maze
    void create_bck() {
        for(int i=0; i<graph.size(); ++i) {
            vector<Node> temp_vec;
            for(int j=0; j < graph[i].size(); ++j) {
                temp_vec.push_back(Node(graph[i][j].coord.first,
                                        graph[i][j].coord.second,
                                        graph[i][j].data,
                                        graph[i][j].visited,
                                        graph[i][j].found,
                                        graph[i][j].depth,
                                        graph[i][j].heuristic));
            }
            graph_bck.push_back(temp_vec);
        }
    }

    void A_star() {
        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].inopen = true;
        graph[current_coord.first][current_coord.second].found = true;
        graph[current_coord.first][current_coord.second].heuristic = heuristic_2(current_coord);
        graph[current_coord.first][current_coord.second].f = heuristic_2(current_coord);

        while(!open.empty()) {

            if(goalTest(open.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {
                current_coord = open.top()->coord;
                open.pop();
                graph[current_coord.first][current_coord.second].inopen = false;

                graph[current_coord.first][current_coord.second].heuristic = heuristic_2(current_coord);
                graph[current_coord.first][current_coord.second].visited = true;
                graph[current_coord.first][current_coord.second].found = true;

                // Initialise neighbors to pass on to moveGen()
                pair<int, int> neighbors[4];
                for(int i = 0; i < 4; i++) {
                    neighbors[i].first = -1;
                    neighbors[i].second = -1;
                }

                // Get possible moves from moveGen() and rest remain (-1, -1)
                moveGen(current_coord, neighbors, graph);
                for(int i = 0; i < 4; i++) {
                    if(neighbors[i].first != -1 && neighbors[i].second != -1) {

                    	if(graph[neighbors[i].first][neighbors[i].second].inopen == false && 
                    		graph[neighbors[i].first][neighbors[i].second].visited == false)
	                    {    
	                    	graph[neighbors[i].first][neighbors[i].second].found = true;
	                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_2(neighbors[i]);
	                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
	                        graph[neighbors[i].first][neighbors[i].second].g = graph[neighbors[i].first][neighbors[i].second].parent->g + 1;
	                        graph[neighbors[i].first][neighbors[i].second].f = graph[neighbors[i].first][neighbors[i].second].g + heuristic_2(neighbors[i]);
	                        open.push(&graph[neighbors[i].first][neighbors[i].second]);
	                        graph[neighbors[i].first][neighbors[i].second].inopen = true;
	                    }

	                    else if(graph[neighbors[i].first][neighbors[i].second].inopen == true)
	                    {
	                    	float neigh_g = graph[current_coord.first][current_coord.second].g + 1;
	                    	if(graph[current_coord.first][current_coord.second].g + 1 < neigh_g)
	                    	{
	                    		graph[neighbors[i].first][neighbors[i].second].found = true;
		                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_2(neighbors[i]);
		                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
		                        graph[neighbors[i].first][neighbors[i].second].g = graph[neighbors[i].first][neighbors[i].second].parent->g + 1;
		                        graph[neighbors[i].first][neighbors[i].second].f = graph[neighbors[i].first][neighbors[i].second].g + heuristic_2(neighbors[i]);
	                    	}
	                    }

	                    else if(graph[neighbors[i].first][neighbors[i].second].visited == true)
	                    {
	                    	float neigh_g = graph[current_coord.first][current_coord.second].g + 1;
	                    	if(graph[current_coord.first][current_coord.second].g + 1 < neigh_g)
	                    	{
	                    		graph[neighbors[i].first][neighbors[i].second].found = true;
		                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_2(neighbors[i]);
		                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
		                        graph[neighbors[i].first][neighbors[i].second].g = graph[neighbors[i].first][neighbors[i].second].parent->g + 1;
		                        graph[neighbors[i].first][neighbors[i].second].f = graph[neighbors[i].first][neighbors[i].second].g + heuristic_2(neighbors[i]);
		                        PropogateImprovement(neighbors[i],graph);                    	
		                    }
	                    }
                    }
                }
            }
        }

        // Print the number of visited states
        cout<< countClosed() << endl;
    }

	


 

    void backTrack() {
        // Backtrack the solved maze and update the path
        int path_len = 1;
        Node *tracer = &graph[dest_coord.first][dest_coord.second];
        while(tracer->parent != NULL) {
            graph[tracer->coord.first][tracer->coord.second].data = '0';
            tracer = tracer->parent;
            path_len++;
        }

        if(mode != '4')
            cout<< path_len<<endl;
    }
};

int main(int argc, char* argv[]) {

    // Take input
    Maze M((string) argv[1]);

    // Do the required search
    M.performSearch();

    // Reconstruct path
    M.backTrack();

    // Print the solution
    M.printMaze();

    return 0;
}

