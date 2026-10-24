#include "..\h_file\system.h"
#include <stack>
#include <cctype>
using namespace std;

static string toLower(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        s[i] = tolower((unsigned char)s[i]);
    return s;
}

vector<Node*> System::searchDFS(Node* start, const string& keyword)
{
    vector<Node*> results;
    if (start == nullptr)
        return results;

    string key = toLower(keyword);
    stack<Node*> st;
    st.push(start);

    while (!st.empty())
    {
        Node* current = st.top();
        st.pop();

        // Hiện chỉ so khớp theo name. Sửa dòng này khi nhóm chốt name+extension hoặc path.
        if (toLower(current->name).find(key) != string::npos)
            results.push_back(current);

        Directory* dir = dynamic_cast<Directory*>(current);
        if (dir != nullptr)
        {
            for (size_t i = 0; i < dir->children.size(); i++)
                st.push(dir->children[i]);
        }
    }
    return results;
}
