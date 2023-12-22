// #define USE_UNORDERED

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#ifdef USE_UNORDERED

#include <unordered_set>
#include <unordered_map>

#else

#include <set>
#include <map>

#endif

#include <functional>

using namespace std;

const int MAX_V = 100;
const int MAX_E = 100;
const int BUFF_SIZE = 128;

const int TWO = 2;

enum EV_STATUS { unvisited = 0, visited, discovery, back, cross, waiting };

class CUGEdge
{
    int m_v1, m_v2;     // vertices' seq_no = index of vector + 1
    EV_STATUS e_status;

public:

#ifdef USE_UNORDERED
    struct HashFunc
    {
        size_t operator () (const CUGEdge& edge) const
        {
            size_t hash1 = hash<int>{}(edge.m_v1);
            size_t hash2 = hash<int>{}(edge.m_v2);
            return hash1 ^ hash2;
        }
    };

    bool operator == (const CUGEdge& other) const
    {
        return (m_v1 == other.m_v1 && m_v2 == other.m_v2) || (m_v2 == other.m_v1 && m_v1 == other.m_v2); 
    }

#else

    struct LessOperator
    {
        bool operator()(const CUGEdge& e1, const CUGEdge& e2) const
        {
            return tie(e1.m_v1, e1.m_v2) < tie(e2.m_v1, e2.m_v2);
        }
    };

#endif  // USE_UNORDERED

    CUGEdge(int v1, int v2) : m_v1(v1), m_v2(v2), e_status(EV_STATUS::unvisited) {}

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

    void set_status(EV_STATUS s) { e_status = s; }
    bool is_visited() { return (e_status != EV_STATUS::unvisited); }
    bool is_incident(int v)
    {
        return v == m_v1 || v == m_v2;
    }
};

#ifdef USE_UNORDERED
using EDGE_SET = unordered_set<CUGEdge, CUGEdge::HashFunc>;
using AD_TABLE = unordered_set<CUGEdge*>;
#else
using EDGE_SET = set<CUGEdge, CUGEdge::LessOperator>;
using AD_TABLE = set<CUGEdge*>;
#endif  // USE_UNORDERED

class CGVertex
{
    AD_TABLE m_ad_table;
    // Adjacent table, should be a subset of edges
    EV_STATUS v_status;

public:
    CGVertex() : v_status(EV_STATUS::unvisited) {}

    void incidence_add(CUGEdge* edge)
    {
        m_ad_table.insert(edge);
    }

    AD_TABLE& incident_edges() { return m_ad_table; }

    void set_status(EV_STATUS s) { v_status = s; };
    bool is_visited() { return (v_status == EV_STATUS::visited); }
    bool is_waiting() { return (v_status == EV_STATUS::waiting); }
};

#ifdef USE_UNORDERED
using VERTICE_MAP = unordered_map<int, CGVertex>;
#else
using VERTICE_MAP = map<int, CGVertex>;
#endif // USE_UNORDERED

class CUGraph
{
    using DFS_VISIT = function<void(int, void**)>;
    using DFS_DISCOVERY = function<void(CUGEdge*, int)>;
    using DFS_TRAV_BACK = function<void(CUGEdge*, int)>;
    using BFS_VISIT = function<void(int, void**)>;
    
    VERTICE_MAP vertices;   // vertices set

    EDGE_SET edges;         // undirect graph, edge between to ends is unique

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
        AD_TABLE& ad_table = vertices[v].incident_edges();
        for(auto it = ad_table.begin(); it != ad_table.end(); it ++)
        {
            CUGEdge* ep = (*it);
            int op_v = ep->opposite(v);
            vertices[op_v].incident_edges().erase(ep);
            // erase the edge in the opposite vertice
            edges.erase(*ep);
        }
        
        ad_table.clear();

        vertices.erase(v);
    }

    bool is_done() { return done; }

    void insert_edge(int v1, int v2)
    {
        auto result = edges.insert(CUGEdge(v1, v2));
        if (result.second)
        {
            CUGEdge* ep = &const_cast<CUGEdge&>(*(result.first));
            vertices.insert({v1, CGVertex()});
            vertices[v1].incidence_add(ep);
            vertices.insert({v2, CGVertex()});
            vertices[v2].incidence_add(ep);
        }
    }

    void visit_print(int v, void** pp)
    {
        char* ptr_buff = (char*) *pp;
        ptr_buff += snprintf(ptr_buff,  BUFF_SIZE / 4, " %d", v);
        
        *pp = (void*) ptr_buff;
    }

    void DFS_traversal(int v, const DFSInterface& dfs_if, void** pp)
    {
        dfs_if.start_visit(v, pp);

        vertices[v].set_status(EV_STATUS::visited);

        dfs_if.visit(v, pp);

        AD_TABLE& ad_tab = vertices[v].incident_edges();
        for (auto it = ad_tab.begin(); it != ad_tab.end() && (!is_done()); it ++)
        {
            CUGEdge* ep = (*it);
            if (!ep->is_visited())
            {
                ep->set_status(EV_STATUS::visited);
                int w = ep->opposite(v);

                if (!vertices[w].is_visited())
                {
                    ep->set_status(EV_STATUS::discovery);
                    dfs_if.trav_discovery(ep, v);
                    if (!is_done())
                    {
                        DFS_traversal(w, dfs_if, pp);
                    }
                }
                else
                {
                    ep->set_status(EV_STATUS::back);
                    dfs_if.trav_back(ep, v);
                }
            }
        }

        if (!is_done()) dfs_if.finish_visit(v, pp);
    }

    void initialize()
    {
        done = false;
        for (auto it = vertices.begin(); it != vertices.end(); it ++)
        {
            it->second.set_status(EV_STATUS::unvisited);
        }
        
        for (auto it = edges.begin(); it != edges.end(); it ++)
        {
            const_cast<CUGEdge&> (*it).set_status(EV_STATUS::unvisited);
        }
    }

    void FDS_trav_print(int s, void** pp)
    {
        initialize();

        DFSInterface dfs_if;
        dfs_if.start_visit = [this](int v, void** pp) { };
        dfs_if.visit = [this](int v, void** pp) { this->visit_print(v, pp); };
        dfs_if.finish_visit = [this](int v, void** pp) { };
        dfs_if.trav_discovery = [this](CUGEdge* e, int v) { };
        dfs_if.trav_back = [this](CUGEdge* e, int v) { };

        DFS_traversal(s, dfs_if, pp);
    }

    void BFS_traversal(int root, BFS_VISIT bfs_visit, void** pp)
    {
        vector<int> level_0, level_1;     // current and next level
        
        level_0.push_back(root);
        
        while(!level_0.empty())
        {
            level_1.clear();   // next level edges
            for (auto it = level_0.begin(); it != level_0.end(); it ++)
            {
                int vi = *it;
                vertices[vi].set_status(EV_STATUS::visited);

                bfs_visit(vi, pp);

                AD_TABLE& edges = vertices[vi].incident_edges();
                for (auto ite = edges.begin(); ite != edges.end(); ite ++)
                {
                    CUGEdge* ep = (*ite);
                    if (!ep->is_visited())
                    {
                        int op_v = ep->opposite(vi);
                        if (!vertices[op_v].is_visited())
                        {
                            ep->set_status(EV_STATUS::discovery);
                            // if the vertice is not in the waiting list
                            if (!vertices[op_v].is_waiting())
                            {
                                vertices[op_v].set_status(EV_STATUS::waiting);
                                level_1.push_back(op_v);
                            }
                        }
                        else
                        {
                            ep->set_status(EV_STATUS::cross);
                        }
                    }

                }
            }
            swap(level_0, level_1);
        }
    }

    void BFS_trav_print(int s, void** pp)
    {
        initialize();

        BFS_VISIT bfs_visit = [this](int v, void** pp) { return this->visit_print(v, pp); };

        BFS_traversal(s, bfs_visit, pp);
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

    char buff[BUFF_SIZE];

    char* tptr_buff = buff;

    graph.FDS_trav_print(0, (void**) &tptr_buff);
    tptr_buff = buff;
    while(*tptr_buff == ' ') tptr_buff ++;  // trim head space
    cout << tptr_buff << endl;

    graph.erase_vertice(del_v);

    tptr_buff = buff;

    graph.BFS_trav_print(0, (void**) &tptr_buff);
    tptr_buff = buff;
    while(*tptr_buff == ' ') tptr_buff ++;  // trim head space
    cout << tptr_buff << endl;
}