#include <stdio.h>
// 定义数组大小的常量
const int N = 1e5 + 10;
int q[N];
// 存储数组元素的数组
// 函数：顺序查找数组中的指定值
int sequentialSearch(int size, int target)
{
    // 从数组末尾开始逐个向前查找目标值
    for (int i = size; i >= 1; --i)
    {
        if (q[i] == target) return i; // 找到目标值，返回索引
    }
}
int main()
{
    int target, n;
    // 读取数组大小
    scanf("%d", &n);
    // 读取数组元素
    for (int i = 1; i <= n; ++i) scanf("%d", &q[i]);
    // 提示用户输入要查找的值
    printf("请输入要查找的值: ");
    scanf("%d", &target);
    // 在数组的第一个位置设置目标值，以便顺序查找能够找到它
    q[0] = target;
    // 调用顺序查找函数
    int index = sequentialSearch(n, target);
    // 输出结果
    if (index) printf("值 %d 在数组中的索引是 %d\n", target, index);
    else printf("值 %d 未找到\n", target);
    return 0;
}