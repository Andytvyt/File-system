#pragma once

#include "directory.h"
#include "file.h"

class System{
    //Bien cua class, thich them gi thi them
    public:
        bool running = true;
        Directory RootDirectory("Root");

        //Path handling, method getPath() của class File và Directory trả về full path của nó
        bool isValidPath(const string& path);
        string getFileName(const string& path);
        string getExtension(const string& path); // nếu là directory, trả về ""
        string getParentPath(const string& path);

        // Module Tao

        // Module Xoa

        //Module Tim kiem
        vector<Node*> searchDFS(Node* start, const string& keyword);
        // Module interface
        void run(); // tui để trong system.cpp, Bắc Sơn muốn dời đi đâu thì dời nha
};
