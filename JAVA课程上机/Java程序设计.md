# 第〇章.课程简介及课程考核方式

# 第一章.Java概述
## 一.程序设计语言概述
## 二.面向对象概述
## 三.Java开发和运行环境

### 1.编译java程序的命令

```sh
java -d classes (input.java)
```
这行命令会在cd工作目录下创建一个$classes/(input.java)$下生成$input.class$文件

### 2.运行java程序的命令(因为会在后面自动追加一个.class,所以不需要添加后缀名)
```sh
java -cp classes input
```

### 3.因为在Windows系统条件下\会被转译，类unix环境下不会转译，所以类unix需要写/，在运行的过程中建议用.

## 四.简单Java开发及调试步骤
## 五.Java语言的特点

# 第二章.Java语言基础

## 一.Java符号
### 1.java语言举例
```java
public class HelloWorld{
    public static void main(String[] args){
        System.out.println("Hello,World!");//在屏幕上输出Hello,world
        for(int i=0;i<args.length;++i){
            System.out.print(args[i]);
            System.out.print("\n");
        }
        System.exit(5);
    }
}
```
* 代码会严格区分大小写，如果把函数入口写成public static void Main(String[] args)则会出错。
* 命令行参数在c/cpp中是可选参数，但是在java中必须要带有。
* System类
* System.out输出流
* System.out.println()输出方法
* System.out.print()输出方法

### 2.注释的书写
* 单行注释：//
* 多行注释：
```java
/**
* L1
* L2
*/
```
多行代码不能嵌套
* 自动文档注释：javadoc
## 二.数据类型和变量

### 1.基本数据类型(原始数据类型)

* 整数

| 类型   | 存储空间 | 范围                  |
|--------|----------|-----------------------|
| byte   | 1B       | -127 ~ 128            |
| short  | 2B       | -32768 ~ 32767        |
| int    | 4B       | -2e32 ~ 2e32-1        |
| long   | 8B       | -2e64 ~ 2e64-1        |

整型字符的前缀形式：0x开头是16进制，0开头为8进制

* 浮点数

| 类型   | 存储空间 | 范围         |
|--------|----------|--------------|
| float  | 4B       | 精度7位      |
| double | 8B       | 精度15位     |

特殊的浮点数
```
Float(Double).POSITIVE_INFINITY正无穷

Float(Double).NEGATIVE_INFINITY负无穷

Float(Double).NaN非数值(Not a Number)
```
* 字符类型

char：16位Unicode(2字节) **和c语言不同**

换码序列：
```
\b 退格
\t 水平制表
\f 换页
\r 会车
\n 换行
\' 单引号
\" 双引号
\\ 反斜杠
```
* 布尔类型boolean

bool 类型只有true和false两种类型，不可以和任何一种类型进行强制变量类型转换

* 科学计数法

```
double m=0x12.2p2;  //m=12.2*2^2
```

### 2.变量的声明
* 格式：类型 变量名1[值1]；
* 变量名要求：同c
* 变量范围和生命周期

*使用钱必须初始化*

*不允许隐藏变量*：不允许出现内部变量隐藏外部变量
* 表达式自动类型提升

byte short char -> int

long 

**boolean不能与其他类型转换**
## 三.表达式与运算符
## 四.数据的输入和输出
## 五.流程控制语句
## 六.方法
## 七.数组

# 第三章.类和对象

## 一.Java的类
## 
# 第四章.继承、多态和接口

# 第五章.常用系统类

# 第六章.Java绘图与Applet

# 第七章.图形和用户界面编程

# 第八章.异常处理

# 第九章.流式输入/输出与文件处理

## 一.I/O流概述
### 1.数据源：键盘、文件、内存缓冲区、管道、网络
### 2.数据池：控制台、网络、管道、内存缓冲区
### 3.流的概念：连续字节序列
### 4.按目标分类
### 5.按内容分类：字节流(字节序列)、字符流(字符序列)
### 6.流操作步骤：

	打开输入流/输出流，建立通数据源/数据池的关联

	从输入流读取/写到输出流（读取可能EOF）

	关闭输入流/输出流

### 7.读InputStream
```
* public abstract int read() throws IOExpection
* public int read(byte[] bytes,int offset,int length)throws IOExpection
* public int read(byte[] bytes) throws IOExpection
```
### 8.写InputStream
```
* public void abstract void write(int unsignedByte) throws IOExpection
* public void write(byte[] bytes,int offset,int length)throws IOExpection
* public void write(byte[] bytes)throws IOExpection
```
### 9.刷出OutputStream
```
public void flush() throws IOException
```
### 10.关闭I/O流

### 11.分层/链式I/O
* 缓冲
* 过滤
* 格式转换
### 12.缓冲I/O
* BufferedInputStream
* BufferedOutputStream
### 13.格式化I/O
* DataInputStream：public final int readInt() throws IOException
# 第十章.多线程

# 第十一章.JDBC技术和数据库应用

# 第十二章.Java的网络编程

# 第十三章.Swing编程

