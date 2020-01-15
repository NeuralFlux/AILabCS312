#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <set>

// As coordinates are used extensively
typedef pair<int, int> coord;

using namespace std;

class Node {
  public:
  // Found means it's added to the stack
  // Visited means it's been traversed
    coord coord;
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
        this->depth = INT32_MAX;  // As INT32_MAX overflows when incremented
        this->parent = NULL;
    }
};

void moveGen(coord current_coord, coord neighbors[], vector<vector<Node>>& graph) {
    // Initialise potential neighbors (DOWN, UP, RIGHT, LEFT)
    coord potential_nbrs[4];
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
bool goalTest(coord current_coord, coord dest_coord) {
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
    coord current_coord;
    coord dest_coord;
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
        current_coord = coord(0, 0);
        graph[0][0].data = '0';
    }

    float heuristic(coord x, coord y) {
        return abs(x.first - y.first) + abs(x.second - y.second);
    }

    bool operator >(const coord &node) {
        return (heuristic(*this, dest_coord) > heuristic(node, dest_coord));
    }

    void performSearch() {
        // Perform the required search
        switch (mode) {
        case '0':
            BFS();
            break;
        
        // case '1':
        //     DFS();
        //     break;
        
        // case '2':
        //     DFID();
        //     break;

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
        current_coord = coord(0, 0);

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
        // Set of nodes
        set <coord> open;

        open.insert(current_coord);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        while(*open.begin())

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
