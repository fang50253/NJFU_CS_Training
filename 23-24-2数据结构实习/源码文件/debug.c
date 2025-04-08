#include <stdio.h>

#define MAX_N 100

int main() {
    int count[MAX_N + 1] = {0};  // 初始化数组，默认所有元素为0
    int n = MAX_N;

    // 计算每个数的因子个数
    for (int i = 1; i <= n; ++i) {
        for (int j = i; j <= n; j += i) {
            count[j] += 1;
        }
    }

    // 输出结果
    for (int i = 1; i <= n; ++i) {
        printf("Number %d has %d factors.\n", i, count[i]);
    }

    return 0;
}