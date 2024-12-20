#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define maxsize 400	/*400访问次数*/
typedef struct p_str {
	int pagenum; /* 页号 */
	int count; /* 访问页面的次数 */
	int aa;/*访问位(Clock)*/
	int mm;/*修改位(Clock)*/
	int longtime;/*未使用的时间(LRU)*/
	struct p_str* next; /* 下一指针 */
}p_str;
int n;
void FIFO();
void LRU();
void Improve_Clock();
 
int main() {
	int m;
	srand((unsigned)time(NULL));
	scanf("%d %d", &n, &m); //输入内存容量和测试次数
	int i = m;
	while (m--) {
		printf("\n第%d次测试\n", i - m);
		//cout << endl << "第" << i - m << "次测试" << endl;
		FIFO();
		LRU();
		Improve_Clock();
	}
}
 
void FIFO() {
	double effective = 0;	/*有效访问次数*/
	int length = 0;		/*队列长度*/
	p_str* begin = (p_str*)malloc(sizeof(p_str));		/*创建头结点*/
	p_str* end = begin;		/*创建尾结点*/
	begin->count = -1; begin->pagenum = -1; begin->next = NULL;
	for (int i = 0; i < maxsize; i++) {	/*400次随机数测试*/
		int t = 0;
		int page = rand() % 400 / 10;
		p_str* pp = begin;
		for (int j = 0; j < length; j++) {		/*先遍历看内存是否有相同的页*/
			pp = pp->next;
			if (pp->pagenum == page) {		/*有则有效次数加一*/
				pp->count++;
				t = 1;
				effective++;
				break;
			}
		}
		if (t == 0) {		/*进行无效访问，即添加结点或替换节点*/
			if (length < n) {		/*内存未满，添加结点*/
				p_str* q = (p_str*)malloc(sizeof(p_str));
				q->count = 1; q->pagenum = page; q->next = NULL;
				end->next = q;
				end = q;
				length++;
			}
			else {		/*内存已满，删除表头结点，新结点加到表尾*/
				begin->next = begin->next->next;
				p_str* q = (p_str*)malloc(sizeof(p_str));
				q->count = 1; q->pagenum = page; q->next = NULL;
				end->next = q;
				end = q;
			}
		}
	}
	printf("在页面流个数为%d，内存容量为%d页的条件下，FIFO命中率为：%f %%\n", maxsize, n, (effective / 400) * 100);
	//cout << "在页面流个数为" << maxsize << "，内存容量为" << n << "页的情况下，FIFO命中率为:" << (effective / 400) * 100 << "%" << endl;
}
 
void LRU() {
	double effective = 0;	/*有效访问次数*/
	int length = 0;		/*队列长度*/
	p_str* begin = (p_str*)malloc(sizeof(p_str));	/*创建头尾结点*/
	p_str* end = begin;
	begin->count = -1; begin->pagenum = -1; begin->longtime = -1; begin->next = NULL;
	for (int i = 0; i < maxsize; i++) {		/*400次随机数测试*/
		int t = 0;
		int page = rand() % 400 / 10;
		p_str* pp = begin;
		for (int j = 0; j < length; j++) {		/*遍历内存看是否可以有效访问*/
			pp = pp->next;
			if (pp->pagenum == page) {
				pp->count++; pp->longtime = -1; t++; effective++; break;
			}
		}
		if (!t) {		/*无效访问*/
			if (length < n) {		/*添加结点*/
				p_str* q = (p_str*)malloc(sizeof(p_str));
				q->count = 1; q->pagenum = page; q->next = NULL; q->longtime = -1;
				end->next = q;
				end = q;
				length++;
			}
			else {
				int max, maxtime = -1;		/*最久未访问结点位置，最久未访问时间*/
				pp = begin;
				for (int j = 0; j < length; j++) {		/*寻找最久未访问的结点*/
					pp = pp->next;
					if (pp->longtime > maxtime) {
						maxtime = pp->longtime;
						max = j;
					}
				}
				pp = begin;
				for (int j = 0; j < max - 1; j++) {		/*删除该结点*/
					pp = pp->next;
				}
				pp->next = pp->next->next;
				p_str* q = (p_str*)malloc(sizeof(p_str));	/*添加新结点*/
				q->count = 1; q->pagenum = page; q->next = NULL; q->longtime = -1;
				end->next = q;
				end = q;
			}
		}
		pp = begin;
		for (int j = 0; j < length; j++) {		/*所有未被访问节点时间加一*/
			pp = pp->next;
			pp->longtime++;
		}
	}
	printf("在页面流个数为%d，内存容量为%d页的条件下，LRU命中率为：%f %%\n", maxsize, n, (effective / 400) * 100);
	//cout << "在页面流个数为" << maxsize << "，内存容量为" << n << "页的情况下，LRU命中率为:" << (effective / maxsize) * 100 << "%" << endl;
}
 
void  Improve_Clock() {
	double effective = 0;	/*有效访问次数*/
	int length = 0;		/*队列长度*/
	p_str* begin = (p_str*)malloc(sizeof(p_str));		/*创建头尾结点*/
	p_str* end = begin;
	begin->count = -1; begin->pagenum = -1; begin->aa = -1; begin->mm = -1; begin->next = NULL;
	for (int i = 0; i < maxsize; i++) {
		int t = 0;
		int page = rand() % 400 / 10;
		int  m = rand() % 2;
		p_str* pp = begin;
		for (int j = 0; j < length; j++) {		/*尝试有效访问*/
			pp = pp->next;
			if (pp == NULL) {
				pp = end;
			}
			if (pp->pagenum == page) {
				pp->aa = 1; t++; effective++; break;
			}
		}
		if (!t) {
			if (length < n) {		/*添加结点*/
				p_str* q = (p_str*)malloc(sizeof(p_str));
				q->count = 1; q->pagenum = page; q->next = NULL; q->longtime = -1; q->aa = 1; q->mm = m;
				end->next = q;
				end = q;
				length++;
			}
			else {
				int f = 0;
				p_str* q = (p_str*)malloc(sizeof(p_str));
				q->count = 1; q->pagenum = page; q->next = NULL; q->longtime = -1; q->aa = 1; q->mm = m;
				p_str* pp = begin;
				for (int j = 0; j < length-1; j++) {
					if (pp->next->aa == 0 && pp->next->mm == 0) {		/*寻找A=0，M=0替换*/
						pp->next = pp->next->next;
						end->next = q;
						end = q;
						f = 1; break;
					}
					else {
						pp = pp->next;
					}
				}
				if (f)continue;
				else {
					pp = begin;
					for (int j = 0; j < length-1; j++) {
						if (pp->next->aa == 0 && pp->next->mm == 1) {		/*寻找A=0,M=1替换*/
							pp->next = pp->next->next;
							end->next = q;
							end = q;
							f = 1; break;
						}
						else {
							pp = pp->next;
						}
					}
				}
				if (f)continue;
				else {
					pp = begin;
					for (int j = 0; j < length; j++) {		/*所有结点A归零*/
						pp = pp->next;
						pp->aa = 0;
					}
				}
				pp = begin;
				for (int j = 0; j < length-1; j++) {
					if (pp->next->aa == 0 && pp->next->mm == 0) {	/*寻找A=0，M=0替换*/
						pp->next = pp->next->next;
						end->next = q;
						end = q;
						f = 1; break;
					}
					else {
						pp = pp->next;
					}
				}
				if (f)continue;
				else {
					pp = begin;
					for (int j = 0; j < length-1; j++) {
						if (pp->next->aa == 0 && pp->next->mm == 1) {	/*寻找A=0,M=1替换*/
							pp->next = pp->next->next;
							end->next = q;
							end = q;
							f = 1; break;
						}
						else {
							pp = pp->next;
						}
					}
				}
			}
		}
	}
	//cout << "在页面流个数为" << maxsize << "，内存容量为" << n << "页的情况下，优化Clock命中率为:" << (effective / maxsize) * 100 << "%" << endl;
	printf("在页面流个数为%d，内存容量为%d页的条件下，优化Clock命中率为：%f %%\n", maxsize, n, (effective / 400) * 100);
}