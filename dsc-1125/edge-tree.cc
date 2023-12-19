#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include <vector>

using namespace std;

const int MAX_NUM = 16;

struct EDGE
{
    int parent;
    int son;
    EDGE(): parent(0), son(0) {}
};

class CEdge
{
EDGE m_edges[MAX_NUM];
int m_count;

public:
    CEdge(int nodes_count)
    {
        assert(nodes_count >= 2 && nodes_count <= 10000);
        m_count = nodes_count;
    }

    EDGE& operator [] (int index)
    {
        return m_edges[index];
    }

    int size()
    {
        return m_count;
    }
};

struct TREE_NODE
{
    list<int> children;     // of child index in CNodeMap
    int number;
    TREE_NODE() : number(0) {}
};

// 用edge老构建tree，需要先构建一个node map
class CNodeMap
{
TREE_NODE m_nodes[MAX_NUM + 1];
int max_nodes;

public:
    CNodeMap()
    {
        max_nodes = MAX_NUM;
        for (int i = 0; i < MAX_NUM + 1; i ++)
        {
            m_nodes[i].children.clear();
            m_nodes[i].number = 0;
        }
    }

    TREE_NODE& operator [] (int index)
    {
        return m_nodes[index];
    }

    bool is_empty(int number)
    {
        return m_nodes[number].number == 0;
    }

    int set(int number)
    {
        m_nodes[number].number = number;
        return number;
    }

    void append_child(int pa_i, int son_i)
    {
        m_nodes[pa_i].children.push_back(son_i);
    }

    void append_child(EDGE edge)
    {
        m_nodes[edge.parent].children.push_back(edge.son);
    }

    int get_root()
    {
        bool is_child[MAX_NUM + 1];
        int i;
        for (i = 1; i < MAX_NUM + 1; i ++)
            is_child[i] = false;

        for (i = 1; i < MAX_NUM + 1; i ++)
        {
            for (list<int>::iterator it = m_nodes[i].children.begin();
                it != m_nodes[i].children.end(); ++ it)
            {
                is_child[*it] = true;
            }
        }

        for (i = 1; i < MAX_NUM + 1; i ++)
        {
            if (!is_child[i] && (m_nodes[i].number != 0))
                return i;
        }

        return -1;
    }
};

class CTree
{
int root;           // tree root index in CNodeMap
CNodeMap map_node;

public:
    // construct a tree from edges
    CTree(CEdge edge) : root(-1)
    {
        // n - 1个edge对应n个tree nodes
        for (int i =  0; i < edge.size(); i ++)
        {
            if (map_node.is_empty(edge[i].parent))
            {
                map_node.set(edge[i].parent);
            }

            if (map_node.is_empty(edge[i].son))
            {
                map_node.set(edge[i].son);
            }

            // 不能有重复或者错误的edge声明
            map_node.append_child(edge[i]);
        }

        // find the root index
        root = map_node.get_root();
    }

    bool find_path(int c_root, int i_node, vector<int>& path)
    {
        if (-1 == c_root)
            return false;

        path.push_back(c_root);

        if (c_root == i_node)
            return true;

        bool found = false;
        for (list<int>::iterator it = map_node[c_root].children.begin();
            it != map_node[c_root].children.end(); it ++)
        {
            found = (found || find_path(*it, i_node, path));
        }
        if (found)
            return true;

        path.pop_back();
        return false;
    }

    int get_lca(int i_node1, int i_node2)
    {
        vector<int> path1, path2;
        if (!find_path(root, i_node1, path1) || !find_path(root, i_node2, path2))
        {
            return -1;
        }

        int i;
        for (i = 0; i < path1.size() && i < path2.size(); i ++)
        {
            if (path1[i] != path2[i])
                break;
        }

        return path1[i - 1];
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

    string line;
    getline(fs_in, line);

    int nodes_count = stoi(line);
    if (nodes_count < 2 || nodes_count > 10000)
    {
        cout << "Invalid nodes count: should be in [2, 10000]" << endl;
        return -1;
    }
    
    CEdge edges = CEdge(nodes_count - 1);
    char delim[] = " ";
    char* token = NULL;

    for (int i = 0; i < nodes_count - 1; i ++)
    {
        // read （n - 1) edges 
        getline(fs_in, line);
        // 当然可以像以往一样getline（sstream, delimeter)
        // 这样用strtok是引入另外一种split string的方法，注意strtok是C function
        token = strtok((char*) (line.c_str()), delim);
        edges[i].parent = stoi(token);
        token = strtok(NULL, delim);
        edges[i].son = stoi(token);
    }

    // Read the last line and get the two nodes of common ancestor
    getline(fs_in, line);

    fs_in.close();

    int number1 = stoi(strtok((char*) (line.c_str()), delim));
    int number2 = stoi(strtok(NULL, delim));

    CTree tree(edges);

    int result = tree.get_lca(number1, number2);
    cout << result << endl;
}