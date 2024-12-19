#include<stdio.h>
int num[100010],tmp[100010],n;
void merge_sort(int l,int r)
{
	if(l>=r) return;
	int mid=(l+r)/2;
	merge_sort(l,mid);
	merge_sort(mid+1,r);
	int k=1,i=l,j=mid+1;
	while(i<=mid&&j<=r)
	{
		if(num[i]<num[j]) tmp[k++]=num[i++];
	    else tmp[k++]=num[j++];
	}
	while(i<=mid) tmp[k++]=num[i++];
	while(j<=r) tmp[k++]=num[j++];
	for(i=1,j=l;j<=r;i++,j++) num[j]=tmp[i];
}
int main()
{
	scanf("%d",&n);
	for(int i=1;i<=n;i++) scanf("%d",&num[i]);
	merge_sort(1,n);
	for(int i=1;i<=n;i++) printf("%d ",num[i]);
	return 0;
}