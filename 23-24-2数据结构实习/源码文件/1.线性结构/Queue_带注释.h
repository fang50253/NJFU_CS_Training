// 循环队列类型定义
typedef struct
{
    QueueElementType elem[maxsize]; 
    // 存储队列元素的数组
    int rear; 
    // 队尾指针，指向下一个插入的位置
    int front; 
    // 队头指针，指向下一个删除的位置
} SeqQueue;
// 初始化循环队列
void InitQueue(SeqQueue *q) 
{
    q->front = 0; 
    // 队头指针初始值
    q->rear = 0; 
    // 队尾指针初始值
}
// 判断循环队列是否为空
int Queue_IsEmpty(SeqQueue *q)
{
    return (q->front == q->rear); 
    // 如果队头指针等于队尾指针，队列为空
}

// 判断循环队列是否已满
int Queue_IsFull(SeqQueue *q)
{
    return ((q->rear + 1) % maxsize == q->front); 
    // 如果队尾指针加1后的位置等于队头指针，队列满
}
// 入队操作，将元素e插入到循环队列中
int Queue_Push(SeqQueue *q, QueueElementType e) 
{
    if (Queue_IsFull(q)) return 0; 
    // 如果队列已满，插入失败
    q->elem[q->rear] = e; 
    // 将元素e插入到队尾
    q->rear = (q->rear + 1) % maxsize; 
    // 更新队尾指针，采用模运算实现循环
    return 1; // 插入成功
}
// 出队操作，从循环队列中删除元素，并将其赋值给e
int Queue_Pop(SeqQueue *q, QueueElementType *e) 
{
    if (Queue_IsEmpty(q)) return 0; 
    // 如果队列为空，删除失败
    *e = q->elem[q->front]; 
    // 获取队头元素
    q->front = (q->front + 1) % maxsize; 
    // 更新队头指针，采用模运算实现循环
    return 1; // 删除成功
}
// 读取队头元素，将其赋值给e，但不删除
int Queue_Front(SeqQueue *q, QueueElementType *e) 
{
    if (Queue_IsEmpty(q)) return 0; 
    // 如果队列为空，读取失败
    *e = q->elem[q->front]; 
    // 获取队头元素
    return 1; // 读取成功
}
// 读取队尾元素，将其赋值给e，但不删除
int Queue_Back(SeqQueue *q, QueueElementType *e) 
{
    if (Queue_IsEmpty(q)) return 0; 
    // 如果队列为空，读取失败
    *e = q->elem[(q->rear - 1 + maxsize) % maxsize]; 
    // 获取队尾元素，采用模运算处理负数情况
    return 1; // 读取成功
}