#include<stdio.h>
void swap(int *a,int *b)
{
    int tmp=*a;*a=*b;*b=tmp;
}
void quick_sort(int q[],int l,int r)
{
    if(l>=r) return;
    int mid=q[l+r>>1];
    int i=l-1,j=r+1;
    while(i<j)
    {
        do ++i; while(q[i]<mid);
        do --j; while(q[j]>mid);
        if(i<j) swap(&q[i],&q[j]);
    }
    quick_sort(q,l,i-1);
    quick_sort(q,j+1,r);
}
int main()
{
    int n,num[100];
    scanf("%d",&n);
    for(int i=0;i<n;++i) scanf("%d",&num[i]);
    quick_sort(num,0,n-1);
    for(int i=0;i<n;++i) printf("%d ",num[i]);
    return 0;
}