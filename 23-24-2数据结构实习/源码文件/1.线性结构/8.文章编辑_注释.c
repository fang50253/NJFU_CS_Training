#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define TEXT_MAX 100000
// 定义文本最大长度
// 从文件中读取文章
char* readFile(const char filename[])
{
    FILE* input = fopen(filename, "r");  
    // 打开文件
    char* text = (char*)malloc(sizeof(char) * TEXT_MAX);  
    // 分配内存
    text[0] = '\0';  
    // 初始化字符串为空
    if (input)
    {
        int pos = 0;
        while (!feof(input))
        {
            // 从文件中逐个读取字符到数组
            text[pos++] = (char)fgetc(input);
        }
        text[pos] = '\0';  
        // 添加字符串结束符
        fclose(input);  
        // 关闭文件
    }
    return text;  
    // 返回读取的文本
}
// 输入文章
char* inputText() 
{
    char* text = (char*)malloc(sizeof(char) * TEXT_MAX);  
    // 分配内存
    int line;
    printf("\n");
    printf("输入行数：");
    scanf("%d", &line);  
    // 输入行数
    printf("输入内容：\n");
    text[0] = '\0';  
    // 初始化字符串为空
    int i = 0;
    while (i < line) 
    {
        char buffer[1024] = {0};  
        // 缓冲区
        fgets(buffer, 1024, stdin);  
        // 从标准输入读取一行
        if (strcmp(buffer, "\n") == 0) 
        {  
            // 如果是空行，则跳过
            continue;
        }
        strcat(text, buffer);  
        // 将缓冲区内容连接到文本
        ++i;
    }
    printf("\n");
    return text;  
    // 返回输入的文本
}

// 显示文章
void printText(const char text[]) 
{
    const char* pstr = text;
    printf("\n");
    while (*pstr) 
    {
        printf("%c", *pstr);  
        // 输出文本内容
        ++pstr;
    }
    printf("\n");
}

// 统计字符
void statCharacters(const char text[]) 
{
    const char* pstr = text;
    int letter = 0;  // 统计字母个数
    int number = 0;  // 统计数字个数
    int blank = 0;   // 统计空格个数
    int total = 0;   // 统计总字符数
    while (*pstr) 
    {
        if (isalpha(*pstr))
            // 如果是字母，累加字母个数
            ++letter;
        else if (isalnum(*pstr))
            // 如果是数字，累加数字个数
            ++number;
        else if (isblank(*pstr))
            // 如果是空格，累加空格个数
            ++blank;
        ++total;  // 累加总字符数
        ++pstr;
    }
    printf("字母：%d\n", letter);  // 输出字母个数
    printf("数字：%d\n", number);  // 输出数字个数
    printf("空格：%d\n", blank);   // 输出空格个数
    printf("总数：%d\n", total);   // 输出总字符数
}

// 统计字符串
void statString(const char text[]) 
{
    const char* pstr = text;
    char word[256] = {0};
    int count = 0;
    printf("请输入字符串：");
    scanf("%s", word);  // 输入要统计的字符串
    while ((pstr = strstr(pstr, word))) 
    {
        // 查找字符串并统计出现次数
        ++count;
        pstr += strlen(word);
    }
    printf("出现次数：%d\n", count);  // 输出字符串出现次数
}

// 删除字符串
void removeString(const char text[]) 
{
    const char* pstr = text;
    char word[256] = { 0 };
    printf("请输入字符串：");
    scanf("%s", word);  // 输入要删除的字符串
    while (1) 
    {
        char* target = strstr(pstr, word);
        if (target == NULL)
            break;  // 如果未找到字符串，则退出
        else 
        {
            // 找到要删除的字符串，则将后续内容前移
            while (*(target + strlen(word))) 
            {
                *target = *(target + strlen(word));
                ++target;
            }
            *target = '\0';  // 添加字符串结束符
        }
    }
    printf("\n");
}

// 主函数
int main() 
{
    char* text = readFile("article.fzy");  // 从文件读取文章
    if (text) 
    {
        while (1) 
        {
            int op;
            printf(" 1 显示文章\n");
            printf(" 2 输入文章\n");
            printf(" 3 统计字符\n");
            printf(" 4 统计字符串\n");
            printf(" 5 删除字符串\n");
            printf(" 0 退出\n");
            scanf("%d", &op);  // 输入操作选项
            if (op == 0) break;  // 退出程序
            else if(op == 1) printText(text);  // 显示文章
            else if(op == 2) text = inputText();  // 输入文章
            else if(op == 3) statCharacters(text);  // 统计字符
            else if(op == 4) statString(text);  // 统计字符串
            else if(op == 5) removeString(text);  // 删除字符串
        }
    }
    return 0;
}