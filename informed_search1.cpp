#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <set>
#include <queue>

using namespace std;

class Node {
  public:
  // Found means it's added to the stack
  // Visited means it's been traversed
    pair<int, int> coord;
    char data;
    bool visited;
    bool found;
    int depth;
    float heuristic;
    Node* parent;

    Node(int row, int col, char data) {
        this->coord.first = row;
        this->coord.second = col;
        this->data = data;
        this->visited = false;
        this->found = false;
        this->depth = INT32_MAX;  // As INT32_MAX overflows when incremented
        this->heuristic = 0;
        this->parent = NULL;
    }

    bool operator<(const Node* &rhs) {
        return this->heuristic < rhs->heuristic;
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
               graph[potential_nbrs[i].first][potential_nbrs[i].second].found == true)) {
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
     mode: BFS, DFS or DFID
    */
    vector<vector<Node>> graph;
    pair<int, int> current_coord;
    pair<int, int> dest_coord;
    int line_len;
    char mode;
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
    }

    /* Heuristics
    
    
    
    
    */

    float heuristic(pair<int, int> x) {
        return abs(x.first - dest_coord.first) + abs(x.second - dest_coord.second);
    }

    void performSearch() {
        // Perform the required search
        switch (mode) {
        case '0':
            BFS();
            break;
        
         case '1':
             HC();
             break;
        
         case '2':
             BS();
             break;

        default:
            cout<< "Please enter a valid search type" << endl;
            break;
        }
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

    void reset() {
        // Reset the maze to initial configuration
        current_coord = make_pair(0, 0);

        for (vector<vector<Node>>::iterator row = graph.begin(); row != graph.end(); ++row) {
            for(vector<Node>::iterator col = row->begin(); col != row->end(); ++col) {
                col->depth = INT32_MAX-1;
                col->found = false;
                col->heuristic = 0;
                col->parent = NULL;
                col->visited = false;
            }
        }

        graph[0][0].depth = 0;
        graph[0][0].found = true;
    }

    void BFS() {
        // Set of nodes
        priority_queue <Node*> open;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;
	//cout << dest_coord.first << " " << dest_coord.second << '\n';
        while(!open.empty()) {
            if(goalTest(open.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {
		//cout << open.top()->heuristic << "   " << open.top()->coord.first << " " << open.top()->coord.second << '\n';
                current_coord = open.top()->coord;
                open.pop();
		
                graph[current_coord.first][current_coord.second].heuristic = heuristic(current_coord);
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
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
			
                        open.push(&graph[neighbors[i].first][neighbors[i].second]);
                    }
                }
		
            }
        }

        // graph[1][0].heuristic = 6;
        // graph[2][0].heuristic = 1;
        // graph[3][0].heuristic = 7;
        // open.push(&graph[1][0]);
        // open.push(&graph[2][0]);
        // open.push(&graph[3][0]);
        
        // while (!open.empty()) {
        //     cout<< "Heu: " << open.top()->heuristic <<endl;
        //     open.pop();
        // }

        // Print the number of visited states
        cout<< countClosed() << endl;
    }



	void HC() {
        // Set of nodes
        priority_queue <Node*> open;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        while(!open.empty()) {
            if(goalTest(open.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {

                current_coord = open.top()->coord;
                open.pop();

                graph[current_coord.first][current_coord.second].heuristic = heuristic(current_coord);
                graph[current_coord.first][current_coord.second].visited = true;
                graph[current_coord.first][current_coord.second].found = true;

                // Initialise neighbors to pass on to moveGen()
                pair<int, int> neighbors[4];
                for(int i = 0; i < 4; i++) {
                    neighbors[i].first = -1;
                    neighbors[i].second = -1;
                }

		//dequeueing the neighbours of the previous current node
		for(int h=0; h<open.size(); h++) {
			open.pop();
		}

                // Get possible moves from moveGen() and rest remain (-1, -1)
                moveGen(current_coord, neighbors, graph);
                for(int i = 0; i < 4; i++) {
                    if(neighbors[i].first != -1 && neighbors[i].second != -1) {
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];

                        open.push(&graph[neighbors[i].first][neighbors[i].second]);
                    }
                }
            }
        }

        cout<< countClosed() << endl;
    }


	
	/*void Con_of_Beam(priority_queue <Node*> open,priority_queue <Node*> beam) {

	while(!beam.empty()) {
            
		
		//for(int j=0; j<beam.size(); j++) 
                current_coord = beam.top()->coord;
                beam.pop();

                graph[current_coord.first][current_coord.second].heuristic = heuristic(current_coord);
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
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];

                        open.push(&graph[neighbors[i].first][neighbors[i].second]);
                    }
                }

            
	
        }

	
	}


	void BS() {
        // Set of nodes
        priority_queue <Node*> open;
	priority_queue <Node*> beam;

	// width of the beam
	int beam_length = 2;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;


	open.pop();
	graph[0][0].heuristic = heuristic(open.top()->coord);
	graph[0][0].visited = true;
	graph[0][0].found = true;
	pair<int, int> neighbor[4];
                for(int i = 0; i < 4; i++) {
                    neighbor[i].first = -1;
                    neighbor[i].second = -1;
                }
	moveGen(current_coord, neighbor, graph);
	for(int i = 0; i < 4; i++) {
                    if(neighbor[i].first != -1 && neighbor[i].second != -1) {
                        graph[neighbor[i].first][neighbor[i].second].found = true;
                        graph[neighbor[i].first][neighbor[i].second].heuristic = heuristic(neighbor[i]);
                        graph[neighbor[i].first][neighbor[i].second].parent = &graph[0][0];

                        open.push(&graph[neighbor[i].first][neighbor[i].second]);
                    }
         }

	//copying beam_length of nodes in beam
	if(open.size() > beam_length) {
		for(int i=0; i<beam_length; i++) {
			beam.push(open.top());
			open.pop();
		}
	}
	else {
		for(int i=0; i<open.size(); i++) {
			beam.push(open.top());
			open.pop();
		}
	}
	
	//dequeueing open
	if(open.size() > beam_length) {
		for(int i=0; i<open.size(); i++) {
			open.pop();
		}
	}



        while(!beam.empty()) {
		cout << "hello\n";
            if(goalTest(beam.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {
		
		Con_of_Beam(open,beam);
	
		for(int i=0; i<beam_length; i++) {
			beam.push(open.top());
			open.pop();
		}

            }
	
        }

	
        cout<< countClosed() << endl;
    }*/



    void backTrack() {
        // Backtrack the solved maze and update the path
        int path_len = 1;
        Node *tracer = &graph[dest_coord.first][dest_coord.second];
        while(tracer->parent != NULL) {
            graph[tracer->coord.first][tracer->coord.second].data = '0';
            tracer = tracer->parent;
            path_len++;
        }

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
