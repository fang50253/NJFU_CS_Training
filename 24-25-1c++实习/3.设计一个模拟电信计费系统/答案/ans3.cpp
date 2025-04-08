#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<iostream>
#include<string>
#include<time.h>
using namespace std;

#define User_NUM  4
#define FDT   1    // 固定长途
#define FLT   2    // 固定本地
#define WDT   4    // 无线长途
#define WLT   8    // 无限本地
#define WAP   16   // 无线接听

char *userTel[User_NUM] ={"15271766778","13429806441","18062612126"};
char *codeStr[User_NUM] ={"yv332a","rrh44c","0dh52m"};
static int login_flag =0 ;

void clear_screen()
{
    system("cls");
}  

void welcome_ui(const char *logUser)//ui登录界面的设计，char *logUser是声明了一个指向char类型的指针变量，来判断账号是否登录
{
       cout<<" "<<endl;
    printf(" \t\t\t= = =      |电信查询系统|  = = =  \n");
    if(NULL == logUser)//如果logUser为空则，代表没有登录，所以账号显示为待登录
    {
        printf("\t\t\t= = =  状态:[ 待登录 ]  = = =   \n");
    }
    else
    {
        printf("\t\t= = =  手机号码: %s  = = =\n",logUser);
    }
    printf("               ====================================================\n");
    printf("\t\t                  1. 登陆                      \n");
    printf("\t\t                  2. 查询电话账单                  \n");
    printf("\t\t                  0. 退出系统                  \n");
	cout<<"\t\t                  *使 用 事 项*                  "<<endl;
    cout<<"\t\t                  ①按1，然后输入您的手机号和密码                  "<<endl;
    cout<<"\t\t                  ②登录后按2，查询您本月电话账单                   "<<endl;
    
  
}

void load()                    
{
	int i, count = 0;
	char str[50], str1[50];
	time_t t1, *t = &t1;
	/*t1=time(NULL)或t1=time(0)
	将空指针传递给time()函数
	并将time()返回值赋给变量t1*/
	printf("\n\n\n\n\n\n");
	printf("\t*                                                *\n");
	printf("\t*                                                *\n");
	printf("\t*                                                *\n");
	printf("\t*                                                *\n");
	printf("\t*           * * * * *     * * * * *              *\n");
	printf("\t*           *       *     *       *              *\n");
	printf("\t*           *       *     *       *        * * * *\n");
	printf("\t*           *       *     *       *        *     *\n");
	printf("\t*           *       *     *       *        *     *\n");
	printf("\t*           *       *     *      * *       *     *\n");
	printf("\t* * * * *   * * * * *     * * *  * * *     * * * *\n");
	i = 6;
	do {
		t1 = time(NULL);   /*t是空指针（NULL），直接返回当前时间。*/
		strcpy(str, ctime(t));
		if (count == 6) break;
		while (strcmp(str, str1) != 0)
			/*strcmp函数是string compare(字符串比较)的缩写
			当s1<s2时，返回为负数；
			当s1=s2时，返回值 = 0；
			当s1>s2时，返回正数。*/
		{
			strcpy(str1, str);//这里的目的是一秒输出一个>然后转换页面
			printf(" >");
			count++;
			break;
		}
	} while (1);
	system("cls");//清屏操作
    
}

int matchUserAndCode(const char *tel,const char *code)//判断是否匹配机制
{
    int ret = -1,i =0,id =-1;
    if(NULL == tel || NULL == code )
        return -1;
    for(i =0 ;i < User_NUM;i++)
    {
        if( 0 ==strncmp(tel,userTel[i],11) )
        {
            id = i ;
            ret =0 ;
            break;
        }

    }
    if(-1 == ret)
    {
         ret = 1;
         return ret ;
    }

    if( 0 != strncmp(code,codeStr[id],6) )
    {
        ret = 2 ;
    }
    return ret ;
}


void log(char ss[])
{
    char user[12] ="\0",code[7] ="\0";
    int  matchState =0,count =1; //count的作用是计算输入错误的次数，以判断其是否会过试用次数

    while(count <= 3)
    {
        if(count >1)
        printf("\n\t\t             输入有错误,请重新输入               \n");
        printf("请输入的手机号:\n");
        printf("手机号:");
        scanf("%s",user);
        printf("密码:");
        
        scanf("%s",code);
        if(11 == strlen(user) && 6 == strlen(code))
            break;
        else
        {
            count++;//每输入错误一次，都会增加计数
        }
    }
    if(count >=4)
    {
        clear_screen();
        printf("==========  错误次数过多  ========== \n\n");
        welcome_ui(NULL);
    }
    else
    {
        matchState = matchUserAndCode(user,code);//按照return的数字配合if来完成相应的功能
        switch(matchState)
        {
            case 0://返回0，登录成功，改变登录状态
                clear_screen();
                load();
				clear_screen();
                printf("\n===  已 登 陆  ===\n\n");
                strncpy(ss,user,11);
                ss[11] ='\0';
                login_flag =1 ;
                welcome_ui(user);
                break;

            case 1://返回1，查询失败
                clear_screen();
                printf("      \n查询失败\n\n");
                welcome_ui(NULL);
                break;

            case 2:
                clear_screen();//返回2，说明输入的密码错误，返回最开始的登录ui
                printf("      \n密码错误\n\n");
                welcome_ui(NULL);
                break;

            default:
                break;

        }
    }
}

int queryBill(const char *logUser)
{
    int i = 0,j =0,id =-1;
    int k[6][2] = {0};
    int tel_time[6][5] = {0};
   char *ss[] ={"C:/Users/pc/Desktop/ccc/FDT.txt","C:/Users/pc/Desktop/ccc/FLT.txt","C:/Users/pc/Desktop/ccc/WDT.txt","C:/Users/pc/Desktop/ccc/WLT.txt","C:/Users/pc/Desktop/ccc/WAP.txt"};

    for(i =0 ;i < User_NUM;i++)
    {
        if( 0 == strncmp(logUser,userTel[i],11) )
        {
            id = i ;
            break;
        }
    }
    if(id <0 || id >5)
        return  -2;
    for(i =0;i < 5; i++)
    {
        FILE *fpRead = fopen(ss[i], "r");
        if (NULL == fpRead)//打开失败，返回-1
        {
            return -1;
        }
        j =0 ;
        while ( fscanf(fpRead, "%d|%d",\
                              &k[j][0], &k[j][1]) !=EOF ) // fscanf为文件标准输入，EOF是文件结束的标志，它等于NULL，也就是0.
        {
           
            if(j >5)
                break;
            else
                j++;

        }
        fclose(fpRead);

        for(j =0 ;j < 6;j++)
        {
            if(k[j][0] >=1 && k[j][0] <=6)
            {
                tel_time[k[j][0]-1][i] = k[j][1];
                j++;
            }
            else
            {
                printf("      信息被修改\n");
                welcome_ui(logUser);
                break;
            }
        }


     }
     printf("\n    您的本月话费账单如下:\n");
     printf(" 手机号码:%s\n",userTel[id]);
     printf("|||----      固定长途:%d分钟,产生费用%.2f元\n",tel_time[id][0],0.02*tel_time[id][0]);
     printf("|||----      固定本地:%d分钟,产生费用%.2f元\n",tel_time[id][1],0.06*tel_time[id][1]);
     printf("|||----      无线长途:%d分钟,产生费用%.2f元\n",tel_time[id][2],1.00*tel_time[id][2]);
     printf("|||----      无线本地:%d分钟,产生费用%.2f元\n",tel_time[id][3],0.60*tel_time[id][3]);
     printf("|||----      无线接听:%d分钟,产生费用%.2f元\n",tel_time[id][4],0.50*tel_time[id][4]);
     printf("|||----      总计费用%.2f元\n\n",0.02*tel_time[id][0]\
            +0.06*tel_time[id][1]+1.00*tel_time[id][2]\
            +0.60*tel_time[id][3]+0.50*tel_time[id][4]);

}

void logOut(char *user)
{
    if(NULL == user)
        return;
    
    login_flag =0 ;
    clear_screen();
    welcome_ui(NULL);
}

int main(void)
{
    char activeUser[12] = {"\0"};
    int user_choice =-1 ,flag =1;

    welcome_ui(NULL);
    while(flag)
    {
        printf("\t\t            请输入需要的服务:    ");
        scanf("%d",&user_choice);
        if( 1 == user_choice )
        {
            log(activeUser);
        }
        else if(2 == user_choice)
        {
            if( 0 == login_flag)
            {
                printf("\n\t\t===========   系统需先登录   ===========\n\n");
                welcome_ui(NULL);
            }
            else
            {
                queryBill(activeUser);
            }
        }
        
        else if( 0 == user_choice)
        {
            printf("\n\t\t===========  服务已结束 ===========\n");
            flag =0 ;
        }

    }
    system("pause");
    return 0;
}

//答案：https://blog.csdn.net/Zcymatics/article/details/106965348