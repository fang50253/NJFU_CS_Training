#include<stdio.h>
#include<string.h>
int main()
{
    int n,a[100],b;
    memset(a,0,sizeof(a));
    scanf("%d",&n);
    for(int i=1;i<=n;i++)
    {
        scanf("%d",&b);
        if(b>100||b<0)
        {
            printf("输入数据错误");
            return 0;
        }
        a[b]++;
    }
    for(int i=0;i<=99;i++)
    {
        while(a[i]>0)
        {
            printf("%d ",i);
            a[i]--;
        }
    }
    return 0;
}