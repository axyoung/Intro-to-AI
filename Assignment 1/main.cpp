/*
 * Alex Young and Steven Bui
 * CS 331 Programming Assignment 1
 * 4/18/2021
 * OSU (engr server)
 * Compile with:   g++ --std=c++11 -o run main.cpp
 * Execute with:   ./run < initial state file > < goal state file > < mode > < output file > 
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <stack>

using namespace std;

// State holds the current state of the two banks as well as the depth/path cost in the tree
struct State {
    int lb_chicken;
    int rb_chicken;
    int lb_wolf;
    int rb_wolf;
    int boat;
    int depth;
    int path_cost;
    int estimate;

    // These operators are changed to work with the State struct
    bool const operator<(const State &op) const {
        return tie(lb_chicken, rb_chicken, lb_wolf, rb_wolf, boat) <
        tie(op.lb_chicken, op.rb_chicken, op.lb_wolf, op.rb_wolf, op.boat);
    }
    // The greater than operator is used to find the path cost for the priority queue which uses weak ordering
    bool const operator>(const State &op) const {
        return estimate < op.estimate;
    }
};

// Node holds a current State and its parent
struct Node {
    struct State node;
    struct State parent;
    
    bool const operator<(const Node &op) const {
        return node < op.node;
    }
    bool const operator>(const Node &op) const {
        return node > op.node;
    }
};

// This function processes a file into the initial and goal states
struct Node processFile(string file_name) {
    vector<int> lb_state;
    vector<int> rb_state;

    // read in the file using fstream into a vector
    fstream input_file;
    input_file.open(file_name);
    if (input_file.is_open()) {
        string buffer_1;
        getline(input_file, buffer_1);
        stringstream ss_1(buffer_1);
        while (ss_1.good()) {
            string value;
            getline(ss_1, value, ',');
            int num = stoi(value);
            lb_state.push_back(num);
        }

        string buffer_2;
        getline(input_file, buffer_2);
        stringstream ss_2(buffer_2);
        while (ss_2.good()) {
            string value;
            getline(ss_2, value, ',');
            int num = stoi(value);
            rb_state.push_back(num);
        }

        input_file.close();
    }

    // update a temporary node which will become the init or goal node
    Node temp;

    temp.node.lb_chicken = lb_state[0];
    temp.node.lb_wolf = lb_state[1];
    temp.node.rb_chicken = rb_state[0];
    temp.node.rb_wolf = rb_state[1];
    if (lb_state[2]) {
        temp.node.boat = 1;
    } else if (rb_state[2]) {
        temp.node.boat = 0;
    }

    return temp;
}

// Function to clear a file to print to
void clearFile(string file_name) {
    fstream output_file;
    output_file.open(file_name, ios::out | ios::trunc);
    output_file.close();
}

// Function prints out the solution to the terminal and the file stream.
void printState(ofstream& out, int step, int lb_chicken, int lb_wolf, int rb_chicken, int rb_wolf, int boat) {
    // Init
    if (step == -2) {
        if (boat == 1) {
            cout << "INIT: LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
            out << "INIT: LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
        }
        else {
            cout << "INIT: LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
            out << "INIT: LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
        }
    }
    // Goal
    else if (step == -1) {
        if (boat == 1) {
            cout << "GOAL: LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
            out << "GOAL: LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
        }
        else {
            cout << "GOAL: LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
            out << "GOAL: LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
        }
    }
    // Solution path steps
    else {
        if (boat == 1) {
            cout << step << ". LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
            out << step << ". LB[" << lb_chicken << ", " << lb_wolf << ", 1] RB[" << rb_chicken << ", " << rb_wolf << ", 0]" << endl;
        }
        else {
            cout << step << ". LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
            out << step << ". LB[" << lb_chicken << ", " << lb_wolf << ", 0] RB[" << rb_chicken << ", " << rb_wolf << ", 1]" << endl;
        }
    }
}

// Function to print if no solution is found
void printNoSolution(ofstream& out) {
    cout << "No solution found." << endl;
    out << "No solution found." << endl;
}

// Function checks if two states are equal
int isEqual(State one, State two) {
    if (one.lb_chicken == two.lb_chicken &&
        one.lb_wolf == two.lb_wolf &&
        one.rb_chicken == two.rb_chicken &&
        one.rb_wolf == two.rb_wolf &&
        one.boat == two.boat) {
        return 1;
    }
    else {
        return 0;
    }
}

// Function to calculate the cost from the node to the goal
int heuristic_func(int depth, int path_cost, int boat) {
    if (boat) {
        return depth + ((path_cost - 1) * 2) - 1;
    } else {
        return depth + (path_cost * 2);
    }
}

// Function generates a successor state
struct State updateChild(int i, State child) {
    if (child.boat == 0) {
        if (i == 0) {
            child.rb_chicken--;
            child.lb_chicken++;
            child.path_cost++;
        } else if (i == 1) {
            child.rb_chicken = child.rb_chicken - 2;
            child.lb_chicken = child.lb_chicken + 2;
            child.path_cost = child.path_cost + 2;
        } else if (i == 2) {
            child.rb_wolf--;
            child.lb_wolf++;
            child.path_cost++;
        } else if (i == 3) {
            child.rb_chicken--;
            child.lb_chicken++;
            child.rb_wolf--;
            child.lb_wolf++;
            child.path_cost = child.path_cost + 2;
        } else if (i == 4) {
            child.rb_wolf = child.rb_wolf - 2;
            child.lb_wolf = child.lb_wolf + 2;
            child.path_cost = child.path_cost + 2;
        }
    } else {
        if (i == 0) {
            child.rb_chicken++;
            child.lb_chicken--;
            child.path_cost--;
        } else if (i == 1) {
            child.rb_chicken = child.rb_chicken + 2;
            child.lb_chicken = child.lb_chicken - 2;
            child.path_cost = child.path_cost - 2;
        } else if (i == 2) {
            child.rb_wolf++;
            child.lb_wolf--;
            child.path_cost--;
        } else if (i == 3) {
            child.rb_chicken++;
            child.lb_chicken--;
            child.rb_wolf++;
            child.lb_wolf--;
            child.path_cost = child.path_cost - 2;
        } else if (i == 4) {
            child.rb_wolf = child.rb_wolf + 2;
            child.lb_wolf = child.lb_wolf - 2;
            child.path_cost = child.path_cost - 2;
        }
    }
    child.boat = !child.boat;

    return child;
}

// Function implements breadth first search on a graph
int bfs(Node init, Node goal, ofstream& out) {
    int counter = 0;
    Node current;

    // Use a queue to store the frontier states (FIFO)
    queue<Node> frontier;
    frontier.push(init);
    // Use maps to store the frontier and explored states for quick access
    map<Node, int> explored;
    map<Node, int> frontier_map = {{init, counter}};
    
    if (isEqual(init.node, goal.node)) {
        return counter;
    }

    while (!frontier.empty()) {
        current = frontier.front();
        frontier.pop();
        frontier_map.erase(current);
        explored.insert(pair<Node, int>(current, counter));
        
        if (isEqual(current.node, goal.node)) {
            stack<Node> path;
            while (!isEqual(init.node, current.node)) {
                path.push(current);
                Node temp;
                temp.node = current.parent;
                current = explored.find(temp)->first;
            }
            path.push(init);
            int temp_c = 0;
            while(!isEqual(goal.node, current.node)) {
                current = path.top();
                path.pop();
                printState(out, temp_c, current.node.lb_chicken, current.node.lb_wolf, current.node.rb_chicken, current.node.rb_wolf, current.node.boat);
                temp_c++;
            }
            return counter;
        }
        counter++;

        for (int i = 0; i < 5; i++) {
            Node child = current;
            child.node = updateChild(i, child.node);
            if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                if (explored.find(child) == explored.end()) {
                    if ((frontier_map.find(child) == frontier_map.end()) || frontier_map.empty()) {
                        child.parent = current.node;
                        frontier.push(child);
                        frontier_map.insert(pair<Node, int>(child, counter));
                    }
                }
            }
        }
    }
    printNoSolution(out);
}

// Function implements depth first search on a graph
int dfs(Node init, Node goal, ofstream& out) {
    int counter = 0;
    Node current;

    // dfs uses a stack to hold frontier states (LIFO)
    stack<Node> frontier;
    frontier.push(init);
    // use a map to store frontier and explored states for quick access
    map<Node, int> explored;
    map<Node, int> frontier_map = {{init, counter}};
    
    if (isEqual(init.node, goal.node)) {
        return counter;
    }

    while (!frontier.empty()) {
        current = frontier.top();
        counter++;
        
        frontier.pop();
        frontier_map.erase(current);
        explored.insert(pair<Node, int>(current, counter));
        for (int i = 0; i < 5; i++) {
            Node child = current;
            child.node = updateChild(i, child.node);
            if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                if (explored.find(child) == explored.end()) {
                    if ((frontier_map.find(child) == frontier_map.end()) || frontier_map.empty()) {
                        if (isEqual(child.node, goal.node)) {
                            stack<Node> path;
                            path.push(child);
                            while (!isEqual(init.node, current.node)) {
                                path.push(current);
                                Node temp;
                                temp.node = current.parent;
                                current = explored.find(temp)->first;
                            }
                            path.push(init);
                            int temp_c = 0;
                            while(!isEqual(goal.node, current.node)) {
                                current = path.top();
                                path.pop();
                                printState(out, temp_c, current.node.lb_chicken, current.node.lb_wolf, current.node.rb_chicken, current.node.rb_wolf, current.node.boat);
                                temp_c++;
                            }
                            return counter;
                        }
                        child.parent = current.node;
                        frontier.push(child);
                        frontier_map.insert(pair<Node, int>(child, counter));
                    }
                }
            }
        }
    }
    printNoSolution(out);
}

// Function implements iterative deepening depth first search on a graph
int iddfs(Node init, Node goal, ofstream& out) {
    // Set a depth for each node to keep track of
    init.node.depth = 0;
    // Set a overal depth limit that will be iteratively increased
    int depth = -1;
    int counter = 0;
    // Set a limit to the number of depth possible to 10,000
    while (depth < 10000) {
        depth++;
        Node current;

        stack<Node> frontier;
        frontier.push(init);
        map<Node, int> explored;
        map<Node, int> frontier_map = {{init, counter}};
        
        if (isEqual(init.node, goal.node)) {
            return counter;
        }

        while (!frontier.empty()) {
            current = frontier.top();
            frontier.pop();
            frontier_map.erase(current);
            explored.insert(pair<Node, int>(current, counter));
            counter++;
            
            for (int i = 0; i < 5; i++) {
                Node child = current;
                child.node = updateChild(i, child.node);
                child.node.depth = current.node.depth + 1;
                if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                    ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                    (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                    if (explored.find(child) == explored.end()) {
                        if ((frontier_map.find(child) == frontier_map.end()) || frontier_map.empty()) {
                            if (isEqual(child.node, goal.node)) {
                                stack<Node> path;
                                path.push(child);
                                while (!isEqual(init.node, current.node)) {
                                    path.push(current);
                                    Node temp;
                                    temp.node = current.parent;
                                    current = explored.find(temp)->first;
                                }
                                path.push(init);
                                int temp_c = 0;
                                while(!isEqual(goal.node, current.node)) {
                                    current = path.top();
                                    path.pop();
                                    printState(out, temp_c, current.node.lb_chicken, current.node.lb_wolf, current.node.rb_chicken, current.node.rb_wolf, current.node.boat);
                                    temp_c++;
                                }
                                return counter;
                            }
                            if (child.node.depth <= depth) {
                                child.parent = current.node;
                                frontier.push(child);
                                frontier_map.insert(pair<Node, int>(child, child.node.depth));
                            }
                        }
                    }
                    else if (child.node.depth < explored.find(child)->first.node.depth || child.node.depth < frontier_map.find(child)->second) {
                        explored.erase(child);
                        frontier_map.erase(child);
                        if (child.node.depth <= depth) {
                            child.parent = current.node;
                            frontier.push(child);
                            frontier_map.insert(pair<Node, int>(child, child.node.depth));
                        }
                    }
                }
            }
        }
    }
    printNoSolution(out);
}

/*
 * Function implements A-star search. It uses heuristic of (total animals on right bank - 1) * 2 - 1 = optimal path cost
 * Because this heuristic to find the path cost is dependent on the number of animals on a bank,
 * the algorithm only need to pay attention to the number of animals on the right bank, which is the same as the path cost that we measure
 */ 
int astar(Node init, Node goal, ofstream& out) {
    // Set a depth and path_cost for each node to keep track of
    init.node.depth = 0;
    init.node.path_cost = 0;
    int counter = 0;
    Node current;
    current.node.estimate = heuristic_func(init.node.depth, init.node.path_cost, init.node.boat);

    // Use a priority queue with the least path cost having the highest priority for the astar search
    priority_queue<Node, vector<Node>, greater<Node>> frontier;
    frontier.push(init);
    map<Node, int> explored;
    map<Node, int> frontier_map = {{init, counter}};
    
    if (isEqual(init.node, goal.node)) {
        return counter;
    }

    while (!frontier.empty()) {
        current = frontier.top();
        counter++;
        frontier.pop();
        frontier_map.erase(current);
        explored.insert(pair<Node, int>(current, counter));

        for (int i = 0; i < 5; i++) {
            Node child = current;
            child.node = updateChild(i, child.node);
            child.node.depth = current.node.depth + 1;
            child.node.estimate = heuristic_func(child.node.depth, child.node.path_cost, child.node.boat);
            if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                if (explored.find(child) == explored.end()) {
                    if ((frontier_map.find(child) == frontier_map.end()) || frontier_map.empty()) {
                        if (isEqual(child.node, goal.node)) {
                            stack<Node> path;
                            path.push(child);
                            while (!isEqual(init.node, current.node)) {
                                path.push(current);
                                Node temp;
                                temp.node = current.parent;
                                current = explored.find(temp)->first;
                            }
                            path.push(init);
                            int temp_c = 0;
                            while(!isEqual(goal.node, current.node)) {
                                current = path.top();
                                path.pop();
                                printState(out, temp_c, current.node.lb_chicken, current.node.lb_wolf, current.node.rb_chicken, current.node.rb_wolf, current.node.boat);
                                temp_c++;
                            }
                            return counter;
                        }
                        child.parent = current.node;
                        frontier.push(child);
                        frontier_map.insert(pair<Node, int>(child, counter));
                    }
                }
            }
        }
    }
    printNoSolution(out);
}

/*
 * This program solves the wolf chicken boat problem using a variety of search algorithms
 * The solution path and number of nodes expanded to will be printed out into the chosen output file and terminal
 * The program can compiled with:
 * g++ --std=c++11 main.cpp
 * on engr server it can be excecuted with:
 * a.out < initial state file > < goal state file > < mode > < output file >
 */
int main(int argc, char* argv[]) {
    string start_file_name = argv[1];
    string goal_file_name = argv[2];
    string mode = argv[3];
    string output_file_name = argv[4];

    Node init = processFile(start_file_name);
    Node goal = processFile(goal_file_name);
    clearFile(output_file_name);

    int counter = 0;

    ofstream out(output_file_name);

    cout << "KEY: LEFT BANK [ chicken , wolf , boat state ] - RIGHT BANK [ chicken , wolf , boat state ]" << endl;
    out << "KEY: LEFT BANK [ chicken , wolf , boat state ] - RIGHT BANK [ chicken , wolf , boat state ]" << endl;
    printState(out, -2, init.node.lb_chicken, init.node.lb_wolf, init.node.rb_chicken, init.node.rb_wolf, init.node.boat);
    printState(out, -1, goal.node.lb_chicken, goal.node.lb_wolf, goal.node.rb_chicken, goal.node.rb_wolf, goal.node.boat);

    if ("bfs" == mode) {
        counter = bfs(init, goal, out);
    }

    if ("dfs" == mode) {
        counter = dfs(init, goal, out);
    }
    
    if ("iddfs" == mode) {
        counter = iddfs(init, goal, out);
    }
    
    if ("astar" == mode) {
        counter = astar(init, goal, out);
    }

    cout << "Expanded Nodes: " << counter << endl;
    out << "Expanded Nodes: " << counter << endl;

    out.close();

    return 0;
}