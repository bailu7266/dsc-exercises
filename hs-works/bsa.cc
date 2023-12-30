#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

int bsa(vector<int>& iv, int low, int high, int k, vector<int>& pv)
{
    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        pv.push_back(iv[mid]);

        if (iv[mid] > k)
            high = mid - 1;
        else if (iv[mid] < k)
            low = mid + 1;
        else
            return mid;
    }
    return -1; 
}

int bsa_rec(vector<int> iv, int low, int high, int key, vector<int>& pv)
{
    if (low <= high)
    {
        int mid = low + (high - low) / 2;

        pv.push_back(iv[mid]);

        if (iv[mid] > key)
            return bsa_rec(iv, low, mid - 1, key, pv);
        else if (iv[mid] < key)
            return bsa_rec(iv, mid + 1, high, key, pv);
        else
            return mid;
    }

    return -1;
}

int main()
{
    string fname_in = "in.txt";
    string fname_out = "out.txt";

    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        return -1;
    }

    string line;

    getline(fs_in, line);
    int s_num;
    sscanf(line.c_str(), "%d", &s_num);

    line.clear();
    getline(fs_in, line);

    stringstream ss(line);
    vector<int> int_v;
    
    char delim = ' ';
    int i = 0;
    int last;

    while(!ss.eof())
    {
        string istr;
        getline(ss, istr, delim);
        int input_i = stoi(istr);
        if (i == 0)
            last = input_i;
        else if (input_i >= last)
            last = input_i;
        else
        {
            cout << "Input integers are not progressively increasing." << endl;
            return -1;
        }
        int_v.push_back(input_i);
        i ++;
    }

    vector<int> pv;
    int ret = bsa_rec(int_v, 0, int_v.size() - 1, s_num, pv);

    ss.str(string());
    ss.clear();
    if (ret != -1)
        // fs_out << "True" << endl;
        ss << "True" << endl;
    else
        // fs_out << "False" << endl;
        ss << "False" << endl;

    for (int i = 0; i < pv.size(); i ++)
        ss << pv[i] << ' ';

    ss << endl;

    fstream fs_out(fname_out, fstream::out);
    if (fs_out.is_open())
    {
        fs_out << ss.str();
        fs_out.close();
    }
    else
        cout << fname_out << "can't open" << endl;
}