package BreakJail;
import java.util.*;
public class BreakJail {
	static int location=1;
	private static void printmap() {
		System.out.println("----------|--------|");
		System.out.println("|         |        |");
		System.out.println("|         -        |");
		System.out.println("|  room1    room2  |           North");
		System.out.println("|         -        |             ^");
		System.out.println("|         |        |             |");
		System.out.println("-----| |-----| |---|     West <-----> East");
		System.out.println("|         |        |             |");
		System.out.println("|         -        |             v");
		System.out.println("|  room3    room4  |           South");
		System.out.println("|         -        |");
		System.out.println("|         |        |");
		System.out.println("----------|--------|");
	}
	private static void version1() {
		char in=(char)System.in.read();
		
	}
	private static void version2() {
		Scanner scanner=new Scanner(System.in);
		char input=scanner.next().charAt(0);
		if((input=='A'||input=='a')&&(location==1||location==3)){
			System.out.println("Cannot move to:"+input);
		}
		if((input=='D'||input=='d')) {
			
		}
		
		
	}
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		printmap();
		version2();
		
	}

}
