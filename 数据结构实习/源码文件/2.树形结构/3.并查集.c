#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100
typedef struct Node
{
    int data;
    struct Node* parent;
    int rank;
} Node;
Node* nodes[MAX_SIZE];
// 初始化集合，每个元素自成一个集合
void InitSet(int x)
{
    nodes[x] = (Node*)malloc(sizeof(Node));
    nodes[x]->data = x;
    nodes[x]->parent = nodes[x];
    nodes[x]->rank = 0;
}
// 查找元素的根，带路径压缩
Node* findSet(Node* node)
{
    if (node->parent != node) node->parent = findSet(node->parent);  // 路径压缩
    return node->parent;
}
// 合并两个集合，按秩合并
void unionSets(int x, int y)
{
    Node* rootX = findSet(nodes[x]);
    Node* rootY = findSet(nodes[y]);
    rootY->parent = rootX;
}
// 输出集合的存储结构
void printSets(int n)
{
    for (int i = 0; i < n; i++)
    {
        Node* root = findSet(nodes[i]);
        printf("Element %d -> Set Representative %d\n", i, root->data);
    }
}
int main()
{
    int n, ch, x, y;
    printf("请输入集合中元素的数量: ");
    scanf("%d", &n);
    while (1)
    {
        printf("\n1. 建立集合\n");
        printf("2. 查找(判定)元素所属集合\n");
        printf("3. 集合的并运算\n");
        printf("0. 退出\n");
        printf("请选择 0-3: ");
        scanf("%d", &ch);
        if(ch==1)
        {
            for(int i=0;i<n;i++) InitSet(i);
        }
         else if(ch==2)
         {
            printf("请输入要查找的元素: ");
            scanf("%d", &x);
            printf("元素 %d 所属集合的代表元是: %d\n", x, findSet(nodes[x])->data);
         }
        else if(ch==3)
        {
            printf("请输入要联合的两个元素: ");
            scanf("%d %d", &x, &y);
            unionSets(x, y);
        }
        else if(ch==0) return 0;
    }
    return 0;
}