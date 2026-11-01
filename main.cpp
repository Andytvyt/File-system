#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "file_directory_sytem/h_file/system.h"

using namespace std;

void clearScreen()
{
    system("cls");
}

// false return to the menu or exit
bool readPrompt(string prompt, string &answer, bool &goToMenu, bool &exitProgram)
{
    goToMenu = false;
    exitProgram = false;

    cout << prompt << endl;
    cout << "Type :menu to return to the main menu or :exit to close the program." << endl;
    cout << "> ";

    if (!getline(cin, answer))
    {
        exitProgram = true;
        return false;
    }

    if (answer == ":menu")
    {
        goToMenu = true;
        return false;
    }

    if (answer == ":exit")
    {
        exitProgram = true;
        return false;
    }

    return true;
}

void handlePromptControl(System &fs, Directory *&currentDir,
                         bool goToMenu, bool exitProgram)
{
    if (goToMenu)
    {
        currentDir = &fs.RootDirectory;
        cout << "Returning to the main menu..." << endl;
    }

    if (exitProgram)
    {
        fs.running = false;
        cout << "Exiting..." << endl;
    }
}

//Linger on result
void pauseAfterResult(System &fs, Directory *&currentDir)
{
    string answer;
    bool goToMenu = false;
    bool exitProgram = false;

    cout << endl;
    cout << "Press Enter to return to the main menu." << endl;

    if (!readPrompt("", answer, goToMenu, exitProgram))
    {
        handlePromptControl(fs, currentDir, goToMenu, exitProgram);
    }
}

// Show current directory and its files and folders
void displayCurrentDirectory(Directory *currentDir)
{
    if (currentDir == NULL)
    {
        return;
    }

    cout << "========================================" << endl;
    cout << "Current Directory: " << currentDir->getPath() << endl;
    cout << "----------------------------------------" << endl;

    if (currentDir->children.empty())
    {
        cout << "This directory is empty." << endl;
    }
    else
    {
        for (int i = 0; i < (int)currentDir->children.size(); i++)
        {
            Node *child = currentDir->children[i];

            if (child == NULL)
            {
                continue;
            }

            if (dynamic_cast<Directory *>(child) != NULL)
            {
                cout << "[DIR]  " << child->name << endl;
            }
            else
            {
                File *file = dynamic_cast<File *>(child);
                if (file != NULL)
                {
                    cout << "[FILE] " << file->name;
                    if (file->extension != "")
                    {
                        cout << "." << file->extension;
                    }
                    cout << endl;
                }
            }
        }
    }

    cout << "----------------------------------------" << endl;
}

// Show main menu
void displayMainMenu()
{
    cout << "Main Menu" << endl;
    cout << "[1] Navigate / Change Directory" << endl;
    cout << "[2] Create File / Directory" << endl;
    cout << "[3] Delete File / Directory" << endl;
    cout << "[4] Search" << endl;
    cout << "[5] Return to Main Menu (Root Directory)" << endl;
    cout << "[6] Exit" << endl;
}

// Ask create/delete a file or a directory
bool readItemType(string action, string &itemType,
                  bool &goToMenu, bool &exitProgram)
{
    string answer;

    while (true)
    {
        cout << endl;
        cout << "Choose item type for " << action << ":" << endl;
        cout << "[1] File" << endl;
        cout << "[2] Directory" << endl;

        if (!readPrompt("Enter 1 or 2:", answer, goToMenu, exitProgram))
        {
            return false;
        }

        if (answer == "1" || answer == "File" || answer == "file")
        {
            itemType = "file";
            return true;
        }

        if (answer == "2" || answer == "Directory" || answer == "directory")
        {
            itemType = "directory";
            return true;
        }

        cout << "Invalid choice. Enter 1 for File or 2 for Directory." << endl;
    }
}

// Display all search results
void displaySearchResults(vector<Node *> results)
{
    if (results.empty())
    {
        cout << "Could not find a matching file or directory." << endl;
        return;
    }

    cout << "Search Results:" << endl;

    for (int i = 0; i < (int)results.size(); i++)
    {
        Node *node = results[i];

        if (node != NULL)
        {
            string path = node->getPath();
            File *file = dynamic_cast<File *>(node);

            if (file != NULL && file->extension != "")
            {
                path = path + "." + file->extension;
            }

            cout << path << endl;
        }
    }
}

int main()
{
    System fs;
    Directory *currentDir = &fs.RootDirectory;

    while (fs.running)
    {
        clearScreen();
        displayCurrentDirectory(currentDir);
        displayMainMenu();

        string choice;
        bool goToMenu = false;
        bool exitProgram = false;

        if (!readPrompt("Choose an option:", choice, goToMenu, exitProgram))
        {
            handlePromptControl(fs, currentDir, goToMenu, exitProgram);
            continue;
        }

        // [1]: Navigate
        if (choice == "1")
        {
            while (fs.running)
            {
                string path;

                if (!readPrompt("Enter target directory path:", path,
                                goToMenu, exitProgram))
                {
                    handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                    break;
                }

                if (path == "")
                {
                    cout << "Path cannot be empty. Try again." << endl;
                    continue;
                }

                if (path == "..")
                {
                    Directory *parentDir = dynamic_cast<Directory *>(currentDir->parent);

                    if (parentDir == NULL)
                    {
                        cout << "You are already at the root directory." << endl;
                        continue;
                    }

                    currentDir = parentDir;
                    cout << "Moved to the parent directory successfully." << endl;
                    pauseAfterResult(fs, currentDir);
                    break;
                }

                if (!fs.isValidPath(path))
                {
                    cout << "Invalid path. Try again or use :menu / :exit." << endl;
                    continue;
                }

                Directory *newDir = fs.resolveDirectoryPath(path);

                if (newDir == NULL)
                {
                    cout << "Directory not found. Try again or use :menu / :exit." << endl;
                    continue;
                }

                currentDir = newDir;
                cout << "Directory changed successfully." << endl;
                pauseAfterResult(fs, currentDir);
                break;
            }
        }
        // [2] Create a file or directory
        else if (choice == "2")
        {
            string itemType;

            if (readItemType("creation", itemType, goToMenu, exitProgram))
            {
                while (fs.running)
                {
                    string itemName;

                    if (itemType == "file")
                    {
                        if (!readPrompt("Enter file name with extension (example: report.txt):",
                                        itemName, goToMenu, exitProgram))
                        {
                            handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                            break;
                        }
                    }
                    else
                    {
                        if (!readPrompt("Enter directory name:", itemName,
                                        goToMenu, exitProgram))
                        {
                            handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                            break;
                        }
                    }

                    if (itemName == "")
                    {
                        cout << "Name cannot be empty. Try again." << endl;
                        continue;
                    }

                    bool created = false;

                    if (itemType == "file")
                    {
                        created = fs.createFile(itemName, currentDir);
                    }
                    else
                    {
                        created = fs.createDirectory(itemName, currentDir);
                    }

                    if (!created)
                    {
                        cout << "Could not create the item. Check the name or try another name." << endl;
                        continue;
                    }

                    cout << "Created successfully." << endl;
                    pauseAfterResult(fs, currentDir);
                    break;
                }
            }
            else
            {
                handlePromptControl(fs, currentDir, goToMenu, exitProgram);
            }
        }
        // [3] Delete a file or directory
        else if (choice == "3")
        {
            string itemType;

            if (readItemType("deletion", itemType, goToMenu, exitProgram))
            {
                while (fs.running)
                {
                    string itemName;

                    if (itemType == "file")
                    {
                        if (!readPrompt("Enter file name with extension (example: report.txt):",
                                        itemName, goToMenu, exitProgram))
                        {
                            handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                            break;
                        }
                    }
                    else
                    {
                        if (!readPrompt("Enter directory name:", itemName,
                                        goToMenu, exitProgram))
                        {
                            handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                            break;
                        }
                    }

                    if (itemName == "")
                    {
                        cout << "Name cannot be empty. Try again." << endl;
                        continue;
                    }

                    bool deleted = false;

                    if (itemType == "file")
                    {
                        deleted = fs.deleteFile(itemName, currentDir);
                    }
                    else
                    {
                        deleted = fs.deleteDirectory(itemName, currentDir);
                    }

                    if (!deleted)
                    {
                        cout << "Could not find the item to delete. Try again." << endl;
                        continue;
                    }

                    cout << "Deleted successfully." << endl;
                    pauseAfterResult(fs, currentDir);
                    break;
                }
            }
            else
            {
                handlePromptControl(fs, currentDir, goToMenu, exitProgram);
            }
        }
        // [4] Search from root directory
        else if (choice == "4")
        {
            string keyword;
            bool shouldSearch = false;

            while (fs.running)
            {
                if (!readPrompt("Enter search keyword:", keyword,
                                goToMenu, exitProgram))
                {
                    handlePromptControl(fs, currentDir, goToMenu, exitProgram);
                    break;
                }

                if (keyword == "")
                {
                    cout << "Search keyword cannot be empty. Try again." << endl;
                    continue;
                }

                shouldSearch = true;
                break;
            }

            if (fs.running && shouldSearch)
            {
                vector<Node *> results = fs.searchDFS(&fs.RootDirectory, keyword);
                displaySearchResults(results);
                pauseAfterResult(fs, currentDir);
            }
        }
        // [5] Return to main menu
        else if (choice == "5")
        {
            currentDir = &fs.RootDirectory;
            cout << "Returned to Main menu." << endl;
            pauseAfterResult(fs, currentDir);
        }
        // [6] Exit
        else if (choice == "6")
        {
            fs.running = false;
            cout << "Exiting..." << endl;
        }
        else
        {
            cout << "Invalid menu choice. Enter a number from 1 to 6." << endl;
            pauseAfterResult(fs, currentDir);
        }
    }

    return 0;
}
