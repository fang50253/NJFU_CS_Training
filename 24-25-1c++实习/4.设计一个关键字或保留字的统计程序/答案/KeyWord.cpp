#include <cstring>
#include <fstream>
#include <string>
#include <iostream>
#define max 10
using namespace std;

class keyWord {
public:
	keyWord();
	void setData(string* keyw, string filen, long key, long kcon, long nkcon, int keyn, int t);
	void search();
	void display();
	~keyWord();
private:
	string keyword[max];
	string filename;
	long key;
	long keycount;
	long nokeycount;
	int keynum;
	int time;
};


void keyWord::display() {
	for (int i = 0; i < keynum; i++) {
		cout << keyword[i] << endl;
	}
}

void keyWord::search() {
	fstream fie;
	fie.open(filename + ".txt");
	ofstream fiet("keywords.txt");

	char str[2000];
	int flag = 0;

	for (int w = 0; w < keynum; w++) {
		while (fie.getline(str, 2000)) {
			for (int i = 0; i < strlen(str); i++) {
				if (str[i] == ' ' || str[i] == ',' || str[i] == '.' || str[i] == '?' || str[i] == '!') {
					key++;
					char* word = new char[i + 1 - flag];
					for (int j = flag; j < i; j++) {
						word[j - flag] = str[j];
					}
					word[i - flag] = '\0';
					string st = string(word);

					if (st == keyword[w]) {
						keycount++;
						flag = i + 1;
					}
					else {
						nokeycount++;
						flag = i + 1;
					}
					delete[] word;
				}
			}
			time++;
			flag = 0;
		}
		fie.clear();
		fie.seekg(0, ios::beg);
	}
	key /= (keynum + 1);

	cout << "一共有 " << key << " 个单词" << endl;
	cout << "其中有 " << keycount << " 个单词匹配，有 " << nokeycount << " 个单词不匹配,一共扫描 " << time << "次文件";
	fiet << "关键字出现次数：" << keycount << "  非关键字出现次数 " << nokeycount << "一共扫描" << time << "次文件" << endl;
}

void keyWord::setData(string* keyw, string filen, long keyo, long kcon, long nkon, int keyn, int ti) {
	for (int i = 0; i < keyn; i++) {
		keyword[i] = keyw[i];
	}
	filename = filen;
	key = keyo;
	keycount = kcon;
	nokeycount = nkon;
	keynum = keyn;
	time = ti;
}

keyWord::keyWord()
{
	keyword[0] = "xxx";
	filename = "xxx";
	keycount = 0;
	nokeycount = 0;
	time = 0;
}

keyWord::~keyWord() {}


#include <iostream>
#include "baoliu.h"
#include <fstream>
using namespace std;

int main()
{
	string filen;

	int k;
	int time = 0;
	cout << "这是一个统计文件加中的关键字的程序" << endl;
	cout << "请输入要匹配的文件名:";
	cin >> filen;
	cout << "输入要查找的关键字个数（最大为10）:";
	cin >> k;

	string* keywod = new string[k];

	for (int i = 0; i < k; i++) {
		cout << "输入第" << i + 1 << "个关键字" << endl;
		cin >> keywod[i];
	}

	ofstream fot("keyfile.txt", ios::in);
	for (int i = 0; i < k; i++) {
		fot << keywod[i] << " ";
	}

	keyWord KeySearch;
	KeySearch.setData(keywod, filen, 0, 0, 0, k, time);
	KeySearch.search();
	return 0;
}

//答案：https://blog.csdn.net/luoyejuanqiufeng/article/details/118345320