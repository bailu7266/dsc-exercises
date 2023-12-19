#include <iostream>
#include <string>
#include <sstream> 
#include <fstream>
#include <stack>
#include <queue>

using namespace std;

#define BUFF_SIZE 128

struct BT_NODE
{
    struct BT_NODE* left;
    struct BT_NODE* right;

    enum BTN_TYPE
    {
        OPTR = 0,   // operator
        OPRD        // operand
    } type;

    union
    {
        char op;
        int val;
    };

    BT_NODE(int _val) : left(NULL), right(NULL), type(OPRD), val(_val) {};
    BT_NODE(char _op) : left(NULL), right(NULL), type(OPTR), op(_op) {};
};

using BTN_PTR = BT_NODE*;
using FP_VISIT = void (*)(BTN_PTR, void**);

inline void free_node(BTN_PTR pn, void** _np = NULL)
{
    delete pn;
}

class CBinTreeExp
{
    BT_NODE* root;

public:
    CBinTreeExp(string exp)
    {
        root = build(exp.c_str());
    }

    ~CBinTreeExp()
    {
        if (root)
            // free_tree(root);
            dfs_postorder(root, free_node);
    }

    void free_tree(BTN_PTR const p_node)
    {
        // 不能采用preorder traversal，因为需要最后才释放root，所以采用postorder
        // 当然也不能采用inorder，因为root被释放后，right节点就无法访问了
        if (p_node->left)
        {
            free_tree(p_node->left);
            p_node->left = NULL;
        }

        if (p_node->right)
        {
            free_tree(p_node->right);
            p_node->right = NULL;
        }

        free(p_node);
    }

    BTN_PTR build(const char exp[])
    {
        stack<char> st_op;
        stack<BTN_PTR> st_node;
        BTN_PTR p_node;
        string postexp;
        string str_val;

        int i = 0;
        bool b_end = false;
        char ch;
        char op;

        while(i < strlen(exp))
        {
            char ch = exp[i];
            switch(ch)
            {
            case '(':       //左括号直接入栈
                st_op.push(ch);
                break;

            case ')':       //右括号弹出左括号及之前的所有符号
                while(!st_op.empty()) {
                    op = st_op.top();
                    if (op == '(') {
                        st_op.pop();     // 应该在此pop '('
                        break;
                    }
                    else {
                        postexp += op;
                        p_node = new BT_NODE(op);
                        st_op.pop();
                        p_node->right = st_node.top();
                        st_node.pop();
                        p_node->left = st_node.top();
                        st_node.pop();
                        st_node.push(p_node);
                    }
                }
                break;

            case '+':
            case '-':
                while(!st_op.empty())
                {      // pop '('前所有的operator
                    op = st_op.top();
                    if (op == '(')
                    {
                        break;
                    }
                    else {
                        postexp += op;

                        p_node = new BT_NODE(op);
                        st_op.pop();
                        p_node->right = st_node.top();
                        st_node.pop();
                        p_node->left = st_node.top();
                        st_node.pop();
                        st_node.push(p_node);
                    }
                }
                st_op.push(ch);
                break;
            
            case '*':
            case '/':
                while(!st_op.empty()) {
                    op = st_op.top();
                    if ((op =='(') || (op == '+') || (op == '-')) 
                        break;
                    else {    // pop 同级operators
                        postexp += op;

                        p_node = new BT_NODE(op);
                        st_op.pop();
                        p_node->right = st_node.top();
                        st_node.pop();
                        p_node->left = st_node.top();
                        st_node.pop();
                        st_node.push(p_node);
                    }
                }
                st_op.push(ch);
                break;

            case ' ':               // 增加对 ' ' 的处理: 忽略
                break;

            case '=':
                b_end = true;
                break;

            default:
                // operand (integer)
                // string d = "";
                str_val.clear();
                while(i < strlen(exp))
                {
                    ch = exp[i];
                    if (ch >= '0' && ch <= '9') {
                        // d += ch;
                        postexp += ch;
                        str_val += ch;
                        i ++;
                    }
                    else break;
                }

                i --;
                // postexp += d;
                postexp += "#";
                st_node.push(new BT_NODE(stoi(str_val)));
            }

            if (b_end) break;           // 忽略 '=' 后面的输入

            i ++;
        }

        while(!st_op.empty()) {
            op = st_op.top();

            postexp += op;

            p_node = new BT_NODE(op);
            st_op.pop();
            p_node->right = st_node.top();
            st_node.pop();
            p_node->left = st_node.top();
            st_node.pop();
            st_node.push(p_node);
        }

        cout << "Postfix expression: " << postexp << endl;
        
        return st_node.top();
    }

    void preorder_traversal(FP_VISIT visit, void** pp)
    {
        if (root) dfs_preorder(root, visit, pp);
    }

    void postorder_traversal(FP_VISIT visit, void** pp)
    {
        if (root) dfs_postorder(root, visit, pp);
    }

    void inorder_traversal(FP_VISIT visit, void** pp)
    {
        if (root) dfs_inorder(root, visit, pp);
    }

    void levelorder_traversal(FP_VISIT visit, void** pp)
    {
        if (root) bfs_traversal(root, visit, pp);
    }

private:
    void __visit(BTN_PTR p_node, void** pp = NULL)
    {

    }

    void dfs_preorder(BTN_PTR p_node, FP_VISIT visit, void** pp = NULL)
    {
        visit(p_node, pp);

        if (p_node->left)
        {
            dfs_preorder(p_node->left, visit, pp);
        }
    
        if (p_node->right)
        {
            dfs_preorder(p_node->right, visit, pp);
        }
    }

    void dfs_postorder(BTN_PTR p_node, FP_VISIT visit, void** pp = NULL)
    {
        if (p_node->left)
        {
            dfs_postorder(p_node->left, visit, pp);
        }
    
        if (p_node->right)
        {
            dfs_postorder(p_node->right, visit, pp);
        }

        visit(p_node, pp);
    }

    void dfs_inorder(BTN_PTR p_node, FP_VISIT visit, void** pp = NULL)
    {
        if (p_node->left)
        {
            dfs_inorder(p_node->left, visit, pp);
        }

        visit(p_node, pp);

        if (p_node->right)
        {
            dfs_inorder(p_node->right, visit, pp);
        }
    }

    void bfs_traversal(BTN_PTR root, FP_VISIT visit, void** pp = NULL)
    {
        queue<BTN_PTR> qn;

        qn.push(root);

        while(!qn.empty())
        {
            BTN_PTR cur = qn.front();
            qn.pop();
            visit(cur, pp);

            if (cur->left) qn.push(cur->left);
            if (cur->right) qn.push(cur->right);
        }
    }
};

void visit_print(BTN_PTR p_node, void** pp)
{
    char* pc = (char*) *pp;
    if (p_node->type == BT_NODE::OPTR)
    {
        *pc = p_node->op;
        pc ++;
        *pc = '\0';
        *pp = pc;
    }
    else if (p_node->type == BT_NODE::OPRD)
    {
        int len = snprintf(pc, BUFF_SIZE, "%d#" , p_node->val);
        pc += len;
        *pp = pc;
    }
}

int calc(BTN_PTR p_node)
{
    if (p_node->type == BT_NODE::OPRD)
        return p_node->val;

    int lv = calc(p_node->left);
    int rv = calc(p_node->right);

    switch(p_node->op)
    {
        case '+':
            return lv + rv;

        case '-':
            return lv - rv;

        case '*':
            return lv * rv;

        case '/':
            return lv / rv;

        default:
            cout << "Unknown operator: " << p_node->op << endl;
            return 0;
    }
}

void visit_eval(BTN_PTR p_node, void** pp)
{
    *((int*) pp) = calc(p_node);
}

int main()
{
    string fin_name = "in.txt";
    istream& is_in = cin;
    ifstream fs_in(fin_name, ifstream::in);
    if (fs_in.is_open())
    {
        cout << "Input the expression from file: " << fin_name << endl;
        is_in.rdbuf(fs_in.rdbuf());
    }
    else
    {
        cout << "Please input the expression:" << endl;
    }

    string input;
    getline(is_in, input);

    if (fs_in.is_open())
        fs_in.close();

    CBinTreeExp bt(input);

    char buff[BUFF_SIZE];
    char* tp = buff;
    bt.preorder_traversal(visit_print, (void**) &tp);
    cout << buff << endl;
    tp = buff;
    bt.inorder_traversal(visit_print, (void**) &tp);
    cout << buff << endl;
    tp = buff;
    bt.postorder_traversal(visit_print, (void**) &tp);
    cout << buff << endl;
    tp = buff;
    bt.levelorder_traversal(visit_print, (void**) &tp);
    cout << buff << endl;

    int result;
    bt.postorder_traversal(visit_eval, (void**) &result);
    cout << result << endl;
}