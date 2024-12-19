#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 定义二叉树节点结构
typedef struct BiTree 
{
    char data; // 节点数据，字符
    int weight; // 节点权重
    struct BiTree *lchild, *rchild; // 左右子树指针
} BiTree;
// 定义哈夫曼编码表的结构
typedef struct 
{
    char data; // 字符
    char code[100]; // 对应的哈夫曼编码
} HuffmanCode;
// 创建新的树节点
BiTree* createNode(char data, int weight) 
{
    BiTree *node = (BiTree *)malloc(sizeof(BiTree)); // 动态分配内存
    node->data = data; // 设置节点数据
    node->weight = weight; // 设置节点权重
    node->lchild = node->rchild = NULL; // 初始化左右子树为空
    return node; // 返回新节点指针
}
// 合并两个节点为一个新的父节点
BiTree* merge(BiTree *left, BiTree *right) 
{
    // 创建一个新的父节点，其权重为左右子节点权重之和
    BiTree *node = createNode('\0', left->weight + right->weight);
    node->lchild = left; // 左子树指向较小的权重节点
    node->rchild = right; // 右子树指向较大的权重节点
    return node; // 返回新的父节点
}
// 比较函数，用于按权重排序
int compare(const void *a, const void *b) 
{
    // 比较两个节点的权重大小，qsort会根据这个结果进行排序
    return (*(BiTree **)a)->weight - (*(BiTree **)b)->weight;
}
// 生成哈夫曼树
BiTree* create(BiTree *nodes[], int n) 
{
    // 不断合并最小的两个节点，直到只剩下一个节点
    while (n > 1) 
    {
        qsort(nodes, n, sizeof(BiTree *), compare); // 按权重排序
        BiTree *left = nodes[0]; // 最小权重节点
        BiTree *right = nodes[1]; // 次小权重节点
        BiTree *newNode = merge(left, right); // 合并这两个节点
        nodes[1] = newNode; // 将新节点放置在数组中
        nodes++; // 数组指针前移，忽略掉已合并的节点
        n--; // 数组中有效节点数减少
    }
    return nodes[0]; // 返回哈夫曼树的根节点
}
// 递归生成哈夫曼编码
void build(BiTree *root, char *code, int length, HuffmanCode huffCodes[], int *idx) 
{
    if (root->lchild == NULL && root->rchild == NULL) 
    {
        // 当前节点为叶子节点，保存编码
        code[length] = '\0'; // 结束符
        huffCodes[*idx].data = root->data; // 保存字符
        strcpy(huffCodes[*idx].code, code); // 保存编码
        (*idx)++; // 增加编码表索引
        return;
    }
    if (root->lchild) 
    {
        // 递归处理左子树
        code[length] = '0'; // 左子树编码为0
        build(root->lchild, code, length + 1, huffCodes, idx);
    }
    if (root->rchild) 
    {
        // 递归处理右子树
        code[length] = '1'; // 右子树编码为1
        build(root->rchild, code, length + 1, huffCodes, idx);
    }
}
// 根据输入字符串生成哈夫曼编码
void encode(char *str, HuffmanCode huffCodes[], int n) 
{
    // 遍历输入字符串中的每个字符
    for (int i = 0; str[i] != '\0'; i++) 
    {
        // 查找字符对应的哈夫曼编码
        for (int j = 0; j < n; j++) 
        {
            if (str[i] == huffCodes[j].data) 
            {
                printf("%s", huffCodes[j].code); // 输出编码
                break;
            }
        }
    }
    printf("\n");
}
// 根据哈夫曼编码解码为字符串
void decode(char *encodedStr, BiTree *root)
{
    BiTree *current = root; // 从根节点开始
    // 遍历编码字符串的每个字符
    for (int i = 0; encodedStr[i] != '\0'; i++)
    {
        // 根据当前编码字符移动到左或右子树
        if (encodedStr[i] == '0')
            current = current->lchild;
        else if (encodedStr[i] == '1')
            current = current->rchild;

        // 如果到达叶子节点，输出对应的字符
        if (current->lchild == NULL && current->rchild == NULL)
        {
            printf("%c", current->data);
            current = root; // 回到根节点，继续解码
        }
    }
    printf("\n");
}
// 菜单界面函数
void menu() {
    printf("输入结点的个数：");
    int len;
    scanf("%d", &len);
    // 动态分配节点数组
    BiTree *nodes[len];
    printf("输入字符串内容+频度：\n");
    // 输入每个字符及其频度
    for (int i = 0; i < len; ++i)
    {
        char str;
        int fre;
        scanf(" %c %d", &str, &fre);
        nodes[i] = createNode(str, fre); // 创建叶子节点
    }
    // 构建哈夫曼树
    BiTree *root = create(nodes, len);
    // 生成哈夫曼编码
    HuffmanCode huffCodes[len];
    char code[100];
    int idx = 0;
    build(root, code, 0, huffCodes, &idx);
    // 输出生成的哈夫曼编码
    printf("生成的哈夫曼编码:\n");
    for (int i = 0; i < len; i++)
    {
        printf("%c: %s\n", huffCodes[i].data, huffCodes[i].code);
    }
    // 菜单选项
    int choice;
    do {
        printf("\n请选择操作:\n");
        printf("1. 编码字符串\n");
        printf("2. 解码字符串\n");
        printf("3. 退出\n");
        printf("输入您的选择: ");
        scanf("%d", &choice);

        if (choice == 1) {
            // 编码字符串
            char str[100];
            printf("输入要编码的字符串: ");
            scanf("%s", str);
            printf("编码结果: ");
            encode(str, huffCodes, len);

        } else if (choice == 2) {
            // 解码字符串
            char encodedStr[100];
            printf("输入要解码的哈夫曼编码: ");
            scanf("%s", encodedStr);
            printf("解码结果: ");
            decode(encodedStr, root);

        } else if (choice != 3) {
            printf("无效的选择，请重新输入。\n");
        }

    } while (choice != 3);

    printf("已退出程序。\n");
}
// 主函数
int main() 
{
    menu(); // 启动程序
    return 0;
}