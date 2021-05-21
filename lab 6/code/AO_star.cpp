#include <iostream>
#include <list>
#include <vector>
#include <iterator>
#include <string>
#include <fstream>
#include <algorithm>

#define vi vector<int>
#define vn vector<Node*>
#define ln list<Node*>
#define loop(i, n, init) for(int i = init; i < n; ++i)

using namespace std;

// Basic component of the AND/OR graph
class Node
{
    /*
        * dims : vector of ints store the dimensions of matrices at current node
        * solved : boolean to store if the node is solved
        * h_val : int to store the cost of this node
        * parents : vector of Node pointers to store the parent nodes of the current node
        * children : vector of Node pointers to store the children of current node
        * marked_child : Node pointer to store the marked child of current node
        * sibling : Node pointer to store the other node required to AND
    */

  public:
    // Dimensions of Matrices to be multiplied
    vi dims;

    // Node Properties
    bool solved;
    int h_val;

    // Node successors and predecessors
    vn parents;
    vn children;
    Node* marked_child;
    Node* sibling;

    Node() {
        solved = false;
        h_val = INT32_MAX;

        marked_child = NULL;
        sibling = NULL;
    }
};

// AND/OR graph made up of Nodes
class AOGraph
{
    /*
        * root : Point to the root of the graph
        * non_terminal_leaves : maintain a list of marked non-terminal leaf nodes
        * mode : 'u' means underestimate and 'o' means overestimate
    */

  public:
    Node* root;
    ln non_terminal_leaves;

    char mode;

    AOGraph(string file_name, char inp_mode) {
        
        // Read file and appoint root
        ifstream fin(file_name);
        root = new Node();
        int dim;

        while(fin >> dim)
            root->dims.push_back(dim);

        mode = inp_mode;

    }

    int numOps(vi &dims) {
        if(dims.size() == 2)
            return 0;
        else if(dims.size() == 3)
            return dims[0] * dims[1] * dims[2];
        else
            cout<< "Error: Dimensions unrealistic" << dims.size() << "\n";
    }

    int approxOps(vi &dims, char mode) {
        int prod;

        if( mode == 'u' ) {
            prod = INT32_MAX;

            loop(i, dims.size() - 2, 0) {
                if( (dims[i] * dims[i+1] * dims[i+2]) < prod )
                    prod = dims[i] * dims[i+1] * dims[i+2];
            }

        } else if(mode == 'o' ) {
            prod = INT32_MIN;

            loop(i, dims.size() - 2, 0) {
                if( (dims[i] * dims[i+1] * dims[i+2]) > prod )
                    prod = dims[i] * dims[i+1] * dims[i+2];
            }

        }

        return prod;
    }

    void processChild(Node* parent, Node* child) {

        // Add parent to child's parents
        child->parents.push_back(parent);

        // Update child properties
        if(child->dims.size() <= 3) {

            child->solved = true;
            child->h_val = numOps(child->dims);

        } else {

            child->h_val = approxOps(child->dims, mode);

        }
    }

    void makeChildren(Node* node) {
        
        // Generate children only for nodes with dims > 3, as they aren't trivial
        if(node->dims.size() > 3) {
            
            // Outer Loop : types of partitions
            loop(i, node->dims.size() - 1, 1) {  // i < N = n - 1
                Node* child1 = new Node();
                Node* child2 = new Node();

                // Inner Loop : Add respective dims to children
                loop(j, i + 1, 0) {
                    child1->dims.push_back(node->dims[j]);
                }
                loop(j, node->dims.size(), i) {
                    child2->dims.push_back(node->dims[j]);
                }

                // make children siblings
                child1->sibling = child2;
                child2->sibling = child1;

                // process child
                processChild(node, child1);
                processChild(node, child2);

                // add child to children
                node->children.push_back(child1);
                node->children.push_back(child2);
            }
        }
    }

    void costRevise(Node* node) {

        // If root has less than 3 dims (trivial), directly return the cost
        if(node->marked_child == NULL && node->dims.size() <= 3) {
            root->solved = true;
            root->h_val = root->dims[0] * root->dims[1] * root->dims[2];
            return;
        }

        // Update cost by adding children's cost and also the cost to multiply children nodes
        Node *old_child = node->marked_child;
        int temp_min_h = INT32_MAX;

        loop(i, node->children.size(), 0) {
            int edge_cost = 0;
            Node *child1, *child2;
            child1 = node->children[i];
            child2 = node->children[i+1];
            edge_cost = child1->dims[0] * child1->dims[child1->dims.size() - 1] * child2->dims[child2->dims.size() - 1];

            if(child1->h_val + child2->h_val + edge_cost < temp_min_h) {
                temp_min_h = child1->h_val + child2->h_val + edge_cost;
                node->marked_child = child1;
                // cout<< "****Best Upto Now**** = " << temp_min_h << " for child " << i << endl;
            }

            // Since children come in pairs, skip a child as its pair is done
            ++i;
        }
        node->h_val = temp_min_h;

        // Remove prev children
        if(old_child != NULL && old_child->sibling == NULL) {
            cout<< "--------------_ERROR_--------------- Sibling not found\n";
        } else if(old_child != NULL && old_child->sibling != NULL) {
            non_terminal_leaves.remove(old_child);
            non_terminal_leaves.remove(old_child->sibling);
        }

        // Add the costliest best child to non-terminal leaves if it isn't solved
        if(node->marked_child->solved == false) {
            if(node->marked_child->h_val >= node->marked_child->sibling->h_val) {
                non_terminal_leaves.push_back(node->marked_child);
            } else if(node->marked_child->sibling->solved == true) {
                non_terminal_leaves.push_back(node->marked_child);
            }
        }
        if(node->marked_child->sibling->solved == false) {
            if(node->marked_child->h_val <= node->marked_child->sibling->h_val) {
                non_terminal_leaves.push_back(node->marked_child->sibling);
            } else if(node->marked_child->solved == true) {
                non_terminal_leaves.push_back(node->marked_child->sibling);
            }
        }

        // Check if best children are solved
        if(node->marked_child->solved == true && node->marked_child->sibling->solved == true)
            node->solved = true;

        // Revise cost for parents of "node" for whom "node" is marked
        // Note: parents added in processChild()
        loop(i, node->parents.size(), 0) {
            if(node == node->parents[i]->marked_child || node == node->parents[i]->marked_child->sibling) {
                costRevise(node->parents[i]);
            }
        }

    }

    void AOStar() {

        // Add root to the marked list
        non_terminal_leaves.push_back(root);

        while(!root->solved) {

            // Get a non-terminal leaf node
            Node* current = non_terminal_leaves.front();
            non_terminal_leaves.pop_front();
            // cout<< "Hello: " << debug << endl;

            // Expand the current node
            makeChildren(current);

            // Current has changed, backpropagate the costs
            costRevise(current);
        }

        cout<< "Best Cost: " << root->h_val << endl;
    }
};


int main(int argc, char* argv[]) {
    if(argc < 3) {
        cout<< "Input: ./a.out <path-to-input> <mode-of-heuristic-('u' or 'o')>\n";
        return 0;
    }

    AOGraph A(argv[1], argv[2][0]);
    A.AOStar();

    return 0;
}