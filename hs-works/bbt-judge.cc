#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>

using namespace std;

struct BTNode
{
    BTNode* left;
    BTNode* right;
    string data;

    BTNode(string str) : data(str), left(nullptr), right(nullptr) {}
};

using BBT_RET = pair<bool, int>;

BBT_RET is_balanced(BTNode* root)
{
    if (root == nullptr)
        return make_pair(true, 0);

    BBT_RET left = is_balanced(root->left);
    BBT_RET right = is_balanced(root->right);

    BBT_RET ret;

    ret.second = max(left.second, right.second) + 1;

    bool b_bal = (abs(left.second - right.second) <= 1);
    if (left.first && right.first && b_bal)
        ret.first = true;
    else
        ret.first = false;

    return ret;
}

BTNode* build_tree(vector<string> bn)
{
    if (bn.empty())
        return nullptr;

    BTNode* root = new BTNode(bn[0]);
    vector<BTNode*> p_bn_0, p_bn_1;
    p_bn_0.push_back(root);
    int i = 1;
    while(i < bn.size())
    {
        p_bn_1.clear();
        for (int j = 0; j < p_bn_0.size(); j ++)
        {
            if (p_bn_0[j])
            {
                BTNode* node;
                if (bn[i] == "null")
                    node = nullptr;
                else
                    node = new BTNode(bn[i]);
                i ++;

                if (i >= bn.size()) break;
                
                p_bn_0[j]->left = node;
                p_bn_1.push_back(node);
                if (bn[i] == "null")
                    node = nullptr;
                else
                    node = new BTNode(bn[i]);
                i ++;
                p_bn_0[j]->right = node;
                p_bn_1.push_back(node);
            }
            else
            {
                p_bn_1.push_back(nullptr);
                p_bn_1.push_back(nullptr);
                i += 2;
            }
        }
        swap(p_bn_0, p_bn_1);
    }

    return root;
}

int main()
{
    string fname_in = "in.txt";
    string fname_out = "out.txt";

    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        return -1;
    }

    string line;

    getline(fs_in, line);

    stringstream ss(line);
    
    vector<string> bn;
    char delim = ' ';
    while(!ss.eof())
    {
        string n_data;
        getline(ss, n_data, delim);
        bn.push_back(n_data);
    }

    BTNode* root = build_tree(bn);

    bool ret = is_balanced(root).first;

    fstream fs_out(fname_out, fstream::out);

    if (ret)
        fs_out << "True" << endl;
    else
        fs_out << "False" << endl;
}