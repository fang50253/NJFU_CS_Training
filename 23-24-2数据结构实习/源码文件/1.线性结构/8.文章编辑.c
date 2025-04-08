#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define TEXT_MAX 100000
//从文件中读取文章
char* readFile(const char filename[])
{
    FILE* input = fopen(filename, "r");
    char* text = (char*)malloc(sizeof(char) * TEXT_MAX);
    text[0] = '\0';
    if (input)
    {
        int pos = 0;
        while (!feof(input))
        {
            //从文件中逐个读取字符到数组
            text[pos++] = (char)fgetc(input);
        }
        text[pos] = '\0';
        fclose(input);
    }
    return text;
}
//输入文章
char* inputText() {
    char* text = (char*)malloc(sizeof(char) * TEXT_MAX);
    int line;
    printf("  输入文章\n");
    printf("---------------\n");
    printf("输入行数：");
    scanf("%d", &line);
    printf("输入内容：\n");
    text[0] = '\0';
    int i = 0;
    while (i < line) {
        char buffer[1024] = { 0 };
        fgets(buffer, 1024, stdin);
        if (strcmp(buffer, "\n") == 0) {
            continue;
        }
        strcat(text, buffer);
        ++i;
    }
    printf("---------------\n");
    return text;
}

//显示文章
void printText(const char text[]) {
    const char* pstr = text;
    printf("  显示内容\n");
    printf("---------------\n");
    while (*pstr) {
        printf("%c", *pstr);
        ++pstr;
    }
    printf("\n");
    printf("---------------\n");
}

//统计字符
void statCharacters(const char text[]) {
    const char* pstr = text;
    int letter = 0;
    int number = 0;
    int blank = 0;
    int total = 0;
    while (*pstr) {
        if (isalpha(*pstr)) {
            //累加字母个数
            ++letter;
        } else if (isalnum(*pstr)) {
            //累加数字个数
            ++number;
        } else if (isblank(*pstr)) {
            //累加空格个数
            ++blank;
        }
        ++total;
        ++pstr;
    }
    printf("  统计字符\n");
    printf("---------------\n");
    printf("字母：%d\n", letter);
    printf("数字：%d\n", number);
    printf("空格：%d\n", blank);
    printf("总数：%d\n", total);
    printf("---------------\n");
}

//统计字符串
void statString(const char text[]) {
    const char* pstr = text;
    char word[256] = { 0 };
    int count = 0;
    printf("  统计字符串\n");
    printf("---------------\n");
    printf("请输入字符串：");
    scanf("%s", word);
    while ((pstr = strstr(pstr, word))) {
        ++count;
        pstr += strlen(word);
    }
    printf("出现次数：%d\n", count);
    printf("---------------\n");
}

//删除字符串
void removeString(const char text[]) {
    const char* pstr = text;
    char word[256] = { 0 };
    printf("  删除字符串\n");
    printf("---------------\n");
    printf("请输入字符串：");
    scanf("%s", word);
    while (1) {
        char* target = strstr(pstr, word);
        if (target == NULL) {
            break;
        } else {
            //找到要删除的字符串，则将后续内容前移
            while (*(target + strlen(word))) {
                *target = *(target + strlen(word));
                ++target;
            }
            *target = '\0';
        }
    }
    printf("---------------\n");
}

//主函数

int main() {
    char* text = readFile("article.fzy");
    if (text) {
        while (1) 
        {
            int op;
            printf(" 1 显示文章\n");
            printf(" 2 输入文章\n");
            printf(" 3 统计字符\n");
            printf(" 4 统计字符串\n");
            printf(" 5 删除字符串\n");
            printf(" 0 退出\n");
            scanf("%d", &op);
            if (op == 0) break;
            else if(op==1) printText(text);
            else if(op==2) text = inputText();
            else if(op==3) statCharacters(text);
            else if(op==4) statString(text);
            else if(op==6) removeString(text);
        }
    }
    return 0;
}