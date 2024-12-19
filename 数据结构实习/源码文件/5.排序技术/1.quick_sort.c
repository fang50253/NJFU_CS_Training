#include<stdio.h>
// 交换两个整数的值
void swap(int *a, int *b)
{
    int tmp = *a;  // 临时变量存储a的值
    *a = *b;       // 将b的值赋给a
    *b = tmp;      // 将临时变量的值（即原来的a值）赋给b
}
// 实现快速排序的递归函数
void quick_sort(int q[], int l, int r)
{
    if(l >= r) return;
    // 如果子数组长度为0或1，直接返回
    int mid = q[l + r >> 1];
    // 选择中间位置的元素作为基准值
    int i = l - 1, j = r + 1;
    // 初始化两个指针：i指向左边界外，j指向右边界外
    while(i < j)
    // 开始分区过程
    {
        do ++i; while(q[i] < mid);
        // 从左向右找到第一个大于等于基准值的元素
        do --j; while(q[j] > mid); 
        // 从右向左找到第一个小于等于基准值的元素
        if(i < j) swap(&q[i], &q[j]);
        // 如果找到的一对元素的位置不正确，交换它们
    }
    // 递归地对分区后的左右部分进行快速排序
    quick_sort(q, l, i - 1);  
    // 对左半部分排序
    quick_sort(q, j + 1, r);  
    // 对右半部分排序
}
//主程序
int main()
{
    int n, num[100]; 
    // 声明变量：n表示数组的大小，num数组存储待排序的数
    scanf("%d", &n); 
    // 读取数组的大小
    for(int i = 0; i < n; ++i) scanf("%d", &num[i]); 
    // 读取数组中的n个元素
    quick_sort(num, 0, n - 1); 
     // 对整个数组进行快速排序
    for(int i = 0; i < n; ++i) printf("%d ", num[i]);  
    // 输出排序后的数组
    return 0;  // 程序结束
}