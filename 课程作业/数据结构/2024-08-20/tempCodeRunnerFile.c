#include<stdio.h>
#include<iostream>
#include<queue>
#include<stack>
#include<string>
#include<string.h>
using namespace std;
const int N=1e5+10;
void fun()
{
    int n;
    scanf("%d",&n);
    if(n%2==0) printf("-1\n");
    else{
        for(int i=1;i<=n;++i) printf("%d ",i);
        printf("\n");
    }
}
int main()
{
    int T;
    scanf("%d",&T);
    while(T--) fun();
    return 0;
}