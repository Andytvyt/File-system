#pragma once
#include "node.h"

class File : public Node {
public:
    string extension;
    string content;
};