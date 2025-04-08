#include<stdio.h>
const int N=110;
int n,m,a[N],b[N],merge[2*N];//定义数组a，b，用于存放归并之前的值，merge用于存放归并以后的值
void swap(int *x, int *y)//定义swap函数用于交换两个值，使用指针传入地址
{
    int tmp = *x;
    *x = *y;
    *y = tmp;
}
void sort(int q[],int l,int r)//定义快速排序函数sort
{
    if(l>=r) return;//递归的终止条件：l>=r
    int i=l-1,j=r+1,x=q[l+r>>1];//x是快速排序的基准值
    while (i<j)//移动双指针，让基准值左边全部小于x，右边全部大于x
    {
        do i++; while(q[i]<x);
        do j--; while(q[j]>x);
        if(i<j) swap(&q[i],&q[j]);//交换
    }
    sort(q,l,j);//递归进行下一层排序
    sort(q,j+1,r);
}
int main()
{
    scanf("%d%d",&n,&m);//输入元素个数
    for(int i=0;i<m;++i) scanf("%d",&a[i]);//输入数组a的值
    for(int i=0;i<n;++i) scanf("%d",&b[i]);//输入数组b的值
    sort(a,0,m-1);//对a数组排序
    sort(b,0,n-1);//对b数组排序
    int x=0,y=0,idx=0;//归并前的准备
    while(x<m&&y<n)//在其中一个走到底之前，归并a，b到merge中
    {
        if(a[x]<b[y]) merge[idx++]=a[x++];
        else merge[idx++]=b[y++]; 
    }
    while(x<m) merge[idx++]=a[x++];//归并剩下的a数组元素到merge中
    while(y<n) merge[idx++]=b[y++];//归并剩下的b数组元素到merge中
    for(int i=0;i<idx;++i) printf("%d ",merge[i]);//输出归并以后的结果
    return 0;
}