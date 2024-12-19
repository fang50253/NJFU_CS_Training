#include<stdio.h>
int cnt(int x)
{
    int res=1,i;//res初始化为1是因为，1一定是小于x的x的因子，但是x/1==x不是，需要特判
    for(i=2;i<x/i;++i)
        if(x%i==0) res+=i+x/i;
    if(i*i==x) res+=i;//特判i是因为，i的平方如果正好是x，那么会被加两遍，所以需要特判
    return res;//返回结果
}
int main()
{
    for(int i=2;i<=1000;++i)//已知2不是完数，遍历剩余的2-1000所有的数字，进行搜索
        if(cnt(i)==i) printf("%d ",i);//输出结果
    return 0;
}