//单链表的初始化
//数据的插入
//数据的删除
//输出单链表内中各元素
//求单链表的长度
//实现单链表中数据结点的按值排序
//实现单链表的逆置
//合并两个有序的单链表（有序的a表和有序的b表合并之后的结果保存在a表中）
//Powered By @Zeyu.Fang
#include<stdio.h>
#include<stdlib.h>
#define ElemType int
//#define CLS "cls"//win
#define CLS "clear"//macos/linux
typedef struct Node
{
    ElemType data;
    struct Node *next;
}Node,*LinkList;
Node *List;
void Init_List(Node *L);
void Insert_List(Node *L,int arr,ElemType x);
void Delete_List(Node *L,int arr);
void Visit(Node *L);
int List_Length(Node *L);
void List_Bubble_Sort(Node *L);
void List_Reserve(Node *L);
void Merge_List(Node *a,Node *b);
void swap(ElemType *a,ElemType *b);
void swap(ElemType *a,ElemType *b)
{
    ElemType tmp=*a;
    *a=*b;
    *b=tmp;
}
void Init_List(Node *L)//单链表的初始化
{
    L->next=NULL;
}
void Insert_List(Node *L,int arr,ElemType x)//向单链表L的第arr位后面插入x
{
    Node *p=L;
    int cnt=-1;
    while(p->next!=NULL&&arr!=-1)
    {
        p=p->next;
        ++cnt;
        if(cnt==arr) break;
    }
    if(cnt<arr)
    {
        printf("Insert_Out_of_Array\n");
        return;
    }
    Node *new_node=(Node*)malloc(sizeof(Node));
    new_node->data=x;
    new_node->next=p->next;
    p->next=new_node;
}
void Delete_List(Node *L,int arr)//删除单链表L中索引号为arr的结点
{
    Node *p=L;
    int cnt=0;
    while(p->next!=NULL&&arr!=0)
    {
        p=p->next;
        ++cnt;
        if(cnt==arr) break;
    }
    if(p->next==NULL)
    {
        printf("Delete_Out_of_Array\n");
        return;
    }
    Node *tmp=p->next;
    p->next=p->next->next;
    free(tmp);
}
void Visit(Node *L)//输出单链表中的所有元素
{
    Node *p=L->next;
    while(p!=NULL) 
    {
        printf("%d ",p->data);
        p=p->next;
    }
    printf("\n");
}
int List_Length(Node *L)//返回1以L为头节点的链表的长度
{
    Node *p=L;
    int cnt=0;
    while(p->next!=NULL)
    {
        p=p->next;
        ++cnt;
    }
    return cnt;
}
void List_Bubble_Sort(Node *L)//对链表L进行冒泡排序
{
    Node *p=L;
    int change=1;
    while(change)
    {
        change=0;
        p=L;
        while(p->next->next!=NULL)
        {
            p=p->next;
            if(p->data>p->next->data)
            {
                swap(&(p->data),&(p->next->data));
                change=1;
            }
        }
    }
}
void List_Reserve(Node *L)//实现单链表的逆置
{
    if(!L->next||!(L->next->next)) return;
    Node *p=L->next->next; L->next->next=NULL;
    while(p)
    {
       Node *temp=p->next; 
       p->next=L->next; L->next=p; 
       p=temp; 
    }
}
void Merge_List(Node *a,Node *b)//合并两个有序的单链表（有序的a表和有序的b表合并之后的结果保存在a表中）
{
    Node *pre=a,*p=a->next,*q=b->next;
    while(p&&q)
    {
        if(p->data<q->data) {pre->next=p;pre=p;p=p->next;}
     else  {pre->next=q;pre=q;q=q->next;}
   }
   pre->next=p?p:q;  
   free(b);
}
void Init_Insert(Node *L,int n)
{
    Init_List(L);
    for(int i=1;i<=n;++i)
    {
        int x;
        scanf("%d",&x);
        Insert_List(L,i-2,x);
    }
}
void fun()
{
    printf("1.初始化链表L，并使用尾插法添加n个元素\n");
    printf("2.在链表L的索引号arr后面插入一个数字x\n");
    printf("3.删除链表L中索引号为arr的项\n");
    printf("4.输出单链表L中所有的项\n");
    printf("5.输出单链表L的长度\n");
    printf("6.将单链表L中的值按照升序排序\n");
    printf("7.将单链表中的值逆序\n");
    printf("8.合并两个有序的单链表\n");
    int ch;
    scanf("%d",&ch);
    if(ch==1)
    {
        printf("2个参数：需要添加的元素个数，所有元素\n");
        int n;
        scanf("%d",&n);
        Init_Insert(List,n);
    }
    else if(ch==2)
    {
        printf("2个参数：在索引号arr后面添加元素x\n");
        int arr,x;
        scanf("%d%d",&arr,&x);
        Insert_List(List,arr,x);
    }
    else if(ch==3)
    {
        printf("1个参数：删除元素的索引号arr\n");
        int arr;
        scanf("%d",&arr);
        Delete_List(List,arr);
    }
    else if(ch==4)
    {
        printf("无参数\n");
        Visit(List);
    }
    else if(ch==5)
    {
        printf("无参数\n");
        printf("List的长度为%d\n",List_Length(List));
    }
    else if(ch==6)
    {
        printf("无参数\n");
        List_Bubble_Sort(List);
    }
    else if(ch==7)
    {
        printf("无参数\n");
        List_Reserve(List);
    }
    else if(ch==8)
    {
        printf("2个参数：需要和List归并的升序数组b的长度，以及他的所有元素\n");
        int n;
        scanf("%d",&n);
        Node *b=malloc(sizeof(Node));
        Init_Insert(b,n);
        Merge_List(List,b);
    }
}
int main()
{
    List=malloc(sizeof(Node));
    while(1) fun();
    return 0;
}