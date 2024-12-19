#include<stdio.h>
struct D//定义一个分数的结构体
{
    int a,b;//a代表分子，b代表分母
}x,y;//定义两个数字
int main()
{
    scanf("%d/%d %d/%d",&x.a,&x.b,&y.a,&y.b);//读取分数，格式类似  3/5 2/7
    if(1.0*x.a/x.b>1.0*y.a/y.b) printf("x is larger than y");//x比y大
    else if(1.0*x.a/x.b<1.0*y.a/y.b) printf("y is larger than x");//y比x大
    else printf("x is smae as y");//两者一样大
    return 0;
}