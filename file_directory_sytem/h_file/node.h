#pragma once

#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class Node {
    public:
        string name;
        Node* parent;
        string getPath();
        Node();
        Node(string name);
        Node(string name, Node* parent);
};