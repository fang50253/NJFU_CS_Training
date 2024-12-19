#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define maxsize 100
#define TElemType char
typedef struct BiTree
{
    TElemType data;
    struct BiTree *lchild,*rchild;
}BiTree;
#define StackElementType BiTree*
#define QueueElementType BiTree*
#include"queue.h"
#include"stack.h"
BiTree *tree;
SeqStack s;
int max(int a,int b)
{
    return a>b?a:b;
}
BiTree *build()//使用先序遍历创建一棵树
{
    char str;
    do str = getchar();while(str=='\n'); 
    if(str=='#') return NULL; 
    BiTree *new_node=(BiTree*)malloc(sizeof(BiTree));
    if(new_node==NULL)
    {
        printf("申请空间失败");
        return NULL;
    }
    new_node->data=str;
    new_node->lchild=build();
    new_node->rchild=build();
    return new_node;
}
void pre_recursion(BiTree *tree)//先序遍历tree
{
    if(tree==NULL) return;
    printf("%c",tree->data);
    pre_recursion(tree->lchild);
    pre_recursion(tree->rchild);
}
void mid_recursion(BiTree *tree)//中序遍历tree
{
    if(tree==NULL) return;
    mid_recursion(tree->lchild);
    printf("%c",tree->data);
    mid_recursion(tree->rchild);
}
void post_recursion(BiTree *tree)//后序遍历tree
{
    if(tree==NULL) return;
    post_recursion(tree->lchild);
    post_recursion(tree->rchild);
    printf("%c",tree->data);
}
int height(BiTree *tree)//统计二叉树的高度
{
    if(tree==NULL) return 0;
    return max(height(tree->lchild),height(tree->rchild))+1;
}
void cnt(BiTree *tree,int t[])
{
    if(tree->lchild==tree->rchild&&tree->lchild==NULL) ++t[0];
    else if(tree->rchild!=NULL&&tree->lchild!=NULL) ++t[2];
    else ++t[1];
    if(tree->lchild!=NULL) cnt(tree->lchild,t);
    if(tree->rchild!=NULL) cnt(tree->rchild,t);
}
void Inorder(BiTree *root)//中序遍历
{
    SeqStack stack;
    InitStack(&stack);
    BiTree *p=root;
    while(p!=NULL||!Stack_IsEmpty(&stack))
    {
        if(p!=NULL)
        {
            Stack_Push(&stack,p);
            p=p->lchild;
        }
        else 
        {
            Stack_Pop(&stack,&p);
            printf("%c",p->data);
            p=p->rchild;
        }
    }
}
int dfs(BiTree *root, char x)
{
    if (root == NULL) return -1;
    if (x == root->data)
    {
        Stack_Push(&s, root);
        return 1;
    }
    if (dfs(root->lchild, x)== 1)
    {
        Stack_Push(&s, root);
        return 1;
    }
    if (dfs(root->rchild, x)==1)
    {
        Stack_Push(&s, root);
        return 1;
    }
    return -1;
}
int width(BiTree *root)
{
    if(root==NULL) return 0;
    else if(root->lchild==NULL&&root->rchild==NULL) return 1;
    else return width(root->lchild)+width(root->rchild);
}
void Preorder(BiTree *root)//先序遍历
{
    SeqStack stack;
    InitStack(&stack);
    BiTree *p=root;
    while(p!=NULL||!Stack_IsEmpty(&stack))
    {
        if(p!=NULL)
        {
            printf("%c",p->data);
            Stack_Push(&stack,p);
            p=p->lchild;
        }
        else 
        {
            Stack_Pop(&stack,&p);
            p=p->rchild;
        }
    }
}
void Levelorder(BiTree *root)//层序遍历
{
    SeqQueue queue;
    InitQueue(&queue);
    BiTree *p=root;
    Queue_Push(&queue,root);
    while(!Queue_IsEmpty(&queue))
    {
        Queue_Pop(&queue,&p);
        if(p!=NULL)
        {
            
            printf("%c",p->data);
            Queue_Push(&queue,p->lchild);
            Queue_Push(&queue,p->rchild);
        }
    }
}

//1. 建立二叉树
//2. 遍历二叉树
//3. 二叉树的高度、宽度
//4. 各类结点的个数
//5. 根结点到指定结点的路径
//0. 退出
int menu()
{
    printf("1.建立二叉树\n2.遍历二叉树\n3.二叉树的高度、宽度\n4.统计二叉树中各类结点的个数\n5.根结点到指定结点到路径\n6.退出\n");
    int op;
    scanf("%d",&op);
    if(op==1) tree=build();
    else if(op==2)
    {
        printf("递归先序遍历：");
        pre_recursion(tree);
        printf("\n递归中序遍历：");
        mid_recursion(tree);
        printf("\n递归后序遍历：");
        post_recursion(tree);
        printf("\n非递归先序遍历：");
        Preorder(tree);
        printf("\n非递归中序遍历：");
        Inorder(tree);
        printf("\n");
    }
    else if(op==3)
    {
        printf("二叉树高度：%d\n",height(tree));
        printf("二叉树宽度：%d\n",width(tree));
    }
    else if(op==4)
    {
        int c[3]={0};
        cnt(tree,c);
        for(int i=0;i<3;++i)
            printf("度为%d到结点有%d个\n",i,c[i]);
    }
    else if(op==5)
    {
        printf("需要查找到字符：");
        char x;
        scanf("\n%c",&x);
        InitStack(&s);
        int v=dfs(tree,x);
        if(v==-1) printf("未找到\n");
        else
        {
            while(!Stack_IsEmpty(&s))
            {
                BiTree *t;
                Stack_Pop(&s,&t);
                printf("%c",t->data);
            }
            printf("\n");
        }
        
    }
    else if(op==6) return 0;
    return 1;
}
int main()
{
    int v=1;
    while(v) v=menu();
    return 0;
}