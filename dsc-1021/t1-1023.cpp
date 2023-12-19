#include <iostream>
#include <fstream>
#include <string>

#define MAX_BED 20
#define MAX_CUSTOMER MAX_BED       // should be less than or equal to MAX_BED

using namespace std;

typedef enum __CUSTOMER_STATUS
{
    UNKNOWN = -1,
    PRESENT = 0,
    CHECKIN_SUCCESS,
    CHECKIN_FAIL,
    OVER                        // 因为一个字母只能出现一对，出现后置OVER，不能在出现
} E_CS;

typedef struct __CUSTOMER
{
    int next;                   // 因为节点采用数组存放，所以此处用数组的index作为next索引
    char name;                  // 输入的客户名称，全部转化为大写
    E_CS status;                // 客户状态
}   S_CUSTOMER;

class c_customer_list
{
public:
    c_customer_list()
    {
        m_head = -1;
        m_next_free = 0;
        for (int i = 0; i < MAX_CUSTOMER; i ++)
        {
            m_customer[i].next = -1;
            m_customer[i].name = ' ';
            m_customer[i].status = E_CS::UNKNOWN;
        }
    }

    S_CUSTOMER m_customer[MAX_CUSTOMER];     // 节点存储空间是constructor直接分配的
    int m_head;
    int m_tail;
    int m_next_free;
    int add(char name)
    {
        // 直接在list的tail增加一个节点
        if (m_head == -1)
        {
            m_head = m_next_free;
        }
        else
        {
            m_customer[m_tail].next = m_next_free;
        }

        m_tail = m_next_free;
        
        m_customer[m_tail].next = -1;
        m_customer[m_tail].name = name;
        m_customer[m_tail].status = E_CS::PRESENT;

        m_next_free ++;             // 为了简单，直接顺序分配，不考虑节点回收

        return m_tail;
    };

    int lookup(char name)
    {
        if (m_head == -1)
            return -1;
        
        int next = m_head;

        while (next != -1)
        {
            if (m_customer[next].name == name)
                return next;

            next = m_customer[next].next;
        }
        
        return -1;
    };

    void checkin(int index)
    {
        m_customer[index].status = E_CS::CHECKIN_SUCCESS;
    }

    void checkin_fail(int index)
    {
        m_customer[index].status = E_CS::CHECKIN_FAIL;
    }

    void left(int index)
    {
        // 客户无论是否成功入住，离开后都置为OVER，不能在出现
        // 由于customer_list只是记录全部可能的客户清单，所以离开后，客户清单不变（只改变状态）
        m_customer[index].status = E_CS::OVER;
    }

    E_CS get_status(int index)
    {
        return m_customer[index].status;
    }
};

class c_tanned
{
public:
    c_tanned(int bed)
    {
        m_bed = bed;
        m_away = 0;
        m_tanned = 0;
    }

    int m_bed;      // idle bed number
    int m_away;     // walked away number
    int m_tanned;   // get tanned number
    c_customer_list m_customer_list;

    void get_tanned(char names[])
    {
        for (int i = 0; i < strlen(names); i ++)
        {
            char upper_name = toupper(names[i]);
            int index = m_customer_list.lookup(upper_name);
            if (-1 == index)
            {
                index = m_customer_list.add(upper_name);
                if (m_bed > 0)
                {
                    m_customer_list.checkin(index);
                    m_bed --;
                }
                else
                {
                    m_customer_list.checkin_fail(index);
                }
            }
            else
            {
                E_CS status = m_customer_list.get_status(index);
                switch(status)
                {
                    case E_CS::CHECKIN_SUCCESS:
                        m_customer_list.left(index);
                        m_bed ++;
                        m_tanned ++;
                        break;
                
                    case E_CS::CHECKIN_FAIL:
                        m_customer_list.left(index);
                        m_away ++;
                        break;

                    case E_CS::OVER:
                        cout << "One customer can only present one time\n";
                        break;

                    default:
                        cout << "Something has been wrong\n";
                } 
            }

        }

        cout << m_away << " customer(s) walked away\n";
    };
};

void test_tanned(string input)
{
    int bed;
    char customers[MAX_CUSTOMER];
    if (2 != sscanf(input.c_str(), "%d%s", &bed, customers))
    {
        return;
    }
    
    if (bed <= 0 || bed >= MAX_BED)
        return;
    
    c_tanned tan = c_tanned(bed);
    tan.get_tanned(customers);
}

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
        test_tanned(input);
    }

    fs_in.close();

    return 0;
}