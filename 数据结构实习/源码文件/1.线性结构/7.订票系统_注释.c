#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define m 40  // 定义航班数组的最大容量
#define n 100 // 定义预订客户数组的最大容量
// 航班信息结构体
typedef struct
{ 
    int flightnum;       // 航班号
    char time[40];       // 起降时间
    char city[100];       // 起降城市
    int price;           // 票价
    int discount;        // 折扣
    int emptysit;        // 空余座位数
}Flightinfo;
// 航班列表结构体
typedef struct
{ 
    Flightinfo Flight[m];  // 航班数组
    int length;            // 航班数量
}FlList;
// 预订客户信息结构体
typedef struct
{ 
    char name[20];         // 客户姓名
    char IDnum[20];        // 客户身份证号
    char phonenum[20];     // 客户电话号码
    int flightnum;         // 预订航班号
}Reserved;
// 客户预订列表结构体
typedef struct 
{ 
    Reserved client[n];  // 客户数组
    int length;          // 客户数量
}ReList;
// 从文件中读取航班信息
void F_Read(FlList *L1)
{ 
    int i;
    FILE *fp;
    if((fp=fopen("flight.fzy","r"))==NULL)
    { 
        printf("文件打开失败!\n");
    }
    fscanf(fp,"%d",&(L1->length));  // 读取航班数量
    for(i=0;i<L1->length;i++)
        fscanf(fp,"%d %s %s %d %d %d",
               &(L1->Flight[i].flightnum),
               L1->Flight[i].time,
               L1->Flight[i].city,
               &(L1->Flight[i].price),
               &(L1->Flight[i].discount),
               &(L1->Flight[i].emptysit));  // 读取每个航班的信息
    fclose(fp);
}
// 从文件中读取预订信息
void R_Read(ReList *L2)
{ 
    FILE *fp;
    int k;
    if((fp=fopen("flight_book.fzy","r"))==NULL)
    { 
        printf("文件打开失败!\n");
    }
    fscanf(fp,"%d",&(L2->length));  // 读取客户数量
    if(L2->length==0) return;
    for(k=0;k<L2->length;k++)
        fscanf(fp,"%s %s %s %d",L2->client[k].name,L2->client[k].IDnum,L2->client[k].phonenum,&(L2->client[k].flightnum));  // 读取每个客户的信息
    fclose(fp);
}
// 按航班号查询航班信息
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
// 按城市查询航班信息
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
// 航班信息查询入口
void Fsearch(FlList *L1)
{ 
    int k;
    printf("1.按航班号查询\n");
    printf("2.按起降城市查询\n");
    scanf("%d",&k);
    switch(k)
    { 
        case 1: FNsearch(L1); break;
        case 2: FCsearch(L1); break;
        default: printf("操作错误!\n"); break;
    }
}
// 按客户身份证号查询预订信息
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
	}
	fprintf(fp1,"%d\n",L1->length);
    for(i=0;i<L1->length;i++)
	fprintf(fp1,"%d %s %s %d %d %d\n",L1->Flight[i].flightnum,L1->Flight[i].time,L1->Flight[i].city,L1->Flight[i].price,L1->Flight[i].discount,L1->Flight[i].emptysit);
	fclose(fp1);
}
// 查询功能入口
void Search(FlList *L1,ReList *L2)
{ 
    int k;
    printf("1.航班信息\n");
    printf("2.订票信息\n");
    scanf("%d",&k);
    switch(k)
    { 
        case 1: Fsearch(L1); break;
        case 2: Rsearch(L2); break;
        default: printf("操作错误!\n"); break;
    }
}
// 如果航班座位已满，推荐相似航班
void Recommend(FlList *L1, int i)
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
// 处理支付和订票功能
void Pay(FlList *L1, ReList *L2, int i) 
{ 
    FILE *fp1, *fp2;
    int k;
    // 获取客户信息
    printf("请输入客户信息\n");
    printf("姓名\n");
    scanf("%s", L2->client[L2->length].name);
    printf("证件号\n");
    scanf(" %s", L2->client[L2->length].IDnum);
    printf("电话\n");
    scanf(" %s", L2->client[L2->length].phonenum);
    // 记录客户所预订的航班号
    L2->client[L2->length].flightnum = L1->Flight[i].flightnum;
    // 计算应支付的票价
    printf("您应支付票价%d元\n", L1->Flight[i].price * L1->Flight[i].discount / 10);
    // 更新航班的空余座位数
    L1->Flight[i].emptysit--;
    // 增加客户预订记录的数量
    L2->length++;
    // 打开航班信息文件，并更新航班信息
    if ((fp1 = fopen("flight.fzy", "w")) == NULL) 
    { 
        printf("文件打开失败!\n");
    }
    fprintf(fp1, "%d\n", L1->length);
    for (i = 0; i < L1->length; i++)
        fprintf(fp1, "%d %s %s %d %d %d\n", L1->Flight[i].flightnum, L1->Flight[i].time, L1->Flight[i].city, L1->Flight[i].price, L1->Flight[i].discount, L1->Flight[i].emptysit);
    fclose(fp1);
    // 打开客户预订信息文件，并更新客户预订信息
    if ((fp2 = fopen("flight_book.fzy", "w")) == NULL) 
    { 
        printf("文件打开失败!\n");
    }
    fprintf(fp2, "%d\n", L2->length);
    for (k = 0; k < L2->length; k++)
        fprintf(fp2, "%s %s %s %d\n", L2->client[k].name, L2->client[k].IDnum, L2->client[k].phonenum, L2->client[k].flightnum);
    fclose(fp2);
    printf("订票成功！\n");
}
// 处理航班预订功能
void Reserve(FlList *L1, ReList *L2) 
{ 
    int i, t;
    // 输入要预定的航班号
    printf("请输入要预定班机的航班号\n");
    scanf("%d", &t);
    // 查找航班号是否存在
    for (i = 0; i < L1->length; i++)
        if (L1->Flight[i].flightnum == t) break;
    // 如果航班不存在，返回提示信息
    if (i == L1->length) 
    { 
        printf("不存在该航班!\n");
        return;
    }
    // 如果航班已满，推荐其他航班，否则进行支付
    if (L1->Flight[i].emptysit == 0) Recommend(L1, i);  // 推荐其他航班
    else Pay(L1, L2, i);    // 进行支付和订票
}
// 处理退票功能
void Withdraw(FlList *L1, ReList *L2) 
{ 
    FILE *fp1, *fp2;
    int k, i;
    char temp[20];
    // 检查是否有预订记录
    if (L2->length == 0) 
    { 
        printf("操作错误！\n");
        return;
    }
    // 输入客户证件号
    printf("请输入客户证件号\n");
    scanf("%s", temp);
    // 查找客户信息
    for (k = 0; k < L2->length; k++)
        if (strcmp(temp, L2->client[k].IDnum) == 0) break;
    // 如果客户不存在，返回提示信息
    if (k == L2->length) 
    { 
        printf("该客户不存在！\n");
        return;
    }
    // 找到客户对应的航班，并增加空余座位数
    for (i = 0; i < L1->length; i++)
        if (L2->client[k].flightnum == L1->Flight[i].flightnum) break;
    L1->Flight[i].emptysit++;
    // 删除客户的预订信息
    for (; k < L2->length - 1; k++)
        L2->client[k] = L2->client[k + 1];
    L2->length--;
    // 更新航班信息文件
    if ((fp1 = fopen("flight.fzy", "w")) == NULL) 
    { 
        printf("文件打开失败!\n");
    }
    fprintf(fp1, "%d\n", L1->length);
    for (i = 0; i < L1->length; i++)
        fprintf(fp1, "%d %s %s %d %d %d\n", L1->Flight[i].flightnum, L1->Flight[i].time, L1->Flight[i].city, L1->Flight[i].price, L1->Flight[i].discount, L1->Flight[i].emptysit);
    fclose(fp1);
    // 更新客户预订信息文件
    if ((fp2 = fopen("flight_book.fzy", "w")) == NULL) 
    { 
        printf("文件打开失败!\n");
    }
    fprintf(fp2, "%d\n", L2->length);
    for (k = 0; k < L2->length; k++)
        fprintf(fp2, "%s %s %s %d\n", L2->client[k].name, L2->client[k].IDnum, L2->client[k].phonenum, L2->client[k].flightnum);
    fclose(fp2);
    printf("退票成功！\n");
}
// 取消航班函数
void Fcan(FlList *L1)
{ 
    FILE *fp1;
    int i, t;
    // 提示输入要取消的航班号
    printf("请输入要取消航班的航班号\n");
    scanf("%d", &t);
    // 查找航班号是否存在
    for(i = 0; i < L1->length; i++)
        if(L1->Flight[i].flightnum == t) break;
    // 如果找不到航班号，提示错误并返回
    if(i == L1->length) 
    { 
        printf("不存在该航班!\n");
        return;
    }
    // 将要取消的航班从列表中移除，方法是将之后的航班信息前移
    for(; i < L1->length - 1; i++)
        L1->Flight[i] = L1->Flight[i + 1];
    // 航班数减少1
    L1->length--;
    // 打开文件，准备将更新后的航班列表写入文件
    if((fp1 = fopen("flight.fzy", "w")) == NULL)
    { 
        printf("文件打开失败!\n");
    }
    // 将航班列表写入文件
    fprintf(fp1, "%d\n", L1->length);
    for(i = 0; i < L1->length; i++)
        fprintf(fp1, "%d %s %s %d %d %d\n", L1->Flight[i].flightnum, L1->Flight[i].time, L1->Flight[i].city, L1->Flight[i].price, L1->Flight[i].discount, L1->Flight[i].emptysit);
    fclose(fp1); // 关闭文件
}

// 修改航班信息函数
void Fchg(FlList *L1)
{ 
    FILE *fp1;
    int i, t;
    // 提示输入要修改的航班号
    printf("请输入要修改航班的航班号\n");
    scanf("%d", &t);
    // 查找航班号是否存在
    for(i = 0; i < L1->length; i++)
        if(L1->Flight[i].flightnum == t) break;
    // 如果找不到航班号，提示错误并返回
    if(i == L1->length) 
    { 
        printf("不存在该航班!\n");
        return;
    }
    // 输入新的起降时间
    printf("请输入新的起降时间\n");
    scanf("%s", L1->Flight[i].time);
    // 输入新的票价
    printf("请输入新的票价\n");
    scanf("%d", &(L1->Flight[i].price));
    // 输入新的票价折扣
    printf("请输入新的票价折扣\n");
    scanf("%d", &(L1->Flight[i].discount));
    // 打开文件，准备将更新后的航班信息写入文件
    if((fp1 = fopen("flight.fzy", "w")) == NULL)
    { 
        printf("文件打开失败!\n");
    }
    // 将航班列表写入文件
    fprintf(fp1, "%d\n", L1->length);
    for(i = 0; i < L1->length; i++)
        fprintf(fp1, "%d %s %s %d %d %d\n", L1->Flight[i].flightnum, L1->Flight[i].time, L1->Flight[i].city, L1->Flight[i].price, L1->Flight[i].discount, L1->Flight[i].emptysit);
    fclose(fp1); // 关闭文件
}

// 调整航班信息函数
void Adjust(FlList *L1)
{ 
    int k;
    // 提示用户选择要执行的功能
    printf("请选择要执行的功能\n");
    printf("1.增加航班\n");
    printf("2.取消航班\n");
    printf("3.改动信息\n");
    scanf("%d", &k);
    // 根据用户选择调用相应的功能
    switch(k)
    { 
        case 1: Fadd(L1); break;  // 增加航班
        case 2: Fcan(L1); break;  // 取消航班
        case 3: Fchg(L1); break;  // 修改航班信息
    }
}
// 主函数
int main()
{ 
    int k;
    FlList Flist;  // 航班列表
    ReList Rlist;  // 预订列表
    // 初始化航班和预订列表的长度
    Flist.length = 0;
    Rlist.length = 0;
    // 从文件读取航班和预订信息
    F_Read(&Flist);
    R_Read(&Rlist);
    do 
    { 
        // 提示用户选择功能
        printf("1.查询\n");
        printf("2.订票\n");
        printf("3.退票\n");
        printf("4.修改\n");
        printf("5.退出\n");
        scanf("%d", &k);
        // 根据用户选择执行相应的操作
        switch(k)
        { 
            case 1: Search(&Flist, &Rlist); break;  // 查询航班信息
            case 2: Reserve(&Flist, &Rlist); break; // 预订航班
            case 3: Withdraw(&Flist, &Rlist); break;// 退票
            case 4: Adjust(&Flist); break;          // 修改航班信息
            case 5: break;                           // 退出程序
            default: printf("无此功能，请重新输入!\n"); break; // 错误提示
        }
    } while(k != 5); // 当用户选择退出时，结束循环
}