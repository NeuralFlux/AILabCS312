#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <stack>

using namespace std;

class node {
  public:
  // Explored means it's added to the stack
  // Visited means it's been traversed
    pair<int, int> coord;
    bool visited;
    bool explored;
    node* parent;

    node() {
        this->coord.first = -1;
        this->coord.second = -1;
        this->visited = false;
        this->explored = false;
        this->parent = NULL;
    }
    node(int row, int col) {
        this->coord.first = row;
        this->coord.second = col;
        this->visited = false;
        this->explored = false;
        this->parent = NULL;
    }
};



void moveGen(pair<int, int> current_coord, pair<int, int> neighbors[], vector<string>& maze, vector<vector<node>>& maze_graph) {
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
            // Check if node is visited/explored already
            if(maze_graph[potential_nbrs[i].first][potential_nbrs[i].second].visited == true ||
               maze_graph[potential_nbrs[i].first][potential_nbrs[i].second].explored == true) {
                continue;
            }

            // Possible to move only if free or goal
            char curr_char = maze[potential_nbrs[i].first][potential_nbrs[i].second];

            if(curr_char == ' ' || curr_char == '*') {
                neighbors[i].first = potential_nbrs[i].first;
                neighbors[i].second = potential_nbrs[i].second;
            }
        }
    }
    return;
}

bool goalTest(pair<int, int> current_coord, pair<int, int> dest_coord) {
    return current_coord == dest_coord;
}

int main() {
    // Read and store input
    string line;
    int line_len;

    vector<string> maze;
    pair<int, int> dest_coord(0, 0);

    ifstream infile("input.txt");

    while (getline(infile, line)) {
        // Get max columns in the maze
        // cout<< line << endl;
        line_len = line.length();

        maze.push_back(line);

        // Extract the destination co-ordinates
        size_t found = line.find("*");
        if(found != string::npos) {
            dest_coord.first = maze.size() - 1;
            dest_coord.second = (int) found;
        }
    }

    // printf("Goal found at %d, %d\n", dest_coord.first, dest_coord.second);
    // cout<< line_len <<endl;

    // Graph representation of maze to store visited flag and parent
    vector<vector<node>> maze_graph;
    for(int i=0; i < maze.size(); i++) {
        vector<node> nodes;
        for(int j=0; j < line_len; j++) {
            nodes.push_back(node (i, j));
        }
        maze_graph.push_back(nodes);
    }
    // cout<< maze_graph.size() << endl << maze_graph[0].size() <<endl;

    // Stack
    stack <pair<int, int>> open;
    pair<int, int> current_coord(0, 0);

    open.push(current_coord);
    maze_graph[current_coord.first][current_coord.second].visited = true;

    int states = 0;

    while(!open.empty()) {
        if(goalTest(current_coord, dest_coord) == true) {
            // printf("Yay! Goal found at %d, %d\n", current_coord.first, current_coord.second);
            cout<< states <<endl;  // Print states
            break;
        } else {

            // Update the current position
            current_coord = open.top();
            open.pop();
            maze_graph[current_coord.first][current_coord.second].visited = true;
            states++;

            // Initialise neighbors to pass on to moveGen()
            pair<int, int> neighbors[4];
            for(int i = 0; i < 4; i++) {
                neighbors[i].first = -1;
                neighbors[i].second = -1;
            }

            // Get possible moves from moveGen() and rest remain (-1, -1)
            moveGen(current_coord, neighbors, maze, maze_graph);
            for(int i = 0; i < 4; i++) {
                if(neighbors[i].first != -1 && neighbors[i].second != -1) {
                    open.push(neighbors[i]);
                    maze_graph[neighbors[i].first][neighbors[i].second].explored = true;
                    maze_graph[neighbors[i].first][neighbors[i].second].parent = &maze_graph[current_coord.first][current_coord.second];
                    // printf("Neighbors:(%d, %d)\n", neighbors[i].first, neighbors[i].second);
                }
            }
        }
    }

    // Backtrack and print the solved maze
    
    int path_len = 1;
    node *tracer = &maze_graph[dest_coord.first][dest_coord.second];
    while(tracer->parent != NULL) {
        maze[tracer->coord.first][tracer->coord.second] = '0';
        tracer = tracer->parent;
        path_len++;
    }
    maze[0][0] = '0';

    int test_states = 0;
    for(int i=0; i < maze.size(); i++) {
        for(int j=0; j < line_len; j++) {
            if(maze_graph[i][j].visited == true) {
                test_states++;
            }
        }
    }

    cout<< test_states << endl << path_len<<endl;

    for (std::vector<string>::const_iterator i = maze.begin(); i != maze.end(); ++i)
        std::cout << *i << endl;

    return 0;
}
