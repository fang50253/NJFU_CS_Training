#include<stdio.h>
#define maxsize 100//表可能达到的最大长度
#define ElemType int
typedef struct
{
    ElemType elem[maxsize];//静态申请
    int last;
}SeqList;
void InitList(SeqList *l)//初始化
{
    l->last=-1;
}
void Visit(SeqList *l)//输出顺序表
{
    int i;
    if(l->last==-1) printf("Empty\n");
    else
    {
        for(int i=0;i<=l->last;++i) printf("%d ",l->elem[i]);
    }
}
int Create(SeqList *L,int n)
//创建了一个长度为n的顺序表
{
    if(n<0) {printf("0");return 0;}
    L->last=n-1;
    for(int i=0;i<n;++i) scanf("%d",&(L->elem[i]));
    return 1;
}
int  Locate(SeqList L,ElemType e)//按照值查找元素
{
    int i=0;      
    while((i<=L.last)&&(L.elem[i]!=e)) i++;
    if(i<=L.last) return i+1;  //若找到值为e的元素，则返回其序号
    else  return 0;  //若没找到，则返回0
}
int  InsList(SeqList *L,int i,ElemType e)//插入元素
{ 
    if((i<1)||i>L->last+2) {printf("error\n");return 0;}
    if(L->last>=maxsize-1) {printf("full_list\n");return 0;}
    for(int k=L->last;k>=i-1;k--)//移动元素
       L->elem[k+1]=L->elem[k]; 
    L->elem[i-1]=e;
    L->last++; 
    return 1;
}
int  DelList(SeqList *L,int i,ElemType *e)//删除
//将被删除元素的值写入e，在插入或者删除元素的时候平均需要移动顺序表的一半元素
{
   if((i<1)||(i>L->last+1)) {printf("error del");return 0;}
   *e= L->elem[i-1]; 
   for(int k=i;k<=L->last;k++) L->elem[k-1]=L->elem[k];//移动元素
   L->last--;
   return 1;
}
void Inverse(SeqList *l)//逆序
{
    int i,j;
    ElemType tmp;
    for(i=0,j=l->last;i<j;++i,--j)
    {
        tmp=l->elem[i];
        l->elem[i]=l->elem[j];
        l->elem[j]=tmp;
    }
}