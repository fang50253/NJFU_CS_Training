#include<iostream>
#include<algorithm>
#include<string>
#include<fstream>
#define spaces 7
#define maxsize 1000
using namespace std;
static int num = 0;
 
class stu {
private:		/*学生信息属于私密信息*/
	string name;	/*姓名*/
	string num;		/*学号*/
	string sex;		/*性别*/
public:
	double English;		/*各科成绩*/
	double Math;
	double Program_design;
	double Sports;
	double total;		/*总成绩*/
	int score;		/*判断此条信息是否完善*/
	stu() {		/*默认构造*/
		name = " "; num = " "; sex = " "; English = 0; Math = 0; Program_design = 0; Sports = 0; score = 0;
		settotal();
	}
	stu(string na, string nu, string se, double en, double ma, double c, double s) {		/*全参数构造*/
		name = na; num = nu;  English = en; Math = ma; Program_design = c; Sports = s; score = 1;
		if (se == "男" || se == "m")sex = 'm';
		else sex = 'w';
		settotal();
	}
	stu(string na, string nu, string se) {		/*不含成绩的构造*/
		name = na, num = nu;
		English = 0; Math = 0; Program_design = 0; Sports = 0; score = 0;
		if (se == "男" || se == "m")sex = 'm';
		else sex = 'w';
		settotal();
	}
	void stucpy(stu st) {		/*值复制函数*/
		name = st.getname();
		num = st.getnum();
		sex = st.getsex();
		English = st.English;
		Math = st.Math;
		Program_design = st.Program_design;
		Sports = st.Sports;
		score = st.score;
		settotal();
	}
	void studisplay() {		/*输出函数*/
		cout << num << " " << name << " " << English << " " << Math << " " << Program_design << " " << Sports << endl;
	}
	void settotal() {		/*计和函数*/
		total = English + Math + Program_design + Sports;
	}
	string getname() {		/*私有成员访问*/
		return name;
	}
	string getnum() {
		return num;
	}
	string getsex() {
		return sex;
	}
	void changename(string na) {		/*私有成员修改*/
		name = na;
	}
	void changenum(string nu) {
		num = nu;
	}
	void changesex(string se) {
		sex = se;
	}
};
stu st[maxsize];
#include"stu_deal.h"		/*函数文件*/
int main() {
	check();		/*查阅人数*/
	long_main();
	return 0;
}
 
 