#include<stdio.h>
int gcd(int a,int b)
{
    return b?gcd(b,a%b):a;//使用辗转相除法计算最大公因数
}
int lcm(int a,int b)
{
    return a/gcd(a,b)*b;//这里写成a/gcd(a,b)*b而不是a*b/gcd(a,b)是为了防止爆int
}
int main()
{
    int a,b;
    scanf("%d%d",&a,&b);//读入两个数字，计算最大公因数和最小公倍数
    printf("%d %d",gcd(a,b),lcm(a,b));//调用函数，输出结果
    return 0;
}