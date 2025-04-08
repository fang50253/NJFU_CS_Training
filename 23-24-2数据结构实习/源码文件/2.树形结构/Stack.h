typedef struct
{
    StackElementType elem[maxsize];
    int top;
}SeqStack;
void InitStack(SeqStack *s)//顺序栈的初始化操作
{
    s->top=-1;
}
int Stack_IsEmpty(SeqStack *s)//顺序栈的判空
{
    if(s->top==-1) return 1;
    else return 0;
}
int Stack_IsFull(SeqStack *s)//顺序栈的判满
{
    if(s->top==maxsize-1) return 1;
    else return 0;
}
int Stack_Push(SeqStack *s,StackElementType e)//顺序栈的入栈操作
{
    if(Stack_IsFull(s)) return 0;
    ++s->top;
    s->elem[s->top]=e;
    return 1;
}
int Stack_Pop(SeqStack *s,StackElementType *e)//顺序栈的出栈操作
{
    if(Stack_IsEmpty(s)) return 0;
    *e=s->elem[s->top--];
    return 1;
}
int Stack_GetTop(SeqStack *s,StackElementType *e)//读取栈顶元素
{
    if(Stack_IsEmpty(s)) return 0;
    *e=s->elem[s->top];
    return 1;
}