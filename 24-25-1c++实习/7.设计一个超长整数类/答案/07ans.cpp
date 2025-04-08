#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <stack>

using namespace std;

class doublelong {
public:
	friend int compare(const doublelong& l, const doublelong& r);
	doublelong();
	doublelong(const int);
	doublelong(const string);
	doublelong(const doublelong& rhs);
	doublelong& operator=(const doublelong&);

	friend istream& operator>>(istream& in, doublelong& rhs);
	friend ostream& operator<<(ostream& out,const doublelong& rhs);

	friend string add(const doublelong& l, const doublelong& r);
	friend string min(const doublelong& l, const doublelong& r);

	const doublelong& operator+(const doublelong&) const;
	const doublelong& operator-(const doublelong&) const;
	const doublelong& operator*(const doublelong&) const;
	const doublelong& operator/(const doublelong&) const;

	const doublelong& operator+=(const doublelong&);
	const doublelong& operator-=(const doublelong&);
	const doublelong& operator/=(const doublelong&);
	const doublelong& operator*=(const doublelong&);


	void chu();
	void chen();
private:
	bool signal;
	int a[500];
	int len;
};

int main()
{
	doublelong a, b;
	cin >> a;
	cin >> b;
	
	cout << " a + b " << endl;
	cout << a + b << endl;

	cout << " a - b " << endl;
	cout << a - b << endl;
	cout << " a * b " << endl;
	cout << a * b << endl;
	cout << " a / b " << endl;
	cout << a / b << endl;
	cout << " a *= b ; a = " << endl;
	a *= b;
	cout << a << endl;
	cout << " a /= b ; a = " << endl;
	a /= b;
	cout << a << endl;
	cout << " a += b ; a = " << endl;
	a += b;
	cout << a << endl;
	cout << " a -= b ; a = " << endl;
	a -= b;
	cout << a << endl;

	return 0;
}


doublelong::doublelong() {
	signal = false;
	len = 1;
	memset(a, 0, sizeof(a));
}

doublelong::doublelong(const int n) {
	int b;
	if (n < 0) {
		signal = true;
		b = -n;
	}
	else {
		signal = false;
		b = n;
	}
	memset(a, 0, sizeof(a));
	len = 0;
	while (b) {
		a[len++] = b % 10;
		b /= 10;
	}
}

doublelong::doublelong(const string s) {
	int one;
	if (s[0] == '-') {
		signal = true;
		one = 1;
	}
	else {
		signal = false;
		one = 0;
	}
	memset(a, 0, sizeof(a));

	int length = s.length();
	len = length - one;
	int i = 0;
	while (length > 0 + one) {
		a[i++] = s[--length] - '0';
	}
}


doublelong::doublelong(const doublelong& rhs) {
	signal = rhs.signal;
	memset(a, 0, sizeof(a));
	len = rhs.len;
	for (int i = 0; i < len; i++) {
		a[i] = rhs.a[i];
	}
}

doublelong& doublelong::operator=(const doublelong& rhs) {
	signal = rhs.signal;
	memset(a, 0, sizeof(a));
	len = rhs.len;
	for (int i = 0; i < len; i++) {
		a[i] = rhs.a[i];
	}
	return *this;
}


istream& operator>>(istream& in, doublelong& rhs) {
	memset(rhs.a, 0, sizeof(rhs.a));
	string s;
	in >> s;
	if (s[0] == '-') {
		rhs.signal = true;
	}
	else {
		rhs.signal = false;
	}
	memset(rhs.a, 0, sizeof(rhs.a));
	rhs.len = 0;
	int length = s.length();
	while (length > 0 + rhs.signal) {
		rhs.a[rhs.len++] = s[--length] - '0';
	}
	return in;
}

ostream& operator<<(ostream& out,const doublelong& rhs) {
	if (rhs.signal) {
		cout << "-";
	}
	for (int i = rhs.len - 1; i >= 0; i--) {
		out << rhs.a[i];
	}
	return out;
}

const doublelong& doublelong::operator+(const doublelong& rhs) const {
	string result;
	bool check = false;
	if (this->signal == rhs.signal) {
		if (this->signal) {
			check = 1;
		}
		result = add(*this , rhs);
	}
	else {
		if (this->signal == false)
			result = min(*this, rhs);
		else
			result = min(rhs, *this);
	}
	if (check) {
		result = "-" + result;
	}
	return *new doublelong(result);
}


const doublelong& doublelong::operator-(const doublelong& r) const {
	string result;
	bool check = 0;
	if (this->signal == r.signal) {
		result = min(*this, r);
		if (this->signal) {
			check = true;
		}
	}
	else {
		result = add(*this, r);
		if (this->signal) {
			check = true;
		}
	}
	if (check) {
		if (result[0] == '-') {
			result.erase(result.begin());
		}
		else {
			result = "-" + result;
		}
	}
	return *new doublelong(result);
}

const doublelong& doublelong::operator*(const doublelong& rhs) const {
	doublelong& ret = *new doublelong();
	int i, j, up;
	int templ, tempr;
	for (i = 0; i < len; i++) {
		up = 0;
		for (j = 0; j < rhs.len; j++) {
			templ = a[i] * rhs.a[j] + ret.a[i + j] + up;
			up = templ / 10;
			ret.a[i + j] = templ % 10;
		}
		if (up != 0)
			ret.a[i + j] = up;
	}
	ret.len = i + j;
	while (ret.a[ret.len - 1] == 0 && ret.len > 1)
		ret.len--;
	if (this->signal == rhs.signal) {
		ret.signal = false;
	}
	else {
		ret.signal = true;
	}
	return ret;
}

const doublelong& doublelong::operator/(const doublelong& rhs) const {
	doublelong& ret = *new doublelong();
	doublelong l(*this);
	doublelong r(rhs);
	string result;
	int k = 0;
	if (l.len == 0) {
		cout << "除数为0   未定义的除法" << endl;
		return ret;
	}
	if (l.len < r.len) {
		return ret;
	}
	while (l.len > r.len) {
		r.chen();
		k++;
	}
	for (int i = 0; i <= k; i++) {
		for (int j = 0; j <= 9; j++) {
			if (compare(l, r * (j + 1)) == 0) {
				result += to_string(j + 1) ;
				l = doublelong(min(l, r * (1 + j)));
				r.chu();
				break;
			}
			else if(compare(l, r * (j + 1)) == -1) {
				result += to_string(j);
				l = doublelong(min(l, r * (j)));
				r.chu();
				while (compare(l, r) < 0) {
					result += "0";
					r.chu();
					k--;
				}
				break;
			}
		}
	}
	if (result[0] = '0')
		result.erase(result.begin());
	if (this->signal != rhs.signal) {
		result = "-" + result;
	}
	return *new doublelong(result);
}

const doublelong& doublelong::operator+=(const doublelong& rhs){
	(*this) = *this + rhs;
	return *this;
}
const doublelong& doublelong::operator-=(const doublelong& rhs) {
	(*this) = *this - rhs;
	return *this;
}

string add(const doublelong& l, const doublelong& r) {
	int length = l.len < r.len ? l.len : r.len;
	const doublelong& t = (l.len < r.len ? r : l);
	string result;
	int jin = 0;
	for (int i = 0; i < length; i++) {
		result = to_string((l.a[i] + r.a[i] +jin) % 10) + result;
		jin = (l.a[i] + r.a[i]) / 10;
	}
	for (length; length < t.len; length++) {
		result = to_string((t.a[length] + jin) % 10) + result;
		jin = (t.a[length] + jin) / 10;
	}
	if (jin > 0) {
		result = to_string(jin) + result;
	}
	return result;
}

string min(const doublelong& l, const doublelong& r) {
	int check = compare(l, r);
	int jin = 0;
	string result;
	if (check == 0) {
		return "";
	}
	else if (check == 1) {
		int i;
		for ( i = 0; i < r.len; i++) {
			int k = l.a[i] - r.a[i] - jin;
			if (k < 0) {
				jin = 1;
				k += 10;
			}
			else
				jin = 0;
			result = to_string(k) + result;
		}
		for(i; i < l.len; i++) {
			int k = l.a[i] - jin;
			if (k < 0) {
				jin = 1;
				k += 10;
			}
			else
				jin = 0;
			result = to_string(k) + result;
		}
		while (result[0] == '0')
			result.erase(result.begin());
		return result;
	}
	else {
		int i;
		for (i = 0; i < l.len; i++) {
			int k = r.a[i] - l.a[i] - jin;
			if (k < 0) {
				jin = 1;
				k += 10;
			}
			else
				jin = 0;
			result = to_string(k) + result;
		}
		for (i; i < r.len; i++) {
			int k = r.a[i] - jin;
			if (k < 0) {
				jin = 1;
				k += 10;
			}
			else
				jin = 0;
			result = to_string(k) + result;
		}
		while(result[0] == '0')
			result.erase(result.begin());
		result = "-" + result;
		return result;
	}
}

int compare(const doublelong& l, const doublelong& r) {
	int i = 0;
	stack<short> store;

	while (i != l.len && i != r.len)
	{
		if (l.a[i] == r.a[i]) {
			store.push(0);
		}
		else if (l.a[i] > r.a[i]) {
			store.push(1);
		}
		else
			store.push(-1);
		i++;
	}

	if (i != l.len) {
		return 1;
	}
	else if (i != r.len) {
		return -1;
	}

	while (1) {
		if (store.size() == 0)
			return 0;
		if (store.top() == 0)
		{
			store.pop();
		}
		else if (store.top() == 1) {
			return 1;
		}
		else if (store.top() == -1) {
			return -1;
		}
	}
}

void doublelong::chu() {
	for (int i = 0; i < len - 1; i++) {
		a[i] = a[i + 1];
	}
	len--;
}

void doublelong::chen() {
	for (int i = len; i > 0; i--) {
		a[i] = a[i - 1];
	}
	a[0] = 0;
	len++;
}

const doublelong& doublelong::operator/=(const doublelong& rhs) {
	*this = *this / rhs;
	return *this;
}
const doublelong & doublelong::operator*=(const doublelong& rhs) {
	*this = *this * rhs;
	return *this;
}
//原作者：https://blog.csdn.net/luoyejuanqiufeng/article/details/118379196