#include<stdio.h>
const int N=1e5+10;
int arr[N],n;
int main()
{
	scanf("%d",&n);
	for(int i=0;i<n;i++) scanf("%d",&arr[i]);
	for(int i=0;i<n;i++)
	{
		for(int j=i;j>=1;j--)
		{
			if(arr[j]<arr[j-1])
			{
				int tmp=arr[j];
				arr[j]=arr[j-1];
				arr[j-1]=tmp;
			}
		}
	}
	for(int i=0;i<n;i++) printf("%d ",arr[i]);
	return 0;
}