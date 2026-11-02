#include "../h_file/system.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

	bool backtoMainMenu = false;
	bool ExitProgram = false;

	string toLowerCopy(string value) {
		transform(value.begin(), value.end(), value.begin(),
		[](unsigned char ch) {
			return static_cast<char>(tolower(ch));
		});
		return value;
	}

	string trimCopy(const string& value) {
		const size_t first = value.find_first_not_of(" \t\r\n");
		if (first == string::npos) return "";
		const size_t last = value.find_last_not_of(" \t\r\n");
		return value.substr(first, last - first + 1);
	}

	bool isMenuCommand(const string& value) {
		return toLowerCopy(trimCopy(value)) == ":menu";
	}

	bool isExitCommand(const string& value) {
		return toLowerCopy(trimCopy(value)) == ":exit";
	}

	bool readLine(const string& prompt, string& value) {
		cout << prompt << "\n";
		cout << "  Type :menu to return to the main menu or :exit to quit.\n";
		cout << "> ";

		if (!getline(cin, value)) {
			ExitProgram = true;
			return false;
		}

		if (isMenuCommand(value)) {
			backtoMainMenu = true;
			return false;
		}

		if (isExitCommand(value)) {
			cout << "Exiting...\n";
			ExitProgram = true;
			return false;
		}

		return true;
	}

	void waitForMainMenu() {
		string input;
		cout << "\nPress Enter or type :menu to return to the main menu.\n";
		cout << "Type :exit to quit.\n> ";

		if (!getline(cin, input)) {
			ExitProgram = true;
		} else if (isExitCommand(input)) {
			cout << "Exiting...\n";
			ExitProgram = true;
		}
	}

	bool parseMenuChoice(const string& input, int& choice) {
		istringstream stream(input);
		char extra;
		if (!(stream >> choice)) return false;
		stream >> ws;
		return stream.eof() && !(stream >> extra);
	}

	string displayName(const Node* node) {
		if (node == nullptr) return "";

		const File* file = dynamic_cast<const File*>(node);
		if (file != nullptr) {
			return file->extension.empty()
			       ? file->name
			       : file->name + "." + file->extension;
		}
		return node->name;
	}

	string displayPath(Node* node) {
		if (node == nullptr) return "";

		string path = node->getPath();
		const File* file = dynamic_cast<const File*>(node);
		if (file != nullptr && !file->extension.empty()) {
			path += "." + file->extension;
		}
		return path;
	}

	void displayCurrentDirectory(Directory* current_dir) {
		if (current_dir == nullptr) {
			cout << "Current directory is unavailable.\n";
			return;
		}

		cout << "\n========================================\n";
		cout << " Current Directory: " << current_dir->getPath() << "\n";
		cout << "========================================\n";

		if (current_dir->children.empty()) {
			cout << "(This directory is empty.)\n";
			return;
		}

		for (const Node* child : current_dir->children) {
			if (child == nullptr) continue;

			if (dynamic_cast<const Directory*>(child) != nullptr) {
				cout << "[DIR]  " << displayName(child) << "\n";
			} else if (dynamic_cast<const File*>(child) != nullptr) {
				cout << "[FILE] " << displayName(child) << "\n";
			} else {
				cout << "[ITEM] " << displayName(child) << "\n";
			}
		}
	}

	void displaySearchResults(const vector<Node*>& results) {
		if (results.empty()) {
			cout << "Couldn't find file or directory.\n";
			return;
		}

		cout << "\nSearch Results Found:\n";
		for (Node* node : results) {
			if (node == nullptr) continue;

			if (dynamic_cast<const Directory*>(node) != nullptr) {
				cout << "[DIR]  " << displayPath(node) << "\n";
			} else {
				cout << "[FILE] " << displayPath(node) << "\n";
			}
		}
	}

	bool isFileChoice(const string& input) {
		const string choice = toLowerCopy(trimCopy(input));
		return choice == "file" || choice == "f";
	}

	bool isDirectoryChoice(const string& input) {
		const string choice = toLowerCopy(trimCopy(input));
		return choice == "directory" || choice == "dir" || choice == "d";
	}

} // namespace

void System::run() {
	Directory* current_dir = &RootDirectory;
	running = true;
	backtoMainMenu = false;
	ExitProgram = false;

	while (running) {
		backtoMainMenu = false;
		system("clear");
		displayCurrentDirectory(current_dir);

		cout << "\n--------------- MAIN MENU ---------------\n";
		cout << "[1] Navigate/Change Directory\n";
		cout << "[2] Create File/Directory\n";
		cout << "[3] Delete File/Directory\n";
		cout << "[4] Search\n";
		cout << "[5] Return to Root Directory\n";
		cout << "[6] Exit\n";
		cout << "Choose an option (or type :exit): ";

		string menuInput;
		if (!getline(cin, menuInput)) {
			cout << "\nInput closed. Exiting...\n";
			running = false;
			break;
		}

		if (isExitCommand(menuInput)) {
			cout << "Exiting...\n";
			running = false;
			break;
		}

		if (isMenuCommand(menuInput)) {
			continue;
		}

		int user_choice = 0;
		if (!parseMenuChoice(menuInput, user_choice)) {
			cout << "Invalid choice, try again.\n";
			waitForMainMenu();
			if (ExitProgram) running = false;
			continue;
		}

		switch (user_choice) {
			case 1: { // [1] Navigate
				string target_path;
				if (!readLine("Enter directory path (for example Root/Documents/DSA): ", target_path)) {
					break;
				}

				if (!isValidPath(target_path)) {
					cout << "Invalid path format, try again.\n";
					break;
				}

				Directory* target = resolveDirectoryPath(target_path);
				if (target == nullptr) {
					cout << "Directory not found. Current directory was not changed.\n";
				} else {
					current_dir = target;
					cout << "Current directory changed to: " << current_dir->getPath() << "\n";
				}
				break;
			}

			case 2: { // [2] Create
				string item_type;
				if (!readLine("Create [F]ile or [D]irectory? ", item_type)) {
					break;
				}

				string item_name;
				if (isFileChoice(item_type)) {
					if (!readLine("Enter file name including extension (for example report.txt): ", item_name)) {
						break;
					}

					if (createFile(item_name, current_dir)) {
						cout << "File created successfully.\n";
					} else {
						cout << "Could not create file. Check the file name/extension or whether it already exists.\n";
					}
				} else if (isDirectoryChoice(item_type)) {
					if (!readLine("Enter directory name: ", item_name)) {
						break;
					}

					if (createDirectory(item_name, current_dir)) {
						cout << "Directory created successfully.\n";
					} else {
						cout << "Could not create directory. Check the name or whether it already exists.\n";
					}
				} else {
					cout << "Invalid item type. Enter File (F) or Directory (D).\n";
				}
				break;
			}

			case 3: { // [3] Delete
				string item_type;
				if (!readLine("Delete File (F) or Directory (D)? ", item_type)) {
					break;
				}

				string item_name;
				if (isFileChoice(item_type)) {
					if (!readLine("Enter file name including extension: ", item_name)) {
						break;
					}

					if (deleteFile(item_name, current_dir)) {
						cout << "File deleted successfully.\n";
					} else {
						cout << "Couldn't find file to delete.\n";
					}
				} else if (isDirectoryChoice(item_type)) {
					if (!readLine("Enter directory name: ", item_name)) {
						break;
					}

					if (deleteDirectory(item_name, current_dir)) {
						cout << "Directory deleted successfully. Its contents were also removed.\n";
					} else {
						cout << "Couldn't find directory to delete.\n";
					}
				} else {
					cout << "Invalid item type. Enter File (F) or Directory (D).\n";
				}
				break;
			}

			case 4: { // [4] Search
				string search_keyword;
				if (!readLine("Enter search keyword: ", search_keyword)) {
					break;
				}

				if (trimCopy(search_keyword).empty()) {
					cout << "Search keyword cannot be empty.\n";
					break;
				}

				vector<Node*> results = searchDFS(&RootDirectory, search_keyword);
				displaySearchResults(results);
				break;
			}

			case 5:
				current_dir = &RootDirectory;
				cout << "Returned to the Root Directory.\n";
				break;

			case 6:
				cout << "Exiting...\n";
				running = false;
				break;

			default:
				cout << "Invalid choice, try again.\n";
				break;
		}

		if (ExitProgram) {
			running = false;
			break;
		}

		if (running && !backtoMainMenu) {
			waitForMainMenu();
		}

		if (ExitProgram) {
			running = false;
		}
	}
}
