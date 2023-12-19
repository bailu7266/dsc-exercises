#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define MAX_NODE    32

using namespace std;

struct ListNode
{
    int val;
    ListNode *next;
    ListNode(int x):val(x),next(NULL) {}
};

class Solution
{
public:
    Solution()
    {
        m_head = NULL;
        m_tail = NULL;
    }

    virtual ~Solution()
    {
        while (m_head)
        {
            ListNode* next = m_head->next;
            delete m_head;
            m_head = next;
        }
    }

    ListNode* m_head;
    ListNode* m_tail;

    void add(int x)
    {
        ListNode* p_node = new ListNode(x);
        if (m_head == NULL)
        {
            m_head = p_node;
        }
        else
        {
            m_tail->next = p_node; 
        }
        
        m_tail = p_node;
        m_tail->next = NULL;        
    }

    void reorderList(/* ListNode * head */)
    {
        // 由于直接在一个class内实现，所以保持有head，无需参数传入
        ListNode* node_table[MAX_NODE];
        ListNode* next = m_head;
        int index = 0;
        while (next)
        {
            // 遍历把节点保存为数组，方便后面重排
            node_table[index] = next;
            next = next->next;
            index ++;
        }

        next = m_head;

        int i, j;
        for (i = 0, j = index - 1; i < j; i ++, j --)
        {
            next->next = node_table[j];
            next = node_table[i + 1];                        
            node_table[j]->next = next;
        }
    /*    
        if (i == j)
        {
            node_table[j]->next = NULL;
        }
        else
        {
            next->next = NULL;
        }
    */    
        node_table[i]->next = NULL;

        showList();
    }

    void showList()
    {
        // show result
        ListNode* next = m_head;
        char buff[MAX_NODE * 4];
        int offset = 0;
        while(next)
        {
            offset += snprintf(buff + offset, MAX_NODE * 4 - offset, "%d ", next->val);

            next = next->next;            
        }

        cout << buff << endl;
    }
};

int main()
{
    // char input[MAX_CUSTOMER + 4];

    string file_name("in.txt");
    fstream fs_in(file_name, fstream::in);
    if(!fs_in.is_open())
    {
        cout << file_name << " does not exist\n";
        return -1;
    }

    string input;

    while (getline(fs_in, input))
    {
        Solution sol;
        string str_val;
        stringstream ss(input);
        while(getline(ss, str_val, ' '))
        {
            sol.add(stoi(str_val));
        }

        // sol.showList();
        
        sol.reorderList();
    }

    fs_in.close();

    return 0;
}