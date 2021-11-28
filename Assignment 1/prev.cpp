#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <map>
#include <queue>
#include <stack>

using namespace std;

struct State {
    int lb_chicken;
    int rb_chicken;
    int lb_wolf;
    int rb_wolf;
    int boat;
    int depth;
    int path_cost;

    bool const operator<(const State &op) const {
        return tie(lb_chicken, rb_chicken, lb_wolf, rb_wolf, boat) <
        tie(op.lb_chicken, op.rb_chicken, op.lb_wolf, op.rb_wolf, op.boat);
    }
    bool const operator>(const State &op) const {
        return tie(path_cost) > tie(op.path_cost);
    }
};

struct Node {
    struct State node;
    struct State parent;
    
    bool const operator<(const Node &op) const {
        return tie(node) < tie(op.node);
    }
    bool const operator>(const Node &op) const {
        return tie(node) > tie(op.node);
    }
};

struct Node processFile(string file_name) {
    vector<int> lb_state;
    vector<int> rb_state;

    fstream start_file;
    start_file.open(file_name);
    if (start_file.is_open()) {
        // store initial state
        string buffer_1;
        getline(start_file, buffer_1);
        stringstream ss_1(buffer_1);
        while (ss_1.good()) {
            string value;
            getline(ss_1, value, ',');
            int num = stoi(value);
            lb_state.push_back(num);
        }

        string buffer_2;
        getline(start_file, buffer_2);
        stringstream ss_2(buffer_2);
        while (ss_2.good()) {
            string value;
            getline(ss_2, value, ',');
            int num = stoi(value);
            rb_state.push_back(num);
        }

        start_file.close();
    }

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

int bfs(Node init, Node goal) {
    int counter = 0;
    Node current;

    queue<Node> frontier;
    frontier.push(init);
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
        //cout << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << endl;
        
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
                cout << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << " " << temp_c << endl;
                temp_c++;
            }
            return counter;
        }
        
        counter++;

        for (int i = 0; i < 5; i++) {
            Node child = current;
            child.node = updateChild(i, child.node);
            //cout << "child: " << child.node.rb_chicken << child.node.rb_wolf << child.node.lb_chicken << child.node.lb_wolf << child.node.boat << endl;
            if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                //cout << "hi " << explored.node.find(child)->first.node.rb_chicken << explored.node.find(child)->first.node.rb_wolf << explored.node.find(child)->first.node.lb_chicken << explored.node.find(child)->first.node.lb_wolf << explored.node.find(child)->first.node.boat << endl;
                //cout << "hi " << explored.node.end()->first.node.rb_chicken << explored.node.end()->first.node.rb_wolf << explored.node.end()->first.node.lb_chicken << explored.node.end()->first.node.lb_wolf << explored.node.end()->first.node.boat << endl;
                if (explored.find(child) == explored.end()) {
                    //cout << "yes" << endl;
                    if ((frontier_map.find(child) == frontier_map.end()) || frontier_map.empty()) {
                        child.parent = current.node;
                        //cout << "parent: " << child.parent.rb_chicken << child.parent.rb_wolf << child.parent.lb_chicken << child.parent.lb_wolf << child.parent.boat << endl;
                        frontier.push(child);
                        frontier_map.insert(pair<Node, int>(child, counter));
                    }
                }
            }
        }
    }
}

int dfs(Node init, Node goal) {
    int counter = 0;
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
        counter++;
        //cout << "current: " << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << endl;
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
                            //cout << "goal: " << child.node.rb_chicken << child.node.rb_wolf << child.node.lb_chicken << child.node.lb_wolf << child.node.boat << endl;
                            stack<Node> path;
                            path.push(child);
                            while (!isEqual(init.node, current.node)) {
                                path.push(current);
                                Node temp;
                                temp.node = current.parent;
                                current = explored.find(temp)->first;
                            }
                            path.push(init);
                            while(!isEqual(goal.node, current.node)) {
                                current = path.top();
                                path.pop();
                                cout << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << endl;
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
}

int iddfs(Node init, Node goal) {
    init.node.depth = 0;
    int depth = -1;
    int counter = 0;
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
            //cout << "current: " << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << current.node.depth << endl;
            frontier.pop();
            frontier_map.erase(current);
            explored.insert(pair<Node, int>(current, counter));
            //cout << counter << endl;
            counter++;
            for (int i = 0; i < 5; i++) {
                Node child = current;
                child.node = updateChild(i, child.node);
                //cout << "child: " << child.node.rb_chicken << child.node.rb_wolf << child.node.lb_chicken << child.node.lb_wolf << child.node.boat << child.node.depth << endl;
                child.node.depth = current.node.depth + 1;
                if (((child.node.rb_wolf <= child.node.rb_chicken) || (child.node.rb_chicken == 0)) &&
                    ((child.node.lb_wolf <= child.node.lb_chicken) || (child.node.lb_chicken == 0)) &&
                    (child.node.lb_chicken >= 0 && child.node.lb_wolf >= 0 && child.node.rb_chicken >= 0 && child.node.rb_wolf >= 0)) {
                    if (child.node.depth < explored.find(child)->first.node.depth || explored.find(child) == explored.end()) {
                        if ((child.node.depth < frontier_map.find(child)->second || frontier_map.find(child) == explored.end()) || frontier_map.empty()) {
                            if (isEqual(child.node, goal.node)) {
                                cout << "goal: " << child.node.rb_chicken << child.node.rb_wolf << child.node.lb_chicken << child.node.lb_wolf << child.node.boat << endl;
                                stack<Node> path;
                                path.push(child);
                                while (!isEqual(init.node, current.node)) {
                                    path.push(current);
                                    Node temp;
                                    temp.node = current.parent;
                                    current = explored.find(temp)->first;
                                }
                                path.push(init);
                                while(!isEqual(goal.node, current.node)) {
                                    current = path.top();
                                    path.pop();
                                    cout << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << " " << current.node.depth << endl;
                                }
                                return counter;
                            }
                            if (child.node.depth <= frontier_map.find(child)->second) {
                                explored.erase(child);
                            }
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
    }
}

int astar(Node init, Node goal) {
    init.node.path_cost = 0;
    int counter = 0;
    Node current;

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
        //cout << "current: " << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << endl;
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
                            //cout << "goal: " << child.node.rb_chicken << child.node.rb_wolf << child.node.lb_chicken << child.node.lb_wolf << child.node.boat << endl;
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
                                cout << current.node.rb_chicken << current.node.rb_wolf << current.node.lb_chicken << current.node.lb_wolf << current.node.boat << " " << current.node.path_cost << " " << temp_c << endl;
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
}


int main(int argc, char* argv[]) {
    string start_file_name = argv[1];
    string goal_file_name = argv[2];
    string mode = argv[3];
    string output_file_name = argv[4];

    Node init = processFile(start_file_name);
    Node goal = processFile(goal_file_name);
    //goal.node.boat = !init.node.boat;

    int count = 0;

    cout << init.node.rb_chicken << init.node.rb_wolf << init.node.lb_chicken << init.node.lb_wolf << init.node.boat << endl;
    cout << goal.node.rb_chicken << goal.node.rb_wolf << goal.node.lb_chicken << goal.node.lb_wolf << goal.node.boat << endl;

    if ("bfs" == mode) {
        count = bfs(init, goal);
    }

    if ("dfs" == mode) {
        count = dfs(init, goal);
    }
    
    if ("iddfs" == mode) {
        count = iddfs(init, goal);
    }
    
    if ("astar" == mode) {
        count = astar(init, goal);
    }

    cout << count << endl;

    return 0;
}