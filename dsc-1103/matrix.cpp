#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

#define MAX_ROW 64
#define MAX_COL 64

int match(int matrix[][MAX_COL], int row, int col)
{
    int row_sum[MAX_ROW];
    int col_sum[MAX_COL];
    int i, j;
    int mc = 0;             // match number

    for(i = 0; i < row; i ++)
    {
        row_sum[i] = 0;
        for (j = 0; j < col; j ++)
        {
            row_sum[i] += matrix[i][j];
        }
        // cout << row_sum[i] << ' ';
    }
    // cout << endl;

    for(j = 0; j < col; j ++)
    {
        col_sum[j] = 0;
        for(i = 0; i < row; i ++)
        {
            col_sum[j] += matrix[i][j];
        }
        // cout << col_sum[j] << ' ';
    }
    // cout << endl;

    for(i = 0; i < row; i ++)
    {
        for(j = 0; j < col; j ++)
        {
            if (row_sum[i] == col_sum[j])
                mc += 1;
        }
    }

    return mc;
}

int main()
{
    string fname_in("in.txt");
    fstream fs_in(fname_in, fstream::in);
    if(!fs_in.is_open())
    {
        cout << fname_in << " does not exist\n";
        return -1;
    }

    string input;

    // 读取 m and n
    getline(fs_in, input);
    
    stringstream ss(input);
    string m_str, n_str;
    
    getline(ss, m_str, ' ');
    getline(ss, n_str, ' ');

    int m = stoi(m_str);
    int n = stoi(n_str);

    int matrix[MAX_ROW][MAX_COL];

    int i, j;

    for(i = 0; i < m; i ++)
    {
        getline(fs_in, input);
        stringstream ss_row(input);
        for(j = 0; j < n; j ++)
        {
            string col_str;
            if(!ss_row.eof())
            {
                getline(ss_row, col_str, ' ');
                // cout << col_str << endl;                
                matrix[i][j] = stoi(col_str);
            }
            else
                matrix[i][j] = 0;
        }
    }

    fs_in.close();
    
    int mc = match(matrix, m, n);

    string fname_out("out.txt");
    fstream fs_out(fname_out, fstream::out);
    if(fs_out.is_open())
    {
        fs_out << mc << endl;
        fs_out.close();
    }
}