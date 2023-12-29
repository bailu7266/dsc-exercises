// #define USE_MEDIAN_3
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

template<class T>
class CQuickSelect
{
    vector<T> ds;   // data set

public:
    function<T&(int, int)> select_pivot;
    
    CQuickSelect(vector<T>& s) : ds(s)
    {
#ifdef USE_MEDIAN_3
        select_pivot = [this](int l, int r) ->
        T& { return this->median_3(l, r); };       // using median of three
#else
        select_pivot = [this](int l, int r) ->
        T& { return this->ds[r]; };   // using last as pivot
#endif
    }

#ifdef USE_MEDIAN_3
    T& median_3(int left, int right)
    {
        int center = (left + right) / 2;
        if (ds[center] < ds[left])
            swap(ds[left], ds[center]);
        if (ds[right] < ds[left])
            swap(ds[left], ds[right]);
        if (ds[right] < ds[center])
            swap(ds[center], ds[right]);

        swap(ds[center], ds[right - 1]);

        return ds[right - 1];
    }

    T& seq_select(int left, int right, int k)
    {
        for (int i = left + 1; i < right; i ++)
        {
            T tmp = move(ds[i]);
            int j;
            for (j = i; j > left && tmp < ds[j - 1]; j --)
                ds[j] = move(ds[j - 1]);

            ds[j] = move(tmp);
        }

        return ds[k];
    }
#endif

    int partition(int left, int right)
    {
        T pivot = select_pivot(left, right);
#ifdef USE_MEDIAN_3
        int i = left;
        int j = right - 1;
#else
        int i = left - 1;   // reserve for first ++ 1, actully start with left
        int j = right;      // reserve for first -- j, actully start with right - 1
#endif
        for(;;)
        {
            while(ds[++ i] < pivot) {}
            while(pivot < ds[-- j]) {}

            if (i < j)
                swap(ds[i], ds[j]);
            else
                break;
        }

#ifdef USE_MEDIAN_3
        swap(ds[i], ds[right - 1]);
#else
        swap(ds[i], ds[right]);
#endif

        return i;
    }

    T& quick_select(int left, int right, int k /**kth smallest*/)
    {
        assert (left <= right);
#ifdef USE_MEDIAN_3
        if (left + 2 < right)
        {
#endif
            int pivot_index = partition(left, right);
            if (k == pivot_index)
                return ds[k];
            else
            {
                if (k < pivot_index)
                    return quick_select(left, pivot_index - 1, k);
                else
                    return quick_select(pivot_index + 1, right, k);
            }
#ifdef USE_MEDIAN_3
        }
        else
            return seq_select(left, right, k);
#endif
    }
};

vector<int> calc_diff(vector<int> iv)
{
    vector<int> diff;
    for (int i = 0; i < iv.size(); i ++)
    {
        for (int j = i + 1; j < iv.size(); j ++)
            diff.push_back(abs(iv[i] - iv[j]));
    }
    return move(diff);
}

int main()
{
    string fname_in = "in.txt";

    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        return -1;
    }

    string line;

    getline(fs_in, line);
    int num_count;
    sscanf(line.c_str(), "%d", &num_count);

    line.clear();
    getline(fs_in, line);

    stringstream ss(line);
    
    char delim = ' ';

    vector<int> int_v;

    int ni = 0;
    for(; ni < num_count && (!ss.eof()); ni ++)
    {
        string istr;
        getline(ss, istr, delim);
        int_v.push_back(stoi(istr));
    }
    if (ni < num_count)
    {
        cout << "insufficient inpunt integers" << endl;
        return -1;
    }

    vector<int> diff = calc_diff(int_v);
    CQuickSelect<int> qi(diff);

    int mid_pos = diff.size() / 2 - 1;

    int median = qi.quick_select(0, diff.size() - 1, mid_pos);

    cout << median << endl;
}