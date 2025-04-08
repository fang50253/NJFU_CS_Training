package Greeter;
/*
public class Greeter {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		for(int i=1;i<=31;i++) {
			System.out.print("*");
		}
		System.out.println("");
		System.out.print("*");
		for(int i=1;i<=29;i++) {
			System.out.print(" ");
		}
		System.out.println("*");
		System.out.print("*");
		for(int i=1;i<=7;i++) {
			System.out.print(" ");
		}
		System.out.println("Welcome to Java       *");
		System.out.print("*");
		for(int i=1;i<=29;i++) {
			System.out.print(" ");
		}
		System.out.println("*");
		System.out.println("* Nanjing Forestry University *");
		System.out.print("*");
		for(int i=1;i<=29;i++) {
			System.out.print(" ");
		}
		System.out.println("*");
		for(int i=1;i<=31;i++) {
			System.out.print("*");
		}
	}

}
*/
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