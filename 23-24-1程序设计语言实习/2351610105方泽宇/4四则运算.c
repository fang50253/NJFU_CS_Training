//@右手fang  2024-07-27
//程序设计语言实习 4四则运算
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
const int MAX=800;
int num1[810],num2[810],idx=0;//定义num1，num2，sign数组，用于输出判重
char sign[810];
int get_random()//获取0-800的随机数
{
    int random=rand()%(MAX+1);
    return random;
}
char get_sign()
{
    int random=rand()%4;//通过1-4这样的随机数，来获取随机的+-*/
    switch(random)
    {
        case 0:return '+';//如果为0则为+
        case 1:return '-';//如果为1则为-
        case 2:return '*';//如果为2则为*
        case 3:return '/';//如果为3则为/
    }
}
void get_problem(FILE *fp)
{
    int tmp1=get_random();//获取第一个数字
    int tmp2=get_random();//获取第二个数字
    char tmp_sign=get_sign();//获取随机的符号
    int ans;//计算结果
    if(tmp_sign=='+') ans=tmp1+tmp2;
    else if(tmp_sign=='-') ans=tmp1-tmp2;
    else if(tmp_sign=='*') ans=tmp1*tmp2;
    else
    {
        int tmp=tmp1/tmp2;
        if(1.0*tmp1/tmp2-tmp<=0.0001) ans=tmp;//判断是否整除，使用浮点数和整型相除的差判断是否整除
        else
        {
            get_problem(fp);//如果发现没有整除，那么递归重新获取一个题目
            return;
        }
    }
    if(ans>MAX||ans<0)
    {
        get_problem(fp);//如果发现所求出的数字不在范围内，则递归重新获取一个题目
        return;
    }
    for(int i=1;i<=idx;++i)
    {
        if(tmp1==num1[i]&&tmp2==num2[i]&&tmp_sign==sign[i])
        {
            get_problem(fp);//如果发现随机生成的题目和原先生成的题目完全一致，则重新生成一个题目
            return;
        }
    }
    ++idx;//这是一个符合要求的新题目，那么编号+1
    num1[idx]=tmp1;
    num2[idx]=tmp2;
    sign[idx]=tmp_sign;
    fprintf(fp, "%d:%d%c%d=%d\n",idx,tmp1,tmp_sign,tmp2,ans);//将题目写入文件
    return;
}
int main()
{
    srand(time(NULL));//生成随机数的种子
    FILE *fp=fopen("4_operations","w+");//打开一个文件
    for(int i=1;i<=50;++i) get_problem(fp);//每一次执行get_problem()函数，获取一个题目
    fclose("4_operations");//关闭文件
    return 0;
}
//这段代码是会生成乘法的，但是因为随机数生成的乘法，结果在800以内的过少，可能需要放大输出量才能看到乘法
//这段代码也是会生成除法的，但是需要前一个数字是后一个数字的概率很低，所以需要放大输出量