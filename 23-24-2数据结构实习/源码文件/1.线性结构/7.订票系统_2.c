#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define m 40
#define n 100
typedef struct
{ 
   
	int flightnum;
	char time[40];
	char city[20];
	int price;
	int discount;
	int emptysit;
}Flightinfo;
typedef struct
{ 
   
	Flightinfo Flight[m];
	int length;
}FlList;
typedef struct
{ 
   
	char name[20];
	char IDnum[20];
	char phonenum[20];
	int flightnum;
}Reserved;
typedef struct 
{ 
   
	Reserved client[n];
	int length;
}ReList;
void F_Read(FlList *L1)
{ 
      int i;
	FILE *fp;
	if((fp=fopen("flight.fzy","r"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fscanf(fp,"%d",&(L1->length));
    for(i=0;i<L1->length;i++)
	fscanf(fp,"%d %s %s %d %d %d",&(L1->Flight[i].flightnum),L1->Flight[i].time,L1->Flight[i].city,&(L1->Flight[i].price),&(L1->Flight[i].discount),&(L1->Flight[i].emptysit));
	fclose(fp);
}
void R_Read(ReList *L2)
{ 
   
	FILE *fp;
	int k;
	if((fp=fopen("flight.fzy","r"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fscanf(fp,"%d",&(L2->length));
	if(L2->length==0) return;
	for(k=0;k<L2->length;k++)
		fscanf(fp,"%c %c %c %d",L2->client[k].name,L2->client[k].IDnum,L2->client[k].phonenum,&(L2->client[k].flightnum));
	fclose(fp);
}
void FNsearch(FlList *L1)
{ 
      int t,i;
	printf("请输入要查询的航班号\n");
	scanf("%d",&t);
	for(i=0;i<L1->length;i++)
		if(L1->Flight[i].flightnum==t) break;
	if(i==L1->length) printf("不存在该航班!\n");
	else
	{ 
   
    printf("该航班信息如下:\n");
	printf("航班号：%d\n起降时间：%s\n起降城市：%s\n票价：%d\n票价折扣：%d\n空余座位数：%d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
    }
}
void FCsearch(FlList *L1)
{ 
      int i;
	char temp[20];
	printf("请输入起降城市\n");
	scanf("%s",temp);
    for(i=0;i<L1->length;i++)
		if(strcmp(L1->Flight[i].city,temp)==0) 
		{ 
   
            printf("符合条件的航班信息如下:\n");
	        printf("航班号：%d\n起降时间：%s\n起降城市：%s\n票价：%d\n票价折扣：%d\n空余座位数：%d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
		}
}
void Fsearch(FlList *L1)
{ 
   
	int k;
    printf("1.按航班号查询\n");
	printf("2.按起降城市查询\n");
	scanf("%d",&k);
	switch(k)
	{ 
   
	  case 1:FNsearch(L1);break;
	  case 2:FCsearch(L1);break;
	  default:printf("操作错误!\n");break;
	}
}
void Rsearch(ReList *L2)
{ 
      int i;
	char temp[20];
	if(L2->length==0) 
	{ 
   
		printf("当前订票人数为0,无法查询!\n");
	    return;
	}
	printf("请输入客户证件号\n");
	scanf("%s",temp);
	for(i=0;i<L2->length;i++)
		if(strcmp(L2->client[i].IDnum,temp)==0) break;
	if(i==L2->length) printf("不存在该客户!\n");
	else
	{ 
   
    printf("该客户信息如下:\n");
	printf("姓名：%s\n证件号：%s\n电话：%s\n预定航班：%d\n",L2->client[i].name,L2->client[i].IDnum,L2->client[i].phonenum,L2->client[i].flightnum);
	}
}
void Search(FlList *L1,ReList *L2)
{ 
   
    int k;
    printf("1.航班信息\n");
	printf("2.订票信息\n");
	scanf("%d",&k);
	switch(k)
	{ 
   
	  case 1:Fsearch(L1);break;
	  case 2:Rsearch(L2);break;
	  default:printf("操作错误!\n");break;
	}
}
void Recommend(FlList *L1,int i)
{ 
   
	int k;
	printf("抱歉，该航班已无空余座位\n");
	for(k=0;k<L1->length;k++)
		if((strcmp(L1->Flight[i].city,L1->Flight[k].city)==0)&&(k!=i)&&(L1->Flight[k].emptysit!=0))
		{ 
   
			printf("您可选择该相似航班:\n");
            printf("航班号：%d\n起降时间：%s\n起降城市：%s\n票价：%d\n票价折扣：%d\n空余座位数：%d\n",L1->Flight[k].flightnum,L1->Flight[k].time,L1->Flight[k].city,L1->Flight[k].price,L1->Flight[k].discount,L1->Flight[k].emptysit);
		}
}
void Pay(FlList *L1,ReList *L2,int i)
{ 
   
	FILE *fp1,*fp2;
	int k;
	printf("请输入客户信息\n");
	printf("姓名\n");
	scanf("%s",L2->client[L2->length].name);
	printf("证件号\n");
    scanf(" %s",L2->client[L2->length].IDnum);
    printf("电话\n");
    scanf(" %s",L2->client[L2->length].phonenum);
	L2->client[L2->length].flightnum=L1->Flight[i].flightnum;
	printf("您应支付票价%d元\n",L1->Flight[i].price*L1->Flight[i].discount/10);
	L1->Flight[i].emptysit--;
	L2->length++;
	if((fp1=fopen("flight.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
    if((fp2=fopen("flight_book.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp2,"%d\n",L2->length);
    for(k=0;k<L2->length;k++)
		fprintf(fp2,"%s %s %s %d\n",L2->client[k].name,L2->client[k].IDnum,L2->client[k].phonenum,L2->client[k].flightnum);
	fclose(fp2);
	printf("订票成功！\n");
}
void Reserve(FlList *L1,ReList *L2)
{ 
   
	int i,t;
	printf("请输入要预定班机的航班号\n");
    scanf("%d",&t);
    for(i=0;i<L1->length;i++)
		if(L1->Flight[i].flightnum==t) break;
	if(i==L1->length) 
	{ 
   
		printf("不存在该航班!\n");
        return;
	}
    if(L1->Flight[i].emptysit==0) Recommend(L1,i);
	else Pay(L1,L2,i);
}
void Withdraw(FlList *L1,ReList *L2)
{ 
   
	FILE *fp1,*fp2;
	int k,i;
	char temp[20];
	if(L2->length==0)
	{ 
   
		printf("操作错误！\n");
		return;
	}
	printf("请输入客户证件号\n");
	scanf("%s",temp);
	for(k=0;k<L2->length;k++)
		if(strcmp(temp,L2->client[k].IDnum)==0) break;
	if(k==L2->length)
	{ 
   
		printf("该客户不存在！\n");
		return;
	}
	for(i=0;i<L1->length;i++)
		if(L2->client[k].flightnum==L1->Flight[i].flightnum) break;
	L1->Flight[i].emptysit++;
	for(;k<L2->length-1;k++)
		L2->client[k]=L2->client[k+1];
	L2->length--;
	if((fp1=fopen("flight.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
    if((fp2=fopen("flight_book.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp2,"%d\n",L2->length);
    for(k=0;k<L2->length;k++)
		fprintf(fp2,"%s %s %s %d\n",L2->client[k].name,L2->client[k].IDnum,L2->client[k].phonenum,L2->client[k].flightnum);
	fclose(fp2);
	printf("退票成功！\n");
}
void Fadd(FlList *L1)
{ 
   
	FILE *fp1;
	int i;
	printf("请输入新增航班信息\n");
	printf("航班号\n");
	scanf("%d",&(L1->Flight[L1->length].flightnum));
	printf("起降时间\n");
	scanf(" %s",L1->Flight[L1->length].time);
	printf("起降城市\n");
	scanf(" %s",L1->Flight[L1->length].city);
	printf("票价\n");
	scanf("%d",&(L1->Flight[L1->length].price));
	printf("票价折扣\n");
	scanf("%d",&(L1->Flight[L1->length].discount));
    printf("空余座位数\n");
	scanf("%d",&(L1->Flight[L1->length].emptysit));
	L1->length++;
	if((fp1=fopen("flight.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
}
void Fcan(FlList *L1)
{ 
      
	FILE *fp1;
	int i,t;
	printf("请输入要取消航班的航班号\n");
	scanf("%d",&t);
    for(i=0;i<L1->length;i++)
		if(L1->Flight[i].flightnum==t) break;
	if(i==L1->length) 
	{ 
   
		printf("不存在该航班!\n");
        return;
	}
	for(;i<L1->length-1;i++)
		L1->Flight[i]=L1->Flight[i+1];
	L1->length--;
	if((fp1=fopen("flight.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
}
void Fchg(FlList *L1)
{ 
   
	FILE *fp1;
	int i,t;
	printf("请输入要修改航班的航班号\n");
	scanf("%d",&t);
    for(i=0;i<L1->length;i++)
		if(L1->Flight[i].flightnum==t) break;
	if(i==L1->length) 
	{ 
   
		printf("不存在该航班!\n");
        return;
	}
	printf("请输入新的起降时间\n");
	scanf("%s",L1->Flight[i].time);
	printf("请输入新的票价\n");
	scanf("%d",&(L1->Flight[i].price));
	printf("请输入新的票价折扣\n");
	scanf("%d",&(L1->Flight[i].discount));
	if((fp1=fopen("flight.fzy","w"))==NULL)
	{ 
   
		printf("文件打开失败!\n");
		exit(0);
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
}
void Adjust(FlList *L1)
{ 
   
	int k;
	printf("请选择要执行的功能\n");
	printf("1.增加航班\n");
	printf("2.取消航班\n");
    printf("3.改动信息\n");
	scanf("%d",&k);
	switch(k)
	{ 
   
	  case 1:Fadd(L1);break;
	  case 2:Fcan(L1);break;
	  case 3:Fchg(L1);break;
	}
}
int main()
{ 
    int k;
	FlList Flist;
	ReList Rlist;
	Flist.length=0;
	Rlist.length=0;
	F_Read(&Flist);
	R_Read(&Rlist);
	do 
	{ 
	    printf("1.查询\n");
	    printf("2.订票\n");
	    printf("3.退票\n");
	    printf("4.修改\n");
        printf("5.退出\n");
		scanf("%d",&k);
		switch(k)
		{ 
   
		  case 1:Search(&Flist,&Rlist);break;
		  case 2:Reserve(&Flist,&Rlist);break;
		  case 3:Withdraw(&Flist,&Rlist);break;
		  case 4:Adjust(&Flist);break;
		  case 5:break;
		  default:printf("无此功能，请重新输入!\n");break;
		}
	}while(k!=5);
}