#include<stdio.h>
typedef struct
{
    int num,deno;
}RatNum;
double fabs(double x)
{
    return (x>0)?x:(-x);
}
int gcd(int a,int b)
{
    return b?gcd(b,a%b):a;
}
int lcm(int a,int b)
{
    return a/gcd(a,b)*b;
}
void create(RatNum *p)
{
    printf("input a num:");
    scanf("%d %d",&(p->num),&(p->deno));
}
void print(RatNum a)
{
    if(a.deno==1) printf("%d",a.num); 
    else printf("%d/%d",a.num,a.deno);
}
RatNum approximation(RatNum a)
{
    int factor=gcd(a.num,a.deno);
    a.deno/=factor;
    a.num/=factor;
    if((a.deno<0&&a.num>0)||(a.deno<0&&a.num<0))
    {
        a.num=-a.num;
        a.deno=-a.deno;
    }
    return a;
}
RatNum add(RatNum a,RatNum b)
{
    RatNum res;
    res.num=a.num*b.deno+b.num*a.deno;
    res.deno=a.deno*b.deno;
    return approximation(res);
}
RatNum sub(RatNum a,RatNum b)
{
    RatNum res;
    res.num=a.num*b.deno-b.num*a.deno;
    res.deno=a.deno*b.deno;
    return approximation(res);
}
RatNum muti(RatNum a,RatNum b)
{
    RatNum res;
    res.num=a.num*b.num;
    res.deno=a.deno*b.deno;
    return approximation(res);
}
RatNum div(RatNum a,RatNum b)
{
    RatNum res={0,0};
    if(a.deno==0||b.deno==0)
    {
        printf("error\n");
        return res;
    }
    res.num=a.num*b.deno;
    res.deno=a.deno*b.num;
    return approximation(res);
}
RatNum Create_RatNum(double x, double error) {
    int sign=(x<0)?-1:1;
    x=(x>0)?x:(-x);
    double lower_n=0,lower_d=1;
    double upper_n=1,upper_d=0;
    int num=1,denom=1;
    while (1)
    {
        num=lower_n+upper_n;
        denom=lower_d+upper_d;
        double approx = (double)num / denom;
        if (fabs(approx - x) < error)
        {
            RatNum result = {sign * num, denom};
            return result;
        }
        if (approx < x)
        {
            lower_n = num;
            lower_d = denom;
        } else
        {
            upper_n = num;
            upper_d = denom;
        }
    }
}
int main()
{
    int ch;
    RatNum a,b,res;
    printf("1.Create from a decimal\n2.clac\n");
    scanf("%d",&ch);
    if(ch==1)
    {
        printf("Input the num and the maxmize error\n");
        double input,error;
        scanf("%lf",&input);
        RatNum x=Create_RatNum(input,error);
        print(x);
    }
    else
    {
        printf("1.add\n2.sub\n3.muti\n4.div\n");
        scanf("%d",&ch);
        printf("give the first number\n");
        create(&a);
        printf("give the second number\n");
        create(&b);
        switch (ch)
        {
            case 1:res=add(a,b);break;
            case 2:res=sub(a,b);break;
            case 3:res=muti(a,b);break;
            case 4:res=div(a,b);break;
            default:break;
        }
        print(res);
    }
    return 0;
}