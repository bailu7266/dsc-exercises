#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BUFF_SIZE 32

typedef struct __NODE
{
    struct __NODE* parent;
    struct __NODE* left;
    struct __NODE* right;
    char  c_elt;
} NODE;

int eval_tree(NODE* const p_node, char** p_chs, size_t* p_size)
{
    if (*p_size <= 0)
        return 0;

    char ch = **p_chs;
    (*p_chs) ++;
    (*p_size) --;

    switch(ch)
    {
        case '(':
        {
            NODE* p_left = (NODE*) malloc(sizeof(NODE));
            p_left->parent = p_node;
            p_left->left = NULL;
            p_left->right = NULL;
            p_node->left = p_left;
            return eval_tree(p_left, p_chs, p_size);
        }

        case ')':
            return eval_tree(p_node->parent, p_chs, p_size);

        case '\0':
        case '\n':
            // 这样处理是不严谨的: 如果输入不完整的话，就会出错。
            return 0;

        case ',':
        {
            NODE* p_right = (NODE*) malloc(sizeof(NODE));
            p_right->parent = p_node->parent;      // p_node is the left node
            p_right->left = NULL;
            p_right->right = NULL;
            p_node->parent->right = p_right;
            return eval_tree(p_right, p_chs, p_size);
        }

        default:
            p_node->c_elt = ch;

            return 1 + eval_tree(p_node, p_chs, p_size);
    }
}

void preorder_traversal(NODE* const p_node, char** p_chs)
{
    **p_chs = p_node->c_elt;
    (*p_chs) ++;
    **p_chs = '\0';

    if (p_node->left)
    {
        preorder_traversal(p_node->left, p_chs);
    }
    
    if (p_node->right)
    {
        preorder_traversal(p_node->right, p_chs);
    }
}

void postorder_traversal(NODE* const p_node, char** p_chs)
{
    if (p_node->left)
    {
        postorder_traversal(p_node->left, p_chs);
    }
    
    if (p_node->right)
    {
        postorder_traversal(p_node->right, p_chs);
    }

    **p_chs = p_node->c_elt;
    (*p_chs) ++;
    **p_chs = '\0';
}

void inorder_traversal(NODE* const p_node, char** p_chs)
{
    if (p_node->left)
    {
        inorder_traversal(p_node->left, p_chs);
    }

    **p_chs = p_node->c_elt;
    (*p_chs) ++;
    **p_chs = '\0';

    if (p_node->right)
    {
        inorder_traversal(p_node->right, p_chs);
    }
}

void free_tree(NODE* const p_node)
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

int main()
{
    char* fname = "in.txt";
    FILE* pf_in = fopen(fname, "r");
    if (!pf_in)
    {
        printf("Can't open file: %s\n", fname);
        return -1;
    }

    size_t buff_size = BUFF_SIZE;
    char* buff = malloc(buff_size);
    if (!buff)
    {
        printf("memory allocation error\n");
        return -1;
    }

    getline(&buff, &buff_size, pf_in);
    fclose(pf_in);

    NODE* root = (NODE*) malloc(sizeof(NODE));
    root->parent = NULL;
    root->left = NULL;
    root->right = NULL;
   
    // buff_size = BUFF_SIZE;
    int ch_count = eval_tree(root, &buff, &buff_size);
    printf("Input %d nodes totally\n", ch_count);

    char* ps_temp = buff;
    preorder_traversal(root, &ps_temp);
    printf("%s\n", buff);

    ps_temp = buff;
    inorder_traversal(root, &ps_temp);
    printf("%s\n", buff);

    ps_temp = buff;
    postorder_traversal(root, &ps_temp);
    printf("%s\n", buff);

    free_tree(root);
}