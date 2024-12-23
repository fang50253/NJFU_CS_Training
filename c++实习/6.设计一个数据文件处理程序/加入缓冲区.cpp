#include <cstdio>
#include <cstring>
#include <ctime>
#include <cstdlib>

const int LINE_MAX = 1e5;
const int VECTOR_MAX = 1 << 8;
const int SAMPLE_MAX = 1e3 + 10;
const int BUFFER_SIZE = 1024 * 1024; // 缓冲区大小（1MB）

class Line {
private:
    char line[LINE_MAX];
    int vectors[VECTOR_MAX];
    int sample_cnt[SAMPLE_MAX];
    FILE *fp, *wfp, *lfp;
    int sample_num;
    int eigenvector;
    int feature = 0; // 动态计算的特征向量数量

    char write_buffer[BUFFER_SIZE]; // 写入缓冲区
    size_t buffer_index = 0;        // 缓冲区当前写入位置

    int max(int a, int b) {
        return a > b ? a : b;
    }

    void read_file(const char filename[]) {
        fp = fopen(filename, "r+");
        if (fp == nullptr) printf("File open failed\n");
    }

    void write_file(const char filename[], const char lable[]) {
        wfp = fopen(filename, "w");
        lfp = fopen(lable, "w");
        if (wfp == nullptr || lfp == nullptr) printf("File open failed\n");
    }

    void flush_buffer() {
        if (buffer_index > 0) {
            fwrite(write_buffer, 1, buffer_index, wfp);
            buffer_index = 0; // 清空缓冲区
        }
    }

    void buffer_write(const char *data) {
        size_t data_len = strlen(data);
        if (buffer_index + data_len >= BUFFER_SIZE) {
            flush_buffer(); // 缓冲区满时写入文件
        }
        memcpy(write_buffer + buffer_index, data, data_len);
        buffer_index += data_len;
    }

    void rtrim(char *str) {
        size_t len = strlen(str);
        while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\n')) str[--len] = '\0';
    }

public:
    bool read_one_line() {
        ++sample_num;
        if (fgets(line, sizeof line, fp) == nullptr) return false; // 读取文件行
        int linelength = strlen(line);
        int tmp = 0, tmp_cnt = 0;
        int vector_position;
        memset(vectors, 0, sizeof vectors);
        line[linelength - 1] = ' ';
        line[linelength] = '\0';
        linelength = strlen(line);
        for (int i = 0; i < linelength + 1; ++i) {
            if (line[i] != ' ' && line[i] != ':') {
                tmp = tmp * 10 + line[i] - '0';
            } else if (line[i] == ' ' && tmp_cnt == 0) {
                eigenvector = tmp;
                ++sample_cnt[tmp];
                tmp = 0;
                ++tmp_cnt;
            } else if (line[i] == ' ' || i == linelength - 1) {
                vectors[vector_position] = tmp;
                feature = max(feature, vector_position); // 更新动态特征向量数量
                tmp = 0;
                ++tmp_cnt;
            } else if (line[i] == ':') {
                vector_position = tmp;
                tmp = 0;
                ++tmp_cnt;
            }
        }
        return true;
    }

    void write_one_line() {
        char buffer[LINE_MAX];
        int offset = 0;
        offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", eigenvector);
        for (int i = 1; i <= feature; ++i) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, "%d ", vectors[i]);
        }
        buffer[offset++] = '\n';
        buffer[offset] = '\0';
        buffer_write(buffer);
    }

    void restore_file(const char lable[], const char full[], const char restore[]) {
        FILE *restore_lable = fopen(lable, "r+");
        FILE *restore_full = fopen(full, "r+");
        FILE *restore_restore = fopen(restore, "w");
        int max_feature = 0; // 用于动态计算特征向量数量
        do {
            fscanf(restore_lable, "%d", &eigenvector);
            fprintf(restore_restore, "%d ", eigenvector);
            fgets(line, sizeof line, restore_full);
            int linelength = strlen(line);
            line[linelength] = ' ';
            int tmp = 0, tmp_cnt = 0;
            for (int i = 0; i < linelength; ++i) {
                if (line[i] == ' ') {
                    ++tmp_cnt;
                    if (tmp == 0) continue;
                    else {
                        fprintf(restore_restore, "%d:%d ", tmp_cnt, tmp);
                        max_feature = max(max_feature, tmp_cnt);
                    }
                    tmp = 0;
                } else {
                    tmp = tmp * 10 + line[i] - '0';
                }
            }
            fprintf(restore_restore, "\n");
        } while (!feof(restore_full) || !feof(restore_lable));
        feature = max(feature, max_feature); // 动态更新特征向量数量
        fclose(restore_lable);
        fclose(restore_full);
        fclose(restore_restore);
    }

    void display_sample() {
        int cnt = 0;
        for (int i = 0; i <= 999; ++i)
            if (sample_cnt[i]) ++cnt;
        printf("Eigenvector_Cnt: %d\n", feature);
        printf("Sample_Cnt: %d\n", sample_num - 1);
    }

    Line(const char readfile[], const char writefile[], const char writelable[]) {
        read_file(readfile);
        write_file(writefile, writelable);
    }

    ~Line() {
        flush_buffer(); // 写入剩余缓冲区内容
        fclose(fp);
        fclose(lfp);
        fclose(wfp);
    }
};

int main() {
    Line *line = new Line("aloi", "aloi_full.txt", "aloi_lable.txt");
    clock_t start = clock();
    while (line->read_one_line()) {
        line->write_one_line();
    }
    line->display_sample();
    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Unzip_Process_Time: %.2f seconds\n", cpu_time_used);
    start = clock();
    line->restore_file("aloi_lable.txt", "aloi_full.txt", "aloi_restore.txt");
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Zip_Process_Time: %.2f seconds\n", cpu_time_used);
    delete line;
    return 0;
}