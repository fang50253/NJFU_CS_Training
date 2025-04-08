#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

class RandomAccessFile {
private:
    std::fstream file;
    std::string filename;
    size_t unit_size; // 单位大小，文本文件为行，非文本文件为512字节

    void ensure_open() {
        if (!file.is_open()) {
            throw std::runtime_error("File is not open.");
        }
    }

    void go_to_unit(size_t index) {
        if (unit_size == 512) {
            file.seekg(index * unit_size, std::ios::beg);
            file.seekp(index * unit_size, std::ios::beg);
        } else {
            file.clear(); // 确保读写流是正常的
            file.seekg(0, std::ios::beg);
            for (size_t i = 0; i < index; ++i) {
                if (!file.ignore(std::numeric_limits<std::streamsize>::max(), '\n')) {
                    throw std::out_of_range("Index exceeds total number of units.");
                }
            }
        }
    }

public:
    // 构造函数
    RandomAccessFile(const std::string &filename, size_t unit_size = 512)
        : filename(filename), unit_size(unit_size) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file) {
            // 如果文件不存在则创建
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    // 析构函数
    ~RandomAccessFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    // 重载操作符 []
    std::string operator[](size_t index) {
        ensure_open();
        go_to_unit(index);

        if (unit_size == 512) { // 非文本文件以512字节为单位读取
            char buffer[512] = {0};
            file.read(buffer, unit_size);
            return std::string(buffer, file.gcount());
        } else { // 文本文件按行读取
            std::string line;
            if (!std::getline(file, line)) {
                throw std::out_of_range("Index exceeds total number of units.");
            }
            return line;
        }
    }

    // 写入操作
    void write(size_t index, const std::string &data) {
        ensure_open();
        go_to_unit(index);

        if (unit_size == 512) {
            if (data.size() > unit_size) {
                throw std::invalid_argument("Data exceeds unit size.");
            }
            char buffer[512] = {0};
            std::memcpy(buffer, data.c_str(), data.size());
            file.write(buffer, unit_size);
        } else {
            file << data << std::endl;
        }
    }

    // 获取单位总数
    size_t total_units() {
        ensure_open();
        file.seekg(0, std::ios::end);
        size_t file_size = file.tellg();
        if (unit_size == 512) {
            return (file_size + unit_size - 1) / unit_size;
        } else {
            file.clear();
            file.seekg(0, std::ios::beg);
            size_t count = 0;
            std::string line;
            while (std::getline(file, line)) {
                ++count;
            }
            return count;
        }
    }
};

int main() {
    try {
        // 创建文本文件操作对象
        RandomAccessFile textFile("test.txt", 1);
        textFile.write(0, "Hello, World!");
        textFile.write(1, "C++ File I/O");
        std::cout << "Line 0: " << textFile[0] << std::endl;
        std::cout << "Line 1: " << textFile[1] << std::endl;
        std::cout << "Total lines: " << textFile.total_units() << std::endl;

        // 创建非文本文件操作对象
        RandomAccessFile binaryFile("test.bin", 512);
        binaryFile.write(0, "This is a binary test.");
        std::cout << "Block 0: " << binaryFile[0] << std::endl;
        std::cout << "Total blocks: " << binaryFile.total_units() << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}