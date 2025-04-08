#include <stdio.h>
// 希尔排序函数
void shell_sort(int arr[], int n)
{
    // 选择初始步长，通常为数组长度的一半
    for (int gap = n / 2; gap > 0; gap /= 2)
    {
        // 对每个步长gap进行分组，并对每组进行插入排序
        for (int i = gap; i < n; i++)
        {
            int temp = arr[i];
            int j;
            // 插入排序部分，将temp插入到它在分组内的合适位置
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) arr[j] = arr[j - gap];
            arr[j] = temp;
        }
    }
}
// 主函数
int main() {
    int n;
    printf("请输入数组的元素数量: ");
    scanf("%d", &n);
    int arr[n];
    printf("请输入数组的元素:\n");
    for (int i = 0; i < n; i++) scanf("%d", &arr[i]);
    shell_sort(arr, n);
    printf("排序后的数组:\n");
    for (int i = 0; i < n; i++) printf("%d ", arr[i]);
    return 0;
}