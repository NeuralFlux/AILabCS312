#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <set>
#include <queue>
#include<algorithm>
#define tabu_tenure 3

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
        this->depth = INT32_MAX - 1;
        this->heuristic = 0;
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
    return lhs->heuristic < rhs->heuristic;
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
        // Perform the required search
        switch (mode) {
        case '0':
            BFS();
            break;
        
         case '1':
             HC(1);
             break;
        
         case '2':
             BS();
             break;

         case '3':
             TS();
             break;

         case '4':
             VND();
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

    void soft_reset() {
        // Reset only depth, found and visited
        for (vector<vector<Node>>::iterator row = graph.begin(); row != graph.end(); ++row) {
            for(vector<Node>::iterator col = row->begin(); col != row->end(); ++col) {
                col->depth = INT32_MAX-1;
                col->found = false;
                col->visited = false;
                col->heuristic = 0;
            }
        }
    }

    void save_state() {
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

    void load_state() {
        for(int i=0; i<graph.size(); ++i) {
            for(int j=0; j < graph[i].size(); ++j) {
                graph[i][j].coord = graph_bck[i][j].coord;
                graph[i][j].data = graph_bck[i][j].data;
                graph[i][j].visited = graph_bck[i][j].visited;
                graph[i][j].found = graph_bck[i][j].found;
            }
        }
    }

    void BFS() {
        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

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
                // cout<< "Top of queue "<< current_coord.first << ", " << current_coord.second <<endl;
                graph[current_coord.first][current_coord.second].heuristic = heuristic_1(current_coord);
                graph[current_coord.first][current_coord.second].visited = true;
                graph[current_coord.first][current_coord.second].found = true;
                // graph[current_coord.first][current_coord.second].data = '0';

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
                        // graph[neighbors[i].first][neighbors[i].second].data = '1';
                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_1(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
                        // printf("Heu(%d, %d): %d\n", neighbors[i].first, neighbors[i].second, heuristic_1(neighbors[i]));

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

	void BS() {
        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

        // Beam size
        int beam = 80;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        // Condition to break the while loop 
        int breakloop = 0;

        while(!open.empty()) {

            if(breakloop == 1) { break; }

            for(int k=0; k<open.size(); k++) {
                if(goalTest(open.top()->coord, dest_coord)) {
                    cout<< "Goal Achieved"<<endl;
                    breakloop = 1;
                    break;
                } else {

                    current_coord = open.top()->coord;
                	open.pop();

                	graph[current_coord.first][current_coord.second].heuristic = heuristic_1(current_coord);
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
                	        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_1(neighbors[i]);
                	        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];

                	        open.push(&graph[neighbors[i].first][neighbors[i].second]);
                	    }
                	}
                }
            }
	    
	        // Filtering the open queue with beam number of elements
            priority_queue <Node*> temporary;
            
			if(open.size() > beam) {
                for(int i=0; i<beam; i++) {
                    temporary.push(open.top());
		            open.pop();
				}
                for(int h=0; h<open.size(); h++) {
				    open.pop();
			    }
                for(int i=0; i<beam; i++) {
                    open.push(temporary.top());
		            temporary.pop();
				}
            }
        }
        cout<< countClosed() << endl;
    }

    void TS() {

        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

        deque<pair<int, int>> tabu;
        bool first = true;

        pair<int, int> best;
        best.first = current_coord.first;
        best.second = current_coord.second;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        while(!open.empty()) {
            if(goalTest(open.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {

                if(tabu.size() == tabu_tenure) {
                    tabu.pop_front();
                }

                if(!first) {
                    tabu.push_back(current_coord);
                }
                first = false;

                current_coord = open.top()->coord;
                open.pop();

                graph[current_coord.first][current_coord.second].heuristic = heuristic_1(current_coord);
                graph[current_coord.first][current_coord.second].visited = true;
                graph[current_coord.first][current_coord.second].found = true;
                // graph[current_coord.first][current_coord.second].data = '0';

                // Initialise neighbors to pass on to moveGen()
                pair<int, int> neighbors[4];
                for(int i = 0; i < 4; i++) {
                    neighbors[i].first = -1;
                    neighbors[i].second = -1;
                }

                //dequeueing the neighbours of the previous current node
                int temp_size = 0;
                while(temp_size < open.size()) {
                    open.pop();
                    temp_size++;
                }

                cout<< "Current Node: (" << current_coord.first << ", " << current_coord.second <<")" <<endl;
                // Get possible moves from moveGen() and rest remain (-1, -1)
                moveGen(current_coord, neighbors, graph);

                bool all_bad = true;
                for(int i = 0; i < 4; i++) {
                    if(neighbors[i].first != -1 && neighbors[i].second != -1) {
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        // graph[neighbors[i].first][neighbors[i].second].data = '1';
                        graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_1(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
                        
                        // Check if this neighbor is better than current
                        if(heuristic_1(neighbors[i]) > heuristic_1(current_coord))
                            all_bad = false;

                        // Check if neighbor is in tabu list
                        if(tabu.size() == 0) {
                            open.push(&graph[neighbors[i].first][neighbors[i].second]);
                        } else {
                            for(deque<pair<int, int>>::const_iterator k=tabu.begin(); k!=tabu.end(); k++){
                                cout<< "Tabu move: (" << k->first <<", "<<k->second <<")" <<endl;
                                if(k->first != neighbors[i].first && k->second != neighbors[i].second) {
                                    open.push(&graph[neighbors[i].first][neighbors[i].second]);
                                    if(heuristic_1(best) < heuristic_1(neighbors[i])){
                                        best.first = neighbors[i].first;
                                        best.second = neighbors[i].second;
                                    }
                                }
                            }
                        }
                    }
                }

                // Aspiration Criteria
                for(int i  = 0; i < 4; i++) {
                    for(deque<pair<int, int>>::const_iterator k=tabu.begin(); k!=tabu.end(); k++){
                        if(k->first == neighbors[i].first && k->second == neighbors[i].second && all_bad && heuristic_1(neighbors[i]) > heuristic_1(best)) {
                            
                            //dequeueing the bad moves
                            int temp_size = 0;
                            while(temp_size < open.size()) {
                                open.pop();
                                temp_size++;
                            }

                            // add the better move and update best move
                            open.push(&graph[neighbors[i].first][neighbors[i].second]);
                            best.first = neighbors[i].first;
                            best.second = neighbors[i].second;
                        }
                    }
                }
        
            }
        }
        cout<< "Current Node: (" << current_coord.first << ", " << current_coord.second <<")" <<endl;
        cout<< "Destination Node: (" << dest_coord.first << ", " << dest_coord.second <<")" <<endl;
        cout<< countClosed() << endl;
    }


    // void function(int max_depth,pair<int,int> current_coord, priority_queue<Node*>& open) {
        

    //     graph[current_coord.first][current_coord.second].heuristic = heuristic(current_coord);
    //     graph[current_coord.first][current_coord.second].visited = true;
    //     graph[current_coord.first][current_coord.second].found = true;

    //     // Initialise neighbors to pass on to moveGen()
    //     vector<pair<int, int>> neigh;

    //     //dequeueing the neighbours of the previous current node
    //     for(int h=0; h<open.size(); h++) {
    //         open.pop();
    //     }

    //     MoveGen1(graph[current_coord.first][current_coord.second], max_depth, neigh, graph);
    //     for(int i=0; i<neigh.size(); i++) {
    //         open.push(&graph[neigh[i].first][neigh[i].second]);
    //     }
    // }

    void DB_DFS(Node &node, int max_depth, vector<pair<int, int>> &next_states) {
        // Update the current position
        current_coord = pair<int, int> (node.coord.first, node.coord.second);

        if(node.depth > max_depth)
            return;

        // Initialise neighbors to pass on to moveGen()
        pair<int, int> neighbors[4];
        for(int i = 0; i < 4; i++) {
            neighbors[i].first = -1;
            neighbors[i].second = -1;
        }

        // Update the visited nodes
        node.visited = true;

        // Get possible moves from moveGen() and rest remain (-1, -1)
        moveGen(current_coord, neighbors, graph);
        for(int i = 0; i < 4; i++) {
        // cout<< neighbors[i].first << " " << neighbors[i].second <<endl;
            if(neighbors[i].first != -1 && neighbors[i].second != -1 && node.depth + 1 <= max_depth) {
                graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_1(neighbors[i]);
                graph[neighbors[i].first][neighbors[i].second].found = true;
                graph[neighbors[i].first][neighbors[i].second].depth = node.depth + 1;
                graph[neighbors[i].first][neighbors[i].second].parent = &node;
                
                bool found_node = false;
                for(vector<pair<int, int>>::iterator idx = next_states.begin(); idx != next_states.end(); ++idx) {
                    if(*idx == neighbors[i]) {
                        found_node = true;
                        break;
                    }
                }
                if(!found_node && !graph_bck[neighbors[i].first][neighbors[i].second].found
                    && !graph_bck[neighbors[i].first][neighbors[i].second].visited &&
                    (graph_bck[neighbors[i].first][neighbors[i].second].data == ' ' || graph_bck[neighbors[i].first][neighbors[i].second].data == '*')) {
                    next_states.push_back(neighbors[i]);
                }

                DB_DFS(graph[neighbors[i].first][neighbors[i].second], max_depth, next_states);
                if(node.depth > max_depth)
                    return;
            }
        }
    }

    void moveGen_variable(pair<int, int> location, int max_depth, vector<pair<int, int>> &next_states) {
        save_state();

        for (int depth = 1; depth <= max_depth; depth++) {
            graph[location.first][location.second].depth = 0;
            graph[location.first][location.second].found = 0;

            DB_DFS(graph[location.first][location.second], depth, next_states);

            load_state();
        }

        current_coord = location;
    }

    void HC(int max_depth) {
        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

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

                cout<< "Current Coord: (" << current_coord.first << ", " << current_coord.second <<")\n";

                graph[current_coord.first][current_coord.second].heuristic = heuristic_1(current_coord);
                graph[current_coord.first][current_coord.second].visited = true;
                graph[current_coord.first][current_coord.second].found = true;

                // Initialise neighbors to pass on to moveGen()
                vector<pair<int, int>> neighbors;

                //dequeueing the neighbours of the previous current node
                for(int h=0; h<open.size(); h++) {
                    open.pop();
                }

                // Get possible moves from moveGen() and rest remain (-1, -1)
                moveGen_variable(current_coord, max_depth, neighbors);
                for(vector<pair<int, int>>::iterator idx = neighbors.begin(); idx != neighbors.end(); ++idx) {
                    cout<< "Neighbor: (" << idx->first << ", " << idx->second << ")" <<endl;
                }
                for(int i = 0; i < neighbors.size(); i++) {
                    graph[neighbors[i].first][neighbors[i].second].found = true;
                    graph[neighbors[i].first][neighbors[i].second].heuristic = heuristic_1(neighbors[i]);
                    graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];

                    open.push(&graph[neighbors[i].first][neighbors[i].second]);
                }
            }
        }

        cout<< countClosed() << endl;
    }

    void VND(){
       /* vector<pair<int,int>> neigh;
        pair<int,int> one = make_pair(3,3);
        MoveGen1(graph[3][3],2,neigh,graph);
        for(int i=0; i<neigh.size(); i++) {
            cout << neigh[i].first << "," << neigh[i].second << "   dis = " << distance(one,neigh[i]) << "\n";
        }*/

        // Set of nodes
        priority_queue <Node*, vector<Node*>, LessThanByHeu> open;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        while(!open.empty()) {
            cout << "loop2\n";
            if(goalTest(open.top()->coord, dest_coord)) {
                cout<< "Goal Achieved"<<endl;
                break;
            } else {

                current_coord = open.top()->coord;
                open.pop();
                int max_depth = 1;
                while(true) {
                    cout << "loop1\n";
                    // function(max_depth, current_coord, open);
                    // if(heuristic(current_coord) > heuristic(open.top()->coord)) {   break;  }
                    max_depth++;
                }
                
            }
        }

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

        cout<< path_len<<endl;
    }
};

int main(int argc, char* argv[]) {
    
    // Take input
    Maze M((string) argv[1]);

    // vector<pair<int, int>> next_states;
    // M.moveGen_variable(make_pair(3, 5), 1, next_states);
    // for(vector<pair<int, int>>::iterator idx = next_states.begin(); idx != next_states.end(); ++idx) {
    //     cout<< "Neighbor: (" << idx->first << ", " << idx->second << ")" <<endl;
    // }

    // Do the required search
    M.performSearch();    

    // Reconstruct path
    M.backTrack();

    // Print the solution
    M.printMaze();

    return 0;
}