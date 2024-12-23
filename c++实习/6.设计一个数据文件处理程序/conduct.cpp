#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
const int LINE_MAX=1e5;
const int VECTOR_MAX=1<<8;
const int SAMPLE_MAX=1e3+10;
class Line
{
    private:
    char line[LINE_MAX];
    int vectors[VECTOR_MAX];
    int sample_cnt[SAMPLE_MAX];
    FILE *fp,*wfp,*lfp;
    int sample_num;
    int eigenvector;//记录特征向量，也就是第一个参数的值
    int feature=0;
    int cnt_feature(const char filename[])
    {
        FILE *fp_tmp = fopen(filename, "r+");
        if(fp_tmp==NULL)
        {
            printf("File open failed\n");
            return -1;
        }
        int max_feature=0;
        do
        {
            fgets(line,sizeof line,fp_tmp);
            if(feof(fp_tmp)) return max_feature;
            int linelength=strlen(line);
            int tmp=0,tmp_cnt=0;
            int vector_position;//记录向量的位置
            memset(vectors,0,sizeof vectors);
            line[linelength-1]=' ';
            line[linelength]='\0';
            linelength=strlen(line);
            for(int i=0;i<linelength+1;++i)
            {
                if(line[i]!=' '&&line[i]!=':')
                {
                    tmp=tmp*10+line[i]-'0';
                }
                else if(line[i]==' '&&tmp_cnt==0)
                {
                    eigenvector=tmp;
                    ++sample_cnt[tmp];
                    tmp=0;
                }
                else if(line[i]==' '||i==linelength-1) 
                {
                    tmp=0;
                    max_feature=max(max_feature,vector_position);
                    ++tmp_cnt;
                }
                else if(line[i]==':')
                {
                    vector_position=tmp;
                    tmp=0;
                    ++tmp_cnt;
                }
            }
        }while(!feof(fp_tmp));
        fclose(fp_tmp);
        return max_feature;
    }
    int max(int a,int b)
    {
        return a>b?a:b;
    }
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
    void rtrim(char *str) //作用是删除字符串最后的空格和换行，便于比较
    {
        size_t len=strlen(str);
        while(len>0&&(str[len-1]==' '||str[len-1]=='\n')) str[--len]='\0';
    }
    public:
    bool read_one_line()//读取成功返回1，否则返回0
    {
        ++sample_num;
        fgets(line,sizeof line,fp);
        int linelength=strlen(line);
        int tmp=0,tmp_cnt=0;
        int vector_position;//记录向量的位置
        memset(vectors,0,sizeof vectors);
        line[linelength-1]=' ';
        line[linelength]='\0';
        linelength=strlen(line);
        for(int i=0;i<linelength+1;++i)
        {
            if(line[i]!=' '&&line[i]!=':')
            {
                tmp=tmp*10+line[i]-'0';
            }
            else if(line[i]==' '&&tmp_cnt==0)
            {
                eigenvector=tmp;
                ++sample_cnt[tmp];
                tmp=0;
                ++tmp_cnt;
            }
            else if(line[i]==' '||i==linelength-1) 
            {
                vectors[vector_position]=tmp;
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
        fprintf(lfp,"%d ",eigenvector);
        for(int i=1;i<=feature;++i)
        {
            fprintf(wfp,"%d ",vectors[i]);
        }
        fprintf(wfp,"\n");
    }
    void print_newest_line()
    {
        for(int i=1;i<=feature;++i)
        {
            printf("%d ",vectors[i]);
        }
    }
    void restore_file(const char lable[],const char full[],const char restore[])
    {
        FILE *restore_lable=fopen(lable,"r+");
        FILE *restore_full=fopen(full,"r+");
        FILE *restore_restore=fopen(restore,"w");
        do
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
        }while(!feof(restore_full)||!feof(restore_lable));
        fclose(restore_lable);
        fclose(restore_full);
        fclose(restore_restore);
    }
    void display_sample()
    {
        int cnt=0;
        for(int i=0;i<=999;++i)
            if(sample_cnt[i]) ++cnt;
        printf("Eigenvector_Cnt:%d\n",feature);
        printf("Sample_Cnt:%d\n",sample_num-1);
    }
    void compare(const char file_1[],const char file_2[])
    {
        FILE *file1=fopen(file_1,"r+");
        FILE *file2=fopen(file_2,"r+");
        if (!file1||!file2) 
        {
            printf("Compare:File open failed\n");
            if (file1) fclose(file1);
            if (file2) fclose(file2);
        }
        char f1[LINE_MAX],f2[LINE_MAX];
        int line_number=0;
        while(1) 
        {
            char *line1=fgets(f1,sizeof(f1),file1);
            char *line2=fgets(f2,sizeof(f2),file2);
            ++line_number;
            if((!line1&&!line2)||strcmp(line1,"\n")||strcmp(line2,"\n")) 
            {
                fclose(file1);
                fclose(file2);
                printf("File is same\n");
                return;
            }
            if((!line1||!line2)&&(strlen(line1)>2||strlen(line2)>2)) 
            {
                printf("Line %d different\n",line_number);
                printf("f1:%s",f1);
                printf("f2:%s\n",f2);
                fclose(file1);
                fclose(file2);
                return;
            }
            rtrim(f1);
            rtrim(f2);
            if (strcmp(f1,f2)!=0) 
            {
                printf("Line %d different\n",line_number);
                printf("f1:%s\n",f1);
                printf("f2:%s\n",f2);
                fclose(file1);
                fclose(file2);
                return;
            }
        }
    }
    Line(const char readfile[],const char writefile[],const char writelable[])
    {
        feature=cnt_feature(readfile);
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
    line->display_sample();
    //line->print_newest_line();
    clock_t end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("Unzip_Process_Time%.2fseconds\n", cpu_time_used);
    start=clock();
    line->restore_file("aloi_lable.txt","aloi_full.txt","aloi_restore.txt");
    end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("Zip_Process_Time%.2fseconds\n", cpu_time_used);
    start=clock();
    line->compare("aloi","aloi_restore.txt");
    //printf("%d",line->debug);
    delete line;
    end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("Compare_Process_Time%.2fseconds\n", cpu_time_used);
    return 0;
}