#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int MAX_ROW = 64;
const int MAX_COL = 64;

int rec_sum_row(int m_row[], int col)
{
    int sum = 0;
    if (col > 0 && col < MAX_COL)
    {
        sum = m_row[0] + rec_sum_row(m_row + 1, col - 1);
        // col --;
    }

    return sum;
}

int rec_sum_col(int m_col[][MAX_COL], int row)
{
    int sum = 0;
    if (row > 0 && row < MAX_ROW)
    {
        // cout << m_col << endl;
        sum = m_col[0][0] + rec_sum_col(m_col + 1, row - 1);
        // 注意m_col + 1, 这指针是(int (*)[MAX_COL]), 不是简单的 + sizeof(int).
        // row --;
    }

    return sum;
}

int match(int matrix[][MAX_COL], int row, int col)
{
    int row_sum[MAX_ROW];
    int col_sum[MAX_COL];
    int i, j;
    int mc = 0;             // match number

    for(i = 0; i < row; i ++)
    {
        /* row_sum[i] = 0;
        for (j = 0; j < col; j ++)
        {
            row_sum[i] += matrix[i][j];
        } */
        row_sum[i] = rec_sum_row(matrix[i], col);
        // cout << row_sum[i] << ' ';
    }
    // cout << endl;

    for(j = 0; j < col; j ++)
    {
        /* col_sum[j] = 0;
        for(i = 0; i < row; i ++)
        {
            col_sum[j] += matrix[i][j];
        } */
        col_sum[j] = rec_sum_col((int (*)[MAX_COL]) &(matrix[0][j]), row);
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
    
    if (m <= 0 || n <= 0 || m > MAX_ROW || n > MAX_COL)
    {
        cout << "m or n must be greater than 0 and less than " << MAX_ROW << endl;
        return -1;
    }

    int mc = match(matrix, m, n);

    string fname_out("out.txt");
    fstream fs_out(fname_out, fstream::out);
    if(fs_out.is_open())
    {
        fs_out << mc << endl;
        fs_out.close();
    }
}