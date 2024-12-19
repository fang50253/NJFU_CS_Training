#include<stdio.h>
bool is_prime(int x)//编写函数is_prime，判断一个数字是否是质数
{
    for(int i=2;i<=x/i;++i)
    //把条件写成i<=x/i而不是i*i<=x，是防止i*i爆int类型
    //把条件写成i<=x/i而不是i<=sqrt(x)，是为了加快程序的运行速度
        if(x%i==0) return 0;
    return 1;//如果所有的i都不是x的因子，那么说明他是一个质数
}
int main()
{
    int st,cnt;
    scanf("%d%d",&st,&cnt);
    for(int i=st+1;cnt;++i)//尝试判断数字i是否是质数
    {
        if(is_prime(i))//如果是质数，打印，再把cnt输出的总字符数-1
        {
            printf("%d ",i);
            --cnt;
        }
    }
    return 0;//程序结束，别忘了返回0
}