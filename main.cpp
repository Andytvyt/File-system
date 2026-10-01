#include <iostream>
#include <vector>
#include <string>
using namespace std;


Struct file{
    string NgayTao;
    string name;
    string extension;
    string path;
    string NoiDung;
}

class directory{
    string NgayTao;
    string name;
    string extension;
    string path;
    vector<directory>;
    vector<file>;
}

class System{
    //Bien cua class, thich them gi thi them
    bool running = true;

    // Module Tao

    // Module Xoa

    //Module Tim kiem

    // Module interface
    void run(){
        while(this->running){
            // Main loop
        };
    }
}


int main(){
    System system;
    system.run();
}