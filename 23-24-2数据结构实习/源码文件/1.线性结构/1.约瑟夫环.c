#include<stdio.h>
#include<stdlib.h>//因为要使用malloc，所以需要include<stdlib.h>
typedef struct List
{
    int data;
    struct List *next;
}List;//创建链表的结点结构题
void insert(List *L,int x)//使用头插法插入数字x
{
    List *new_node=(List *)malloc(sizeof(List));//申请一个空间，插入链表的表头
    new_node->data=x;//先用
    new_node->next=L->next;//后改
    L->next=new_node;//修改头指针指向的位置
}
void fun(List *L, int m)
{
    List *p=L->next, *last=L;
    while (p!=p->next)// 仅剩一个节点时停止
    {  
        for (int i = 1; i < m; ++i)
        {
            last=p;//保存上一个结点的位置，方便后面修改next指针
            p=p->next;//向后扫描一个结点
        }
        last->next=p->next;
        if(p==L->next) L->next=p->next;//处理一个特殊情况，如果表头所指向的元素发生改变
        free(p);//释放内存，节省内存占用
        p=last->next;//下一个结点
    }
}
int main()
{
    List *head=(List *)malloc(sizeof(List));//申请一个单链表
    head->next=NULL;//将头节点的next域赋值为NULL，方便后面扫描单链表的表尾
    int n,m;
    scanf("%d%d",&n,&m);
    for(int i=n;i>=1;--i) insert(head,i);//倒着将数据使用头插法将数据插入单链表
    List *p=head->next;//遍历单链表
    while(p->next!=NULL) p=p->next;//访问链表中的最后一个元素
    p->next=head->next;//将单链表的尾结点连上头节点所连接的结点，创建循环链表
    fun(head,m);//执行删除操作
    printf("%d",head->next->data);//输出结果
    return 0;
}