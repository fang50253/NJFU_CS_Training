#include<stdio.h>
const int MAXSIZE=100;
int map[MAXSIZE][MAXSIZE],n=0x3f3f3f3f,m=0x3f3f3f3f;
const int dx[4]={0,0,1,-1};
const int dy[4]={1,-1,0,0};
typedef struct Node
{
    int x,y;
}Node;
typedef struct Queue
{
    Node data[MAXSIZE*MAXSIZE];
    int front,rear;
}Queue;
typedef struct Stack
{
    Node data[MAXSIZE*MAXSIZE];
    int top;
}Stack;
int empty(Queue *que)
{
    if(que->rear==que->front) return 1;
    else return 0;
}
Node pop(Queue *que)
{
    int tmp=que->front;
    que->front=(que->front+1)%MAXSIZE;
    return que->data[tmp];
}
void push(Queue *que,Node *x)
{
    que->data[que->rear].x=x->x;
    que->data[que->rear].y=x->y;
    que->rear=(que->rear+1)%MAXSIZE;
}
int in_map(Node *x)
{
    if(x->x<=n&&x->x>=1&&x->y<=m&&x->y>=1) return 1;
    else return 0;
}
void bfs(Queue *que)
{
    Node search=pop(que);
    for(int i=0;i<4;++i)
    {
        Node new_node;
        new_node.x=search.x+dx[i];
        new_node.y=search.y+dy[i];
        if(in_map(&new_node))
        {
            if(map[new_node.x][new_node.y]!=0) continue;
            map[new_node.x][new_node.y]=map[search.x][search.y]+1;
            push(que,&new_node);
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
        }
        printf("\n");
    }
}
void way()
{
    Stack stack;
    stack.top=-1;
    int step=map[n][m];
    Node node;
    node.x=n;node.y=m;
    stack.data[++stack.top]=node;
    int x=n,y=m;
    while(!(x==1&&y==1))
    {
        for(int i=0;i<4;++i)
        {
            node.x=x+dx[i];
            node.y=y+dy[i];
            if(in_map(&node))
            {
                if(map[node.x][node.y]==step-1)
                {
                    stack.data[++stack.top]=node;
                    x=node.x;
                    y=node.y;
                    --step;
                    break;
                }
            }
        }
    }
    printf("依次经过\n");
    while(stack.top>=0)
    {
        printf("%d %d\n",stack.data[stack.top].x,stack.data[stack.top].y);
        --stack.top;
    }
}
int main()
{
    while(n>99||m>99) scanf("%d%d",&n,&m);
    for(int i=1;i<=n;++i)
    {
        for(int j=1;j<=m;++j)
        {
            int x;
            scanf("%d",&x);
            if(x==1) map[i][j]=-1;
        }
    }
    if(map[1][1]==-1)
    {
        printf("输入数据错误");
        return 0;
    }
    Node dot;
    Queue que;
    que.front=que.rear=0;
    dot.x=dot.y=1;
    push(&que,&dot);
    map[1][1]=1;
    while(!empty(&que)) bfs(&que);
    if(map[n][m]==-1||map[n][m]==0)
    {
        printf("无路径\n");
        return 0;
    }
    way();
    return 0;
}