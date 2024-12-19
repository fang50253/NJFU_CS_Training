#include<stdio.h>
int get_pow(int x)//计算一个数字的四次方
{
    x*=x;
    return x*x;
}
int get_left(int a,int b,int c,int d)//将数字的每一位数的四次方数相加
{
    return get_pow(a)+get_pow(b)+get_pow(c)+get_pow(d);
}
int get_right(int a,int b,int c,int d)//将四位数字直接组合
{
    return a*1000+b*100+c*10+d;
}
int main()
{
    for(int a=1;a<=9;++a)//a从1开始，确保了这个数字一定是一个四位数，如果从0开始的话，这个数字有可能会生成出一个三位数
    {
        for(int b=0;b<=9;++b)
        {
            for(int c=0;c<=9;++c)
            {
                for(int d=0;d<=9;++d)
                {
                    int left=get_left(a,b,c,d);
                    int right=get_right(a,b,c,d);//这里将数字进行了组合
                    if(left==right) printf("%d ",left);//比较两个数字的大小，如果相同，那么就将这个数字输出
                }
            }
        }
    }
    return 0;
}