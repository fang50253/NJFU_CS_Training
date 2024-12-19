#include<stdio.h>
const int N=1e5+10;
int arr[N],n;
int main()
{
	scanf("%d",&n);
	for(int i=0;i<n;i++) scanf("%d",&arr[i]);
	for(int i=0;i<n;i++)
	{
		int tmp=i; 
		for(int j=i+1;j<n;j++)
		{
			if(arr[j]<arr[tmp]) tmp=j;
		}
		int mid=arr[tmp];
		arr[tmp]=arr[i];
		arr[i]=mid;
	}
	for(int i=0;i<n;i++) printf("%d ",arr[i]);
	return 0;
}