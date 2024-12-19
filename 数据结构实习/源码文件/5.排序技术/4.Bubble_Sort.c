#include<stdio.h>
const int N=10;
int a[N];
int main()
{
	for(int i=0;i<10;i++) scanf("%d",&a[i]);
	for(int i=0;i<10;i++)
	{
		for(int j=1;j<10;j++)
		{
			if(a[j-1]>a[j])
			{
				int tmp=a[j-1];
				a[j-1]=a[j];
				a[j]=tmp;
			}
		}
	}
	for(int i=0;i<10;i++) printf("%d ",a[i]);
	return 0;
}