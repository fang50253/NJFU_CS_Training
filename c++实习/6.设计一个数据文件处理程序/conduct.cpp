#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
const int LINE_MAX=2e5;
const int VECTOR_MAX=129;
class Line
{
    private:
    char line[LINE_MAX];
    int vectors[VECTOR_MAX];
    FILE *fp,*wfp,*lfp;
    int eigenvector;//记录特征向量，也就是第一个参数的值
    void read_file(const char filename[])
    {
        fp=fopen(filename,"r+");
        if(fp==nullptr) printf("File open failed\n");
    }
    void write_file(const char filename[],const char lable[])
    {
        wfp=fopen(filename,"w");
        lfp=fopen(lable,"w");
        if(wfp==nullptr||lfp==nullptr) printf("File open failed\n");
    }
    public:
    bool read_one_line()//读取成功返回1，否则返回0
    {
        fgets(line,sizeof line,fp);
        int linelength=strlen(line);
        int tmp=0,tmp_cnt=0;
        int vector_position;//记录向量的位置
        memset(vectors,0,sizeof vectors);
        line[linelength]=' ';
        line[linelength+1]='\0';
        //printf("size:%d\n",linelength);
        //printf("%s",line);
        for(int i=0;i<linelength+1;++i)
        {
            if(line[i]!=' '&&line[i]!=':')
            {
                tmp=tmp*10+line[i]-'0';
            }
            else if(line[i]==' '&&tmp_cnt==0)
            {
                eigenvector=tmp;
                tmp=0;
                ++tmp_cnt;
            }
            else if(line[i]==' '||i==linelength-1) 
            {
                vectors[vector_position]=tmp;
                if(vector_position==128)
                {
                    //printf("%d\n",tmp);
                    //exit(0);
                }
                //printf("Write in:%d,value:%d\n",vector_position,tmp);
                tmp=0;
                ++tmp_cnt;
            }
            else if(line[i]==':')
            {
                vector_position=tmp;
                tmp=0;
                ++tmp_cnt;
            }
        }
        if(feof(fp)) return 0;
        else return 1;
    }
    void write_one_line()
    {
        for(int i=1;i<=1<<7;++i)
        {
            fprintf(wfp,"%d ",vectors[i]);
        }
        fprintf(wfp,"\n");
        fprintf(lfp,"%d ",eigenvector);
    }
    void print_newest_line()
    {
        for(int i=1;i<=1<<7;++i)
        {
            printf("%d ",vectors[i]);
        }
    }
    void restore_file(const char lable[],const char full[],const char restore[])
    {
        FILE *restore_lable=fopen(lable,"r+");
        FILE *restore_full=fopen(full,"r+");
        FILE *restore_restore=fopen(restore,"w");
        while(!feof(restore_lable))
        {
            fscanf(restore_lable,"%d",&eigenvector);
            fprintf(restore_restore,"%d ",eigenvector);
            fgets(line,sizeof line,restore_full);
            int linelength=strlen(line);
            line[linelength]=' ';
            int tmp=0,tmp_cnt=0;
            for(int i=0;i<linelength;++i)
            {
                if(line[i]==' ')
                {
                    ++tmp_cnt;
                    if(tmp==0) continue;
                    else
                    {
                        fprintf(restore_restore,"%d:%d ",tmp_cnt,tmp);
                    }
                    tmp=0;
                }
                else tmp=tmp*10+line[i]-'0';
            }
            fprintf(restore_restore,"\n");
        }
    }
    Line(const char readfile[],const char writefile[],const char writelable[])
    {
        read_file(readfile);
        write_file(writefile,writelable);
    }
    ~Line()
    {
        memset(line,0,sizeof line);
        fclose(fp);
        fclose(lfp);
        fclose(wfp);
    }
};
int main()
{
    Line *line=new Line("aloi","aloi_full.txt","aloi_lable.txt");
    int cnt=0;
    clock_t start=clock();
    double cpu_time_used;
    while(line->read_one_line()) line->write_one_line();
    //line->print_newest_line();
    clock_t end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("Unzip_Process_Time%.2fseconds\n", cpu_time_used);
    start=clock();
    line->restore_file("aloi_lable.txt","aloi_full.txt","aloi_restore.txt");
    delete line;
    end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("Zip_Process_Time%.2fseconds\n", cpu_time_used);
    return 0;
}