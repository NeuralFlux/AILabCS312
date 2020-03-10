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
        // or_node = false;
        solved = false;
        h_val = INT32_MAX;

        marked_child = NULL;
        sibling = NULL;
    }
};

// AND/OR graph made up of Nodes
class AOGraph
{

  public:
    Node* root;
    ln non_terminal_leaves;

    // AOGraph() {
    //     root = new Node();
    //     root->dims.push_back(10);
    //     root->dims.push_back(30);
    //     root->dims.push_back(5);
    //     root->dims.push_back(60);
    // }

    AOGraph(string file_name) {
        
        // Read file and appoint root
        ifstream fin(file_name);
        root = new Node();
        int dim;

        while(fin >> dim)
            root->dims.push_back(dim);

    }

    int numOps(vi &dims) {
        if(dims.size() == 2)
            return 0;
        else if(dims.size() == 3)
            return dims[0] * dims[1] * dims[2];
        else
            cout<< "Error: Dimensions unrealistic" << dims.size() << "\n";
    }

    int approxOps(vi &dims) {
        int prod_min = INT32_MAX;

        loop(i, dims.size() - 2, 0) {
            if( (dims[i] * dims[i+1] * dims[i+2]) < prod_min )
                prod_min = dims[i] * dims[i+1] * dims[i+2];
        }

        return prod_min;
    }

    void processChild(Node* parent, Node* child) {
        // Add parent to child's parents
        child->parents.push_back(parent);

        // Update child properties
        if(child->dims.size() <= 3) {
            child->solved = true;
            child->h_val = numOps(child->dims);
        } else {
            // child->or_node = true;
            child->h_val = approxOps(child->dims);
        }
        cout<< "Child hval = " << child->h_val <<endl;
    }

    void makeChildren(Node* node) {
        // generate all children
        if(node->dims.size() > 3) {

            int temp_min_h = node->h_val;

            loop(i, node->dims.size() - 1, 1) {  // i < N = n - 1
                Node* child1 = new Node();
                Node* child2 = new Node();

                cout<< "Partition: "<< i<<endl;

                // Add respective dims to children
                loop(j, i + 1, 0) {
                    child1->dims.push_back(node->dims[j]);
                    cout<< "Child 1 dims: " << node->dims[j] <<endl;
                }
                loop(j, node->dims.size(), i) {
                    child2->dims.push_back(node->dims[j]);
                    cout<< "Child 2 dims: " << node->dims[j] <<endl;
                }

                // make children siblings
                child1->sibling = child2;
                child2->sibling = child1;

                // process child
                cout<< "Child 1 -----------\n";
                processChild(node, child1);
                cout<< "Child 2 -----------\n";
                processChild(node, child2);

                // add child to children
                node->children.push_back(child1);
                node->children.push_back(child2);

                // get child with min h_val
                // if((child1->h_val + child2->h_val) < temp_min_h) {
                //     temp_min_h = (child1->h_val + child2->h_val);
                //     node->marked_child = child1;
                // }
            }

            // node->h_val = temp_min_h;
            // cout<< "\n\nBest child dims:\n";
            // loop(i, node->marked_child->dims.size(), 0) {
            //     cout<< node->marked_child->dims[i] << " ";
            // }
            // cout<< endl;
        }
    }

    void costRevise(Node* node) {
        // If root has less than 3 dims => 2 matrices, directly return the cost
        if(node == root) {
            if(node->marked_child == NULL && root->dims.size() <= 3) {
                root->solved = true;
                root->h_val = root->dims[0] * root->dims[1] * root->dims[2];
                return;
            }
        }

        // Update cost
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
                cout<< "****Best Upto Now**** = " << temp_min_h << " for child " << i << endl;
            }

            ++i;
        }
        node->h_val = temp_min_h;

        // Add the best child to non-terminal leaves and remove prev children
        cout<< "hello\n";
        loop(i, node->dims.size(), 0)
            cout<< node->dims[i] << " \n";
        cout<< "dims: " << node->dims.size() << endl;
        if(node->marked_child->solved == false)
            non_terminal_leaves.push_back(node->marked_child);
        if(node->marked_child->sibling->solved == false)
            non_terminal_leaves.push_back(node->marked_child->sibling);

        if(old_child != NULL && old_child->sibling == NULL) {
            cout<< "--------------_ERROR_--------------- Sibling not found\n";
        } else if(old_child != NULL && old_child->sibling != NULL) {
            non_terminal_leaves.remove(old_child);
            non_terminal_leaves.remove(old_child->sibling);
        }

        // TODO May need to call cost-revise(node) again, if its cost changes
        
        // Check if best children are solved
        if(node->marked_child->solved == true && node->marked_child->sibling->solved == true)
            node->solved = true;

        // Revise cost for parents of "node" for whom "node" is marked
        // Note: parents updated in process child
        loop(i, node->parents.size(), 0) {
            if(node == node->parents[i]->marked_child || node == node->parents[i]->marked_child->sibling) {
                costRevise(node->parents[i]);
            }
        }

    }

    void AOStar() {
        non_terminal_leaves.push_back(root);
        
        while(!root->solved) {
            // Get a non-terminal leaf node
            Node* current = non_terminal_leaves.front();
            non_terminal_leaves.pop_front();

            // Expand the current node
            makeChildren(current);

            // Current has changed, backpropagate the costs
            cout<< "------------------Gone----------------\n";
            costRevise(current);
        }

        cout<< "Best Cost: " << root->h_val << endl;
    }
};


int main(int argc, char** argv) {
    AOGraph A(argv[1]);
    // A.makeChildren(A.root);
    A.AOStar();

    return 0;
}