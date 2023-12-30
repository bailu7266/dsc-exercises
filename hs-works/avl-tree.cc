#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

template<class T>
struct AVL_NODE
{
    AVL_NODE<T>* left;
    AVL_NODE<T>* right;
    int height;
    T data;
    int count;

    AVL_NODE(T v) : data(v), count(0), height(0),
                    left(nullptr), right(nullptr) {}
};

template<class T>
class CAVLTree
{
    using VISIT_FUNC = function<void(AVL_NODE<T>*, int&, vector<AVL_NODE<T>*>&)>;

    AVL_NODE<T>* left_rotate(AVL_NODE<T>* op)
    {
        AVL_NODE<T>* np = op->right;
    
        op->right = np->left;
        np->left = op;

        // update height
        op->height = 1 + max(height(op->left), height(op->right));
        np->height = 1 + max(height(np->left), height(np->right));

        return np;
    }

    AVL_NODE<T>* right_rotate(AVL_NODE<T>* op)
    {
        AVL_NODE<T>* np = op->left;
    
        op->left = np->right;
        np->right = op;

        // update height
        op->height = 1 + max(height(op->left), height(op->right));
        np->height = 1 + max(height(np->left), height(np->right));

        return np;
    }

public:
    CAVLTree() {}

    int get_balance(AVL_NODE<T>* node)
    {
        if (node == nullptr)
            return 0;

        return height(node->left) - height(node->right);
    }

    int height(AVL_NODE<T>* node)
    {
        if (nullptr == node)
            return 0;
        
        return node->height;
    }

    AVL_NODE<T>* insert(AVL_NODE<T>* node, T& key)
    {
        if (node == nullptr)
        {
            node = new AVL_NODE(key);
            node->count = 1;
            return node;
        }

        if (key < node->data)
            node->left = insert(node->left, key);
        else if (key > node->data)
            node->right = insert(node->right, key);
        else
        {
            node->count += 1;
            return node;
        }

        // update height
        node->height = 1 + max(height(node->left), height(node->right));

        int balance = get_balance(node);

        // left left rotation
        if (balance > 1 && key < node->left->data)
            return right_rotate(node);
        
        // right right rotation
        if (balance < -1 && key > node->right->data)
            return left_rotate(node);

        // left right rotation
        if (balance > 1 && key > node->left->data)
        {
            node->left = left_rotate(node->left);
            return right_rotate(node);
        }

        // right left rotation
        if (balance < -1 && key < node->right->data)
        {
            node->right = right_rotate(node->right);
            return left_rotate(node);
        }

        return node;    // nothing happened
    }

    AVL_NODE<T>* min_val_node(AVL_NODE<T>* node)
    {
        AVL_NODE<T>* cur_node = node;
        while (cur_node)
            cur_node = cur_node->left;

        return cur_node;
    }

    AVL_NODE<T>* erase(AVL_NODE<T>* root, T& key)
    {
        if (root == nullptr)
            return root;

        if (key < root->data)
            root->left = erase(root->left, key);
        else if (key > root->data)
            root->right = erase(root->right, key);
        else
        {
            if (root->left == nullptr || root->right == nullptr)
            {
                AVL_NODE<T>* tmp = root->left;
                if (tmp == nullptr)
                    tmp = root->right;
                if (tmp == nullptr)
                {
                    tmp = root;
                    root = nullptr;
                }
                else
                    *root = *tmp;

                free(tmp);
            }
            else
            {
                AVL_NODE<T>* tmp = min_val_node(root->right);
                root->data = tmp->data;
                root->right = erase(root->right, tmp->data);
            }
        }

        if (nullptr == root)
            return root;

        root->height = 1 + max(height(root->left), height(root->right));

        int balance = get_balance(root);

        if (balance > 1 && get_balance(root->left) >= 0)
            return left_rotate(root);

        if (balance < -1 && get_balance(root-right) <= 0)
            return right_rotate(root);

        if (balance > 1 && get_balance(root->left) < 0)
        {
            root->left = left_rotate(root->left);
            return right_rotate(root);
        }

        if (balance < -1 && get_balance(root->right) > 0)
        {
            root->right = right_rotate(root->right);
            return left_rotate(root);
        }

        return root;
    }

    AVL_NODE<T>* search(AVL_NODE<T>* node, T& key)
    {
        if (node == nullptr)
        {
            return node;
        }

        if (key < node->data)
            node->left = search(node->left, key);
        else if (key > node->data)
            node->right = search(node->right, key);
        else
        {
            return node;
        }
    }

    void inoder_trav(AVL_NODE<T>* root, VISIT_FUNC do_visit, int& total, vector<AVL_NODE<T>*>& nv)
    {
        if (nullptr == root)
            return;

        inoder_trav(root->left, do_visit, total, nv);

        do_visit(root, total, nv);

        inoder_trav(root->right, do_visit, total, nv);
    }
};

using AVL_STRING_PTR = AVL_NODE<string>*;
/*
void do_sum(AVL_NODE<string>* node, int& total, vector<AVL_STRING_PTR>& nv)
{
    total += node->count;
}

void print_node(AVL_NODE<string>* node, int& total, vector<AVL_STRING_PTR>& nv)
{
    double ratio = 100 * (double) node->count / (double) total;
    char buff[128];
    snprintf(buff, sizeof(buff), "%s %8.4f", node->data.c_str(), ratio);
    cout << buff << endl;
}
*/
void do_all_in_one(AVL_NODE<string>* node, int& total, vector<AVL_STRING_PTR>& nv)
{
    total += node->count;
    nv.push_back(node);
}

string trim_leading_spaces(string line)
{
    size_t pos_nsp = line.find_first_not_of(' ');
    if (pos_nsp != string::npos)
        return line.substr(pos_nsp);
    
    return "";
}

int main()
{
    CAVLTree<string> avl_tree;
    AVL_NODE<string>* root = nullptr;

    string fname_in = "in.txt";

    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        return -1;
    }

    string line;

    while(!fs_in.eof())
    {
        getline(fs_in, line);

        string input = trim_leading_spaces(line);
        if (input.size() == 0)
            continue;

        if (line.size() > 30)
        {
            cout << line << " is over long (30)" << endl;
        }
        else
            root = avl_tree.insert(root, line);    

        line.clear();
    }

    int total = 0;
    vector<AVL_STRING_PTR> nodes;

    // avl_tree.inoder_trav(root, &do_sum, total, nodes);

    // avl_tree.inoder_trav(root, &print_node, total, nodes);

    total = 0;

    avl_tree.inoder_trav(root, &do_all_in_one, total, nodes);

    for(int i = 0; i < nodes.size(); i ++)
    {
        char buff[64];
        snprintf(buff, sizeof(buff), "%s %8.4f",
            nodes[i]->data.c_str(), 100 * (double) nodes[i]->count / (double) total);
        cout << buff << endl;
    }
}