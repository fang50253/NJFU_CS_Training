#include<stdio.h>
#define ElemType int
#define ERROR 1
#define maxsize 110
//#define CLS "cls"//win
#define CLS "clear"//macos/linux
typedef struct
{
    ElemType elem[maxsize];//静态申请顺序表所需要的空间
    int last;
}SeqList;
SeqList List;
void Init_List(SeqList *L);
void Visit(SeqList *L);
void Create(SeqList *L,int n);
void Insert_List(SeqList *L,int arr,ElemType x);
int Seq_Length(SeqList *L);
void Reserve_List(SeqList *L,int l,int r);
void Sort_List(SeqList *L,int l,int r);
void Merge_List(SeqList *a,SeqList *b,SeqList *c);
void swap(ElemType *a,ElemType *b);
void Merge_List(SeqList *a,SeqList *b,SeqList *c);
void Delete_List(SeqList *L,int arr);
void Merge_Sort_List(SeqList *a,SeqList *b,SeqList *c);
void swap(ElemType *a,ElemType *b)
{
    ElemType tmp=*a;
    *a=*b;
    *b=tmp;
}
void Init_List(SeqList *L)//初始化操作
{
    L->last=-1;
}
void Visit(SeqList *L)//按顺序输出顺序表
{
    if(L->last==-1)
    {
        printf("Empty_array\n");
        return;
    }
    for(int i=0;i<=L->last;++i) printf("%d ",L->elem[i]);
    printf("\n");
}
void Create(SeqList *L,int n)//创建一个单链表，并使用尾插法输入n个数字
{
    if(n>maxsize)
    {
        printf("Failed to create the array,the num is out of the size\n");
    }
    Init_List(L);
    //初始化链表L
    for(int i=0;i<n;++i)
    {
        //依次输入数据，并将其添加至顺序表尾部
        ElemType input;
        scanf("%d",&input);
        Insert_List(L,i-1,input);
    }
}
void Insert_List(SeqList *L,int arr,ElemType x)//在顺序表L中，索引号arr的数字之后插入数据元素x
{
    //第一步，移动索引号为arr+1到last到元素，使其索引号+1
    if(L->last==maxsize)
    {
        //处理静态顺序表满的异常
        printf("Error,the array is full\n");
        return;
    }
    else ++L->last;
    for(int i=L->last;i>=arr+1;--i) L->elem[i+1]=L->elem[i];
    //第二步，将x添加到顺序表的arr结点当中
    L->elem[arr+1]=x;
}
int Seq_Length(SeqList *L)//返回顺序表L的长度
{
    return L->last+1;
}
void Reserve_List(SeqList *L,int l,int r)//对顺序表L中，索引号为l到r的项进行逆序操作
{
    if(l>r) printf("Error,the num L is larger than R\n");
    while(l<r)
    {
        swap(&(L->elem[l]),&(L->elem[r]));
        ++l,--r;
    }
}
void Sort_List(SeqList *L,int l,int r)//将顺序表中,对索引号l到r的项从小到大排序
{
    if(l>=r) return;
    int arr=L->elem[l+r>>1];
    int i=l-1,j=r+1;
    while(i<j)
    {
        do ++i; while(L->elem[i]<arr);
        do --j; while(L->elem[j]>arr);
        if(i<j) swap(&(L->elem[i]),&(L->elem[j]));
    }
    Sort_List(L,l,j);
    Sort_List(L,j+1,r);
}
void Merge_List(SeqList *a,SeqList *b,SeqList *c)//将a和b两个顺序表合并，并将答案存入c中
{
    int i=0,j=0,k=0;
    for(int i=0;i<=a->last;++i,++k) Insert_List(c,k-1,a->elem[i]);
    for(int i=0;i<=b->last;++i,++k) Insert_List(c,k-1,b->elem[i]);
}
void Merge_Sort_List(SeqList *a,SeqList *b,SeqList *c)//合并两个有序的顺序表
{
    int i=0,j=0,k=0;
    while(i<=a->last&&j<=b->last)
    {
        if(a->elem[i]<b->elem[j])
        {
            Insert_List(c,k-1,a->elem[i]);
            ++i;
        }
        else
        {
            Insert_List(c,k-1,b->elem[j]);
            ++j;
        }
        ++k;
    }
    while(i<=a->last)
    {
        Insert_List(c,k-1,a->elem[i]);
        ++i,++k;
    }
    while(j<=b->last)
    {
        Insert_List(c,k-1,b->elem[j]);
        ++j,++k;
    }
}
void Delete_List(SeqList *L,int arr)//删除顺序表L中索引号为arr的元素
{
    for(int i=arr+1;i<=L->last;++i) L->elem[i-1]=L->elem[i];
    --L->last;
}
void fun()
{
    //system(CLS);
    printf("1.在顺序表中插入数组元素\n");
    printf("2.删除数组中的元素\n");
    printf("3.求顺序表的长度\n");
    printf("4.顺序表的逆序\n");
    printf("5.将顺序表中的值从小到大排序\n");
    printf("6.合并两个顺序表\n");
    printf("7.合并两个有序的顺序表\n");
    printf("8.输出L\n");
    printf("9.使用尾插法一次性向List输入数据\n");
    int ch;
    scanf("%d",&ch);
    if(ch==1)
    {
        printf("2个参数：在顺序表L中，索引号arr的数字之后插入数据元素x\n");
        int arr;
        ElemType x;
        scanf("%d%d",&arr,&x);
        Insert_List(&List,arr,x);
    }
    else if(ch==2)
    {
        printf("1个参数：在顺序表L中，删除索引号为arr的元素\n");
        int arr;
        scanf("%d",&arr);
        Delete_List(&List,arr);
    }
    else if(ch==3)
    {
        printf("0个参数\n");
        printf("顺序表L的长度为：%d\n",Seq_Length(&List));
    }
    else if(ch==4)
    {
        printf("2个参数：需要逆序部分索引号的左边界和右边界\n");
        int l,r;
        scanf("%d%d",&l,&r);
        Reserve_List(&List,l,r);
    }
    else if(ch==5)
    {
        printf("2个参数：需要排序的索引号左边界和右边界\n");
        int l,r;
        scanf("%d%d",&l,&r);
        Sort_List(&List,l,r);
    }
    else if(ch==6)
    {
        SeqList a,b;
        printf("4个参数：请先输入顺序表a的长度，再依次输入a中的值；再输入顺序表b的长度，再依次输入b中的值\n");
        int len;
        scanf("%d",&len);
        Create(&a,len);
        scanf("%d",&len);
        Create(&b,len);
        Init_List(&List);
        Merge_List(&a,&b,&List);
    }
    else if(ch==7)
    {
        SeqList a,b;
        printf("4个参数：请先输入顺序表a的长度，再依次输入a中的值；再输入顺序表b的长度，再依次输入b中的值\n");
        int len;
        scanf("%d",&len);
        Create(&a,len);
        scanf("%d",&len);
        Create(&b,len);
        Init_List(&List);
        Merge_Sort_List(&a,&b,&List);
    }
    else if(ch==8)
    {
        Visit(&List);
    }
    else if(ch==9)
    {
        printf("2个参数：输入需要创建顺序表的表长，以及顺序表的每个数字\n");
        int n;
        scanf("%d",&n);
        Create(&List,n);
    }
}
int main()
{
    Init_List(&List);
    while(1) fun();
    return 0;
}
/*
在顺序表中插入数据元素
删除数据元素
求顺序表的长度
顺序表的逆置
顺序表的按值从小到大排序
合并有序的两个顺序表*/