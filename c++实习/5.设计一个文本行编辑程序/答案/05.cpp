#include<iostream>
#include<string>
#include<fstream>
using namespace std;
typedef struct N {
	string s;
	struct N* next;
}Node;
int len = 0;	/*链表长度*/
string file;	/*访问文件路径*/
int main() {
	Node* begin = new Node();		/*文本链表*/
	Node* end = begin;
	ifstream ifs;
	while (true) {
		file = "testpage.dat";
		ifs.open(file, ios::in);
		if (file == "null")return 0;
		else {
			string buf;			//读入原有内容到链表
			while (getline(ifs, buf))
			{
				Node* n = new Node();
				n->next = NULL;
				n->s = buf;
				end->next = n;
				end = n;
				len++;
			}
			ifs.close();
			break;
		}
	}
	string command[] = { "*L","*I","*D","*R","*X","*Q" };		/*指令种类*/
	int comlen = 6;
	while (true) {
		string com;
		int c;
		cout << "共有"<<len<<"行文本" << endl << "输入指令" << endl;
		cin >> com;
		bool p = true;
		for (int i = 0; i < 6; i++) {
			if (command[i] == com) {		/*返回位置+1*/
				c = i+1;
				p = false;
				break;
			}
		}
		if (p) {
			cout << "无效指令!" << endl; continue;
		}
		int m, n;
		switch (c) {
		case 1: {
			while (true) {
				cin >> m >> n;
				if (m <= len && m >= 0 && n <= len && n >= 0 && m < n)break;
				else cout << "m,n中存在的不合法数据，请重新输入！" << endl;
			}
			Node* p = begin;
			for (int i = 0; i < m; i++) {		/*直接输出*/
				p = p->next;
			}
			for (int i = m; i <= n; i++) {
				cout << p->s << endl;
				p = p->next;
			}
			break;
		}
		case 2: {
			while (true) {
				cin >> m;
				if (m <= len && m >= 0)break;
				else cout << "m不合法，请重新输入！" << endl;
			}
			int kk = 1;
			if (kk)getchar();	/*吞掉m后的回车*/
			kk--;
			Node* p = begin;
			for (int i = 0; i < m; i++) {
				p = p->next;
			}
			while (true) {
				string txt;
				while (true) {
					char c;
					c = getchar();
					if (c != '\n')txt.push_back(c);		/*不是回车就算在一个字符串里面*/
					else break;
				}
				if (txt == "^Z")break;
				len++;
				Node* q = new Node();		/*插入结点*/
				q->s = txt;
				q->next = p->next;
				p->next = q;
				p = p->next;
			}
			break;
		}
		case 3: {
			cin >> m >> n;
			len -= (n - m + 1);
			Node* p = begin;
			for (int i = 0; i < m-1; i++) {
				p = p->next;
			}
			Node* q = begin;
			for (int i = 0; i < n; i++) {
				q = q->next;
			}
			p->next = q->next;		/*直接连接*/
			break;
		}
		case 4: {
			while (true) {
				cin >> m >> n;
				if (m <= len && m >= 0 && n <= len && n >= 0 && m < n)break;
				else cout << "m,n中存在的不合法数据，请重新输入！" << endl;
			}
			int kk = 1;
			if (kk == 1)getchar();		/*吞n后面的回车*/
			kk--;
			Node* p = begin;
			for (int i = 0; i < m - 1; i++) {
				p = p->next;
			}
			Node* p1 = begin;
			for (int i = 0; i < n; i++) {
				p1 = p1->next;
			}
			len -= (n - m + 1);
			p->next = p1->next;
			while (true) {
				string txt;
				kk--;
				while (true) {
					char c;
					c = getchar();
					if (c != '\n')txt.push_back(c);	/*不是回车就算在一个字符串里面*/
					else break;
				}
				if (txt == "^Z")break;
				len++;
				Node* q = new Node();
				q->s = txt;
				q->next = p->next;
				p->next = q;
				p = p->next;
			}
			break;
		}
		case 5: {
			ofstream ofs;
			ofs.open(file, ios::trunc);		/*保存写入新链表*/
			Node* p = begin->next;
			while (p != NULL) {
				ofs << p->s << endl;
				p = p->next;
			}
			ofs.close();
			return 0;
		}
		case 6: {
			return 0;		/*不保存*/
		}
		}
	}
}