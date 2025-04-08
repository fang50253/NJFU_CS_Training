//顺序栈和循环队列的基本运算
// 1.实现顺序栈的初始化
// 2.顺序栈的判空
// 3.顺序栈的入栈
// 4.顺序栈的出栈
// 5.读取栈顶元素
// 6.基于顺序栈实现表达式或者文本括号匹配的检验运算


//构建循环队列类型
// 1.实现循环队列的初始化
// 2.循环队列的判空
// 3.循环队列的入队
// 4.循环队列的出队
// 5.读取队头元素
// 6.读取队尾元素内容
// 7.基于循环队列实现杨辉三角形N行数据输出

// @右手fang   2024-08-19

#include<stdio.h>
#define maxsize 100
#define StackElementType int
#include"Stack.h"
SeqStack s;
void menu()
{
    printf("1.实现顺序栈的初始化\n2.顺序栈的判空\n3.顺序栈的入栈\n4.顺序栈的出栈\n5.读取栈顶元素\n6.基于顺序栈实现表达式或者文本括号匹配的检验运算\n");
    int op;
    scanf("%d",&op);
    if(op==1) InitStack(&s);
    else if(op==2)
    {
        if(Stack_IsEmpty(&s)) printf("The stack is empty\n");
        else printf("It is not an empty stack\n");
    }
    else if(op==3)
    {
        if(Stack_IsFull(&s))
        {
            printf("You cannot push number into the stack\n");
            return;
        }
        printf("Input the number that you want to push\n");
        int x;
        scanf("%d",&x);
        Stack_Push(&s,x);
    }
    else if(op==4)
    {
        int x;
        if(Stack_Pop(&s,&x)) printf("OK\n");
        else printf("Oops!The stack is empty\n");
    }
    else if(op==5)
    {
        int x;
        if(Stack_GetTop(&s,&x)) printf("The top of the stack is:%d\n",x);
        else printf("Oops!The stack is empty\n");
    }
}
int main()
{
    while(1) menu();
    return 0;
}