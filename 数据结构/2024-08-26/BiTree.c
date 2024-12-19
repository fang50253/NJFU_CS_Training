//构建二叉树的二叉链表存储结构
//实现二叉树的创建
//二叉树的先序/中序/后序递归遍历
//统计二叉树的高度
//统计各类结点的个数
//先序/中序非递归遍历
//层序遍历等运算
//二叉树中数据元素的类型统一抽象表示为TElemType类型，在程序中将TElemType类型具体化为char类型
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
int max(int a,int b)
{
    return a>b?a:b;
}
BiTree *build()//使用先序遍历创建一棵树
{
    char str;
    str=getchar();
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
int menu()
{
    printf("1.退出\n2.二叉树的先序/中序/后序递归遍历\n3.统计二叉树的高度\n4.统计各类结点的个数\n5.先序/中序非递归遍历\n6.层序遍历等运算\n");
    int op;
    scanf("%d",&op);
    if(op==1) return 0;
    else if(op==2)
    {
        printf("基于递归的先序遍历:");
        pre_recursion(tree);
        printf("\n基于递归的中序遍历:");
        mid_recursion(tree);
        printf("\n基于递归的后序遍历:");
        post_recursion(tree);
        printf("\n");
    }
    else if(op==3) printf("树的高度为:%d\n",height(tree));
    else if(op==4)
    {
        int c[3];
        memset(c,0,sizeof c);
        cnt(tree,c);
        printf("入度为0:%d\n入度为1:%d\n入度为2:%d\n",c[0],c[1],c[2]);
    }
    else if(op==5)
    {
        printf("基于非递归的先序遍历:");
        Preorder(tree);
        printf("\n基于非递归的中序遍历:");
        Inorder(tree);
        printf("\n");
    }
    else if(op==6)
    {
        printf("层序遍历:");
        Levelorder(tree);
        printf("\n");
    }
    return 1;
}
int main()
{
    tree=build();
    int v=1;
    while(v) v=menu();
    return 0;
}