#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace std;

const int MAX_V = 100;
const int MAX_E = 100;

const int TWO = 2;

enum EV_STATUS { unvisited = 0, visited, discovery, back };

class CUGEdge
{
    int m_v1, m_v2;     // vertices' seq_no = index of vector + 1
    EV_STATUS status;

public:
    struct HashFunc
    {
        size_t operator () (const CUGEdge& edge) const
        {
            hash<int> hasher;
            return hasher(edge.m_v1) ^ hasher(edge.m_v2);
        }
    };

    bool operator == (const CUGEdge& other) const
    {
        return (m_v1 == other.m_v1 && m_v2 == other.m_v2) || (m_v2 == other.m_v1 && m_v1 == other.m_v2); 
    }

    CUGEdge(int v1, int v2) : m_v1(v1), m_v2(v2) {}

    bool is_incident_on(int v)
    {
        return v == m_v1 || v == m_v2;
    }

    void end_vertices(int& v1, int& v2)
    {
        v1 = m_v1;
        v2 = m_v2;
    }

    int opposite(int v)
    {
        if (m_v1 == v) return m_v2;
        else if (m_v2 == v) return m_v1;
        assert(false);
    }

    void set_status(EV_STATUS s) { status = s; }
    bool is_visited() { return status >= EV_STATUS::visited; }
    bool is_incident(int v)
    {
        return v == m_v1 || v == m_v2;
    }
};

using EDGE_SET = unordered_set<CUGEdge, CUGEdge::HashFunc>;
using DFS_VISIT = function<void(int)>;
using DFS_DISCOVERY = function<void(CUGEdge&, int)>;
using DFS_TRAV_BACK = function<void(CUGEdge&, int)>;

class CGVertex
{
    EDGE_SET m_ad_table;
    // Adjacent table, should be a subset of edges
    EV_STATUS status;

public:
    CGVertex() : status(EV_STATUS::unvisited) {}

    void incidence_add(CUGEdge edge)
    {
        m_ad_table.insert(edge);
    }

    EDGE_SET& incident_edges() { return m_ad_table; }

    void set_status(EV_STATUS s) { status = s; };
    bool is_visited() { return status == EV_STATUS::visited; }
};

using VERTICE_MAP = unordered_map<int, CGVertex>;

class CUGraph
{
    VERTICE_MAP vertices;   // vertices set
    EDGE_SET edges;         // undirect graph, edge between to ends is unique
    vector<CUGEdge> cycle;
    int start;          // start point of the loop
    bool done;

public:
    struct DFSInterface
    {
        DFS_VISIT start_visit;
        DFS_VISIT visit;
        DFS_VISIT finish_visit;
        DFS_DISCOVERY trav_discovery;
        DFS_TRAV_BACK trav_back;
    };


    CUGraph() {}

    void insert_vertice(int v)
    {
        vertices[v] = CGVertex();
    }

    void erase_vertice(int v)
    {

    }

    bool is_done() { return done; }

    void insert_edge(int v1, int v2)
    {
        auto result = edges.insert(CUGEdge(v1, v2));
        if (result.second)
        {
            vertices.insert({v1, CGVertex()});
            vertices[v1].incidence_add(*(result.first));
            vertices.insert({v2, CGVertex()});
            vertices[v2].incidence_add(*(result.first));
        }
    }

    void start_visit(int v)
    {/*
        cycle.push_back(v);

        if (start == v)
        {
            if (cycle.size() > 2)
            {
                done = true;
            }
        }*/
    }

    void finish_visit(int v)
    {
        if (!cycle.empty())
            cycle.pop_back();
    }

    void traverse_discovery(CUGEdge& e, int v)
    {
        cycle.push_back(e);
    }

    void traverse_back(CUGEdge& e, int v)
    {
        if (!is_done())
        {
            done = true;
            cycle.push_back(e);
            start = e.opposite(v);
        }
    }

    void DFS_traversal(int v, const DFSInterface& dfs_if)
    {
        dfs_if.start_visit(v);

        dfs_if.visit(v);

        EDGE_SET& ad_tab = vertices[v].incident_edges();
        for (auto it = ad_tab.begin(); it != ad_tab.end() && (!is_done()); it ++)
        {
            CUGEdge edge = (*it);
            if (!edge.is_visited())
            {
                edge.set_status(EV_STATUS::visited);
                int w = edge.opposite(v);

                if (!vertices[w].is_visited())
                {
                    dfs_if.trav_discovery(edge, v);
                    edge.set_status(EV_STATUS::discovery);
                    if (!is_done())
                    {
                        dfs_if.trav_back(edge, w);
                        edge.set_status(EV_STATUS::back);
                    }
                }
                else
                {
                    dfs_if.trav_back(edge, v);
                    edge.set_status(EV_STATUS::back);
                }
            }
        }

        if (!is_done()) finish_visit(v);
    }

    void initialize()
    {
        done = false;
        for (auto it = vertices.begin(); it != vertices.end(); it ++)
        {
            (*it).second.set_status(EV_STATUS::unvisited);
        }
        
        for (auto it = edges.begin(); it != edges.end(); it ++)
        {
            CUGEdge e = (*it);
            e.set_status(EV_STATUS::unvisited);
        }
    }

    vector<CUGEdge>& FDS_cycle(int s)
    {
        initialize();
        start = s;
        cycle.clear();

        DFSInterface dfs_if;
        dfs_if.start_visit = [this](int v) { this->start_visit(v); };
        dfs_if.visit = [this](int v) { };
        dfs_if.finish_visit = [this](int v) { this->finish_visit(v); };
        dfs_if.trav_discovery = [this](CUGEdge&e, int v) { this->traverse_discovery(e, v); };
        dfs_if.trav_back = [this](CUGEdge& e, int v) { this->traverse_back(e, v); };

        DFS_traversal(s, dfs_if);
           
        if (!cycle.empty() && s != start)
        {
            auto it = cycle.begin();
            while(it != cycle.end())
            {
                if ((*(it ++)).is_incident_on(start)) break;
            }
            cycle.erase(cycle.begin(), it);
        }

        return cycle;
    }

    void solution()
    {
        int i, j;
        // int longest[MAX_V];

        for (i = 1; i <= vertices.size(); i ++)
        {
            FDS_cycle(i);
        }

    }
};

int main()
{
    istream& is_edges = cin;
    string fn_edges = "ug_edges.txt";
    ifstream fs_edges(fn_edges, ifstream::in);
    if (fs_edges.is_open())
    {
        cout << "Input Huffman codes from: " << fn_edges << endl;
        is_edges.rdbuf(fs_edges.rdbuf());
    }
    else
    {
        cout << "Input Huffman codes:" << endl;
    }

    string line;
    getline(is_edges, line);

    int v_num, e_num;
    if (sscanf(line.c_str(), "%d %d", &v_num, &e_num) != TWO)
    {
        cout << "Invalid input: expect two integers." << endl;
        return -1;
    }

    CUGraph graph;

    for (int i = 0; i < e_num && !is_edges.eof(); i ++)
    {
        line.clear();
        getline(is_edges, line);

        int v1, v2;
        if (sscanf(line.c_str(), "%d %d", &v1, &v2) != TWO)
        {
            cout << "Invalid input: expect two integers." << endl;
            return -1;
        }

        graph.insert_edge(v1, v2);
    }

    int del_v;

    if (!is_edges.eof())
    {
        line.clear();
        getline(is_edges, line);
        sscanf(line.c_str(), "%d",  &del_v);
    }
    else
    {
        cout << "Imcomplete input." << endl;
        return -1;
    }

    if (fs_edges.is_open())
        fs_edges.close();

    graph.solution();    
}