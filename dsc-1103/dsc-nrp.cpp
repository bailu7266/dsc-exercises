#include <iostream>
#include <string>

using namespace std;

typedef enum __NODE_TYPE
{
    EXPRESSION = 0,
    OPERATOR,
    OPERAND
} E_NODE_TYPE;

class RPN_STACK_NODE
{
public:
    RPN_STACK_NODE* next;
    E_NODE_TYPE type;
    union                   // 用无名union出入栈时不用类型转换
    {
        CRpnExp* p_exp;
        /***************************************
         *  对于()有两种戳方法：
         * 1. 直接把()保存为一个RPN expression, 最后递归计算结果
         * 2. 把()内的Expression计算出结果，把结果保存到stack中，
         *      如果这样做，就不需要这个成员了
         ***************************************/
        int i_val;          // integer valude
        char c_op;          // operator name
    };

    RPN_STACK_NODE() : next(NULL), p_exp(NULL) {}
};

class CRpnExp
{
public:
    CRpnExp(string str) : m_stack_top(NULL)
    {
        RPN_STACK_NODE* p_node;
        const char* input = str.c_str();
        int len = strlen(input);
        int i = 0;                  // input当然处理入口
        int j;                      // 剥离的字符数目
        while((j = get_node(&(input[i]), len - i, p_node)) != -1)
        {
            push(p_node);
            i += j;
        }
    }

    ~CRpnExp()
    {
        RPN_STACK_NODE* p_next = m_stack_top;
        while(p_next)
        {
            if (m_stack_top->type == E_NODE_TYPE::EXPRESSION)
            {
                delete m_stack_top->p_exp;
            }

            delete m_stack_top;

            m_stack_top = p_next->next;
        }       
    }

    RPN_STACK_NODE* m_stack_top;
    // 当然也可以用 stack<RPN_STACK_NODE> m_stack

    int Calc()
    {
        RPN_STACK_NODE* p_node;
        int op_index = 0;           // operand的个数
        int operands[2];            // 目前的operator都是2个operands
        while((p_node = pop()))
        {
            switch (p_node->type)
            {
            case E_NODE_TYPE::EXPRESSION:
                operands[op_index] = p_node->p_exp->Calc();
                delete p_node->p_exp;
                op_index ++;
                break;

            case E_NODE_TYPE::OPERAND:
                operands[op_index] = p_node->i_val;
                op_index ++;
                break;

            case E_NODE_TYPE::OPERATOR:
                if (op_index != 2)
                {
                    cout << "Input was wrong" << endl;
                    return -1;
                }

                switch (p_node->c_op)
                {
                case '+':
                    
                case '-':
                case '*':
                case '\\':
                }
                break;

            default:
                break;
            }

            delete p_node;
        }
    }

    void push(RPN_STACK_NODE* p_node)
    {
        p_node->next = m_stack_top;
        m_stack_top = p_node;
    }

    RPN_STACK_NODE* pop()
    {
        // 注意：pop出来的node是一个指针，使用后务必记得delete it
        if (m_stack_top)
        {
            RPN_STACK_NODE* p_next = m_stack_top->next;
            m_stack_top = p_next;
            return p_next;
        }

        return NULL;
    }

    /**************************************************************************
     * extract ()中的全部内容，包括可能的嵌套 ()
     * 输入：input - 当然正在处理的输入
     *      len - 剩余待处理子串长度
     * 返回：extracted出的字符数目（包括最后的‘)‘)
     *      input[0-i]正好包含有()内的全部子串，去除input[0]并置input[i] = ‘\0’, 
     *      就得到了()内的expression了
     **************************************************************************/
    int get_exp(char* input, int len)
    {
        int i = 0;
        int j = 0;          // '('的数目
        // int len = strlen(input);
        
        assert(input[i] == '(');        // 调用这个function的前提

        i ++;    
        j ++;

        for(;i < len; i ++)
        {
            if (input[i] == '(') j ++;
            else if (input[i] == ')') j --;
            if (j == 0) break;
        }

        if (i < len)
            return i;
        else
            return -1;
    }

    /**************************************************************************
     * extract 完整输入字符串，可能包含字母或者非operator, '('和')'字符
     * 输入：input - 当然正在处理的输入
     *      len - 剩余待处理子串长度
     *      buff - 复制了input中的操作数(除operator, ' '和'()'外的字符, 以'\0'结束
     * 返回：extracted出的字符数目
     **************************************************************************/
    int get_operand(char* input, int len, char buff[])
    {
        int i = 0;
        bool b_end = false;
        // int len = strlen(input);
        char delimiters[] = {'+', '-', '*', '\\', '(', ')', ' '};
        for (; i < len; i ++)
        {
            for (int j = 0; j < sizeof(delimiters); j ++)
            {
                if (input[i]  == delimiters[j])
                {
                    b_end = true;
                    break;
                }
            }
            if (b_end)
            {
                buff[i] = '\0';
                break;
            }
            else
                buff[i] = input[i];
        }

        if (i < len)
            return i - 1;       // 以便保存input中的operator等特殊字符
        else
            return -1;
    }

    int get_node(char* input, int len, RPN_STACK_NODE* &p_node)
    {
        E_NODE_TYPE type;
        p_node = NULL;
        int i = 0;
        int j = 0;

        while(input[i] == ' ') i ++;        // 去除input前面的' '

        if ((input[i] == '\0') || (i >= len))
            return -1;

        switch(input[i])
        {
            case '(':
                j = get_exp(&(input[i]), len - i);
                if (j == -1)
                    return -1;
                input[i + j] = '\0';        // 把extract出来的子串用'\0'结束，构成完整子串
                p_node = new RPN_STACK_NODE();
                p_node->type = E_NODE_TYPE::EXPRESSION;
                p_node->p_exp = new CRpnExp(&(input[i + 1]));   // 从[i + 1]开始，去除开始的'('
                break;

            case '+':
            case '-':
            case '*':
            case '\\':
                p_node = new RPN_STACK_NODE();
                p_node->type = E_NODE_TYPE::OPERATOR;
                p_node->c_op = input[i];
                i ++;
                break;
            
            default:
                char buff[64];
                j = get_operand(&(input[i]), len - i, buff);
                if (j == -1)
                    return -1;
                p_node = new RPN_STACK_NODE();
                p_node->type = E_NODE_TYPE::OPERAND;
                p_node->i_val = atoi(buff);
        }

        return i + j;
    }
};