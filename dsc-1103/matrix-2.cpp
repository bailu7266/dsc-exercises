#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

const int MAX_ROW = 64;
const int MAX_COL = 64;

template <class T>
class Matrix
{
    T m_2d_matrix[MAX_ROW][MAX_COL];
    int m_row;
    int m_col;

public:
    Matrix() : m_row(0), m_col(0) {}

    Matrix(int row, int col)
    {
        if (row <= 0 || row > MAX_ROW || col < 0 || col > MAX_COL)
        {
            cout << "Matrix size can't oversize (" << MAX_ROW << ", " << MAX_COL << ")" << endl;
        }
        else
        {
            m_row = row;
            m_col = col;
        }
    }

    T rec_sum_row(int row, int col_start)
    {
        T sum = 0;
        if (col_start < m_col)
        {
            // cout << m_2d_matrix[row][col_start] << " ";
            sum = m_2d_matrix[row][col_start] + rec_sum_row(row, col_start + 1);
        }

        // cout << endl;

        return sum;
    }

    T rec_sum_col(int col, int row_start)
    {
        T sum = 0;
        if (row_start < m_row)
        {
            // cout << m_2d_matrix[row_start][col] << " ";
            sum = m_2d_matrix[row_start][col] + rec_sum_col(col, row_start + 1);
        }

        // cout << endl;

        return sum;
    }

    void set(int row, int col, T t)
    {
        m_2d_matrix[row][col] = t;
    }

    T& at(int row, int col)
    {
        return m_2d_matrix[row][col];        
    }

    int match()
    {
        T row_sum[MAX_ROW];
        T col_sum[MAX_COL];
        int i, j;
        int mc = 0;             // match number
        
        for (i = 0; i < m_row; i ++)
            row_sum[i] = rec_sum_row(i, 0);
        for (j = 0; j < m_col; j ++)
            col_sum[j] = rec_sum_col(j, 0);

        for(i = 0; i < m_row; i ++)
        {
            for(j = 0; j < m_col; j ++)
            {
                if (row_sum[i] == col_sum[j])
                    mc += 1;
            }
        }

        return mc;
    }
};

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

    Matrix<int> matrix(m, n);

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
                // matrix.set(i, j, stoi(col_str));
                matrix.at(i, j) = stoi(col_str);
            }
            else
            {
                // matrix.set(i, j, 0);
                matrix.at(i, j) = 0;
            }
        }
    }

    fs_in.close();
    
    if (m <= 0 || n <= 0 || m > MAX_ROW || n > MAX_COL)
    {
        cout << "m or n must be greater than 0 and less than " << MAX_ROW << endl;
        return -1;
    }

    int mc = matrix.match();

    string fname_out("out.txt");
    fstream fs_out(fname_out, fstream::out);
    if(fs_out.is_open())
    {
        fs_out << mc << endl;
        fs_out.close();
    }
}