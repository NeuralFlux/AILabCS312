#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>
#include <set>
#include <queue>
#define tabu_tenure 5

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

         case '3':
             TS();
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

                        //if(heuristic(current_coord) > heuristic(neighbors[i])) {
                        open.push(&graph[neighbors[i].first][neighbors[i].second]);
                        //}
                    }
                }
            }
        }

        cout<< countClosed() << endl;
    }


	void BS() {
        // Set of nodes
        priority_queue <Node*> open;
        // Beam size
        int beam = 2;

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
	    
	        // Filtering the open queue with beam number of elements
            priority_queue <Node*> temporary;
            //cout << open.size() << "  open.size() before\n";
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

            }//cout << open.size() << "  open.size() after\n";
        }
        cout<< countClosed() << endl;
    }	



    class Queue {
    private:
        Node items[tabu_tenure], front, rear;
    
    public:
        Queue(){
            front = -1;
            rear = -1;
            
        }
        
       /* bool isFull(){
            if(front == 0 && rear == tabu_tenure - 1){
                return true;
            }
            if(front == rear + 1) {
                return true;
            }
            return false;
        }
    
        bool isEmpty(){
            if(front == -1) return true;
            else return false;
        }
    
        void enQueue(int element){
            if(isFull()){
                cout << "Queue is full";
            } else {
                if(front == -1) front = 0;
                rear = (rear + 1) % tabu_tenure;
                items[rear] = element;
                //cout << endl << "Inserted " << element << endl;
            }
        }
    
        int deQueue(){
            int element;
            if(isEmpty()){
                cout << "Queue is empty" << endl;
                return(-1);
            } else {
                element = items[front];
                if(front == rear){
                    front = -1;
                    rear = -1;
                } 
                else {
                    front=(front+1) % tabu_tenure;
                }
                return(element);
            }
        }
    
        void display()
        {
        
            int i;
            if(isEmpty()) {
                cout << endl << "Empty Queue" << endl;
            }
            else
            {
                cout << "Front -> " << front;
                cout << endl << "Items -> ";
                for(i=front; i!=rear;i=(i+1)%tabu_tenure)
                    cout << items[i];
                    cout << items[i];
                    cout << endl << "Rear -> " << rear;
            }
        }*/
    
    };


    void TS() {

        // Set of nodes
        priority_queue <Node*> open;

        Queue closed;

        pair<int, int> best;

        open.push(&graph[current_coord.first][current_coord.second]);
        graph[current_coord.first][current_coord.second].visited = true;
        graph[current_coord.first][current_coord.second].found = true;

        while(!open.empty())    {
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
