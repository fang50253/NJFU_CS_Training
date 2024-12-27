#include<iostream>
#include<algorithm>
#include<string>
#include<fstream>
#define spaces 7
#define maxsize 1000
using namespace std;

 

void check() {		/*查阅函数，统计文件中学生个数*/
	num = 0;
	ifstream ifs;
	ifs.open("student.dat", ios::in);
	if (!ifs.is_open())
	{
		cout << "Erro Read" << endl;
	}
	string buf;			//文件读取
	while (getline(ifs, buf))
	{
		num++;
	}
	ifs.close();
}
 
void findspace(string s, int* a) {	/*找字符串中空格方便进行字符串分割*/
	int len = 0;
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ' ') {
			a[len] = i;
			len++;
		}
	}
}
 
string strsplit(string s, int begin, int end) {		/*取begin到end的所有字符*/
	string ss;
	for (int i = begin; i <= end; i++) {
		ss.push_back(s[i]);
	}
	return ss;
}
 
double stringtodouble(string s) {		/*从文件中提取分数为string类型，将其转化为double类型（有精度损失）*/
	double ss = 0, sss = 0;
	int loc = s.length();
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == '.')loc = i;
	}
	for (int i = 0; i < loc; i++) {		/*整数位*/
		ss *= 10;
		ss += (int)s[i] - 48;
	}
	for (int i = s.length() - 1; i >= loc; i--) {		/*小数位*/
		sss /= 10;
		sss += (int)s[i] - 48;
	}
	return sss + ss;
}
 
 
void stringtostudent(string s, stu* student) {		/*从文件提取的信息为string类型，将其转化为student（class）类型*/
	string na, nu, se, en, ma, c, sp;
	int* a;
	a = new int[spaces + 1];
	findspace(s, a);
	nu = strsplit(s, 0, a[0] - 1);		/*从长字符串中提取各项信息*/
	na = strsplit(s, a[0] + 1, a[1] - 1);
	se = strsplit(s, a[1] + 1, a[2] - 1);
	en = strsplit(s, a[2] + 1, a[3] - 1);
	ma = strsplit(s, a[3] + 1, a[4] - 1);
	c = strsplit(s, a[4] + 1, a[5] - 1);
	sp = strsplit(s, a[5] + 1, a[6] - 1);
	student->changename(na);
	student->changenum(nu);
	student->changesex(se);
	student->English = stringtodouble(en);
	student->Math = stringtodouble(ma);
	student->Program_design = stringtodouble(c);
	student->Sports = stringtodouble(sp);
	student->score = 1;
}
 
 
 
int Mainmenu() {		/*主菜单*/
	cout << endl << "档案中共有学生" << num << "人" << endl;
	cout << "请选择操作：" << endl;
	cout << "1:查询学生" << endl;
	cout << "2:添加学生" << endl;
	cout << "3:平均成绩" << endl;
	cout << "4:分数排名" << endl;
	cout << "5:退出" << endl;
	int chose;
	do {
		cin >> chose;
		if (chose < 1 || chose>5)cout << "无效指令，重新输入！" << endl;		/*指令异常处理*/
		else break;
	} while (true);
	return chose;
}
 
int findstumenu() {		/*查询信息菜单*/
	cout << endl << "选择查询方式:" << endl;
	cout << "1.学号查询" << endl;
	cout << "2.姓名查询" << endl;
	cout << "3.班级序号查询" << endl;
	cout << "4.后退" << endl;
	int chose;
	do {
		cin >> chose;
		if (chose < 1 || chose>4)cout << "无效指令，重新输入！" << endl;
		else break;
	} while (true);
	return chose;
}
 
int addstumenu() {		/*添加信息菜单*/
	cout << endl << "选择添加方式:" << endl;
	cout << "1.手动输入" << endl;
	cout << "2.文件导入" << endl;
	cout << "3.后退" << endl;
	int chose;
	do {
		cin >> chose;
		if (chose < 1 || chose>3)cout << "无效指令，重新输入！" << endl;
		else break;
	} while (true);
	return chose;
}
 
int scoresortmenu() {		/*分数排名菜单*/
	cout << endl<<"选择排序所参照的成绩" << endl;
	cout << "1.总分" << endl;
	cout << "2.英语" << endl;
	cout << "3.数学" << endl;
	cout << "4.程序设计" << endl;
	cout << "5.体育" << endl;
	cout << "6.后退" << endl;
	int chose;
	do {
		cin >> chose;
		if (chose < 1 || chose>5)cout << "无效指令，重新输入！" << endl;
		else break;
	} while (true);
	return chose;
}
 
void long_main() {
	ifstream ifs;
	while (true) {
		switch (Mainmenu())
		{
		case 1: {
			switch (findstumenu()) {
			case 1: {
				bool p = false;		/*是否查到*/
				string num;
				cout << "请输入学生学号:";
				cin >> num;
				ifs.open("student.dat", ios::in);
				string buf;			//文件读取
				while (getline(ifs, buf))
				{
					stu* stu1 = new stu();
					stringtostudent(buf, stu1);
					if (stu1->getnum() == num) {
						stu1->studisplay();
						p = true;
					}
				}
				if (!p)cout << "无此学生信息" << endl;
				ifs.close();
				break;
			}
			case 2: {
				string name;
				int len = 0;
				cout << "请输入学生姓名:";
				cin >> name;
				ifs.open("student.dat", ios::in);
				string buf;			//文件读取
				while (getline(ifs, buf))
				{
					stu* stu1 = new stu();
					stringtostudent(buf, stu1);
					if (stu1->getname() == name) {
						st[len].stucpy(*stu1);
						len++;
					}
				}
				if (!len)cout << "无此学生信息" << endl;
				else {
					for (int i = 0; i < len; i++) {		/*输出排序*/
						for (int j = 0; j < len - 1; j++) {
							if (st[j].getnum() > st[j + 1].getnum()) {
								stu s;
								s.stucpy(st[j]);
								st[j].stucpy(st[j + 1]);
								st[j + 1].stucpy(s);
							}
						}
					}
					cout << "查询到" << len << "条信息：" << endl;
					for (int i = 0; i < len; i++) {
						cout << i + 1 << ": " << st[i].getnum() << " " << st[i].getname() << endl;		/*不确定具体信息时只展示姓名和学号*/
					}
					cout << "选择查询学生序号:";
					int k;		/*选择具体信息编号*/
					do {
						cin >> k;
						if (k > len || k < 1)cout << "无效的指令,请重新输入" << endl;
						else break;
					} while (true);
					st[k - 1].studisplay();
				}
				ifs.close();
				break;
			}
			case 3: {
				string clas;
				int len = 0;
				cout << "请输入班号:";
				cin >> clas;
				ifs.open("student.dat", ios::in);
				string buf;			//文件读取
				while (getline(ifs, buf))
				{
					stu* stu1 = new stu();
					stringtostudent(buf, stu1);
					if (strsplit(stu1->getnum(), 0, 7) == clas) {
						st[len].stucpy(*stu1);
						len++;
					}
				}
				if (!len)cout << "无此班级" << endl;
				else {
					cout << "查询到" << len << "条信息：" << endl;
					for (int i = 0; i < len; i++) {
						cout << i + 1 << ": " << st[i].getnum() << " " << st[i].getname() << endl;		/*不确定具体信息时只展示姓名和学号*/
					}
					cout << "选择查询学生序号:";
					int k;
					do {
						cin >> k;
						if (k > len || k < 1)cout << "无效的指令,请重新输入" << endl;
						else break;
					} while (true);
					st[k - 1].studisplay();
				}
				ifs.close();
				break;
			}
			case 4:break;
			}break;
		case 2: {
			switch (addstumenu())
			{
			case 1: {
 
				cout << "输入学号(十位)，姓名，性别(m/w)，英语成绩，数学成绩，程序设计成绩，体育成绩" << endl;
				stu stu1;
				string nu, na, se;
				cin >> nu >> na >> se >> stu1.English >> stu1.Math >> stu1.Program_design >> stu1.Sports;		/*手动输入（空格为间隔）*/
				bool p = false;		/*相同学号是否已存在*/
				int loc = 0;
				ifs.open("student.dat", ios::in);
				string buf;			//文件读取
				while (getline(ifs, buf))
				{
					stu* stu2 = new stu();
					stringtostudent(buf, stu2);
					if (stu2->getnum() == nu) {
						 p = true;
						break;
					}
					loc++;
				}
				ifs.close();
				if (!p) {		/*不存在添加*/
					int len = 0;
					ifs.open("student.dat", ios::in);
					string buf;			//文件读取
					while (getline(ifs, buf))
					{
						stu* stu2 = new stu();
						stringtostudent(buf, stu2);
						st[len].stucpy(*stu2);
						len++;
					}
					ofstream ofs;
					ofs.open("student.dat", ios::out);
					for (int i = 0; i < len; i++) {
						ofs << st[i].getnum() << " " << st[i].getname() << " "<<st[i].getsex()<<" " << st[i].English << " " << st[i].Math << " " << st[i].Program_design << " " << st[i].Sports << " " << "1" << endl;
					}
					ofs << nu << " " << na << " " << se << " " << stu1.English << " " << stu1.Math << " " << stu1.Program_design << " " << stu1.Sports << " " << "1" << endl;
					ofs.close();
					num++;
				}
				else {		/*存在则更新*/
					int len = 0;
					ifs.open("student.dat", ios::in);
					string buf;			//文件读取
					while (getline(ifs, buf))
					{
						stu* stu2 = new stu();
						stringtostudent(buf, stu2);
						st[len].stucpy(*stu2);
						len++;
					}
					ifs.close();
					st[loc].stucpy(stu1);
					st[loc].changename(na);
					st[loc].changenum(nu);
					st[loc].changesex(se);
					ofstream ofs;
					ofs.open("student.dat", ios::out);
					for (int i = 0; i < len; i++) {
						ofs << st[i].getnum() << " " << st[i].getname() << " "<<st[i].getsex()<<" " << st[i].English << " " << st[i].Math << " " << st[i].Program_design << " " << st[i].Sports << " " << "1" << endl;
					}
					ofs.close();
				}
				break;
			}
			case 2: {
				cout << "输入文件路径" << endl;
				string ss;
				cin >> ss;
				int len = 0;
				int len1 = 0;
				stu st1[maxsize];
				ifs.open("student.dat", ios::in);
				string buf;			//文件读取
				while (getline(ifs, buf))
				{
					stu* stu2 = new stu();
					stringtostudent(buf, stu2);
					st[len].stucpy(*stu2);
					len++;
				}
				ifs.close();
				ifs.open(ss, ios::in);
				if (!ifs.is_open()) {		/*打不开证明没有这个文件，或者权限不够*/
					cout << "文件路径无效！" << endl;
					break;
				}
				while (getline(ifs, buf))
				{
					stu* stu2 = new stu();
					stringtostudent(buf, stu2);
					st1[len1].stucpy(*stu2);
					len1++;
				}
				ifs.close();
				ofstream ofs;
				ofs.open("student.dat", ios::app);
				for (int i = 0; i < len1; i++) {
					bool t = true;		/*是否已存在*/
					for (int j = 0; j < len; j++) {
						if (st1[i].getnum() == st[j].getnum())t = false;
					}
					if (t)ofs << st1[i].getnum() << " " << st1[i].getname() << " " << st1[i].getsex() << " " << st1[i].English << " " << st1[i].Math << " " << st1[i].Program_design << " " << st1[i].Sports << " " << "1" << endl;		/*未存在添加，已存在不添加*/
				}
				ofs.close();
				check();
				break;
			}
			case 3:break;
			}
			break;
		}break;
		case 3: {
			int len = 0;
			ifs.open("student.dat", ios::in);
			string buf;			//文件读取
			while (getline(ifs, buf))
			{
				bool b = true;	/*未找到班级*/
				stu* stu1 = new stu();
				stringtostudent(buf, stu1);
				for (int i = 0; i < len; i++) {
					if (st[i].getnum() == strsplit(stu1->getnum(), 0, 7)) {		/*班级名和学生学号前八位相等*/
						b = false;		
						st[i].English += stu1->English;		/*此段代码中st数组作为班级数组*/
						st[i].Math += stu1->Math;
						st[i].Program_design += stu1->Program_design;
						st[i].Sports += stu1->Sports;
						st[i].score++;		/*记录人数*/
					}
				}
				if (b) {		/*没有班级则新建班级*/
					st[len].changenum(strsplit(stu1->getnum(), 0, 7));
					st[len].Math = st[len].Program_design = st[len].Sports = st[len].English =st[len].score=0;
					st[len].English += stu1->English;
					st[len].Math += stu1->Math;
					st[len].Program_design += stu1->Program_design;
					st[len].Sports += stu1->Sports;
					st[len].score++;		/*记录人数*/
					len++;
				}
			}
			for (int i = 0; i < len; i++) {		/*输出排序*/
				for (int j = 0; j < len - 1; j++) {
					if (st[j].getnum() > st[j + 1].getnum()) {
						stu s;
						s.stucpy(st[j]);
						st[j].stucpy(st[j + 1]);
						st[j + 1].stucpy(s);
					}
				}
			}
			for (int i = 0; i < len; i++) {
				cout << "班号：" << st[i].getnum() << "  " << "英语平均分：" << st[i].English / st[i].score << "  数学平均分：" << st[i].Math / st[i].score << "  程序设计平均分" << st[i].Program_design / st[i].score << "  体育平均分" << st[i].Sports / st[i].score << endl;
			}
			ifs.close();
		}break;
		case 4: {
			ifstream ifs;
			int len = 0;
			ifs.open("student.dat", ios::in);
			string buf;			//文件读取
			while (getline(ifs, buf))
			{
				stu* stu2 = new stu();
				stringtostudent(buf, stu2);
				st[len].stucpy(*stu2);
				len++;
			}
			ifs.close();
			switch (scoresortmenu()) {
			case 1: {
				for (int i = 0; i < len; i++) {
					for (int j = 0; j < len - 1; j++) {		/*成绩排序*/
						if (st[j].total < st[j + 1].total) {
							stu stu1;
							stu1.stucpy(st[j]);
							st[j].stucpy(st[j + 1]);
							st[j+1].stucpy(stu1);
						}
					}
				}
				ofstream ofs;
				ofs.open("Total_stu.dat", ios::trunc);		/*重写文件*/
				for (int i = 0; i < len; i++) {
					ofs << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")ofs << "男";
					else ofs << "女";
					ofs<<" "<<st[i].total << endl;
					cout << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")cout << "男";
					else cout << "女";
					cout << " " << st[i].total<< endl;
				}
				ofs.close();
				break;
			}
			case 2: {
				for (int i = 0; i < len; i++) {
					for (int j = 0; j < len - 1; j++) {
						if (st[j].English < st[j + 1].English) {
							stu stu1;
							stu1.stucpy(st[j]);
							st[j].stucpy(st[j + 1]);
							st[j + 1].stucpy(stu1);
						}
					}
				}
				ofstream ofs;
				ofs.open("English_stu.dat", ios::trunc);
				for (int i = 0; i < len; i++) {
					ofs << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")ofs << "男";
					else ofs << "女";
					ofs << " " << st[i].English  << endl;
					ofs.close();
					cout << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")cout << "男";
					else cout << "女";
					cout << " " << st[i].English << endl;
				}
				break;
			}
			case 3: {
				for (int i = 0; i < len; i++) {
					for (int j = 0; j < len - 1; j++) {
						if (st[j].Math < st[j + 1].Math) {
							stu stu1;
							stu1.stucpy(st[j]);
							st[j].stucpy(st[j + 1]);
							st[j + 1].stucpy(stu1);
						}
					}
				}
				ofstream ofs;
				ofs.open("Math_stu.dat", ios::trunc);
				for (int i = 0; i < len; i++) {
					ofs << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")ofs << "男";
					else ofs << "女";
					ofs << " " << st[i].Math<< endl;
					cout << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")cout << "男";
					else cout << "女";
					cout << " " << st[i].Math<< endl;
				}
				ofs.close();
				break;
			}
			case 4: {
				for (int i = 0; i < len; i++) {
					for (int j = 0; j < len - 1; j++) {
						if (st[j].Program_design < st[j + 1].Program_design) {
							stu stu1;
							stu1.stucpy(st[j]);
							st[j].stucpy(st[j + 1]);
							st[j + 1].stucpy(stu1);
						}
					}
				}
				ofstream ofs;
				ofs.open("Program_design_stu.dat", ios::trunc);
				for (int i = 0; i < len; i++) {
					ofs << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")ofs << "男";
					else ofs << "女";
					ofs << " " << st[i].Program_design  << endl;
					cout << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")cout << "男";
					else cout << "女";
					cout << " " << st[i].Program_design << endl;
				}
				ofs.close();
				break;
			}
			case 5: {
				for (int i = 0; i < len; i++) {
					for (int j = 0; j < len - 1; j++) {
						if (st[j].Sports < st[j + 1].Sports) {
							stu stu1;
							stu1.stucpy(st[j]);
							st[j].stucpy(st[j + 1]);
							st[j + 1].stucpy(stu1);
						}
					}
				}
				ofstream ofs;
				ofs.open("Sports_stu.dat", ios::trunc);
				for (int i = 0; i < len; i++) {
					ofs << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")ofs << "男";
					else ofs << "女";
					ofs << " "<< st[i].Sports << endl;
					cout << st[i].getnum() << " " << st[i].getname() << " ";
					if (st[i].getsex() == "m")cout << "男";
					else cout<< "女";
					cout << " "<< st[i].Sports << endl;
				}
				ofs.close();
				break;
			}
			case 6:break;
			}
		}break;
		default:return;
		}
 
		}
	}
}