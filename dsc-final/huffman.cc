#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

using HFM_CODE = unordered_map<char, string>;

struct HFM_NODE
{
    char symbol;
    HFM_NODE* left;
    HFM_NODE* right;

    HFM_NODE(char s) : symbol(s), left(NULL), right(NULL) {}
};

class CHuffmanTree
{
    HFM_NODE* m_root;

public:
    CHuffmanTree(HFM_CODE codes)
    {
        m_root = build(codes);
    }

    HFM_NODE* root() { return m_root; };

    HFM_NODE* build(HFM_CODE codes)
    {
        HFM_NODE* _root = new HFM_NODE('\0');

        for (const auto me : codes)
        {
            const char sym = me.first;
            const string& code = me.second;

            HFM_NODE* cur_node = _root;

            for (char bit : code)
            {
                if (bit == '0')
                {
                    if (cur_node->left == NULL)
                        cur_node->left = new HFM_NODE('\0');
                    cur_node = cur_node->left;
                }
                else if (bit == '1')
                {
                    if (cur_node->right == NULL)
                        cur_node->right = new HFM_NODE('\0');
                    cur_node = cur_node->right;
                }
            }

            cur_node->symbol = sym;
        }

        return _root;
    }

    void decode(HFM_NODE* p_node, const string& encode_msg, size_t& cur_pos, string& decode_msg)
    {
        if (p_node == NULL) return;

        if (p_node->symbol != '\0')
        {
            decode_msg += p_node->symbol;
            return;
        }

        if (cur_pos < encode_msg.length() && encode_msg[cur_pos] == '0')
        {
            cur_pos ++;
            decode(p_node->left, encode_msg, cur_pos, decode_msg);
        }
        else if (cur_pos < encode_msg.length() && encode_msg[cur_pos] == '1')
        {
            cur_pos ++;
            decode(p_node->right, encode_msg, cur_pos, decode_msg);
        }
    }
};

int main()
{
    HFM_CODE hfm_codes;

    istream& is_code = cin;
    string fn_code = "hfm_codes.txt";
    ifstream fs_code(fn_code, ifstream::in);
    if (fs_code.is_open())
    {
        cout << "Input Huffman codes from: " << fn_code << endl;
        is_code.rdbuf(fs_code.rdbuf());
    }
    else
    {
        cout << "Input Huffman codes:" << endl;
    }

    string input;
    getline(is_code, input);
    int code_num = stoi(input);

    if (code_num <= 0) return -1;

    for(int i = 0; i < code_num; i ++)
    {
        input.clear();
        getline(is_code, input);
        int pos = input.find(':');
        if (pos == string::npos)
        {
            cout << "Invalid input without ':'." << endl;
            return -1;
        }
        
        hfm_codes.insert({(input.c_str())[pos - 1], input.substr(pos + 1)});
    }

    if(fs_code.is_open())
        fs_code.close();

    CHuffmanTree hfm(hfm_codes);

    string fin_name = "in.txt";
    ifstream fs_in(fin_name, ifstream::in);
    if (!fs_in.is_open())
    {
        cout << fin_name << " does not exist." << endl;
        return -1;
    }

    input.clear();
    getline(fs_in, input);

    size_t pos = 0;
    string decode_msg;

    while(pos < input.length())
        hfm.decode(hfm.root(), input, pos, decode_msg);

    cout << decode_msg << endl;

    if (fs_in.is_open())
        fs_in.close();
}