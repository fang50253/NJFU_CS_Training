//构建循环队列类型
// 1.实现循环队列的初始化
// 2.循环队列的判空
// 3.循环队列的入队
// 4.循环队列的出队
// 5.读取队头元素
// 6.读取队尾元素内容
// 7.基于循环队列实现杨辉三角形N行数据输出

// @右手fang   2024-08-19
typedef struct 
{
    QueueElementType elem[maxsize];
    int rear;
    int front;
}SeqQueue;
void InitQueue(SeqQueue *q)//循环链表的初始化操作
{
    q->front=0;
    q->rear=0;
}
int Queue_IsEmpty(SeqQueue *q)//循环队列的判空
{
    if(q->front==q->rear) return 1;
    else return 0;
}
int Queue_IsFull(SeqQueue *q)
{
    return (q->rear+1)%maxsize==q->rear;
}
int Queue_Push(SeqQueue *q,QueueElementType e)
{
    if(Queue_IsFull(q)) return 0;
    q->elem[q->rear]=e;
    q->rear=(q->rear+1)%maxsize;
    return 1;
}
int Queue_Pop(SeqQueue *q,QueueElementType *e)
{
    if(Queue_IsEmpty(q)) return 0;
    *e=q->elem[q->front];
    q->front=(q->front+1)%maxsize;
    return 1;
}
int Queue_Front(SeqQueue *q,QueueElementType *e)
{
    if(Queue_IsEmpty(q)) return 0;
    *e=q->elem[q->front];
    return 1;
}
int Queue_Back(SeqQueue *q,QueueElementType *e)
{
    if(Queue_IsEmpty(q)) return 0;
    *e=q->elem[(q->rear-1+maxsize)%maxsize];
    return 1;
}