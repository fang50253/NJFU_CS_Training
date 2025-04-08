#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define maxsize 100
#define TElemType char
// 将数据元素类型定义为char
typedef struct BiTree
{
    TElemType data;
    // 节点的数据部分
    struct BiTree *lchild,*rchild;
    // 左右孩子指针
}BiTree;
#define StackElementType BiTree*
// 定义栈元素类型为指向BiTree的指针
#define QueueElementType BiTree*
// 定义队列元素类型为指向BiTree的指针
#include"queue.h"
// 引入队列的头文件
#include"stack.h"
// 引入栈的头文件
BiTree *tree;
// 定义全局的树指针
SeqStack s;
// 定义全局的栈
int max(int a,int b)// 返回两个数中的最大值
{
    return a>b?a:b;
}
BiTree *build()//使用先序遍历创建一棵树
{
    char str;
    do str = getchar();while(str=='\n'); 
    // 跳过换行符
    if(str=='#') return NULL;
    // 结束条件，返回NULL表示空节点
    BiTree *new_node=(BiTree*)malloc(sizeof(BiTree));
    // 分配新节点空间
    if(new_node==NULL)
    {
        printf("申请空间失败");
        // 如果分配失败，输出错误信息
        return NULL;
    }
    new_node->data=str;
    // 设置节点数据
    new_node->lchild=build();
    // 递归构建左子树
    new_node->rchild=build();
    // 递归构建右子树
    return new_node;
    // 返回构建好的节点
}
void pre_recursion(BiTree *tree)//先序遍历tree
{
    if(tree==NULL) return;
    // 如果当前节点为空，直接返回
    printf("%c",tree->data);
    // 输出当前节点数据
    pre_recursion(tree->lchild);
    // 递归遍历左子树
    pre_recursion(tree->rchild);
    // 递归遍历右子树
}
void mid_recursion(BiTree *tree)//中序遍历tree
{
    if(tree==NULL) return;
    // 如果当前节点为空，直接返回
    mid_recursion(tree->lchild);
    // 递归遍历左子树
    printf("%c",tree->data);
    // 输出当前节点数据
    mid_recursion(tree->rchild);
    // 递归遍历右子树
}
void post_recursion(BiTree *tree)//后序遍历tree
{
    if(tree==NULL) return;
    // 如果当前节点为空，直接返回
    post_recursion(tree->lchild);
    // 递归遍历左子树
    post_recursion(tree->rchild);
    // 递归遍历右子树
    printf("%c",tree->data);
    // 输出当前节点数据
}
int height(BiTree *tree)//统计二叉树的高度
{
    if(tree==NULL) return 0;
    // 如果树为空，高度为0
    return max(height(tree->lchild),height(tree->rchild))+1;
    // 返回左、右子树中高度较大的值+1
}
void cnt(BiTree *tree,int t[])
{
    if(tree->lchild==tree->rchild&&tree->lchild==NULL) ++t[0];
    // 如果节点是叶子节点，叶子节点计数加1
    else if(tree->rchild!=NULL&&tree->lchild!=NULL) ++t[2];
    // 如果节点有两个子节点，度为2的节点计数加1
    else ++t[1];  // 如果节点有一个子节点，度为1的节点计数加1
    if(tree->lchild!=NULL) cnt(tree->lchild,t);
    // 递归统计左子树
    if(tree->rchild!=NULL) cnt(tree->rchild,t);
    // 递归统计右子树
}
void Inorder(BiTree *root)//中序遍历
{
    SeqStack stack;
    InitStack(&stack);// 初始化栈
    BiTree *p=root;
    while(p!=NULL||!Stack_IsEmpty(&stack))
    {
        if(p!=NULL)
        {
            Stack_Push(&stack,p);
            // 当前节点不为空时，入栈
            p=p->lchild;
            // 移动到左子节点
        }
        else 
        {
            Stack_Pop(&stack,&p);
            // 当前节点为空时，出栈
            printf("%c",p->data);
            // 输出节点数据
            p=p->rchild;
            // 移动到右子节点
        }
    }
}
int dfs(BiTree *root, char x)// 深度优先搜索路径
{
    if (root == NULL) return -1;
    // 如果当前节点为空，返回-1表示未找到
    if (x == root->data)
    {
        Stack_Push(&s, root);
        // 找到目标节点，入栈
        return 1;  // 返回1表示找到
    }
    if (dfs(root->lchild, x)== 1)
    {
        Stack_Push(&s, root);
        // 在左子树中找到目标节点，当前节点入栈
        return 1;  // 返回1表示找到
    }
    if (dfs(root->rchild, x)==1)
    {
        Stack_Push(&s, root);
        // 在右子树中找到目标节点，当前节点入栈
        return 1;  // 返回1表示找到
    }
    return -1;  // 未找到，返回-1
}
int width(BiTree *root)
{
    if (root == NULL) return 0;
    // 定义一个队列来进行层序遍历
    SeqQueue queue;
    InitQueue(&queue);
    Queue_Push(&queue, root);
    int max_width = 0;
    while (!Queue_IsEmpty(&queue))
    {
        // 当前层的节点数
        int level_size = (queue.rear - queue.front + maxsize) % maxsize;
        if (level_size > max_width) max_width = level_size;  // 更新最大宽度
        // 遍历当前层的所有节点
        for (int i = 0; i < level_size; i++)
        {
            BiTree *node;
            Queue_Pop(&queue, &node);
            // 将子节点加入队列
            if (node->lchild != NULL) Queue_Push(&queue, node->lchild);
            if (node->rchild != NULL) Queue_Push(&queue, node->rchild);
        }
    }
    return max_width;  // 返回最大宽度
}
void Preorder(BiTree *root)//先序遍历
{
    SeqStack stack;
    InitStack(&stack);// 初始化栈
    BiTree *p=root;
    while(p!=NULL||!Stack_IsEmpty(&stack))
    {
        if(p!=NULL)
        {
            printf("%c",p->data);
            // 输出当前节点数据
            Stack_Push(&stack,p);
            // 当前节点入栈
            p=p->lchild;  // 移动到左子节点
        }
        else 
        {
            Stack_Pop(&stack,&p);
            // 当前节点为空时，出栈
            p=p->rchild;  // 移动到右子节点
        }
    }
}
void Levelorder(BiTree *root)//层序遍历
{
    SeqQueue queue;
    InitQueue(&queue);
    // 初始化队列
    BiTree *p=root;
    Queue_Push(&queue,root);
    // 根节点入队
    while(!Queue_IsEmpty(&queue))
    {
        Queue_Pop(&queue,&p);  // 出队
        if(p!=NULL)
        {
            printf("%c",p->data);  // 输出当前节点数据
            Queue_Push(&queue,p->lchild);
            // 左子节点入队
            Queue_Push(&queue,p->rchild);
            // 右子节点入队
        }
    }
}

//1. 建立二叉树
//2. 遍历二叉树
//3. 二叉树的高度、宽度
//4. 各类结点的个数
//5. 根结点到指定结点的路径
//0. 退出
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
    scanf("%d",&op);  // 获取用户选择的操作
    if(op==1) tree=build();  // 选项1：创建二叉树
    else if(op==2)
    {
        printf("递归先序遍历：");
        pre_recursion(tree);  // 递归实现先序遍历
        printf("\n递归中序遍历：");
        mid_recursion(tree);  // 递归实现中序遍历
        printf("\n递归后序遍历：");
        post_recursion(tree);  // 递归实现后序遍历
        printf("\n非递归先序遍历：");
        Preorder(tree);  // 非递归实现先序遍历
        printf("\n非递归中序遍历：");
        Inorder(tree);  // 非递归实现中序遍历
        printf("\n");
    }
    else if(op==3)
    {
        printf("二叉树高度：%d\n",height(tree));
        // 输出二叉树的高度
        printf("二叉树宽度：%d\n",width(tree));
        // 输出二叉树的宽度
    }
    else if(op==4)
    {
        int c[3]={0};  // 初始化节点计数数组
        cnt(tree,c);  // 统计各类节点的个数
        for(int i=0;i<3;++i)
            printf("度为%d的结点有%d个\n",i,c[i]);
            // 输出度为0、1、2的节点个数
    }
    else if(op==5)
    {
        printf("需要查找的字符：");
        char x;
        scanf("\n%c",&x);  // 获取用户输入的字符
        InitStack(&s);  // 初始化栈，用于存储路径
        int v=dfs(tree,x);  // 深度优先搜索目标节点的路径
        if(v==-1) printf("未找到\n");
        // 如果未找到，输出提示信息
        else
        {
            while(!Stack_IsEmpty(&s))
            {
                BiTree *t;
                Stack_Pop(&s,&t);  // 依次输出路径上的节点
                printf("%c",t->data);
            }
            printf("\n");
        }
    }
    else if(op==6) return 0;  // 选项6：退出程序
    return 1;
}

int main()
{
    int v=1;
    while(v) v=menu();  // 循环显示菜单并处理用户输入
    return 0;
}