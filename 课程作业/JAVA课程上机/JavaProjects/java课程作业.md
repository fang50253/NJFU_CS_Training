# java课程作业
## Greeter
### 1.Greeter
```java
public class Greeter{
	private static void greet(int cols) {
		String str="";
		for(int i=1;i<=cols;i++) str+="*";
		System.out.println(str);
	}
	private static void greet(int cols,String words) {
		int len=words.length();
		String str="*";
		int blank=29-len>>1;
		for(int i=1;i<=blank;i++) {
			str+=" ";
		}
		str+=words;
		for(int i=1;i<=blank;i++) {
			str+=" ";
		}
		str+="*";
		System.out.println(str);
	}
	public static void main(String[] args) {
		greet(31);
		greet(31," ");
		greet(31,"Welcome to Java");
		greet(31," ");
		greet(31,"Nanjing Forestry University");
		greet(31," ");
		greet(31);
	}
}
```
### 2.Greeter2
```java
package Greeter;

public class Greeter2 {
	private static void greet(int cols) {
		for(int i=1;i<=cols;i++) System.out.print("*");
		System.out.println("");
	}
	private static void greet(int cols,String words) {
		int len=words.length();
		System.out.print("*");
		int blank=29-len>>1;
		for(int i=1;i<=blank;i++) {
			System.out.print(" ");
		}
		System.out.print(words);
		for(int i=1;i<=blank;i++) {
			System.out.print(" ");
		}
		System.out.println("*");
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		greet(31);
		greet(31," ");
		greet(31,"Welcome to Java");
		greet(31," ");
		greet(31,"Nanjing Forestry University");
		greet(31," ");
		greet(31);
	}

}
```
## Factorial
```java
package Factorial;
public class Factorial {
	static long last_ans=0;
	private static long calculate(int n) {
		long ans=1;
		for(int i=1;i<=n;++i) {
			ans*=i;
		}
		last_ans=ans;
		return ans;
	}
	private static long getFact() {
			return last_ans;
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("5!="+calculate(5));
	}

}
```
## BreakJail
### BreakJail

### BreakJail2