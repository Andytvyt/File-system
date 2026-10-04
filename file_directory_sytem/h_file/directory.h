#pragma once

#include "node.h"
#include <vector>

class Directory : public Node {
public:
    vector<Node*> children;
};
