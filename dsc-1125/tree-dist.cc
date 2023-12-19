#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>

using namespace std;

const int MAX_VAL = 500;

struct TreeNode
{
    TreeNode* parent;
    TreeNode* left;
    TreeNode* right;
    int val;
    TreeNode() : parent(NULL), left(NULL), right(NULL), val(-1) {}
};

class Solution
{
TreeNode nodes[MAX_VAL + 1];
TreeNode* root;

public:
    Solution(vector<int> vals) : root(NULL)
    {
        int i = 1;      // root is no 1 instead of no 0
        for(vector<int>::iterator it = vals.begin(); it != vals.end(); it ++, i ++)
        {
            int val = *it;
            if (val == -1)      // Do nothing because all pointer initialized to NULL
                continue;

            nodes[i].val = val;

            if (i == 1)         // root node
            {
                // nodes[i].parent = NULL;
                root = nodes + i;       // &(nodes[i]);
                continue;
            }

            int i_pa = (i >> 1);    // parent index

            nodes[i].parent = nodes + i_pa;     // &(nodes[i_pa]);
            
            if ((i % 2) == 0)       // even: left
            {
                nodes[i_pa].left = nodes + i;    // &(nodes[i]);
            }
            else                    // odd: right
            {
                nodes[i_pa].right = nodes + i;   // &(nodes[i]);
            }
        }
    }

    TreeNode* find(TreeNode* c_root, int val)
    {
        if (c_root == NULL)
            return c_root;

        if (c_root->val == val)
            return c_root;

        TreeNode* l_found = NULL;
        TreeNode* r_found = NULL;
        if (c_root->left)
            l_found = find(c_root->left, val);
        
        if (c_root->right)
            r_found = find(c_root->right, val);

        if (l_found == NULL && r_found == NULL)
            return NULL;

        if (l_found) return l_found;

        return r_found;
    }

    TreeNode* get_root()
    {
        return root;
    }

    void decends_dist(TreeNode* target, int k, vector<int>& results)
    {
        if (k == 0)
        {
            results.push_back(target->val);
            return;
        }

        // for decendents
        if (target->left)
        {
            decends_dist(target->left, k - 1, results);
        }

        if (target->right)
        {
            decends_dist(target->right, k - 1, results);
        }
    }

    void grandpa_dist(TreeNode* target, int k, vector<int>& results)
    {
        // Grand parent: 同样需要排除指向自己的分支
        TreeNode* parent = target->parent;

        if (parent == NULL) return;

        while(parent)
        {
            k = k - 1;

            if (k == 0)
            {
                results.push_back(parent->val);
                return;
            }

            target = parent;
            parent = target->parent;        // go up to grand parent
            // k = k - 1;

            siblings_dist(target, k, results);
        }
    }

    void siblings_dist(TreeNode* target, int k, vector<int>& results)
    {
        // Siblings: 需要排除原来的自己的分支
        TreeNode* parent = target->parent;
        k = k - 1;

        if (parent == NULL  || k < 1)
            return;

        if (parent->left == target)
        {
            if (parent->right)
            {
                decends_dist(parent->right, k - 1, results);
            }
        }
        else
        {
            if (parent->left)
            {
                decends_dist(parent->left, k - 1, results);
            }
        }
    }

    vector<int> distanceK(TreeNode* root, TreeNode* target, int k)
    {
        vector<int> results;
        decends_dist(target, k, results);
        siblings_dist(target, k, results);
        grandpa_dist(target, k, results);
        return results;
    }
};

int main()
{
    string fname_in("in.txt");
    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        cout << fname_in << "doesn't exist" << endl;
        return -1;
    }

    vector<int> vals;

    string line;

    getline(fs_in, line);

    int start = line.find('[');
    int end = line.find(']');
    string sub_line = line.substr(start + 1, end - start - 1);
    // char* token = strtok((char*) (line.c_str()), (char*) "[");
    // token = strtok(NULL, (char*) "]");
    char delim[] = ",";
    char* token = strtok((char*) (sub_line.c_str()), delim);
    while (token)
    {
        // 当然可以像以往一样getline（sstream, delimeter)
        // 这样用strtok是引入另外一种split string的方法，注意strtok是C function
        if (strchr(token, '#'))
            vals.push_back(-1);
        else
            vals.push_back(stoi(token));

        token = strtok(NULL, delim);
    }

    // Read the target
    getline(fs_in, line);
    int i_target = stoi(line);

    // Read the distance
    getline(fs_in, line);
    int distance = stoi(line);

    fs_in.close();
    
    Solution solution(vals);

    TreeNode* target = solution.find(solution.get_root(), i_target);

    vector<int> result = solution.distanceK(NULL, target, distance);

    char str_out[128] = "[";
    int str_offset = strlen(str_out);

    for (vector<int>::iterator it = result.begin(); it != result.end(); it ++)
    {
        str_offset += snprintf(str_out + str_offset, 128 - str_offset, "%d, ", *it);
    }

    // cut the last two chars ", "
    str_out[strlen(str_out) - 2] = '\0';
    strcat(str_out, "]");

    cout << str_out << endl;
}