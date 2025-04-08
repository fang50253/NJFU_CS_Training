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
