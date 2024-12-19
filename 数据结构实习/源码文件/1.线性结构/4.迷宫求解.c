#include<stdio.h>
const int MAXSIZE=100;
//用一个全局的常量去定义地图的最大大小
int map[MAXSIZE][MAXSIZE],n=0x3f3f3f3f,m=0x3f3f3f3f;
//定义一个地图，最大的大小为100*100
const int dx[4]={0,0,1,-1};
const int dy[4]={1,-1,0,0};
//定义两个向量，方便以后想下一步走
typedef struct Node
{
    int x,y;
}Node;
//为每一个点的坐标定义一个结点类型
typedef struct Queue
{
    Node data[MAXSIZE*MAXSIZE];
    int front,rear;
}Queue;
//定义一个队列，用于存放广度优先搜索(bfs)
typedef struct Stack
{
    Node data[MAXSIZE*MAXSIZE];
    int top;
}Stack;
//定义一个栈，用于逆序输出路径
int empty(Queue *que)
{
    if(que->rear==que->front) return 1;
    else return 0;
}
//判断队列是否为空(用处：如果队列为空说明bfs结束)
Node pop(Queue *que)
{
    int tmp=que->front;
    //保存临时变量，指向需要输出的结点的位置
    que->front=(que->front+1)%MAXSIZE;
    //front指针向前移动
    return que->data[tmp];
}
//将队列中的最前面的一个元素返回，并且删除掉这个元素
void push(Queue *que,Node *x)
{
    que->data[que->rear].x=x->x;
    que->data[que->rear].y=x->y;
    //将结点的两个域分别更新
    que->rear=(que->rear+1)%MAXSIZE;
    //rear指针后移
}
int in_map(Node *x)
{
    if(x->x<=n&&x->x>=1&&x->y<=m&&x->y>=1) return 1;
    //如果没有超出地图边界，那么返回1，否则返回0
    else return 0;
}
void bfs(Queue *que)
//这是一个广度优先搜索遍历的算法，当然我们也可以对这张图进行深度优先搜索
//但是bfs搜索的是最短路径，这道题我只输出一种最短路径
{
    Node search=pop(que);
    //从队列里取出一个结点
    for(int i=0;i<4;++i)
    {
        Node new_node;
        new_node.x=search.x+dx[i];
        new_node.y=search.y+dy[i];
        //使用向量，更新待搜索的结点
        if(in_map(&new_node))
        //如果没有超出边界
        {
            if(map[new_node.x][new_node.y]!=0) continue;
            //如果这个地方是障碍物，或者已经更新过到达这个点的最短路径，那么直接continue
            map[new_node.x][new_node.y]=map[search.x][search.y]+1;
            //如果满足要求，则这个点到(1,1)点的最短路径就是其上一个结点+1
            push(que,&new_node);
            //将这个结点送入队列中
        }
    }
}
void print()
{
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=m;++j)
        {
            printf("%d ",map[i][j]);
            //依次输出每一个点到(1,1)的最短路，输出-1代表该点无法到达(1,1)
        }
        printf("\n");
    }
}
void way()
{
    Stack stack;
    //定义一个栈
    //因为bfs到终点以后，可以倒着推出路径，但是正着无法推出，所以需要用一个栈来将结果逆序
    stack.top=-1;
    //初始化栈
    int step=map[n][m];
    //将最终步数放入临时变量step中，上一次走过的结点的步数一定是step-1
    Node node;
    //创建一个新的结点
    node.x=n;node.y=m;
    stack.data[++stack.top]=node;
    //将结果放入栈中
    int x=n,y=m;
    while(!(x==1&&y==1))
    {
        for(int i=0;i<4;++i)
        {
            node.x=x+dx[i];
            node.y=y+dy[i];
            //通过向量更新出上一步有可能走过的点
            if(in_map(&node))
            //这个点是否在地图内
            {
                if(map[node.x][node.y]==step-1)
                //如果步数正好是step-1，则说明这是路径上的一点
                {
                    stack.data[++stack.top]=node;
                    //将这个点加入栈
                    x=node.x;
                    y=node.y;
                    --step;
                    //循环，继续寻找上一个结点
                    break;
                }
            }
        }
    }
    printf("依次经过\n");
    //输出结果
    while(stack.top>=0)
    //依次出栈
    {
        printf("%d %d\n",stack.data[stack.top].x,stack.data[stack.top].y);
        --stack.top;
    }
}
int main()
{
    while(n>99||m>99) scanf("%d%d",&n,&m);
    //输入地图的大小，保证输入的大小是在一个合法的范围内，防止超出地图的边界
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=m;++j)
        {
            int x;
            scanf("%d",&x);
            if(x==1) map[i][j]=-1;
            //输入地图，0代表可以走，1代表是墙壁，但是1和第一步冲突，为了防止冲突，读入数据以后改成-1，代表走不了
        }
    }
    if(map[1][1]==-1)
    //如果起点为-1，墙壁
    {
        printf("输入数据错误");
        //出错返回
        return 0;
    }
    Node dot;
    //新建一个点，这个点是原点
    Queue que;
    //新建一个队列，用于存储bfs序列
    que.front=que.rear=0;
    //初始化队列，方式segmentation fault
    dot.x=dot.y=1;
    push(&que,&dot);
    //将原点推入队列中
    map[1][1]=1;
    while(!empty(&que)) bfs(&que);
    if(map[n][m]==-1||map[n][m]==0)
    {
        printf("无路径\n");
        //最后一个点无法到达，出错返回
        return 0;
    }
    //print();
    way();
    return 0;
}