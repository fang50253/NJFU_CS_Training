#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct PolyNode
{
    int factor;
    int power;
    struct PolyNode *next;
} PolyNode, *PolyList;
void insertTerm(PolyList *L, int factor, int power)
{
    PolyNode *newNode = (PolyNode *)malloc(sizeof(PolyNode));
    newNode->factor = factor;
    newNode->power = power;
    newNode->next = NULL;
    if (*L==NULL||(*L)->power<power)
    {
        newNode->next=*L;
        *L=newNode;
    }
    else
    {
        PolyNode *p=*L;
        while(p->next&&p->next->power>power) p=p->next;
        if (p->power == power)
        {
            p->factor+=factor;
            free(newNode);
            if (p->factor == 0)
            {
                PolyNode *temp = p->next;
                free(p);
                *L = temp;
            }
        }
        else
        {
            newNode->next = p->next;
            p->next = newNode;
        }
    }
}
void print(PolyList L)
{
    if (L == NULL)
    {
        printf("0\n");
        return;
    }
    PolyNode *p = L;
    while (p)
    {
        if (p->factor>0&&p!=L) printf("+");
        if (p->power==0) printf("%d",p->factor);
        else if(p->power==1&&p->factor==1) printf("x");
        else if(p->power==1&&p->factor==-1) printf("-x");
        else if(p->power==1&&p->factor!=1&&p->factor!=-1) printf("%dx",p->factor);
        else if(p->factor==1) printf("x%d", p->power);
        else if(p->factor==-1) printf("-x%d",p->power);
        else printf("%dx%d", p->factor, p->power);
        p = p->next;
    }
    printf("\n");
}
PolyList addPoly(PolyList A, PolyList B)
{
    PolyList result=NULL;
    PolyNode *pa=A,*pb=B;
    while (pa&&pb)
    {
        if (pa->power>pb->power)
        {
            insertTerm(&result,pa->factor,pa->power);
            pa = pa->next;
        }
        else if(pa->power<pb->power)
        {
            insertTerm(&result,pb->factor,pb->power);
            pb=pb->next;
        }
        else
        {
            int sum=pa->factor+pb->factor;
            if(sum!=0) insertTerm(&result,sum,pa->power);
            pa=pa->next;
            pb=pb->next;
        }
    }
    while (pa)
    {
        insertTerm(&result,pa->factor,pa->power);
        pa=pa->next;
    }
    while (pb)
    {
        insertTerm(&result,pb->factor,pb->power);
        pb=pb->next;
    }
    return result;
}
PolyList subtractPoly(PolyList A,PolyList B)
{
    PolyList result=NULL;
    PolyNode *pa=A,*pb=B;
    while (pa && pb)
    {
        if (pa->power>pb->power)
        {
            insertTerm(&result,pa->factor,pa->power);
            pa = pa->next;
        }
        else if (pa->power<pb->power)
        {
            insertTerm(&result,-pb->factor,pb->power);
            pb = pb->next;
        }
        else
        {
            int diff = pa->factor - pb->factor;
            if(diff!=0) insertTerm(&result,diff,pa->power);
            pa=pa->next;
            pb=pb->next;
        }
    }
    while (pa)
    {
        insertTerm(&result,pa->factor,pa->power);
        pa = pa->next;
    }
    while (pb)
    {
        insertTerm(&result,-pb->factor,pb->power);
        pb=pb->next;
    }
    return result;
}
void add(PolyList *L)
{
    char ch[100];
    scanf("%s",&ch);
    int factor=0,sign=1,power=0;
    for(int i=0;i<strlen(ch);++i)
    {
        if(ch[i]=='-')
        {
            sign=-1;
            continue;
        }
        else if(ch[i]=='+') continue;
        else if(ch[i]=='x')
        {
            if(factor==0) factor=1;
            for(++i;ch[i]!='+'&&ch[i]!='-';++i)
            {
                power*=10;
                power+=ch[i]-'0';
            }
            if(power==0&&(ch[i]=='+'||ch[i]=='-')) power=1;
            insertTerm(L,sign*factor,power);
            --i;
            power=0;factor=0;sign=1;
        }
        else
        {
            factor*=10;
            factor+=ch[i]-'0';
        }
    }
    printf("fa:%d",factor);
    if(factor!=0) insertTerm(L,factor,0);
}
int main()
{
    PolyList A=NULL;
    PolyList B=NULL;
    add(&A);
    add(&B);
    PolyList C = addPoly(A,B);
    printf("A+B:");
    print(C);
    C=subtractPoly(A,B);
    printf("A-B:");
    print(C);
    return 0;
}