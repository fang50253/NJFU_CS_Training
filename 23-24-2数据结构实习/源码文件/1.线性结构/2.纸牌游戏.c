#include<stdio.h>
int st[53];
int main()
{
    for(int i=2;i<=52;++i)//以1-52为基数
        for(int j=1;i*j<=52;++j)//遇到基数的整数倍就翻牌一次
            ++st[i*j];//记录翻牌的次数
    for(int i=1;i<=52;++i)//检查被翻牌的牌
        if(st[i]%2==0)//如果被翻了偶数次，那么还是正面朝上
            printf("%d ",i);//输出结果
    return 0;//程序结束
}