#include <stdio.h>
const int N = 1e5 + 10; // 定义数组的最大容量
int arr[N], n; // 全局数组 arr 和数组大小 n
int main()
{
    // 读取数组的大小
    scanf("%d", &n);    
    // 读取数组元素
    for (int i = 1; i <= n; ++i) scanf("%d", &arr[i]);
    // 读取要查找的目标值
    int target;
    scanf("%d", &target);
    // 初始化二分查找的左右边界
    int l = 1, r = n;
    // 执行二分查找
    while (l < r)
    {
        // 计算中间位置
        int mid = l + r >> 1;
        // 比较中间值与目标值
        if (arr[mid] >=target) 
            // 如果中间值大于等于目标值，则目标值可能在左半部分
            r = mid;
        else 
            // 如果中间值小于目标值，则目标值可能在右半部分
            l = mid+1;
    }
    // 检查目标值是否存在于数组中
    if (arr[l] == target) 
        // 如果目标值存在，输出其在数组中的位置
        printf("数字 %d 是第 %d 个数字\n", target, l);
    else
        // 如果目标值不存在，输出提示信息
        printf("数字不存在\n");
    return 0;
}