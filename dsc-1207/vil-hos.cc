#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int MAX_NUM = 10;
const int DIST_INF = 99999;

// Shortest path in Floyd Warshall Algorithm
class CDGAdjacentMatrix
{
int m_dist[MAX_NUM][MAX_NUM];
int village_num;

public:
    CDGAdjacentMatrix(fstream* fs)
    {
        string line;
        getline(*fs, line);
        village_num = stoi(line);

        int i, j;

        for (i  = 0; i < village_num; i ++)
        {
            for (j = 0; j < village_num; j ++)
            {
                if (i == j)
                    m_dist[i][j] = 0;
                else
                    m_dist[i][j] = DIST_INF;
            }
        }

        for (i = 0; i < village_num; i ++)
        {
            getline(*fs, line);

            stringstream ss(line);
            string str_b;
            char delim = ' ';
            for (j = 0; j < village_num; j ++)
            {
                getline(ss, str_b, delim);
                int dist = stoi(str_b);
                if (dist == 0)
                    dist = DIST_INF;
                m_dist[i][j] = dist;
                m_dist[j][i] = dist;
            }
        }
    }

    void FloydWarShall()
    {
        int i, j, k;

        for (k = 0; k < village_num; k ++)
            for (i = 0; i < village_num; i ++)
                for (j = 0; j < village_num; j ++)
                    if (m_dist[i][k] != DIST_INF && m_dist[k][j] != DIST_INF &&
                        (m_dist[i][j] > m_dist[i][k] + m_dist[k][j]))
                            m_dist[i][j] = m_dist[i][k] + m_dist[k][j];
    }

    void solution()
    {
        FloydWarShall();
        int i, j;
        int longest[MAX_NUM];

        for (i = 0; i < village_num; i ++)
        {
            longest[i] = 0;
            for (j = 0; j < village_num; j ++)
            {
                if (m_dist[i][j] > longest[i])
                {
                    longest[i] = m_dist[i][j];
                }
            }
        }

        int shortest = DIST_INF;
        int serial_no;

        for (i = 0; i < village_num; i ++)
        {
            if (longest[i] < shortest)
            {
                shortest = longest[i];
                serial_no = i;
            }
        }

        if (shortest == DIST_INF)
            cout << "village isolated" << endl;
        else
            cout << serial_no + 1 << endl;
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