#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <queue>
#include <stack>

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
    Node* parent;

    Node(int row, int col, char data) {
        this->coord.first = row;
        this->coord.second = col;
        this->data = data;
        this->visited = false;
        this->found = false;
        this->depth = INT32_MAX-1;  // As INT32_MAX overflows when incremented
        this->parent = NULL;
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
        current_coord = pair<int, int>(0, 0);
        graph[0][0].data = '0';
    }

    void performSearch() {
        // Perform the required search
        switch (mode) {
        case '0':
            BFS();
            break;
        
        case '1':
            DFS();
            break;
        
        case '2':
            DFID();
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
        current_coord = pair<int, int>(0, 0);

        for (vector<vector<Node>>::iterator row = graph.begin(); row != graph.end(); ++row) {
            for(vector<Node>::iterator col = row->begin(); col != row->end(); ++col) {
                col->depth = INT32_MAX-1;
                col->found = false;
                col->parent = NULL;
                col->visited = false;
            }
        }

        graph[0][0].depth = 0;
        graph[0][0].found = true;
    }

    void BFS() {
        // Queue
        queue <pair<int, int>> open;

        open.push(current_coord);
        graph[current_coord.first][current_coord.second].visited = true;

        while(!open.empty()) {
            if(goalTest(current_coord, dest_coord) == true) {
                break;
            } else {

                // Update the current position
                current_coord = open.front();
                open.pop();
                graph[current_coord.first][current_coord.second].visited = true;

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
                        open.push(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
                    }
                }
            }
        }

        // Print the number of visited states
        cout<< countClosed() << endl;
    }


    void DFS() {
        // Stack
        stack <pair<int, int>> open;
        open.push(current_coord);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].depth = 0;

        int states = 0;

        while(!open.empty()) {
            if(goalTest(current_coord, dest_coord) == true) {
                break;
            } else {

                // Update the current position
                current_coord = open.top();
                open.pop();
                graph[current_coord.first][current_coord.second].visited = true;

                states++;

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
                        open.push(neighbors[i]);
                        graph[neighbors[i].first][neighbors[i].second].found = true;
                        graph[neighbors[i].first][neighbors[i].second].depth = graph[current_coord.first][current_coord.second].depth + 1;
                        graph[neighbors[i].first][neighbors[i].second].parent = &graph[current_coord.first][current_coord.second];
                    }
                }
            }
        }

        // Print the number of visited states
        cout<< countClosed() <<endl;
    }

    void DB_DFS(Node &node, int max_depth, int &states) {
        // Update the current position
        current_coord = pair<int, int> (node.coord.first, node.coord.second);

        // Return if already achieved the goal
        if(goalTest(current_coord, dest_coord))
            return;

        // Initialise neighbors to pass on to moveGen()
        pair<int, int> neighbors[4];
        for(int i = 0; i < 4; i++) {
            neighbors[i].first = -1;
            neighbors[i].second = -1;
        }

        // Update the visited nodes
        node.visited = true;
        states++;

        // Get possible moves from moveGen() and rest remain (-1, -1)
        moveGen(current_coord, neighbors, graph);
        for(int i = 0; i < 4; i++) {
            if(neighbors[i].first != -1 && neighbors[i].second != -1 && node.depth + 1 <= max_depth) {
                graph[neighbors[i].first][neighbors[i].second].found = true;
                graph[neighbors[i].first][neighbors[i].second].depth = node.depth + 1;
                graph[neighbors[i].first][neighbors[i].second].parent = &node;

                DB_DFS(graph[neighbors[i].first][neighbors[i].second], max_depth, states);

                // Return if already achieved the goal
                if(goalTest(current_coord, dest_coord))
                    return;
            }
        }
    }

    void DFID() {
        int max_depth = 0, states = 0, temp_states = 0;

        while (!goalTest(current_coord, dest_coord)) {
            reset();
            temp_states = 0;

            DB_DFS(graph[current_coord.first][current_coord.second], max_depth, temp_states);
            states += temp_states;
            max_depth++;
        }
        cout<< states << endl;
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

    // Do the required search
    M.performSearch();    

    // Reconstruct path
    M.backTrack();

    // Print the solution
    M.printMaze();

    return 0;
}
