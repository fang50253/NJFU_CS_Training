#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define CLS "clear"//在win系统中改成"cls" 调试端为macOS
const int MAXN=1e3+10;//数据量大上限
int data_open;//日志模式开关
int idx=0;//数据量顶
int add_users(char num[],char name[]);//用于添加用户
int buy(char num[],int fee);//用于消费
void main_tab();//首页
int recharge(char num[],int fee);//用于充值
int read();//用于读取数据
int write();//用于写入数据
void add_users_input();
void buy_input();
void recharge_input();
//代码中所有的input均为前端处理函数
int query_residue(char num[]);
struct DataBase//数据库结构体
{
    char num[30];
    char name[30];
    int money;//这边使用int类型存储是防止double类型存储的金钱存在偏移，扩大100倍显示
}info[MAXN];
void query_residue_output()
{
    char num[100];
    printf("请输入需要查询的卡号，回车键结束\n");
    scanf("%s",num);
    int v=query_residue(num);//记录返回值等待处理
    if(v==-1)
    {
        if(data_open) system(CLS);
        printf("用户名不存在\n");//处理特殊情况
        main_tab();//返回主菜单
        return;
    }
    if(data_open) system(CLS);
    printf("余额为%.2lf\n",v/100.0);//输出余额
    main_tab();
    return;
}
int read()
{
    FILE *fp=fopen("data.fzy","r");//以制度模式打开文件
    if(fp==NULL) return 0;//如果文件读取失败那么不再继续执行，代表程序为新程序
    fscanf(fp,"%d",&data_open);//读取日志模式状态
    char num[100],name[100];//文件读取缓冲区
    int money;//文件读取缓冲区
    memset(info,0,sizeof info);//清空原有数据
    idx=0;//清空数据顶
    while(fscanf(fp,"%s%s%d",num,name,&money)==3)
    {
        ++idx;
        if(idx>=MAXN-10) printf("Error,数据量过大\n");
        //数据最大规模可以在代码顶部的全局变量区进行调整
        strcpy(info[idx].num,num);//将文件写入内存
        strcpy(info[idx].name,name);//将文件写入内存
        info[idx].money=money;//将文件写入内存
    }
    fclose(fp);//关闭文件，停止占用
    return 0;
}
int write()
{
    FILE *fp=fopen("data.fzy","w+");//写文件
    fprintf(fp,"%d\n",data_open);//写入日志模式配置文件
    for(int i=1;i<=idx;++i)
        fprintf(fp,"%s %s %d\n",info[i].num,info[i].name,info[i].money);
    fclose(fp);//关闭fp文件停止占用
    printf("数据保存成功\n");//宿处保存成功提示
    return 0;
}
int query_residue(char num[])
//返回的数组是余额的100倍
//返回值-1代表用户名不存在
{
    int users_num=-1;//进行标记防止数据读入失败
    for(int i=1;i<=idx;++i)
    {
        struct DataBase output=info[i];
        if(strcmp(output.num,num)==0)//找到用户名所在的位置
        {
            users_num=i;
            break;
        }
    }
    if(users_num==-1) return -1;//用户名不存在
    return info[users_num].money;//返回结果
}
void recharge_input()
{
    char num[100];//用户名缓冲区
    double drawin;//存入的钱(原始金额)
    printf("请输入卡号和充值金额，使用空格分开\n");
    scanf("%s%lf",num,&drawin);
    int v=recharge(num,(int)(drawin*100));
    //通过recharge()函数获取一个返回值
    if(v==-1)
    {
        if(data_open) system(CLS);
        printf("操作失败(返回值-1)：用户名不存在\n");//处理异常情况
        main_tab();//返回主界面
        return;
    }
    printf("用户名%s充值后卡内余额剩余%.2lf元\n",num,v/100.0);//前端提示
    main_tab();//返回主页面
    return;
}
void buy_input()
{
    char num[100];
    double fee;
    printf("请输入卡号和消费金额，使用空格分开\n");
    scanf("%s %lf",num,&fee);
    int v=buy(num,(int)(fee*100));
    switch(v)
    {
        case -1://处理买饭的第一个异常：用户名不存在
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-1)：用户名不存在\n");
            main_tab();
            return;
        }
        case -2://处理买饭的第二个异常：余额不足5元
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-2)：原始卡内余额不足5元，请充值\n");
            main_tab();
            return;
        }
        case -3://处理买饭的第三个异常：余额有5元但不足以支付
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-3)：无法完成此次消费，因为卡内余额不足以支付饭费\n");
            main_tab();
            return;
        }
        case 0://没有异常
        {
            if(data_open) system(CLS);
            int residue=query_residue(num);
            //这边的residue不可能为-1，如果返回值为-1，用户不存在，则上面的case -1已经解决了这个问题
            printf("操作成功，卡号%s已完成消费%.2lf元,余额%.2lf元\n",num,fee,residue/100.0);
            //返回消费的金额，剩余的余额等信息
            main_tab();//返回主屏幕
            return;
        }
    }
}
void add_users_input()//新增用户的前端页面
{
    char num[100],name[100];//用户名和卡号的缓冲区
    printf("请输入卡号和用户，使用空格分开，要求不超过25个字符\n");
    scanf("%s %s",num,name);//输入想要添加的用户名
    int v=add_users(num,name);//获取一个返回值
    switch(v)
    {
        case -1://处理用户名添加的第一个异常：用户名重复
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-1)：用户重复添加\n");
            main_tab();
            return;
        }
        case -2://获取用户名添加的第二个异常：卡号溢出
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-2)：卡号超长\n");
            main_tab();
            return;
        }
        case -3://获取用户名添加的第三个异常：用户名溢出
        {
            if(data_open) system(CLS);
            printf("操作失败(返回值-3)：用户名超长\n");
            main_tab();
            return;
        }
        case 0://没有异常
        {
            if(data_open) system(CLS);
            printf("操作成功，已添加用户%s %s\n",num,name);
            main_tab();
            return;
        }
    }
}
void main_tab()
{
    //下面是主界面
    for(int i=1;i<=50;++i) printf("#");
    for(int i=1;i<=3;++i) printf("\n");
    printf("   fzy饭卡管理程序@2024-07-28\n");
    printf("   请输入以下数字，选择您所要进行的操作：\n");
    printf("   1.从磁盘中读取记录\n");
    printf("   2.向磁盘中写入记录\n");
    printf("   3.新增用户\n");
    printf("   4.买饭消费\n");
    printf("   5.卡片充值\n");
    printf("   6.查询卡内余额\n");
    printf("   7.打开或关闭日志开关\n");
    printf("   8.退出系统\n");
    for(int i=1;i<=2;++i) printf("\n");
    for(int i=1;i<=50;++i) printf("#");
    printf("\n请输入您的选择(回车键确认)：");
    //主界面打印完成，供用户选择
    while(1)
    {
        int choose;
        scanf("%d",&choose);//不断重复询问选择
        switch(choose)
        {
            case 1:read();break;//读文件
            case 2:write();break;//写文件
            case 3:add_users_input();break;//增加用户
            case 4:buy_input();break;//买饭
            case 5:recharge_input();break;//充值
            case 6:query_residue_output();break;//查询余额
            case 7:data_open=!data_open;break;//打开或关闭数据日志
            case 8:return;//退出软件
        }
    }
}
int add_users(char num[],char name[])
//返回值为-1，代表程序执行出现错误:用户重复添加
//返回值为-2:卡号超长
//返回值为-3:姓名超长
//返回值为0，代表用户添加成功
{
    for(int i=1;i<=idx;++i)
    {
        struct DataBase output=info[i];
        if(strcmp(output.num,num)==0) return -1;
    }
    //执行到这一步，代表用户没有被重复添加
    ++idx;
    if(strlen(num)>25) return -2;//卡号超长判断
    if(strlen(name)>25) return -3;//姓名超长判断
    strcpy(info[idx].num,num);//将数据写入
    strcpy(info[idx].name,name);//将数据读出
    info[idx].money=0;
    write();
    return 0;
}
int buy(char num[],int fee)
//返回值为-1，代表用户不存在
//返回值为-2，代表余额不足
//返回值为-3，代表余额不足以支付饭费
//返回值为0，代表程序正常运行结束
{
    int users_num=-1;
    for(int i=1;i<=idx;++i)
    {
        struct DataBase output=info[i];
        if(strcmp(output.num,num)==0)
        {
            users_num=i;
            break;
        }
    }
    if(users_num==-1) return -1;//用户名不存在
    if(info[users_num].money<500) return -2;//卡内原有余额不足5元，需充值
    if(info[users_num].money-fee<0) return -3;//卡内余额不足以消费，需充值
    info[users_num].money-=fee;
    write();
    return 0;
}
int recharge(char num[],int fee)
//返回-1：用户名不存在
//返回到数字为充值后到余额
{
    int users_num=-1;//搜索是否存在用户
    for(int i=1;i<=idx;++i)
    {
        struct DataBase output=info[i];
        if(strcmp(output.num,num)==0)
        {
            users_num=i;
            break;
        }
    }
    if(users_num==-1) return -1;//用户名不存在
    info[users_num].money+=fee;
    write();//写入文件，保存信息
    return info[users_num].money;//返回余额(余额的100倍)
}
int main()
{
    read();//读取数据
    if(data_open) system(CLS);
    main_tab();//进入主页面
    printf("已退出系统\n");//退出提示
    return 0;
}