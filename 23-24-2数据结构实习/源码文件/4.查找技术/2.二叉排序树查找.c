#include <stdio.h>
#include <stdlib.h>
// 定义二叉排序树的节点结构
typedef struct TreeNode
{
    int value;               
    // 节点值
    struct TreeNode *left;   
    // 左子节点
    struct TreeNode *right;  
    // 右子节点
} TreeNode;
// 创建一个新的节点
TreeNode* createNode(int value) 
{
    TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));
    newNode->value = value;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// 插入节点到二叉排序树
TreeNode* insert(TreeNode *root, int value) 
{
    if (root == NULL) 
    {
        // 树为空或到达叶子节点位置，创建新节点
        return createNode(value);
    }
    if (value < root->value) 
    {
        // 如果值小于当前节点的值，插入到左子树
        root->left = insert(root->left, value);
    } 
    else if (value > root->value) 
    {
        // 如果值大于当前节点的值，插入到右子树
        root->right = insert(root->right, value);
    }
    // 如果值等于当前节点的值，不插入，返回原节点
    return root;
}
// 查找二叉排序树中的值
TreeNode* search(TreeNode *root, int target) 
{
    if (root == NULL || root->value == target) 
    {
        // 树为空或找到目标值
        return root;
    }
    if (target < root->value) 
    {
        // 如果目标值小于当前节点的值，继续在左子树中查找
        return search(root->left, target);
    } else 
    {
        // 如果目标值大于当前节点的值，继续在右子树中查找
        return search(root->right, target);
    }
}
// 中序遍历树，输出节点值（用于测试）
void inorderTraversal(TreeNode *root)
{
    if (root != NULL)
    {
        inorderTraversal(root->left);
        printf("%d ", root->value);
        inorderTraversal(root->right);
    }
}

int main()
{
    TreeNode *root = NULL;  // 初始化空树
    int n, value, target;
    printf("请输入节点数量: ");
    scanf("%d", &n);
    printf("请输入 %d 个节点的值:\n", n);
    for (int i = 0; i < n; i++) 
    {
        scanf("%d", &value);
        root = insert(root, value);
    }
    printf("请输入要查找的值: ");
    scanf("%d", &target);
    TreeNode *result = search(root, target);
    if (result != NULL) printf("值 %d 在树中存在\n", target);
    else printf("值 %d 不存在于树中\n", target);
    return 0;
}