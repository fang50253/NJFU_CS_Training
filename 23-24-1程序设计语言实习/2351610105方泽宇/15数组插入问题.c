#include<stdio.h>
int arr[11];
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
    for(int i=0;i<10;++i) scanf("%d",&arr[i]);
    sort(arr,0,9);
    for(int i=0;i<10;++i) printf("%d ",arr[i]);
    printf("\n");
    scanf("%d",&arr[10]);
    sort(arr,0,10);
    for(int i=0;i<11;++i) printf("%d ",arr[i]);
    return 0;
}