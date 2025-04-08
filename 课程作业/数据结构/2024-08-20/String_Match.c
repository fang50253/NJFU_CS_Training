//构建串的定长顺序存储结构；实现串的创建，串的访问输出；实现模式串和主串的简单匹配算法和kmp模式匹配算法。
#include<stdio.h>
#include<string.h>
#define maxlength 100
typedef struct 
{
    char ch[maxlength];
    int len;
}SString;
void get_next(SString *str,int nxt[])
{
    int i=1,j=0;
    nxt[0]=-1;nxt[1]=0;
    while(i<str->len)
    {
        if(j==-1||str->ch[i]==str->ch[j]) {++i;++j;nxt[i]=j;}
        else j=nxt[j];
    }
}
int KMP_Pair(SString *str,SString *substr,int nxt[])
{
    int i=0,j=0;
    while(i<str->len)
    {
        if(str->ch[i]==substr->ch[j]) ++i,++j;
        else j=nxt[j];
        if(j==substr->len) return i-j+1;
    }
    return -1;
}
int Easy_Pair(SString *str,SString *substr)
{
    int i=0,j=0;
    while(i<str->len&&j<substr->len)
    {
        if(str->ch[i]==substr->ch[j]) ++i,++j;
        else {i=i-j+1;j=0;}
        if(j==substr->len) return i-j+1;
    }
    return -1;
}
int get_str(SString *str)
{
    scanf("%s",str->ch);
    str->len=strlen(str->ch);
    return 1;
}
void print_str(SString *str)
{
    printf("%s",str->ch);
}
int main()
{
    int nxt[100];
    SString str,substr;
    int ch;
    printf("1.简单模式匹配\n2.KMP模式匹配\n3.输入一个串并输出\n");
    scanf("%d",&ch);
    if(ch==1)
    {
        printf("Input the main string\n");
        get_str(&str);
        printf("Input the sub str\n");
        get_str(&substr);
        int ans=Easy_Pair(&str,&substr);
        if(ans==-1) printf("Error\n");
        else printf("The start of the string is %d",ans);
    }
    else if(ch==2)
    {
        printf("Input the main string\n");
        get_str(&str);
        printf("Input the sub str\n");
        get_str(&substr);
        int nxt[100];
        get_next(&substr,nxt);
        int ans=KMP_Pair(&str,&substr,nxt);
        if(ans==-1) printf("Error\n");
        else printf("The start of the string is %d",ans);
    }
    else if(ch==3)
    {
        get_str(&str);
        print_str(&str);
    }
    return 0;
}
