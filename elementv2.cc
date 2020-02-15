#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

/* Operation spec

00 = Constant Sum (varg0, carg0)
01 = Variable Sum (varg0, varg1)
02 = Constant Difference (varg0, carg0)
03 = Variable Difference (varg0, varg1)
04 = Constant Product (varg0, carg0)
05 = Variable Product (varg0, varg1)
06 = Constant Divide (varg0, carg0)
07 = Variable Divide (varg0, varg1)
08 = Constant Modulus (varg0, carg0)
09 = Variable Modulus (varg0, varg1)
10 = Constant Maximum (varg0, carg0)
11 = Variable Maximum (varg0, varg1)
12 = Constant Minimum (varg0, carg0)
13 = Variable Minimum (varg0, varg1)
14 = Constant Exponent (varg0, carg0)
15 = Varaible Exponent (varg0, varg1)
16 = Absolute (varg0)
17 = Mean (varg0, varg1)
18 = Input Node (carg0)

*/

class node {
    public:
    char operation;

    // Variable arguments
    node* varg0;
    node* varg1;

    // Constant arguments
    int carg0;

    int lastComputedValue;
    bool hasBeenRecomputed;
};

vector<node*> queue;

// Determine whether the queue already has a node registered with it
bool nodeAlreadyInQueue (node* n) {
    for (node* no : queue) if (no == n) return true;
    return false;
}

// Reset the computation state of the net ready for computation
void clearAllNodeComputations () {
    for (node* no : queue) no->hasBeenRecomputed = false;
}

// Register the requirements for the computation of a node recursively with the queue, only as long as it is not already there
void registerNodeRequirements (node* n) {
    // Find out what we need
    node** req = getNodeFirstLevelRequirements (n);
    // Recursively register requirements with the queue
    if (req[0] != NULL) registerNodeRequirements (req[0]);
    if (req[1] != NULL) registerNodeRequirements (req[1]);

    // Finally, add ourselves to the queue
    if (!nodeAlreadyInQueue (n))
        queue.push_back (n);
}

// Find the immediate requirements for a node to be computed
node** getNodeFirstLevelRequirements (node* n) {
    node** out = new node*[2];
    if ((n->operation % 2) == 1) out[1] = n->varg1;
    else out[1] = NULL;
    if (n->operation != 18)
        out[0] = n->varg0;
    return out;
}

// Compute a single node, returning status false for failure, true for success
bool computeNodeOnly (node* n) {
    // First work out what nodes we need
    node** requirements = getNodeFirstLevelRequirements (n);
    // Check if these have been computed yet
    if (requirements[0] != NULL && !requirements[0]->hasBeenRecomputed) return false;
    if (requirements[1] != NULL && !requirements[1]->hasBeenRecomputed) return false;

    // Grab the values quickly
    int v[2] = {requirements[0]->lastComputedValue, requirements[1]->lastComputedValue};
    int c = n->carg0;

    // Complete the operation
    int finalValue;
    switch (n->operation) {
    case 0:
        finalValue = v[0] + c;
        break;
    case 1:
        finalValue = v[0] + v[1];
        break;
    case 2:
        finalValue = v[0] - c;
        break;
    case 3:
        finalValue = v[0] - v[1];
        break;
    case 4:
        finalValue = v[0] * c;
        break;
    case 5:
        finalValue = v[0] * v[1];
        break;
    case 6:
        finalValue = v[0] / c;
        break;
    case 7:
        finalValue = v[0] / v[1];
        break;
    case 8:
        finalValue = v[0] % c;
        break;
    case 9:
        finalValue = v[0] % v[1];
        break;
    case 10:
        finalValue = (v[0] > c) ? v[0] : c;
        break;
    case 11:
        finalValue = (v[0] > v[1]) ? v[0] : v[1];
        break;
    case 12:
        finalValue = (v[0] < c) ? v[0] : c;
        break;
    case 13:
        finalValue = (v[0] < v[1]) ? v[0] : v[1];
        break;
    case 14:
        finalValue = pow (v[0], c);
        break;
    case 15:
        finalValue = pow (v[0], v[1]);
        break;
    case 16:
        finalValue = abs (v[0]);
        break;
    case 17:
        finalValue = (v[0] + v[1])/2;
        break;
    default:
        // Unsupported operation
        break;
    }

    // Set the node's computed status
    n->hasBeenRecomputed = true;
    n->lastComputedValue = finalValue;
    return true;
}

vector<node*> outputs;
vector<int> outputValues;

// Compute the entire net and dump the outputs it produces
void computeNetOutputs () {
    // Generate requirements for the net to compute, in order of requirement
    // TODO: Only do this if the net has changed since last update
    queue.clear();
    for (node* output : outputs) {
        registerNodeRequirements (output);
    }
    
    // Compute all nodes in order
    // TODO: Make this faster (concurrency, workers, etc)
    for (node* tbc : queue) {
        computeNodeOnly (tbc);
    }

    // Dump output values
    for (node* output : outputs) {
        outputValues.push_back (output->lastComputedValue);
    }

    // Reset ready for next time
    clearAllNodeComputations();
}


// TOOD: Main
int main () {
    cout << sizeof(node) << endl;
    return 0;
}