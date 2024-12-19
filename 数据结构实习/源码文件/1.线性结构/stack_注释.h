// 顺序栈类型定义
typedef struct
{
    StackElementType elem[maxsize];
    // 存储栈元素的数组
    int top;
    // 栈顶指针，指向栈顶元素的位置
} SeqStack;
// 初始化顺序栈
void InitStack(SeqStack *s)
{
    s->top = -1; 
    // 初始化栈顶指针为-1，表示栈为空
}
// 判断顺序栈是否为空
int Stack_IsEmpty(SeqStack *s)
{
    return (s->top == -1); 
    // 如果栈顶指针为-1，栈为空
}
// 判断顺序栈是否已满
int Stack_IsFull(SeqStack *s)
{
    return (s->top == maxsize - 1); 
    // 如果栈顶指针等于最大容量减1，栈满
}
// 入栈操作，将元素e压入栈中
int Stack_Push(SeqStack *s, StackElementType e)
{
    if (Stack_IsFull(s)) return 0; 
    // 如果栈已满，入栈失败
    ++s->top; // 栈顶指针加1
    s->elem[s->top] = e; 
    // 将元素e压入栈顶
    return 1; // 入栈成功
}
// 出栈操作，从栈中弹出栈顶元素，并将其赋值给e
int Stack_Pop(SeqStack *s, StackElementType *e)
{
    if (Stack_IsEmpty(s)) return 0; 
    // 如果栈为空，出栈失败
    *e = s->elem[s->top--]; 
    // 获取栈顶元素并将栈顶指针减1
    return 1; // 出栈成功
}
// 读取栈顶元素，但不弹出它
int Stack_GetTop(SeqStack *s, StackElementType *e)
{
    if (Stack_IsEmpty(s)) return 0; 
    // 如果栈为空，读取失败
    *e = s->elem[s->top]; 
    // 将栈顶元素赋值给e
    return 1; // 读取成功
}