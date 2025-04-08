#include<stdio.h>
// 声明全局变量
int num[100010], tmp[100010], n;
// 归并排序的递归函数
void merge_sort(int l, int r)
{
    if(l >= r) return; 
    // 如果子数组长度为0或1，直接返回，递归结束条件
    int mid=l+r>>1;
    // 计算中间位置，用于将数组分成两部分
    merge_sort(l, mid); 
    // 递归排序左半部分
    merge_sort(mid + 1, r); 
    // 递归排序右半部分
    int k = 1, i = l, j = mid + 1;
    // 初始化指针：k用于临时数组，i用于左半部分，j用于右半部分
    // 合并两个有序的子数组
    while(i <= mid && j <= r)
    {
        if(num[i] < num[j]) tmp[k++] = num[i++];
        // 如果左半部分当前元素较小，放入临时数组
        else tmp[k++] = num[j++];
        // 否则放入右半部分当前元素
    }
    // 如果左半部分有剩余元素，继续放入临时数组
    while(i <= mid) tmp[k++] = num[i++];
    // 如果右半部分有剩余元素，继续放入临时数组
    while(j <= r) tmp[k++] = num[j++];
    // 将临时数组中的元素拷贝回原数组对应位置
    for(i = 1, j = l; j <= r; i++, j++) num[j] = tmp[i];
}
int main()
{
    scanf("%d", &n);
    // 读取数组大小n
    for(int i = 1; i <= n; i++) scanf("%d", &num[i]);
    // 读取n个元素到num数组中
    merge_sort(1, n);
    // 对整个数组进行归并排序
    for(int i = 1; i <= n; i++) printf("%d ", num[i]);
    // 输出排序后的数组
    return 0;
}