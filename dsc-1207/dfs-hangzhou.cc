#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

const int MAX_V = 100;
const int MAX_E = 1000;
const int DIST_INF = 99999;
const bool UNVISITED = false;
const bool VISITED = true;

class CGEdge
{
    int m_v1, m_v2;     // vertices' seq_no = index of vector + 1
    int m_cost;
    bool status;

public:
    CGEdge(int _v1, int _v2, int _cost) : m_v1(_v1), m_v2(_v2), m_cost(_cost) {}

    /* bool is_incident(CGVertex& v)
    {
        return v.v_id() == m_v1 || v.v_id() == m_v2;
    } */

    void vertices(int& _v1, int& _v2)
    {
        _v1 = m_v1;
        _v2 = m_v2;
    }

    int opposite(int v)
    {
        if (m_v1 == v) return m_v2;
        else if (m_v2 == v) return m_v1;
        assert(false);
    }

    int& cost() { return m_cost; }
    void unvisit() { status = UNVISITED; }
    void visit() { status = VISITED; }
    bool is_visited() { return status; }
    bool is_incident(int v)
    {
        return v == m_v1 || v == m_v2;
    }
};

class CGVertex
{
    vector<int> m_ad_list;      // Adjacent list stored in a vector
    int m_id;
    int status;

public:
    CGVertex() : m_id(-1) {}

    bool ad_exist(int e_idx)
    {
        for (auto it = m_ad_list.begin(); it != m_ad_list.end(); it ++)
        {
            if ((*it) == e_idx) return true;
        }

        return false;
    }

    int& v_id() { return m_id; }

    void ad_append(int e_idx)
    {
        m_ad_list.push_back(e_idx);
    }

    vector<int>& incident_edges() { return m_ad_list; }

    void unvisit() { status = UNVISITED; }
    void visit() { status = VISITED; }
    bool is_visited() { return status; }
};

class CFDS_Graph
{
    vector<CGVertex> vertices;
    vector<CGEdge> edges;
    vector<int> cycle;
    int start;          // start point of the loop
    bool done;

public:
    CFDS_Graph(fstream* fs)
    {
        eval_edges(fs);
        extract_vertices();
    }

    CGVertex& vertices_by_id(int id)
    {
        return vertices[id - 1];
    }

    CGEdge& edge_by_idx(int e_idx)
    {
        return edges[e_idx];
    }

    bool is_done() { return done; }

    void eval_edges(fstream* fs)
    {
        string line;
        char delim = ' ';
        getline(*fs, line);

        stringstream ss(line);
        string str_b;

        getline(ss, str_b, delim);
        int vp_num = stoi(str_b);
        vertices.resize(vp_num);

        getline(ss, str_b, delim);
        int road_num = stoi(str_b);

        for (int i  = 0; i < road_num; i ++)
        {
            getline(*fs, line);
            ss.clear();
            ss.str(line);
            getline(ss, str_b, delim);
            int v1 = stoi(str_b);            
            getline(ss, str_b, delim);
            int v2 = stoi(str_b);            
            getline(ss, str_b, delim);
            int cost = stoi(str_b);            
            edges.push_back(CGEdge(v1, v2, cost));
        }
    }

    void extract_vertices()
    {
        for (auto it = edges.begin(); it != edges.end(); it ++)
        {
            int v1, v2;
            (*it).vertices(v1, v2);
            int cost = (*it).cost();

            vertices_by_id(v1).v_id() = v1;
            int e_idx = distance(edges.begin(), it);
            if (!vertices_by_id(v1).ad_exist(e_idx))
                vertices_by_id(v1).ad_append(e_idx);

            vertices_by_id(v2).v_id() = v2;
            if (!vertices_by_id(v2).ad_exist(e_idx))
                vertices_by_id(v2).ad_append(e_idx);

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

    void traverse_discovery(int e, int v)
    {
        cycle.push_back(e);
    }

    void traverse_back(int e, int v)
    {
        if (!done)
        {
            done = true;
            cycle.push_back(e);
            start = edge_by_idx(e).opposite(v);
        }
    }

    void FDS_traversal(int v)
    {
        start_visit(v);

        vertices_by_id(v).visit();

        for (auto it = vertices_by_id(v).incident_edges().begin();
            (it != vertices_by_id(v).incident_edges().end()) && (!is_done()); it ++)
        {
            CGEdge& edge = edge_by_idx(*it);
            if (!edge.is_visited())
            {
                edge.visit();
                int w = edge.opposite(v);

                if (!vertices_by_id(w).is_visited())
                {
                    traverse_discovery(*it, v);
                    if (!is_done())
                        FDS_traversal(w);
                }
                else
                    traverse_back(*it, v);
            }
        }

        if (!is_done()) finish_visit(v);
    }

    void initialize()
    {
        done = false;
        for (auto it = vertices.begin(); it != vertices.end(); it ++)
            (*it).unvisit();
        
        for (auto it = edges.begin(); it != edges.end(); it ++)
            (*it).unvisit();
    }

    vector<int>& FDS_cycle(int s)
    {
        initialize();
        start = s;
        cycle.clear();

        FDS_traversal(s);
           
        if (!cycle.empty() && s != start)
        {
            auto it = cycle.begin();
            while(it != cycle.end())
            {
                if (edge_by_idx(*(it ++)).is_incident(start)) break;
            }
            cycle.erase(cycle.begin(), it);
        }

        return cycle;
    }

    void solution()
    {
        int i, j;
        int longest[MAX_V];

        for (i = 1; i <= vertices.size(); i ++)
        {
            FDS_cycle(i);

            longest[i] = 0;
            
            if (cycle.size() <= 2)
                longest[i] = DIST_INF;
            else
            {
                for (auto it = cycle.begin(); it != cycle.end(); it ++)
                {
                    longest[i] += edge_by_idx(*it).cost();
                }
            }
        }

        int shortest = DIST_INF;

        for (i = 1; i <= vertices.size(); i ++)
        {
            if (longest[i] < shortest)
            {
                shortest = longest[i];
            }
        }

        if (shortest == DIST_INF)
            cout << "It's impossible" << endl;
        else
            cout << shortest << endl;
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

    CFDS_Graph graph(&fs_in);

    fs_in.close();

    graph.solution();    
}