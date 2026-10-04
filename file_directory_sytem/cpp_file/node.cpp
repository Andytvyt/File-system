#include "..\h_file\node.h"
using namespace std;

string Node::getPath()
{
    std::vector<std::string> parts;

    Node* current = this;

    while (current != nullptr)
    {
        parts.push_back(current->name);
        current = current->parent;
    }

    reverse(parts.begin(), parts.end());

    string path;

    for (string part : parts)
    {
        if (!path.empty())
            path += "/";

        path += part;
    }

    return path;
}

Node::Node(){
    this->name = "";
    this->parent = nullptr;
}

Node::Node(string name){
    this->name = name;
    this->parent = nullptr;
}

Node::Node(string name, Node* parent){
    this->name = name;
    this->parent = parent;
}