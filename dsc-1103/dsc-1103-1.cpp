#include<iostream>
#include<stack>
#include<string>

using namespace std;

class Express{
    string exp;
    string postexp;
public:
    Express(string str){
        exp=str;
        postexp="";
    }

    string getpostexp(){
        return postexp;
    }

    void Trans() {
        stack<char> opor;
        int i = 0;
        bool b_end = false;
        char ch,e;

        while(i < exp.length())
        {
            ch = exp[i];
            switch(ch)
            {
            case '(':       //左括号直接入栈
                opor.push(ch);
                break;

            case ')':       //右括号弹出左括号及之前的所有符号
                while(!opor.empty()) {
                    e = opor.top();
                    if (e == '(') {
                        opor.pop();     // 应该在此pop '('
                        break;
                    }
                    else {
                        opor.pop();
                        postexp += e;
                    }
                }
                // opor.pop();
                break;

            case '+':
            case '-':
                while(!opor.empty()) {      // pop '('前所有的operator
                    e = opor.top();
                    if (e == '(')
                    {
                        break;
                    }
                    else {
                        opor.pop();
                        postexp += e;
                    }
                }
                opor.push(ch);
                break;
            
            case '*':
            case '/':
                while(!opor.empty()) {
                    e = opor.top();
                    if ((e =='(') || (e == '+') || (e == '-')) 
                        break;
                    else {    // pop 同级operators
                        // e=opor.top();
                        opor.pop();
                        postexp += e;
                    }
                }
                opor.push(ch);
                break;

            case ' ':               // 增加对 ' ' 的处理: 忽略
                break;

            case '=':
                b_end = true;
                break;

            default:
                // string d = "";
                while(i < exp.length()) {
                    ch = exp[i];
                    if (ch >= '0' && ch <= '9') {
                        // d += ch;
                        postexp += ch;
                        i ++;
                    }
                    else break;
                }

                i --;
                // postexp += d;
                postexp += "#";
            }

            if (b_end) break;           // 忽略 '=' 后面的输入

            i ++;
        }

        while(!opor.empty()) {
            e = opor.top();
            opor.pop();
            postexp += e;
        }
    }

    double GetValue(){
        stack<double> opand;
        double a, b, c, d;
        char ch;
        int i= 0;
        while(i<postexp.length()){
            ch=postexp[i];
            switch(ch){
                case'+':
                    a=opand.top();
                    opand.pop();
                    b=opand.top();
                    opand.pop();
                    c=a+b;
                    opand.push(c);
                    break;
                case'-':
                    a=opand.top();
                    opand.pop();
                    b=opand.top();
                    opand.pop();
                    c=b-a;
                    opand.push(c);
                    break;
                case'*':
                    a=opand.top();
                    opand.pop();
                    b=opand.top();
                    opand.pop();
                    c=b*a;
                    opand.push(c);
                    break;
                case'/':
                    a=opand.top();
                    opand.pop();
                    b=opand.top();
                    opand.pop();
                    c=b/a;
                    opand.push(c);
                    break;
                default:
                    d=0;
                    while(ch>='0'&&ch<='9'){
                        d=10*d+(ch-'0');
                        i++;
                        ch=postexp[i];
                    }
                    opand.push(d);
                    break;
            }
            i++;
        }
        return opand.top();
    }
};

int main(){
    string str;
    //string str="2*   (3+  4) -2/ (1+1)  =";
    // cin>>str;
    getline(cin, str);
    Express obj(str);
    obj.Trans();
    cout << obj.getpostexp() << endl;
    cout << obj.GetValue() << endl;
    return 0;
}