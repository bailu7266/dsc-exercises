#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int MAX_NUM = 100;
const int DIST_INF = 99999;

// Shortest path in Floyd Warshall Algorithm
class CDGAdjacentMatrix
{
int m_dist[MAX_NUM][MAX_NUM];
int node_num;
int start;

public:
    CDGAdjacentMatrix(fstream* fs)
    {
        string line;
        getline(*fs, line);
        stringstream ss(line);
        ss >> node_num;
        ss >> start;
        ss.clear();

        int i, j;

        for (i  = 0; i < node_num; i ++)
        {
            for (j = 0; j < node_num; j ++)
            {
                if (i == j)
                    m_dist[i][j] = 0;
                else
                    m_dist[i][j] = DIST_INF;
            }
        }

        while(getline(*fs, line))
        {
            int source, dest, weight;
            sscanf(line.c_str(), "%d %d %d", &source, &dest, &weight);
            m_dist[source - 1][dest - 1] = weight;
            /*
            // ss.clear();
            ss << line;
            string str_b;
            char delim = ' ';
            getline(ss, str_b, delim);
            int source = stoi(str_b) - 1;   // node no start from 1
            getline(ss, str_b, delim);
            int dest = stoi(str_b) - 1;
            getline(ss, str_b, delim);
            m_dist[source][dest] = stoi(str_b);
            ss.clear(); */
        }

        FloydWarShall();
    }

    void FloydWarShall()
    {
        int i, j, k;

        for (k = 0; k < node_num; k ++)
            for (i = 0; i < node_num; i ++)
                for (j = 0; j < node_num; j ++)
                    if (m_dist[i][k] != DIST_INF && m_dist[k][j] != DIST_INF &&
                        (m_dist[i][j] > m_dist[i][k] + m_dist[k][j]))
                            m_dist[i][j] = m_dist[i][k] + m_dist[k][j];
    }

    int networkDelayTime(int k)
    {
        int delay = 0;
        for (int i = 0; i < node_num; i ++)
        {
            if (m_dist[k - 1][i] > delay)
                delay = m_dist[k - 1][i];
        }

        return delay;
    }

    void solution()
    {
        int delay = networkDelayTime(start);
        if (delay == DIST_INF)
            cout << "there is at least one isolated endpoint" << endl;
        else
            cout << delay << endl;
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

    CDGAdjacentMatrix adj_matrix(&fs_in);

    fs_in.close();

    adj_matrix.solution();    
}