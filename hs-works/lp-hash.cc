#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include <functional>

using namespace std;

using LPH_RESULT = pair<int, vector<int>>;

template<class T, class HASH_FUNC>
class CLPHash
{
    enum BKT_TYPE
    {
        EMPTY = 0,
        ACTIVE
    };

    template <class HBT> class HashBucket
    {
    public:
        HBT val;
        BKT_TYPE type;

        HashBucket() : type(BKT_TYPE::EMPTY) {}
    };

    vector<HashBucket<T>> table;
    size_t hash_size;
    // function<size_t (T)> my_hash = [this](T v) {} (HASH_FUNC(v) % this->hash_size)
    size_t my_hash(T v)
    {
        HASH_FUNC hf;
        return hf(v);
    }

    function<size_t (int)> lpf = [](int i) -> size_t { return i; };

public:
    CLPHash(int s = 199) : hash_size(s), table(s) {}
    int insert(T v)
    {
        int pi = 0;
        int hki = my_hash(v) % hash_size;
        int index = hki;

        while(table[index].type != BKT_TYPE::EMPTY)
        {
            pi ++;
            index = (hki + lpf(pi)) % hash_size;
        }

        table[index].val = v;
        table[index].type = BKT_TYPE::ACTIVE;

        return index;
    }

    int find(T v, vector<int>& ps)       // -1: not found
    {
        int pi = 0;
        int hki = my_hash(v) % hash_size;
        int index = hki; //lpf(0) = 0
        
        while(table[index].type == BKT_TYPE::ACTIVE)
        {
            ps.push_back(index); // Trial position sequence
            if (table[index].val == v)
                return index;

            pi ++;
            index = (hki + lpf(pi)) % hash_size;
        }

        return -1;
    }

    void remove(T v)
    {
        int i = find(v);
        if (i == -1)
            return;

        table[i].type = EMPTY;
    }
};

struct CMyHash
{
    size_t operator () (int i) { return i%199; }
};

int main()
{
    CLPHash<int, CMyHash> lph;
    string fname_in = "in.txt";
    string fname_out = "out.txt";

    fstream fs_in(fname_in, fstream::in);
    if (!fs_in.is_open())
    {
        return -1;
    }

    string line;

    getline(fs_in, line);
    int num;
    sscanf(line.c_str(), "%d", &num);

    line.clear();
    getline(fs_in, line);

    stringstream ss(line);
    
    char delim = ' ';
    while(!ss.eof())
    {
        string istr;
        getline(ss, istr, delim);
        lph.insert(stoi(istr));
    }

    vector<int> ps;
    int ret = lph.find(num, ps);

    ss.str(string());
    ss.clear();
    if (ret != -1)
        // fs_out << "True" << endl;
        ss << "True" << endl;
    else
        // fs_out << "False" << endl;
        ss << "False" << endl;

    for (int i = 0; i < ps.size(); i ++)
        ss << ps[i] << ' ';

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