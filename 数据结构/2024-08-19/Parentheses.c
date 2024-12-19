#include<stdio.h>
#define StackElementType char
#define maxsize 100
#include"Stack.h"
int match(char str1,char str2)
{
    if(str1=='('&&str2==')') return 1;
    if(str1=='['&&str2==']') return 1;
    if(str1=='{'&&str2=='}') return 1;
    return 0;
}
void BracketMatch(char str[])
{
    SeqStack s;
    int i;
    char ch;
    InitStack(&s);
    for(i=0;str[i]!='\0';++i)
    {
        switch(str[i])
        {
            case '(':
            case '[':
            case '{':
                Stack_Push(&s,str[i]);break;
            case ')':
            case ']':
            case '}':
                if(Stack_IsEmpty(&s)) 
                    {printf("右括号多余\n");return;}
                else
                {
                    Stack_GetTop(&s,&ch);
                    if(match(ch,str[i])) Stack_Pop(&s,&ch);
                    else {printf("括号类型不匹配\n");return;}
                }
                break;
        }
    }
    if (Stack_IsEmpty(&s)) printf("括号匹配成功\n");
    else printf("左括号多余\n");
}
int main()
{
    char str[100];
    scanf("%s",str);
    BracketMatch(str);
    return 0;
}