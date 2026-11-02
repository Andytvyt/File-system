#include "..\h_file\system.h"
#include <cctype>
#include <string>
#include <vector>


namespace {


bool hasControlCharacter(const string& value)
{
    for (unsigned char ch : value) {
        if (ch < 32 || ch == 127) return true;
    }
    return false;
}


bool isValidComponent(const string& component)
{
    if (component.empty() || component == "." || component == "..") return false;
    if (component.back() == ' ' || component.back() == '.') return false;
    if (hasControlCharacter(component)) return false;

    const string forbidden = "<>:\"|?*/\\";
    if (component.find_first_of(forbidden) != string::npos) return false;
    return true;
}

bool splitAndValidatePath(const string& input, vector<string>& parts)
{
    parts.clear();
    if (input.empty() || input.front() == '/' || input.back() == '/') return false;
    if (input.find('\\') != string::npos || hasControlCharacter(input)) return false;

    size_t start = 0;
    while (start < input.size()) {
        size_t end = input.find('/', start);
        if (end == string::npos) end = input.size();
        string part = input.substr(start, end - start);
        if (!isValidComponent(part)) return false;
        parts.push_back(part);
        start = end + 1;
    }
    return !parts.empty();
}

bool isValidFileName(const string& fileName)
{
    if (!isValidComponent(fileName)) return false;

    // A file must have a non-empty name and a non-empty final extension.
    const size_t dot = fileName.find_last_of('.');
    if (dot == string::npos || dot == 0 || dot == fileName.size() - 1) return false;
    return true;
}

string fullFileName(const File* file)
{
    return file ? file->name + "." + file->extension : string();
}

bool itemExists(const Directory* directory, const string& itemName)
{
    if (!directory) return false;
    for (const Node* child : directory->children) {
        if (!child) continue;
        if (const File* file = dynamic_cast<const File*>(child)) {
            if (fullFileName(file) == itemName) return true;
        } else if (child->name == itemName) {
            return true;
        }
    }
    return false;
}

void destroyNode(Node* node)
{
    if (!node) return;
    if (Directory* directory = dynamic_cast<Directory*>(node)) {
        for (Node* child : directory->children) destroyNode(child);
        directory->children.clear();
    }
    delete node;
}

// Remove an optional leading "Root" component. The remaining path is
// interpreted relative to RootDirectory.
bool normalizeRootPrefix(vector<string>& parts)
{
    if (!parts.empty() && parts.front() == "Root") parts.erase(parts.begin());
    return true;
}

} // namespace

// Everything in this section is mostly for naming validations - feel free to adjust 


Directory* System::resolveDirectoryPath(const string& path)
{
    if (path.empty() || path == "Root") return &RootDirectory;

    vector<string> parts;
    if (!splitAndValidatePath(path, parts)) return nullptr;
    normalizeRootPrefix(parts);

    Directory* current = &RootDirectory;
    for (const string& part : parts) {
        Directory* next = nullptr;
        for (Node* child : current->children) {
            if (child && child->name == part) {
                next = dynamic_cast<Directory*>(child);
                break;
            }
        }
        if (!next) return nullptr;
        current = next;
    }
    return current;
}

const Directory* System::resolveDirectoryPath(const string& path) const
{
    return const_cast<System*>(this)->resolveDirectoryPath(path);
}

// ---------------------------- CREATE FILE ----------------------------
bool System::createFile(const string& filePath)
{
    vector<string> parts;
    if (!splitAndValidatePath(filePath, parts)) return false;
    normalizeRootPrefix(parts);
    if (parts.empty()) return false;

    const string fileName = parts.back();
    if (!isValidFileName(fileName)) return false;

    parts.pop_back();
    string parentPath;
    for (const string& part : parts) {
        if (!parentPath.empty()) parentPath += '/';
        parentPath += part;
    }

    Directory* parent = resolveDirectoryPath(parentPath);
    if (!parent || itemExists(parent, fileName)) return false;

    const size_t dot = fileName.find_last_of('.');
    File* file = new File();
    file->name = fileName.substr(0, dot);
    file->extension = fileName.substr(dot + 1);
    file->parent = parent;
    parent->children.push_back(file);
    return true;
}

// -------------------------- CREATE DIRECTORY --------------------------
bool System::createDirectory(const string& directoryPath)
{
    vector<string> parts;
    if (!splitAndValidatePath(directoryPath, parts)) return false;
    normalizeRootPrefix(parts);
    if (parts.empty()) return false; // Cannot recreate/rename the root.

    const string directoryName = parts.back();
    parts.pop_back();

    string parentPath;
    for (const string& part : parts) {
        if (!parentPath.empty()) parentPath += '/';
        parentPath += part;
    }

    Directory* parent = resolveDirectoryPath(parentPath);
    if (!parent || itemExists(parent, directoryName)) return false;

    Directory* directory = new Directory();
    directory->name = directoryName;
    directory->parent = parent;
    parent->children.push_back(directory);
    return true;
}

// ----------------------------- DELETE FILE ----------------------------
bool System::deleteFile(const string& filePath)
{
    vector<string> parts;
    if (!splitAndValidatePath(filePath, parts)) return false;
    normalizeRootPrefix(parts);
    if (parts.empty() || !isValidFileName(parts.back())) return false;

    const string fileName = parts.back();
    parts.pop_back();
    string parentPath;
    for (const string& part : parts) {
        if (!parentPath.empty()) parentPath += '/';
        parentPath += part;
    }

    Directory* parent = resolveDirectoryPath(parentPath);
    if (!parent) return false;

    for (size_t i = 0; i < parent->children.size(); ++i) {
        File* file = dynamic_cast<File*>(parent->children[i]);
        if (file && fullFileName(file) == fileName) {
            delete file;
            parent->children.erase(parent->children.begin() + static_cast<long>(i));
            return true;
        }
    }
    return false;
}

// -------------------------- DELETE DIRECTORY --------------------------
bool System::deleteDirectory(const string& directoryPath)
{
    vector<string> parts;
    if (!splitAndValidatePath(directoryPath, parts)) return false;
    normalizeRootPrefix(parts);
    if (parts.empty()) return false; // Root cannot be deleted.

    const string directoryName = parts.back();
    parts.pop_back();
    string parentPath;
    for (const string& part : parts) {
        if (!parentPath.empty()) parentPath += '/';
        parentPath += part;
    }

    Directory* parent = resolveDirectoryPath(parentPath);
    if (!parent) return false;

    for (size_t i = 0; i < parent->children.size(); ++i) {
        Directory* directory = dynamic_cast<Directory*>(parent->children[i]);
        if (directory && directory->name == directoryName) {
            destroyNode(directory);
            parent->children.erase(parent->children.begin() + static_cast<long>(i));
            return true;
        }
    }
    return false;
}

// ----------- BACKWARD-COMPATIBLE DIRECTORY-POINTER OVERLOADS -----------
bool System::createFile(const string& fileName, Directory* current)
{
    if (!current || !isValidFileName(fileName) || itemExists(current, fileName)) return false;
    const size_t dot = fileName.find_last_of('.');
    File* file = new File();
    file->name = fileName.substr(0, dot);
    file->extension = fileName.substr(dot + 1);
    file->parent = current;
    current->children.push_back(file);
    return true;
}

bool System::createDirectory(const string& directoryName, Directory* current)
{
    if (!current || !isValidComponent(directoryName) || itemExists(current, directoryName)) return false;
    Directory* directory = new Directory();
    directory->name = directoryName;
    directory->parent = current;
    current->children.push_back(directory);
    return true;
}

bool System::deleteFile(const string& fileName, Directory* current)
{
    if (!current || !isValidFileName(fileName)) return false;
    for (size_t i = 0; i < current->children.size(); ++i) {
        File* file = dynamic_cast<File*>(current->children[i]);
        if (file && fullFileName(file) == fileName) {
            delete file;
            current->children.erase(current->children.begin() + static_cast<long>(i));
            return true;
        }
    }
    return false;
}

bool System::deleteDirectory(const string& directoryName, Directory* current)
{
    if (!current || !isValidComponent(directoryName)) return false;
    for (size_t i = 0; i < current->children.size(); ++i) {
        Directory* directory = dynamic_cast<Directory*>(current->children[i]);
        if (directory && directory->name == directoryName) {
            destroyNode(directory);
            current->children.erase(current->children.begin() + static_cast<long>(i));
            return true;
        }
    }
    return false;
}
