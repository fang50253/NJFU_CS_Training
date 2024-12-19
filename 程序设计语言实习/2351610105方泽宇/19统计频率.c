#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>//引入字符类型判断头文件
int digit,others,alpha,blank;//初始化每一个桶
int main()
{
    FILE *fp=fopen("input","r");//以只读形式打开一个文件
    char input;//读取字符
    while((input=fgetc(fp))!=EOF)//逐个读取字符，直到文件结束
    {
        if(isdigit(input)) {++digit;continue;}//数字
        if(isalpha(input)) {++alpha;continue;}//字母
        if(input==' ') {++blank;continue;}//空格
        ++others;//其他
    }
    printf("%d %d %d %d",alpha,digit,blank,others);//输出统计个数
    fclose(fp);//关闭文件
    return 0;
}