#include<iostream>
#include<cstring>
#include<fstream>
#include<string>
using namespace std;

static int sequence_txt = 0;

struct LinkNode{
    int data;
    int sequence_Node;      // 这个东西通常叫time stamp
    LinkNode* next;
    LinkNode():next(NULL){};
    LinkNode(int d):data(d),next(NULL){};
};

class LinkStack{
public:
    LinkStack(){
        // sequence_txt=0;              // static变量只需初始化一次就可以了
        head=new LinkNode();            // 如果在constructor中分配内存空间，则需要在destructor中delete这些空间
                                        // 虽然目前的C++ runtime可以做内存回收了
    }

    ~LinkStack()
    {
        while(head)
        {
            LinkNode* p_next = head->next;
            delete head;
            head = p_next;
        }
    }

    LinkNode* head;

    void push(/*LinkStack &A,*/int x){
        sequence_txt++;
        LinkNode*p =new LinkNode(x);
        p->sequence_Node=sequence_txt;
        // p->next=head->next;
        // head->next=p;
        p->next = head;         // push进来的node将成为新的head, 所以它的next就是原来的head
        head = p;
    }

    int pop(/*LinkStack &A*/){
        LinkNode*p;
        p=head->next;
        // int e=p->data;
        int e = head->data;     // pop出来的节点是原来的head，此时的的p已经指向了原来head的next，也就是新的head
        // head->next=p->next;
        // delete p;
        delete head;            // 要delete的是原来head，而不是新的head（原head的next）
        head = p;               // 原来head的next(p)自然就是新的head了
        return e;
    }
    
//    void merge(/*LinkStack &A,*/ LinkStack &B){
/*        LinkNode* pB=B.head->next;

        while(pB!=NULL){
            LinkNode* pA= head;                 // A.head;
            while(pA->next!=NULL && pB->sequence_Node<pA->sequence_Node && pB->sequence_Node>(pA->next)->sequence_Node){
                pA=pA->next;
            }
            
            LinkNode* temp = pB->next;
            pB->next = pA->next;
            pA->next = pB;
            pB = temp;
        }

        B.head->next = NULL; // Clear stack B
    }
*/

/* 这么复杂的逻辑判断，通常还是分开判断比较好，太多的逻辑组合容易出错 */
/***********************************************
我先用自然语言写一个merge函数
void merge(LinkStack &B)
{
    如果B是空栈，返回(A保持不变);
    如果A是空栈
    {
        // A就是B，下面的代码也可以称为伪码
        A.head = B.head;
        B.head = NULL;
        返回；
    }

    下面将对按照sequence的顺序把B stack插入到A stack
    把A stack的插入点置为A head
    把B stack的待插入点置为B head
    先把B stack中比A stack更早的节点加入到A stack的头部（如果有话)，并更新插入点和待插入点
    while(B stack还有待插入节点)
    {
        把B head节点添加到A的合适的位置(A(n).sequence <= B.sequence < A(n+1).sequence)
        需要说明的是，插入的B节点应该是一个片段
        如果A stack到底（next == NULL)，直接把A tail->next置为当前B节点，break/返回;
        // 你的代码本身没有错，只是没有判断A stack到底怎么办
        如果B stack的待插入节点的sequence大于A stack head的sequence，则把这个节点设为新的A stack head\
        即插入到A stack的head部，同时A stack的插入点保持不变（还是原来的head）
        移动A stack到next A(n)
        移动B stack到next
    }

    B stack清空；
}
************************************************/

/* 下面是C++ 代码*/
    void merge(LinkStack* pB)           // 能够用指针的地方，最好用指针
    {
        // B stack是空的
        if (pB->head == NULL) return;

        // A stack是空的
        if (head == NULL)
        {
            head = pB->head;
            pB->head = NULL;
            return;
        }

        // B stack节点插入到A stack的A0和A1之间
        LinkNode* pA0 = head;           // B stack的节点插入到这个节点之后
        LinkNode* pA1 = head;           // B stack的节点插入到这个节点之前
        // pBStart和pBEnd定义插入A stack A0-A1之间的片段
        LinkNode* pBStart = pB->head;   // B stack开始插入的节点
        LinkNode* pBEnd = NULL;         // B stack下次开始插入的节点
        LinkNode* pBNext = pBStart;

        if (pBNext->sequence_Node > head->sequence_Node)
        {
            pBStart = pBNext;
            pBEnd = pBNext;
            pBNext = pBNext->next;
            while (pBNext && (pBNext->sequence_Node > head->sequence_Node))
            {
                pBEnd = pBNext;
                pBNext = pBNext->next;
                // if (pBNext == NULL) break;
            }
            pBEnd->next = head;
            head = pBStart;
            pA0 = pBEnd;
        }

        while (pA1 && pBNext)
        {
            // 定位A0, A1
            while (pBNext->sequence_Node <= pA1->sequence_Node)
            {
                pA0 = pA1;
                pA1 = pA1->next;

                if (pA1->next == NULL)
                {
                    pA1->next = pBNext;
                    pB->head = NULL;
                    return;
                }
            }

            pBStart = pBNext;
            pBEnd = pBNext;
            pBNext = pBNext->next;
            while (pBNext && (pBNext->sequence_Node > pA1->sequence_Node))
            {
                pBEnd = pBNext;
                pBNext = pBNext->next;
            }
            pA0->next = pBStart;
            pBEnd->next = pA1;
        }

        pB->head = NULL;
    }
};

int main()
{
    LinkStack A=LinkStack(); 
    LinkStack B=LinkStack(); 

    string file_name("in.txt");
    fstream fs_in(file_name, fstream::in);
    if(!fs_in.is_open())
    {
        cout << file_name << " does not exist.\n";
        return -1;
    }

    int n;
    fs_in >> n;
    fs_in.ignore(256, '\n'); // Ignore the newline character after reading n

    while (n > 0)
    {
        string instruction;
        getline(fs_in, instruction);

        if (instruction.substr(0, 4) == "push")
        {
            char stack_name = instruction[5];
            int x = stoi(instruction.substr(7));
            if (stack_name == 'A')
                A.push(/* A, */x);
            else if (stack_name == 'B')
                B.push(/*B, */x);
        }
        else if (instruction.substr(0, 3) == "pop")
        {
            char stack_name = instruction[4];
            int popped_element;
            if (stack_name == 'A')
                popped_element = A.pop(/*A*/);
            else if (stack_name == 'B')
                popped_element = B.pop(/*B*/);
            
            cout << popped_element << endl;
        }
        else if (instruction.substr(0, 5) == "merge")
        {
            // A.merge(/*A, */B);
            A.merge(&B);
        }

        n--;
    }

    fs_in.close();

    return 0;
}