#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct PolyNode
{
    int factor;//系数
    int power;//指数
    struct PolyNode *next;
    //next指针，指向下一个结点
}PolyNode, *PolyList;
//创建结点类型和结点指针类型
void insertTerm(PolyList *L, int factor, int power)
{
    PolyNode *newNode = (PolyNode *)malloc(sizeof(PolyNode));
    //创建一个新的结点
    newNode->factor = factor;
    //传入新结点的系数
    newNode->power = power;
    //传入新结点的指数
    newNode->next = NULL;
    //新建的结点没有下一个指针位置
    if (*L==NULL||(*L)->power<power)
    //如果说发现需要插入的链表为空，或者是新插入的这个结点的次数高于原有的最高次数
    {
        newNode->next=*L;
        //那么直接将该结点插入头节点即可
        *L=newNode;
        //修改头指针的位置
    }
    else
    {
        PolyNode *p=*L;
        //新建一个p指针，用于遍历链表
        while(p->next&&p->next->power>power) p=p->next;
        //扫描链表，直到次数相等或者次数刚好大于这个点
        if (p->power == power)
        //如果恰巧停下来的时候，次数相等，那么合并
        {
            p->factor+=factor;
            //将系数直接相加
            free(newNode);
            //释放这个已经合并同类项的点
            if (p->factor == 0)
            //如果次数干好为0，那么把合并以后的点也直接释放掉
            {
                PolyNode *temp = p->next;
                //删除操作，先保存后面一个结点的指针，先连接后删除
                free(p);
                *L = temp;
            }
        }
        else
        {
            newNode->next = p->next;
            //否则，将新的结点插入链表
            p->next = newNode;
        }
    }
}
void print(PolyList L)
//这个函数用于输出表达式
{
    if (L == NULL)
    //如果表达式保存的链表为空，说明表达式不存在，直接输出0即可
    {
        printf("0\n");
        return;
    }
    PolyNode *p = L;
    //声明一个指针类型p，用于遍历链表
    while (p)
    {
        if (p->factor>0&&p!=L) printf("+");
        //在非头节点的情况底下，如果系数大于0，那么需要输出正号
        if (p->power==0) printf("%d",p->factor);
        //如果次数正号为0，那么直接输出系数即可
        else if(p->power==1&&p->factor==1) printf("x");
        //如果系数和次数都恰好为1，那么直接输出x
        else if(p->power==1&&p->factor==-1) printf("-x");
        //如果次数为1，系数为-1，那么直接输出-x
        else if(p->power==1&&p->factor!=1&&p->factor!=-1)
            printf("%dx",p->factor);
            //如果次数恰好为1，系数无特殊输出系数和x
        else if(p->factor==1) printf("x%d", p->power);
        //如果系数恰好为1，且次数无特殊，那么输出x和次数
        else if(p->factor==-1) printf("-x%d",p->power);
        //如果系数为-1，次数无特殊，那么输出-x和次数
        else printf("%dx%d", p->factor, p->power);
        //否则，输出系数、x、次数
        p = p->next;
        //遍历下一个结点
    }
    printf("\n");
    //输出换行，输出完成
}
PolyList addPoly(PolyList A, PolyList B)
//新建加法运算
{
    PolyList result=NULL;
    //首先先清空答案result链表
    PolyNode *pa=A,*pb=B;
    //创建两个指针类型变量pa，pb，用于遍历两个链表
    while (pa&&pb)
    //如果在pa和pb都没有走到头掉情况底下，需要比较两个链表的次数
    {
        if (pa->power>pb->power)
        //如果说a链表的次数大于b链表的次数，那么需要先计算a链表里值
        {
            insertTerm(&result,pa->factor,pa->power);
            //调用insert函数将a链表中pa指向的系数和次数插入result链表中
            pa=pa->next;
            //pa走向下一个结点
        }
        else if(pa->power<pb->power)
        //同样，如果说b链表的次数大于a链表的次数，那么需要先计算b链表里值
        {
            insertTerm(&result,pb->factor,pb->power);
            //调用insert函数将b链表中pb指向的系数和次数插入result链表中
            pb=pb->next;
            //pb走向下一个结点
        }
        else
        //最后只剩下了两者次数相等的情况
        {
            int sum=pa->factor+pb->factor;
            //这种情况下，把两个系数相加即可
            if(sum!=0) insertTerm(&result,sum,pa->power);
            //如果说相加得到的结果为0，也没有必要自己给自己找麻烦，直接跳过插入操作
            //如果相加得到的结果不为0，那么直接将这样一个系数和次数插入到result链表当中
            pa=pa->next;
            pb=pb->next;
            //两个指针移向下位
        }
    }
    while (pa)
    //收尾，如果pb已经走到了链表的表尾，将pa走到底就行了
    {
        insertTerm(&result,pa->factor,pa->power);
        pa=pa->next;
    }
    while (pb)
    //收尾，如果pa已经走到了链表的表尾，将pb走到底就行了
    {
        insertTerm(&result,pb->factor,pb->power);
        pb=pb->next;
    }
    return result;
    //把答案返回给主调函数
}
PolyList subPoly(PolyList A,PolyList B)
//这是一个减法的过程，实现原理和上面是类似的
{
    PolyList result=NULL;
    //首先定义一个结果链表，这个链表原始是空的，因为里面什么也没有
    PolyNode *pa=A,*pb=B;
    //定义两个用于循环的指针pa和pb
    while (pa&&pb)
    //如果指向两个链表的指针都没有走到表尾
    {
        if (pa->power>pb->power)
        //如果pa指向的结点的次数高于pb指向结点的次数
        {
            insertTerm(&result,pa->factor,pa->power);
            //将pa的结点直接插入链表中即可
            pa = pa->next;
            //pa走向下一个结点
        }
        else if (pa->power<pb->power)
        //相反的，如果pb指向的结点的次数高于pa指向的结点的次数
        {
            insertTerm(&result,-pb->factor,pb->power);
            //那么pb指向的结点，取相反数以后插入result链表中即可
            //注意，这里和上面的加法不一样，因为b是被减数，所以需要取负数
            pb = pb->next;
            //pb走向下一个结点
        }
        else
        //最后只剩下了一种情况，就是两者的次数恰好相等
        {
            int ans=pa->factor-pb->factor;
            //那么将两者的系数相减
            if(ans!=0) insertTerm(&result,ans,pa->power);
            //如果两者系数相减正号为0，也没必要白费功夫了，直接跳过即可
            //将结点插入result当中
            pa=pa->next;
            pb=pb->next;
            //pa，pb均可以走向下一个结点
        }
    }
    while (pa)//收尾工作
    {
        insertTerm(&result,pa->factor,pa->power);
        pa = pa->next;
        //将pa剩下的结点全部插入到result中
    }
    while (pb)
    {
        insertTerm(&result,-pb->factor,pb->power);
        pb=pb->next;
        //将pb剩下的结点全部插入道result中
    }
    return result;
    //将结果返回给主调函数
}
void add(PolyList *L)
//这是一个读入字符串分离字符建立链表的字符串操作函数
{
    char ch[100];
    //本程序可以接受的最大表达式长度为100
    scanf("%s",&ch);
    //将字符串读入程序中
    int factor=0,sign=1,power=0;
    //定义三个变量，factor用于指示系数
    //sign用于指示符号
    //power用于指示次数
    for(int i=0;i<strlen(ch);++i)
    //依次遍历整个字符串
    {
        if(ch[i]=='-')
        //如果遇到了负号，将系数取相反数
        {
            sign=-1;
            continue;
        }
        else if(ch[i]=='+') continue;
        //如果遇到了加号，说明不需要处理
        else if(ch[i]=='x')
        //如果遇到了x，那就说明遇到了系数和次数的分界点
        {
            if(factor==0) factor=1;
            //如果系数为0，那就说明x前面什么都没有，那系数就是1
            for(++i;ch[i]!='+'&&ch[i]!='-';++i)
            //通过一个循环读取次数
            {
                power*=10;
                power+=ch[i]-'0';
            }
            if(power==0&&(ch[i]=='+'||ch[i]=='-')) power=1;
            //如果还没读就结束了，说明次数也是1
            insertTerm(L,sign*factor,power);
            //将这个结点插入道链表当中
            --i;
            //多读了一个符号位，说不定后面需要特殊处理，返回去重新处理
            power=0;factor=0;sign=1;//将标志还原
        }
        else
        {
            factor*=10;
            factor+=ch[i]-'0';
            //这两个代码用来读取系数
        }
    }
    if(factor!=0) insertTerm(L,factor*sign,0);
    //收尾工作，如果最后没有读到x，那就说明存在0次方项，将0次方项插入链表
}
int main()
{
    PolyList A=NULL;//新建两个链表
    PolyList B=NULL;
    add(&A);//读入A
    add(&B);
    PolyList C = addPoly(A,B);//C是答案，AB相加
    printf("A+B:");
    print(C);
    C=subPoly(A,B);//C是答案，AB相减
    printf("A-B:");
    print(C);
    return 0;//程序运行结束
}