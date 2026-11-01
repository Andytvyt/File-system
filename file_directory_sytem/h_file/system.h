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

       // Directory Resolvers
        Directory* resolveDirectoryPath(const string& path);
        const Directory* resolveDirectoryPath(const string& path) const;

        // Module Tao
        bool createFile(const string& filePath);
        bool createDirectory(const string& directoryPath);

        bool createFile(const string& fileName, Directory* current);
        bool createDirectory(const string& directoryName, Directory* current);

        // Module Xoa
        bool deleteFile(const string& filePath);
        bool deleteDirectory(const string& directoryPath);

        bool deleteFile(const string& fileName, Directory* current);
        bool deleteDirectory(const string& directoryName, Directory* current);
        //Module Tim kiem
        vector<Node*> searchDFS(Node* start, const string& keyword);
        // Module interface
        void run(); // tui để trong system.cpp, Bắc Sơn muốn dời đi đâu thì dời nha
};
