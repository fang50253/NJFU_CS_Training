#include<stdio.h>
#include<iostream>
#include<string.h>
#include<stdexcept>
#define STUDENT_NUMBER 100
enum Sex{male,female};
enum Subject{english,math,program,physical};
enum Mode{id,name};
struct Student
{
    char name[100];//姓名
    char id[100];//学号
    Sex sex;//性别
    int english;//英语成绩*10
    int math;//数学成绩*10
    int program;//程序设计成绩*10
    int physical;//体育成绩*10
};
bool cmp_english(struct Student a,struct Student b)
{
    return a.english<b.english;
}
bool cmp_math(struct Student a,struct Student b)
{
    return a.math<b.math;
}
class Manager
{
    private:
    static struct Student student[STUDENT_NUMBER];
    static int num;//表示学生的数量
    void swap(struct Student &a,struct Student &b)
    {
        auto c=a;
        a=b;
        b=c;
    }
    void read(char filename[])
    {
        FILE *fp=fopen(filename,"r+");
        if(fp==NULL)
        {
            printf("failed to open the file\n");
            exit(0);
        }
        fscanf(fp,"%d",&num);
        for(int i=1;i<=num;++i)
        {
            fscanf(fp,"%s",student[i].name);
            fscanf(fp,"%s",student[i].id);
            fscanf(fp,"%d",&student[i].sex);
            fscanf(fp,"%d",&student[i].english);
            fscanf(fp,"%d",&student[i].math);
            fscanf(fp,"%d",&student[i].program);
            fscanf(fp,"%d",&student[i].physical);
        }
        fclose(fp);
    }
    void write(char filename[])
    {
        FILE *fp=fopen(filename,"r+");
        if(fp==NULL)
        {
            printf("failed to open the file\n");
            exit(0);
        }
        fprintf(fp,"%d ",num);
        for(int i=1;i<=num;++i)
        {
            fprintf(fp,"%s ",student[i].name);
            fprintf(fp,"%s ",student[i].id);
            fprintf(fp,"%d ",student[i].sex);
            fprintf(fp,"%d ",student[i].english);
            fprintf(fp,"%d ",student[i].math);
            fprintf(fp,"%d ",student[i].program);
            fprintf(fp,"%d ",student[i].physical);
        }
        fclose(fp);
    }
    int query_name(char name[])
    {
        for(int i=1;i<=num;++i)
        {
            if(strcmp(name,student[i].name)==0)
            {
                return i;
            }
        }
        throw std::runtime_error("query failed\n");
        exit(0);
    }
    int query_id(char id[])
    {
        for(int i=1;i<=num;++i)
        {
            if(strcmp(id,student[i].id)==0)
            {
                return i;
            }
        }
        throw std::runtime_error("query failed\n");
        exit(0);
    }
    void display_one(int index)
    {
        printf("name:%s ",student[index].name);
        printf("id:%s ",student[index].id);
        if(student[index].sex==male) printf("sex:male ");
        else printf("sex:female ");
        printf("English_score:%.1lf ",student[index].english/10.0);
        printf("Math_score:%.1lf ",student[index].math/10.0);
        printf("Program_score:%.1lf ",student[index].program/10.0);
        printf("Physical_score:%.1lf \n",student[index].physical/10.0);
    }
    public:
    Manager()
    {
        memset(student,0,sizeof student);
        char filename[]="student.dat";
        read(filename);
    }
    ~Manager()
    {
        memset(student,0,sizeof student);
        char filename[]="student.dat";
        write(filename);
    }
    void sort(Subject subject,bool rules)
    //采用冒泡排序,rules=1
    {
        for(int j=1;j<=num;++j)
        {
            for(int i=2;i<=num;++i)
            {
                bool cmp;
                switch(subject)
                {
                    case english:
                    {
                        cmp=student[i-1].english<student[i].english;
                        break;
                    }
                    case math:
                    {
                        cmp=student[i-1].math<student[i].math;
                        break;
                    }
                    case program:
                    {
                        cmp=student[i-1].program<student[i].program;
                        break;
                    }
                    case physical:
                    {
                        cmp=student[i-1].physical<student[i].physical;
                        break;
                    }
                }
                cmp=rules?cmp:(!cmp);
                if(cmp) swap(student[i-1],student[i]);
            }
        }
    }
    void query(char q[],Mode mode)
    {
        if(mode==id)
        {
            try
            {
                display_one(query_id(q));
            }
            catch(const std::exception& e)
            {
                std::cerr<<e.what()<<std::endl;
            }
        }
        else
        {
            try
            {
                display_one(query_name(q));
            }
            catch(const std::exception& e)
            {
                std::cerr<<e.what()<<std::endl;
            }
        }
    }
    void average()
    {
        int english=0,math=0,physical=0,program=0;
        for(int i=1;i<=num;++i)
        {
            english+=student[i].english;
            math+=student[i].math;
            program+=student[i].program;
            physical+=student[i].physical;
        }
        printf("English_Average:%.1lf\n",english/num/10.0);
        printf("Math_Average:%.1lf\n",math/num/10.0);
        printf("Program_Average:%.1lf\n",program/num/10.0);
        printf("Physical_Average:%.1lf\n",physical/num/10.0);
    }
    void create()
    {
        ++num;
        printf("name:");
        scanf("%s\n",student[num].name);
        printf("id:");
        scanf("%s\n",student[num].id);
        printf("sex(male/female):");
        char sex[10];
        scanf("%s\n",sex);
        if(strcmp(sex,"male")==0) student[num].sex=male;
        else student[num].sex=female;
        printf("English_score:");
        double tmp;
        scanf("%lf\n",&tmp);
        student[num].english=tmp*10;
        printf("Math_score:");
        scanf("%lf\n",&tmp);
        student[num].math=tmp*10;
        printf("Program_score:");
        scanf("%lf\n",&tmp);
        student[num].program=tmp*10;
        printf("Physical_score:");
        scanf("%lf\n",&tmp);
        student[num].physical=tmp*10;
    }
};
int main()
{

}