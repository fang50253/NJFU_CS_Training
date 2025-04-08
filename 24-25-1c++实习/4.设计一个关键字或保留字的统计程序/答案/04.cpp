#include<iostream>
#include<string>
#include<fstream>
#include<time.h>
#define Maxsize 5000
using namespace std;
class word {	/*封装（面向对象）*/
private:
	string s;	/*关键字/非关键字本体*/
	int num;	/*出现次数*/
public:
	word() {		/*默认构造*/
		s = " ";
		num = 0;
	}
	word(string ss) {		/*带参构造*/
		s = ss;
		num = 0;	
	}
	string getword() { return s; }		/*关键字提取*/
	void changeword(string ss) { s = ss; }	/*单词改变*/
	void appear() { num++; }	/*出现函数，为该关键字次数加一*/
	int getnum() { return num; }	/*取得出现次数*/
};
word key[Maxsize];
word nokey[Maxsize];
int length = 0, ulength = 0;	/*个数*/
int legal(char c);
int main() {
	ifstream ifs;
	ofstream ofs;
	string buf;
	ifs.open("time.dat", ios::in);
	getline(ifs, buf);
	int n=0;
	for (int i = 0; i <buf.length(); i++) {		/*转次数为整数*/
		n *= 10;
		n += (int)(buf[i] - 48);
	}
	n++;
	cout << "第" << n << "次扫描" << endl;
	ifs.close();
	ofs.open("time.dat", ios::trunc);		/*清空再写*/
	ofs << n << endl;
	ofs.close();
	int start = clock();
	ifs.open("Allkey_words.dat", ios::in);		/*可以修改路径，需要统计的关键字*/
	if (!ifs.is_open())
	{
		cout << "Erro Read" << endl;
	}
	while (getline(ifs, buf))//文件读取
	{
		bool p = true;
		for (int i = 0; i < length; i++) {
			if (key[i].getword() == buf) {		/*判重*/
				p = false;
				break;
			}
		}
		if (p) {
			key[length].changeword(buf);
			length++;
		}
	}
	ifs.close();
	cout << "共有" << length+1 << "个关键字" << endl;
	ifs.open("stu_deal.cpp", ios::in);
	while (getline(ifs, buf))
	{
		for (int i = 0; i < buf.length(); i++) {
			string s;
			bool t = true;	/*是否为关键字*/
			while (legal(buf[i])) {
				s.push_back(buf[i]);		/*符合要求字符组合成字符串*/
				i++;
			}
			if (s == " " || s.length() == 0)continue;
			for (int i = 0; i < length; i++) {
				if (key[i].getword() == s) {
					key[i].appear();
					t = false;
					break;
				}
			}
			if (t) {
				bool p = true;
				for (int i = 0; i < ulength; i++) {		/*先找有没有出现过*/
					if (nokey[i].getword() == s) {
						p = false;
						nokey[i].appear();
						break;
					}
				}
				if (p) {	/*没出现过新建非关键字*/
					nokey[ulength].changeword(s);
					nokey[ulength].appear();
					ulength++;
				}
			}
		}
	}
	ofs.open("no_keywords.dat", ios::trunc);		/*轮流写入*/
	ofs << "非关键字：" << endl;
	for (int i = 0; i < ulength; i++) {
		ofs << nokey[i].getword() << " " << nokey[i].getnum() << "次" << endl;
	}
	ofs.close();
	ofs.open("keywords.dat", ios::trunc);
	ofs << "关键字：" << endl;
	for (int i = 0; i < length; i++) {
		ofs << key[i].getword() << " " << key[i].getnum() << "次" << endl;
	}
	ofs.close();
	int end = clock();
	cout << "此次扫描花费时间为：" << (end - start) << "毫秒" << endl;
}
 
 
int legal(char c) {		/*判断是否为允许的字符*/
	if (c >= 'a' && c <= 'z') {
		return 1;
	}
	else if (c >= 'A' && c <= 'Z') {
		return 1;
	}
	else if (c >= '0' && c <= '9') {
		return 1;
	}
	else if (c == '_')return 1;
	else return 0;
}